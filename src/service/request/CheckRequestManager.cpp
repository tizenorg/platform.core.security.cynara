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
 * @file        src/service/request/CheckRequestManager.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Definition of CheckRequestManager class
 */

#include <cstdint>

#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <request/RequestContext.h>

#include "CheckRequestManager.h"

namespace Cynara {

CheckContextPtr CheckRequestManager::createContext(const PolicyKey &key,
                                                   const RequestContextPtr &request,
                                                   ProtocolFrameSequenceNumber checkId,
                                                   const ExternalPluginPtr &plugin,
                                                   const AgentTalkerPtr &agentTalkerPtr) {
    auto mapKey = CheckContainerKey(request->responseQueue(), checkId);

    const auto &checkContext = m_checks.find(mapKey);
    if (checkContext != m_checks.end())
        return checkContext->second;

    CheckContextPtr checkPtr = std::make_shared<CheckContext>(key, request, checkId, plugin,
                                                              agentTalkerPtr);
    if (m_checks.insert(std::make_pair(mapKey, checkPtr)).second) {
        return checkPtr;
    }

    return CheckContextPtr();
}

CheckContextPtr CheckRequestManager::getContext(const LinkId &linkId,
                                                ProtocolFrameSequenceNumber checkId) {
    const auto &checkContext = m_checks.find(CheckContainerKey(std::ref(linkId), checkId));
    return checkContext != m_checks.end() ? checkContext->second : CheckContextPtr();
}

CheckContextPtr CheckRequestManager::getContext(const AgentTalkerPtr &talker) {
    for (const auto &context : m_checks) {
        if (context.second->m_agentTalker == talker) {
            return context.second;
        }
    }
    return nullptr;
}

void CheckRequestManager::removeRequest(const CheckContextPtr &checkContextPtr) {
    for (const auto &context : m_checks) {
        if (context.second == checkContextPtr) {
            m_checks.erase(context.first);
            LOGD("Check context removed: <%s>:[%" PRIu16 "]",
                 context.second->m_key.toString().c_str(), context.second->m_checkId);
            return;
        }
    }
}

} // namespace Cynara
