/**
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
 * @file        Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file implements main class of logic layer in cynara service
 */

#include <log/log.h>
#include <common.h>
#include <exceptions/PluginNotFoundException.h>
#include <exceptions/BucketNotExistsException.h>
#include <exceptions/DefaultBucketDeletionException.h>
#include <exceptions/DefaultBucketSetNoneException.h>
#include <signal.h>

#include <main/Cynara.h>
#include <request/CheckRequest.h>
#include <request/InsertOrUpdateBucketRequest.h>
#include <request/RemoveBucketRequest.h>
#include <request/RequestContext.h>
#include <request/SetPoliciesRequest.h>
#include <request/SignalRequest.h>
#include <response/CheckResponse.h>
#include <response/CodeResponse.h>
#include <storage/Storage.h>

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

void Logic::execute(RequestContextPtr context, CheckRequestPtr request) {
    PolicyResult result(PredefinedPolicyType::DENY);
    if (check(context, request->key(), result)) {
        context->returnResponse(context, std::make_shared<CheckResponse>(result,
                                request->sequenceNumber()));
    }
}

bool Logic::check(RequestContextPtr context UNUSED, const PolicyKey &key,
                  PolicyResult& result) {
    result = m_storage->checkPolicy(key);

    switch (result.policyType()) {
        case PredefinedPolicyType::ALLOW :
            LOGD("check of policy key <%s> returned ALLOW", key.toString().c_str());
            return true;
        case PredefinedPolicyType::DENY :
            LOGD("check of policy key <%s> returned DENY", key.toString().c_str());
            return true;
    }
    //todo pass question to proper plugin that:
    //  1) returns false when answer has to be waited for (UI)
    //  2) returns true if Response is to be generated
    // In case 1) context should be saved in plugin in order to return answer when ready.

    //in case no proper plugin is found
    throw PluginNotFoundException(result);
}

void Logic::execute(RequestContextPtr context, InsertOrUpdateBucketRequestPtr request) {
    auto code = CodeResponse::Code::OK;

    try {
        m_storage->addOrUpdateBucket(request->bucketId(), request->result());
        onPoliciesChanged();
    } catch (const DefaultBucketSetNoneException &ex) {
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
