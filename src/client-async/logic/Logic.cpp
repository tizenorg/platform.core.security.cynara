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
/**
 * @file        src/client-async/logic/Logic.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main
 *              libcynara-client-async class
 */

#include <memory>

#include <cache/CapacityCache.h>
#include <common.h>
#include <config/PathConfig.h>
#include <exceptions/Exception.h>
#include <exceptions/NoMemoryException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <plugins/NaiveInterpreter.h>
#include <protocol/ProtocolClient.h>
#include <request/CancelRequest.h>
#include <request/CheckRequest.h>
#include <response/CancelResponse.h>
#include <response/CheckResponse.h>
#include <sockets/Socket.h>

#include "Logic.h"

namespace Cynara {

Logic::Logic(cynara_status_callback callback, void *userStatusData)
    : m_statusCallback(callback, userStatusData), m_operationPermitted(true),
      m_inAnswerCancelResponseCallback(false) {
    m_socketClient = std::make_shared<SocketClientAsync>(
        PathConfig::SocketPath::client, std::make_shared<ProtocolClient>());

    m_cache = std::make_shared<CapacityCache>();
    auto naiveInterpreter = std::make_shared<NaiveInterpreter>();
    for (auto &type : naiveInterpreter->getSupportedPolicyDescr()) {
        m_cache->registerPlugin(type, naiveInterpreter);
    }
}

Logic::~Logic() {
    m_operationPermitted = false;
    for (auto &kv : m_checks) {
        if (!kv.second.cancelled())
            kv.second.callback().onFinish(kv.first);
    }
    m_statusCallback.onDisconnected();
}

int Logic::checkCache(const std::string &client, const std::string &session,
                      const std::string &user, const std::string &privilege) {
    if (!m_operationPermitted)
        return CYNARA_API_OPERATION_NOT_ALLOWED;

    if (!checkCacheValid())
        return CYNARA_API_CACHE_MISS;

    return m_cache->get(session, PolicyKey(client, user, privilege));
}

int Logic::createRequest(const std::string &client, const std::string &session,
                         const std::string &user, const std::string &privilege,
                         cynara_check_id &checkId, cynara_response_callback callback,
                         void *userResponseData) {
    if (!m_operationPermitted)
        return CYNARA_API_OPERATION_NOT_ALLOWED;

    if (!ensureConnection())
        return CYNARA_API_SERVICE_NOT_AVAILABLE;

    ProtocolFrameSequenceNumber sequenceNumber;
    if (!m_sequenceContainer.get(sequenceNumber))
        return CYNARA_API_MAX_PENDING_REQUESTS;

    PolicyKey key(client, user, privilege);
    ResponseCallback responseCallback(callback, userResponseData);
    m_checks.insert(CheckPair(sequenceNumber, CheckData(key, session, responseCallback)));
    m_socketClient->appendRequest(std::make_shared<CheckRequest>(key, sequenceNumber));

    onStatusChange(m_socketClient->getSockFd(), cynara_async_status::CYNARA_STATUS_FOR_RW);
    checkId = static_cast<cynara_check_id>(sequenceNumber);

    return CYNARA_API_SUCCESS;
}

int Logic::process(void) {
    if (!m_operationPermitted)
        return CYNARA_API_OPERATION_NOT_ALLOWED;

    bool completed;
    while (true) {
        int ret = completeConnection(completed);
        if (!completed)
            return ret;
        if (processOut() && processIn())
            return CYNARA_API_SUCCESS;
        onDisconnected();
        if (!connect())
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
}

int Logic::cancelRequest(cynara_check_id checkId) {
    if (!m_operationPermitted)
        return CYNARA_API_OPERATION_NOT_ALLOWED;

    if (!ensureConnection())
        return CYNARA_API_SERVICE_NOT_AVAILABLE;

    auto it = m_checks.find(checkId);
    if (it == m_checks.end() || it->second.cancelled())
        return CYNARA_API_INVALID_PARAM;

    m_socketClient->appendRequest(std::make_shared<CancelRequest>(it->first));

    it->second.cancel();

    bool onAnswerCancel = m_inAnswerCancelResponseCallback;
    m_inAnswerCancelResponseCallback = true;
    it->second.callback().onCancel(it->first);
    m_inAnswerCancelResponseCallback = onAnswerCancel;

    onStatusChange(m_socketClient->getSockFd(), cynara_async_status::CYNARA_STATUS_FOR_RW);

    return CYNARA_API_SUCCESS;
}

bool Logic::isFinishPermitted(void) {
    return m_operationPermitted && !m_inAnswerCancelResponseCallback;
}

bool Logic::checkCacheValid(void) {
    return m_socketClient->isConnected();
}

void Logic::prepareRequestsToSend(void) {
    for (auto it = m_checks.begin(); it != m_checks.end();) {
        if (it->second.cancelled()) {
            m_sequenceContainer.release(it->first);
            it = m_checks.erase(it);
        } else {
            m_socketClient->appendRequest(std::make_shared<CheckRequest>(it->second.key(), it->first));
            ++it;
        }
    }
}

bool Logic::processOut(void) {
    switch (m_socketClient->sendToCynara()) {
        case Socket::SendStatus::ALL_DATA_SENT:
            onStatusChange(m_socketClient->getSockFd(),
                           cynara_async_status::CYNARA_STATUS_FOR_READ);
        case Socket::SendStatus::PARTIAL_DATA_SENT:
            return true;
        default:
            return false;
    }
}

void Logic::processCheckResponse(CheckResponsePtr checkResponse) {
    LOGD("checkResponse: policyType = [%" PRIu16 "], metadata = <%s>",
         checkResponse->m_resultRef.policyType(),
         checkResponse->m_resultRef.metadata().c_str());

    auto it = m_checks.find(checkResponse->sequenceNumber());
    if (it == m_checks.end()) {
        LOGC("Critical error. Unknown checkResponse received: sequenceNumber = [%" PRIu16 "]",
             checkResponse->sequenceNumber());
        throw UnexpectedErrorException("Unexpected response from cynara service");
    }
    int result = m_cache->update(it->second.session(), it->second.key(),
                                 checkResponse->m_resultRef);
    CheckData checkData(std::move(it->second));
    m_sequenceContainer.release(it->first);
    m_checks.erase(it);
    if (!checkData.cancelled()) {
        bool onAnswerCancel = m_inAnswerCancelResponseCallback;
        m_inAnswerCancelResponseCallback = true;
        checkData.callback().onAnswer(
            static_cast<cynara_check_id>(checkResponse->sequenceNumber()), result);
        m_inAnswerCancelResponseCallback = onAnswerCancel;
    }
}

void Logic::processCancelResponse(CancelResponsePtr cancelResponse) {
    LOGD("cancelResponse");

    auto it = m_checks.find(cancelResponse->sequenceNumber());
    if (it == m_checks.end()) {
        LOGC("Critical error. Unknown cancelResponse received: sequenceNumber = [%" PRIu16 "]",
             cancelResponse->sequenceNumber());
        throw UnexpectedErrorException("Unexpected response from cynara service");
    }
    if (!it->second.cancelled()) {
        LOGC("Critical error. CancelRequest not sent: sequenceNumber = [%" PRIu16 "]",
             cancelResponse->sequenceNumber());
        throw UnexpectedErrorException("Unexpected response from cynara service");
    }
    m_sequenceContainer.release(it->first);
    m_checks.erase(it);
}

void Logic::processResponses(void) {
    ResponsePtr response;
    CheckResponsePtr checkResponse;
    CancelResponsePtr cancelResponse;
    while (response = m_socketClient->getResponse()) {
        checkResponse = std::dynamic_pointer_cast<CheckResponse>(response);
        if (checkResponse) {
            processCheckResponse(checkResponse);
            continue;
        }

        cancelResponse = std::dynamic_pointer_cast<CancelResponse>(response);
        if (cancelResponse) {
            processCancelResponse(cancelResponse);
            continue;
        }

        LOGC("Critical error. Casting Response to known response failed.");
        throw UnexpectedErrorException("Unexpected response from cynara service");
    }
}

bool Logic::processIn(void) {
    if (!m_socketClient->receiveFromCynara())
        return false;
    processResponses();
    return true;
}

cynara_async_status Logic::socketDataStatus(void) {
    return m_socketClient->isDataToSend() ? cynara_async_status::CYNARA_STATUS_FOR_RW
                                          : cynara_async_status::CYNARA_STATUS_FOR_READ;
}

bool Logic::ensureConnection(void) {
    if (m_socketClient->isConnected())
        return true;
    onDisconnected();

    return connect();
}

bool Logic::connect(void) {
    switch (m_socketClient->connect()) {
        case Socket::ConnectionStatus::CONNECTION_SUCCEEDED:
            prepareRequestsToSend();
            onStatusChange(m_socketClient->getSockFd(), socketDataStatus());
            return true;
        case Socket::ConnectionStatus::CONNECTION_IN_PROGRESS:
            prepareRequestsToSend();
            onStatusChange(m_socketClient->getSockFd(), cynara_async_status::CYNARA_STATUS_FOR_RW);
            return true;
        default:
            onServiceNotAvailable();
            return false;
    }
}

int Logic::completeConnection(bool &completed) {
    switch (m_socketClient->completeConnection()) {
        case Socket::ConnectionStatus::ALREADY_CONNECTED:
            completed = true;
            return CYNARA_API_SUCCESS;
        case Socket::ConnectionStatus::CONNECTION_SUCCEEDED:
            onStatusChange(m_socketClient->getSockFd(), socketDataStatus());
            completed = true;
            return CYNARA_API_SUCCESS;
        case Socket::ConnectionStatus::CONNECTION_IN_PROGRESS:
            completed = false;
            return CYNARA_API_SUCCESS;
        default:
            completed = false;
            onDisconnected();
            onServiceNotAvailable();
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
}

void Logic::onStatusChange(int sock, cynara_async_status status) {
    m_operationPermitted = false;
    m_statusCallback.onStatusChange(sock, status);
    m_operationPermitted = true;
}

void Logic::onServiceNotAvailable(void)
{
    m_operationPermitted = false;
    for (auto &kv : m_checks) {
        if (!kv.second.cancelled())
            kv.second.callback().onDisconnected(kv.first);
    }
    m_checks.clear();
    m_sequenceContainer.clear();
    m_operationPermitted = true;
}

void Logic::onDisconnected(void) {
    m_operationPermitted = false;
    m_cache->clear();
    m_statusCallback.onDisconnected();
    m_operationPermitted = true;
}

} // namespace Cynara
