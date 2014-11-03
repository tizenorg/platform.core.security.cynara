/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file        src/service/logic/Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements main class of logic layer in cynara service
 */

#include <cinttypes>
#include <functional>

#include <log/log.h>
#include <common.h>
#include <exceptions/BucketNotExistsException.h>
#include <exceptions/DatabaseException.h>
#include <exceptions/DefaultBucketDeletionException.h>
#include <exceptions/DefaultBucketSetNoneException.h>
#include <exceptions/InvalidBucketIdException.h>
#include <exceptions/PluginErrorException.h>
#include <exceptions/PluginNotFoundException.h>

#include <signal.h>

#include <main/Cynara.h>
#include <request/AdminCheckRequest.h>
#include <request/AgentActionRequest.h>
#include <request/AgentRegisterRequest.h>
#include <request/CancelRequest.h>
#include <request/CheckRequest.h>
#include <request/InsertOrUpdateBucketRequest.h>
#include <request/RemoveBucketRequest.h>
#include <request/RequestContext.h>
#include <request/SetPoliciesRequest.h>
#include <request/SignalRequest.h>
#include <response/AgentRegisterResponse.h>
#include <response/CancelResponse.h>
#include <response/CheckResponse.h>
#include <response/CodeResponse.h>
#include <storage/Storage.h>

#include <cynara-agent.h>

#include <agent/AgentManager.h>
#include <sockets/LinkMonitor.h>
#include <sockets/SocketManager.h>

#include "Logic.h"

