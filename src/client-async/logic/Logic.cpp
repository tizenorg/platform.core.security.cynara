/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/*
 * @file        Logic.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main
 *              libcynara-client-async class
 */

#include <cinttypes>

#include <cache/NaiveInterpreter.h>
#include <common.h>
#include <log/log.h>
#include <protocol/ProtocolClientAsync.h>
#include <request/CheckRequest.h>
#include <request/CancelRequest.h>
#include <request/pointers.h>
#include <response/CheckResponse.h>
#include <response/CancelResponse.h>
#include <response/pointers.h>
#include <types/ProtocolFields.h>

#include "Logic.h"

namespace Cynara {

const std::string clientAsyncSocketPath("/run/cynara/cynara-async.socket");

Logic::Logic()
{
    m_socket = std::make_shared<SocketClientAsync>(
        clientAsyncSocketPath, std::make_shared<ProtocolClientAsync>());
    auto naiveInterpreter = std::make_shared<NaiveInterpreter>();
    m_plugins[PredefinedPolicyType::ALLOW] = naiveInterpreter;
    m_plugins[PredefinedPolicyType::DENY] = naiveInterpreter;
    m_plugins[PredefinedPolicyType::BUCKET] = naiveInterpreter;
}

int Logic::connect(int &sockFd) noexcept
{
    if (m_socket->isConnected())
        return CYNARA_ASYNC_API_ALREADY_CONNECTED;

    onDisconnected();
    if (!m_socket->connect(sockFd))
        return CYNARA_ASYNC_API_SERVICE_NOT_AVAILABLE;
    return CYNARA_ASYNC_API_SUCCESS;
}

int Logic::check(const std::string &client, const std::string &session UNUSED,
                 const std::string &user, const std::string &privilege,
                 cynara_check_id &checkId) noexcept
{
    if (!m_socket->isConnected()) {
        onDisconnected();
        return CYNARA_ASYNC_API_SERVICE_NOT_AVAILABLE;
    }

    PolicyKey key(client, user, privilege);

    ProtocolFrameSequenceNumber sequenceNumber;
    if (!m_sequenceContainer.generateCheck(sequenceNumber)) {
        LOGW("Error generating check id. Too many pending requests.");
        return CYNARA_ASYNC_API_MAX_PENDING_CHECKS;
    }

    RequestPtr request = std::make_shared<CheckRequest>(key, sequenceNumber);
    if (!m_socket->askCynaraServer(request)) {
        onDisconnected();
        m_sequenceContainer.removeCheck(sequenceNumber);
        return CYNARA_ASYNC_API_SERVICE_NOT_AVAILABLE;
    }
    checkId = sequenceNumber;
    return CYNARA_ASYNC_API_ANSWER_NOT_READY;
}

int Logic::receive(cynara_check_id &checkId) noexcept
{
    while (true) {
        ResponsePtr response;
        if (!m_socket->getAnswerFromCynaraServer(response)) {
            onDisconnected();
            return CYNARA_ASYNC_API_SERVICE_NOT_AVAILABLE;
        }

        if (!response)
            return CYNARA_ASYNC_API_ANSWER_NOT_READY;

        checkId = response->sequenceNumber();
        CheckResponsePtr checkResponse = std::dynamic_pointer_cast<CheckResponse>(response);
        if (checkResponse)
            return receiveCheck(checkResponse);
        CancelResponsePtr cancelResponse = std::dynamic_pointer_cast<CancelResponse>(response);
        if (cancelResponse) {
            receiveCancel(cancelResponse);
        } else {
            LOGC("Critical error. Casting Response failed.");
            return CYNARA_ASYNC_API_ACCESS_DENIED;
        }
    }
}

int Logic::receiveCheck(CheckResponsePtr &response)
{
    if (!m_sequenceContainer.removeCheck(response->sequenceNumber())) {
        LOGE("No check entry received.");
        return CYNARA_ASYNC_API_ACCESS_DENIED;
    }

    LOGD("check response: policyType = %" PRIu16 ", metadata = %s",
         response->m_resultRef.policyType(),
         response->m_resultRef.metadata().c_str());

    PolicyResult result = response->m_resultRef;
    LOGD("Fetched new entry.");

    auto pluginIt = m_plugins.find(result.policyType());
    if (pluginIt == m_plugins.end()) {
        LOGE("No registered plugin for given PolicyType: %" PRIu16,
             result.policyType());
        return CYNARA_ASYNC_API_ACCESS_DENIED;
    }

    if (!pluginIt->second->toResult(result))
        return CYNARA_ASYNC_API_ACCESS_DENIED;
    return CYNARA_ASYNC_API_SUCCESS;
}

void Logic::receiveCancel(CancelResponsePtr &response)
{
    if (!m_sequenceContainer.removeCancel(response->sequenceNumber())) {
        LOGE("No cancel entry received.");
        return;
    }

    LOGD("cancel response: checkId = %" PRIu16,
         response->m_checkIdRef);
    if (!m_sequenceContainer.removeCheck(response->m_checkIdRef)) {
        LOGE("No registered check");
    }
}

int Logic::cancel(const cynara_check_id checkId UNUSED) noexcept
{
    ProtocolFrameSequenceNumber sequenceNumber;
    if (!m_sequenceContainer.generateCancel(sequenceNumber)) {
        LOGW("Error generating cancel id. Too many pending requests.");
        return CYNARA_ASYNC_API_MAX_PENDING_CHECKS;
    }

    RequestPtr request = std::make_shared<CancelRequest>(checkId, sequenceNumber);
    if (!m_socket->askCynaraServer(request)) {
        onDisconnected();
        m_sequenceContainer.removeCancel(sequenceNumber);
        return CYNARA_ASYNC_API_SERVICE_NOT_AVAILABLE;
    }
    return CYNARA_ASYNC_API_SUCCESS;
}

void Logic::onDisconnected(void)
{
    m_sequenceContainer.clear();
}

} // namespace Cynara
