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
#include <exceptions/Exception.h>
#include <exceptions/NoMemoryException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <plugins/NaiveInterpreter.h>
#include <protocol/ProtocolClient.h>
#include <request/CheckRequest.h>
#include <response/CheckResponse.h>
#include <sockets/Socket.h>
#include <sockets/SocketPath.h>

#include "Logic.h"

namespace Cynara {

Logic::Logic(cynara_status_callback callback, void *userStatusData)
    : m_statusCallback(callback, userStatusData) {
    m_socket = std::make_shared<SocketClientAsync>(clientSocketPath,
                                                   std::make_shared<ProtocolClient>());

    m_cache = std::make_shared<CapacityCache>();
    auto naiveInterpreter = std::make_shared<NaiveInterpreter>();
    m_cache->registerPlugin(PredefinedPolicyType::ALLOW, naiveInterpreter);
    m_cache->registerPlugin(PredefinedPolicyType::DENY, naiveInterpreter);
    m_cache->registerPlugin(PredefinedPolicyType::BUCKET, naiveInterpreter);
}

Logic::~Logic() {
    for (auto &kv : m_checks)
        kv.second.callback().onFinish(kv.first);
    m_statusCallback.onDisconnected();
}

int Logic::checkCache(const std::string &client, const std::string &session,
                      const std::string &user, const std::string &privilege) noexcept {
    int ret = checkCacheValid();
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    try {
        ret = m_cache->get(session, PolicyKey(client, user, privilege));
    } catch (const std::bad_alloc &) {
        return CYNARA_API_OUT_OF_MEMORY;
    }
    return ret;
}

int Logic::createRequest(const std::string &client, const std::string &session,
                         const std::string &user, const std::string &privilege,
                         cynara_check_id &checkId, cynara_response_callback callback,
                         void *userResponseData) noexcept {
    int ret = ensureConnection();
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    ProtocolFrameSequenceNumber sequenceNumber;
    try {
        if (!m_sequenceContainer.generate(sequenceNumber))
            return CYNARA_API_MAX_PENDING_REQUESTS;

        PolicyKey key(client, user, privilege);
        ResponseCallback responseCallback(callback, userResponseData);
        m_checks.insert(CheckPair(sequenceNumber, CheckData(key, session, responseCallback)));
        m_socket->appendRequest(std::make_shared<CheckRequest>(key, sequenceNumber));
    } catch (const std::bad_alloc &) {
        return CYNARA_API_OUT_OF_MEMORY;
    }

    m_statusCallback.onStatusChange(m_socket->getSockFd(),
                                    cynara_async_status::CYNARA_STATUS_FOR_RW);
    checkId = static_cast<cynara_check_id>(sequenceNumber);

    return CYNARA_API_SUCCESS;
}

int Logic::process(void) noexcept {
    bool connectionInProgress;
    int ret = completeConnection(connectionInProgress);
    if (ret != CYNARA_API_SUCCESS || connectionInProgress) {
        return ret;
    }

    while (true) {
        ret = processOut();
        if (ret == CYNARA_API_SUCCESS) {
            ret = processIn();
            if (ret == CYNARA_API_SUCCESS)
                return ret;
        }
        if (ret == CYNARA_API_SERVICE_NOT_AVAILABLE) {
            onDisconnected();
            ret = connect(connectionInProgress);
            if (ret == CYNARA_API_SUCCESS) {
                if (connectionInProgress)
                    return CYNARA_API_SUCCESS;
                continue;
            }
        }
        return ret;
    }
}

int Logic::cancelRequest(cynara_check_id checkId UNUSED) noexcept {
    int ret = ensureConnection();
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    // MOCKUP
    return CYNARA_API_SUCCESS;
}

int Logic::checkCacheValid(void) {
    try {
        if (m_socket->isConnected())
            return CYNARA_API_SUCCESS;
        else
            return CYNARA_API_CACHE_MISS;
    } catch (const Exception &ex) {
        LOGE("Unexpected error occured");
        return CYNARA_API_UNKNOWN_ERROR;
    }
}

int Logic::prepareRequestsToSend(void) {
    try {
        for (auto &kv : m_checks) {
            // MOCKUP
            m_socket->appendRequest(std::make_shared<CheckRequest>(kv.second.key(), kv.first));
        }
    } catch (const std::bad_alloc &) {
        return CYNARA_API_OUT_OF_MEMORY;
    }
    m_statusCallback.onStatusChange(m_socket->getSockFd(), socketDataStatus());
    return CYNARA_API_SUCCESS;
}

int Logic::processOut(void) {
    Socket::Socket::SendStatus status;
    try {
        status = m_socket->sendToCynara();
    } catch (const UnexpectedErrorException &ex) {
        LOGE(ex.message().c_str());
        return CYNARA_API_UNKNOWN_ERROR;
    } catch (const NoMemoryException &ex) {
        LOGE(ex.message().c_str());
        return CYNARA_API_OUT_OF_MEMORY;
    } catch (std::bad_alloc &) {
        return CYNARA_API_OUT_OF_MEMORY;
    }
    switch (status) {
        case Socket::SendStatus::ALL_DATA_SENT:
            m_statusCallback.onStatusChange(m_socket->getSockFd(),
                                            cynara_async_status::CYNARA_STATUS_FOR_READ);
        case Socket::SendStatus::PARTIAL_DATA_SENT:
            return CYNARA_API_SUCCESS;
        default:
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
}

int Logic::processCheckResponse(CheckResponsePtr checkResponse) {
    LOGD("checkResponse: policyType = [%" PRIu16 "], metadata = <%s>",
         checkResponse->m_resultRef.policyType(),
         checkResponse->m_resultRef.metadata().c_str());

    auto it = m_checks.find(checkResponse->sequenceNumber());
    if (it == m_checks.end()) {
        LOGC("Critical error. Unknown response received: sequenceNumber = [%" PRIu16 "]",
             checkResponse->sequenceNumber());
        return CYNARA_API_UNKNOWN_ERROR;
    }
    int result;
    try {
        result = m_cache->update(it->second.session(), it->second.key(), checkResponse->m_resultRef);
    } catch (const std::bad_alloc &) {
        return CYNARA_API_OUT_OF_MEMORY;
    }
    // MOCKUP
    it->second.callback().onAnswer(static_cast<cynara_check_id>(it->first), result);
    m_sequenceContainer.remove(it->first);
    m_checks.erase(it);
    return CYNARA_API_SUCCESS;
}

int Logic::processResponses(void) {
    ResponsePtr response;
    CheckResponsePtr checkResponse;
    while (true) {
        try {
            response = m_socket->getResponse();
            if (!response)
                break;
        } catch (const std::bad_alloc &) {
            return CYNARA_API_OUT_OF_MEMORY;
        }

        checkResponse = std::dynamic_pointer_cast<CheckResponse>(response);
        if (checkResponse) {
            int ret = processCheckResponse(checkResponse);
            if (ret != CYNARA_API_SUCCESS)
                return ret;
            continue;
        }
        // MOCKUP
        LOGC("Critical error. Casting Response to CheckResponse failed.");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    return CYNARA_API_SUCCESS;
}

int Logic::processIn(void) {
    bool connected;
    try {
        connected = m_socket->receiveFromCynara();
    } catch (const UnexpectedErrorException &ex) {
        LOGE(ex.message().c_str());
        return CYNARA_API_UNKNOWN_ERROR;
    } catch (const std::bad_alloc &) {
        return CYNARA_API_OUT_OF_MEMORY;
    }
    if (!connected) {
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
    return processResponses();
}

cynara_async_status Logic::socketDataStatus(void) {
    return m_socket->isDataToSend()
        ? cynara_async_status::CYNARA_STATUS_FOR_RW
        : cynara_async_status::CYNARA_STATUS_FOR_READ;
}

int Logic::ensureConnection(void) {
    try {
        if (m_socket->isConnected())
            return CYNARA_API_SUCCESS;
    } catch (const UnexpectedErrorException &ex) {
        LOGE(ex.message().c_str());
        return CYNARA_API_UNKNOWN_ERROR;
    }
    onDisconnected();

    bool connectionInProgress;
    return connect(connectionInProgress);
}

int Logic::connect(bool &connectionInProgress) {
    Socket::ConnectionStatus status;
    try {
        status = m_socket->connect();
    } catch (const Exception &ex) {
        LOGE(ex.message().c_str());
        return CYNARA_API_UNKNOWN_ERROR;
    }
    int ret;
    switch (status) {
        case Socket::ConnectionStatus::CONNECTION_SUCCEEDED:
            ret = prepareRequestsToSend();
            if (ret != CYNARA_API_SUCCESS)
                return ret;
            m_statusCallback.onStatusChange(m_socket->getSockFd(), socketDataStatus());
            connectionInProgress = false;
            return CYNARA_API_SUCCESS;
        case Socket::ConnectionStatus::CONNECTION_IN_PROGRESS:
            ret = prepareRequestsToSend();
            if (ret != CYNARA_API_SUCCESS)
                return ret;
            m_statusCallback.onStatusChange(m_socket->getSockFd(),
                                            cynara_async_status::CYNARA_STATUS_FOR_RW);
            connectionInProgress = true;
            return CYNARA_API_SUCCESS;
        default:
            onServiceNotAvailable();
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
}

int Logic::completeConnection(bool &connectionInProgress) {
    Socket::ConnectionStatus status;
    try {
        status = m_socket->completeConnection();
    } catch (const Exception &ex) {
        LOGE("Unexpected error occured");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    switch (status) {
        case Socket::ConnectionStatus::ALREADY_CONNECTED:
            connectionInProgress = false;
            return CYNARA_API_SUCCESS;
        case Socket::ConnectionStatus::CONNECTION_SUCCEEDED:
            m_statusCallback.onStatusChange(m_socket->getSockFd(), socketDataStatus());
            connectionInProgress = false;
            return CYNARA_API_SUCCESS;
        case Socket::ConnectionStatus::CONNECTION_IN_PROGRESS:
            connectionInProgress = true;
            return CYNARA_API_SUCCESS;
        default:
            onDisconnected();
            onServiceNotAvailable();
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
}

void Logic::onServiceNotAvailable()
{
    for (auto &kv : m_checks)
        kv.second.callback().onDisconnected(kv.first);
    m_checks.clear();
    m_sequenceContainer.clear();
}

void Logic::onDisconnected() {
    m_cache->clear();
    m_statusCallback.onDisconnected();
}

} // namespace Cynara