namespace Cynara {
Logic::Logic() {
}

Logic::~Logic() {
}

void Logic::execute(RequestContextPtr context UNUSED, SignalRequestPtr request) {
    LOGD("Processing signal: [%d]", request->signalNumber());

    switch (request->signalNumber()) {
    case SIGTERM:
        LOGI("SIGTERM received!");
        m_socketManager->mainLoopStop();
        break;
    }
}

void Logic::execute(RequestContextPtr context, AdminCheckRequestPtr request) {
    PolicyResult result = m_storage->checkPolicy(request->key(), request->startBucket(),
                                                 request->recursive());

    context->returnResponse(context, std::make_shared<CheckResponse>(result,
                            request->sequenceNumber()));
}

void Logic::execute(RequestContextPtr context, AgentActionRequestPtr request) {
    AgentTalkerPtr talkerPtr = m_agentManager->getTalker(context->responseQueue(),
                                                         request->sequenceNumber());
    if (!talkerPtr) {
        LOGD("Received response from agent with invalid request id: [%" PRIu16 "]",
             request->sequenceNumber());
        return;
    }

    CheckContextPtr checkContextPtr = m_checkRequestManager.getContext(talkerPtr);
    if (!checkContextPtr) {
        LOGE("No matching check context for agent talker.");
        m_agentManager->removeTalker(talkerPtr);
        return;
    }

    PluginData data(request->data().begin(), request->data().end());
    if (request->type() == CYNARA_MSG_TYPE_CANCEL) {
        cancel(checkContextPtr->m_key, checkContextPtr->m_checkId, data,
               checkContextPtr->m_requestContext, checkContextPtr->m_plugin);
    } else if (request->type() == CYNARA_MSG_TYPE_ACTION) {
        update(checkContextPtr->m_key, checkContextPtr->m_checkId, data,
               checkContextPtr->m_requestContext, checkContextPtr->m_plugin);
    } else {
        LOGE("Invalid response type [%d] in response from agent <%s>",
             static_cast<int>(request->type()), talkerPtr->agentType().c_str());
        // For now I'm not removing check from manager. I'm not sure what is the correct approach.
        return;
    }

    m_agentManager->removeTalker(talkerPtr);
    m_checkRequestManager.removeRequest(checkContextPtr);
}

void Logic::execute(RequestContextPtr context, AgentRegisterRequestPtr request) {
    m_linkMonitor->registerLink(context->responseQueue(), request->agentType());
    auto result = m_agentManager->registerAgent(request->agentType(), context->responseQueue());
    context->returnResponse(context, std::make_shared<AgentRegisterResponse>(
                            result, request->sequenceNumber()));
}

void Logic::execute(RequestContextPtr context, CancelRequestPtr request) {
    CheckContextPtr checkContextPtr = m_checkRequestManager.getContext(context->responseQueue(),
                                                                       request->sequenceNumber());
    if (!checkContextPtr) {
        LOGE("Cancel request id: [%" PRIu16 "] with no matching request in progress.",
             request->sequenceNumber());
        return;
    }

    if (!checkContextPtr->m_agentTalker->cancel()) {
        LOGE("Error in sending cancel request to agent: <%s>",
             checkContextPtr->m_agentTalker->agentType().c_str());
        m_agentManager->removeTalker(checkContextPtr->m_agentTalker);
        m_checkRequestManager.removeRequest(checkContextPtr);
    }

    LOGD("Returning response for cancel request id: [%" PRIu16 "].", request->sequenceNumber());
    context->returnResponse(context, std::make_shared<CancelResponse>(request->sequenceNumber()));
}

void Logic::execute(RequestContextPtr context, CheckRequestPtr request) {
    PolicyResult result(PredefinedPolicyType::DENY);
    if (check(context, request->key(), request->sequenceNumber(), result)) {
        context->returnResponse(context, std::make_shared<CheckResponse>(result,
                                request->sequenceNumber()));
    }
}

bool Logic::check(const RequestContextPtr &context, const PolicyKey &key,
                  ProtocolFrameSequenceNumber checkId, PolicyResult &result) {
    std::ostringstream stream;
    stream << "Service@" << std::hex << &context->responseQueue();
    m_linkMonitor->registerLink(context->responseQueue(), stream.str());

    result = m_storage->checkPolicy(key);

    switch (result.policyType()) {
        case PredefinedPolicyType::ALLOW :
            LOGD("check of policy key <%s> returned ALLOW", key.toString().c_str());
            return true;
        case PredefinedPolicyType::DENY :
            LOGD("check of policy key <%s> returned DENY", key.toString().c_str());
            return true;
    }

    return pluginCheck(context, key, checkId, result);
}

bool Logic::pluginCheck(const RequestContextPtr &context, const PolicyKey &key,
                        ProtocolFrameSequenceNumber checkId, PolicyResult &result) {

    LOGD("Trying to check policy: <%s> in plugin.", key.toString().c_str());

    ExternalPluginPtr plugin = m_pluginManager->getPlugin(result.policyType());
    if (!plugin) {
        LOGE("Plugin not found for policy: [0x%x]", result.policyType());
        result = PolicyResult();
        return true;
    }

    AgentType requiredAgent;
    PluginData pluginData;

    auto ret = plugin->check(key.client().toString(), key.user().toString(),
                             key.privilege().toString(), result, requiredAgent, pluginData);

    switch (ret) {
        case ExternalPluginInterface::PluginStatus::ANSWER_READY:
            return true;
        case ExternalPluginInterface::PluginStatus::ANSWER_NOTREADY: {
                result = PolicyResult(PredefinedPolicyType::DENY);
                if (!m_agentManager->agentExists(requiredAgent)) {
                    LOGE("Required agent: <%s> is not registered.", requiredAgent.c_str());
                    return true;
                }

                AgentTalkerPtr agentTalker = m_agentManager->createTalker(requiredAgent);
                if (!agentTalker) {
                    LOGE("Required agent talker for: <%s> could not be created.",
                         requiredAgent.c_str());
                    return true;
                }

                CheckContextPtr checkContextPtr = m_checkRequestManager.createContext(key, context,
                        checkId, plugin, agentTalker);
                if (!agentTalker->send(pluginData)) {
                    LOGE("Error sending data to agent: <%s>", requiredAgent.c_str());
                    m_checkRequestManager.removeRequest(checkContextPtr);
                    m_agentManager->removeTalker(agentTalker);
                    return true;
                }
            }
            return false;
        default:
            throw PluginErrorException(key); // This 'throw' should be removed or handled properly.
    }
}

bool Logic::update(const PolicyKey &key, ProtocolFrameSequenceNumber checkId,
                   const PluginData &agentData, const RequestContextPtr &context,
                   const ExternalPluginPtr &plugin) {

    LOGD("Check update: <%s>:[%" PRIu16 "]", key.toString().c_str(), checkId);

    PolicyResult result;
    bool answerReady = false;
    auto ret = plugin->update(key.client().toString(), key.user().toString(),
                              key.privilege().toString(), agentData, result);
    switch (ret) {
        case ExternalPluginInterface::PluginStatus::SUCCESS:
            answerReady = true;
            break;
        case ExternalPluginInterface::PluginStatus::ERROR:
            result = PolicyResult(PredefinedPolicyType::DENY);
            answerReady = true;
            break;
        default:
            throw PluginErrorException(key);
    }

    if (answerReady && m_linkMonitor->linkExists(context->responseQueue())) {
        context->returnResponse(context, std::make_shared<CheckResponse>(result, checkId));
        return true;
    }
    return false;
}

bool Logic::cancel(const PolicyKey &key UNUSED, ProtocolFrameSequenceNumber checkId,
                   const PluginData &agentData UNUSED, const RequestContextPtr &context,
                   const ExternalPluginPtr &plugin UNUSED) {
    LOGD("Check cancel: <%s>:[%" PRIu16 "]", key.toString().c_str(), checkId);
    // TODO: I have no idea whether update should be called or not in case of cancel
    // If not then most of arguments are not needed and may be removed
    //    PolicyResult result;
    //    auto ret = plugin->update(key.client().toString(), key.user().toString(),
    //                              key.privilege().toString(),  agentData, result);

    if (m_linkMonitor->linkExists(context->responseQueue())) {
        context->returnResponse(context, std::make_shared<CancelResponse>(checkId));
    }
    return true;
}

void Logic::execute(RequestContextPtr context, InsertOrUpdateBucketRequestPtr request) {
    auto code = CodeResponse::Code::OK;

    try {
        m_storage->addOrUpdateBucket(request->bucketId(), request->result());
        onPoliciesChanged();
    } catch (const DatabaseException &ex) {
        code = CodeResponse::Code::FAILED;
    } catch (const DefaultBucketSetNoneException &ex) {
        code = CodeResponse::Code::NOT_ALLOWED;
    } catch (const InvalidBucketIdException &ex) {
        code = CodeResponse::Code::NOT_ALLOWED;
    }

    context->returnResponse(context, std::make_shared<CodeResponse>(code,
                            request->sequenceNumber()));
}

void Logic::execute(RequestContextPtr context, RemoveBucketRequestPtr request) {
    auto code = CodeResponse::Code::OK;
    try {
        m_storage->deleteBucket(request->bucketId());
        onPoliciesChanged();
    } catch (const DatabaseException &ex) {
        code = CodeResponse::Code::FAILED;
    } catch (const BucketNotExistsException &ex) {
        code = CodeResponse::Code::NO_BUCKET;
    } catch (const DefaultBucketDeletionException &ex) {
        code = CodeResponse::Code::NOT_ALLOWED;
    }
    context->returnResponse(context, std::make_shared<CodeResponse>(code,
                            request->sequenceNumber()));
}

void Logic::execute(RequestContextPtr context, SetPoliciesRequestPtr request) {
    auto code = CodeResponse::Code::OK;
    try {
        m_storage->insertPolicies(request->policiesToBeInsertedOrUpdated());
        m_storage->deletePolicies(request->policiesToBeRemoved());
        onPoliciesChanged();
    } catch (const DatabaseException &ex) {
        code = CodeResponse::Code::FAILED;
    } catch (const BucketNotExistsException &ex) {
        code = CodeResponse::Code::NO_BUCKET;
    }
    context->returnResponse(context, std::make_shared<CodeResponse>(code,
                            request->sequenceNumber()));
}

void Logic::onPoliciesChanged(void) {
    m_storage->save();
    m_socketManager->disconnectAllClients();
    //todo remove all saved contexts (if there will be any saved contexts)
}

} // namespace Cynara
