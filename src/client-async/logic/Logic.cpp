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

#include <memory>

#include <common.h>
#include <exceptions/Exception.h>
#include <log/log.h>
#include <protocol/ProtocolClient.h>
#include <socket/SocketPath.h>
#include <sockets/Socket.h>

#include "Logic.h"

namespace Cynara {

Logic::Logic(cynara_status_callback callback,
             void *userStatusData)
    : m_statusCallback(callback, userStatusData)
{
    m_socket = std::make_shared<SocketClientAsync>(
        clientSocketPath, std::make_shared<ProtocolClient>());
}

Logic::~Logic()
{
    onDisconnected();
}

int Logic::checkCache(const std::string &client UNUSED,
                      const std::string &session UNUSED,
                      const std::string &user UNUSED,
                      const std::string &privilege UNUSED) noexcept
{
    int ret = checkCacheValid();
    switch (ret) {
        case CYNARA_API_SUCCESS:
            // MOCKUP
            return CYNARA_API_CACHE_MISS;
        default:
            return ret;
    }
}

int Logic::createRequest(const std::string &client UNUSED,
                         const std::string &session UNUSED,
                         const std::string &user UNUSED,
                         const std::string &privilege UNUSED,
                         cynara_check_id &checkId UNUSED,
                         cynara_response_callback callback UNUSED,
                         void *userResponseData UNUSED) noexcept
{
    int ret = connect();
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    // MOCKUP
    return CYNARA_API_MAX_PENDING_REQUESTS;
}


int Logic::process(void) noexcept
{
    bool connectionInProgress;
    int ret = completeConnection(connectionInProgress);
    if (ret != CYNARA_API_SUCCESS || connectionInProgress) {
        return ret;
    }

    // MOCKUP
    return CYNARA_API_SUCCESS;
}

int Logic::cancelRequest(cynara_check_id checkId UNUSED) noexcept
{
    int ret = connect();
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    // MOCKUP
    return CYNARA_API_SUCCESS;
}

int Logic::checkCacheValid(void)
{
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

int Logic::connect(void)
{
    Socket::ConnectionStatus status;
    try {
        status = m_socket->connect();
    } catch (const Exception &ex) {
        LOGE("Unexpected error occured");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    switch (status) {
        case Socket::ConnectionStatus::ALREADY_CONNECTED:
            return CYNARA_API_SUCCESS;
        case Socket::ConnectionStatus::CONNECTION_SUCCEEDED:
            m_statusCallback.onStatusChange(m_socket->getSockFd(),
                                            cynara_async_status::CYNARA_STATUS_FOR_READ);
            return CYNARA_API_SUCCESS;
        case Socket::ConnectionStatus::CONNECTION_IN_PROGRESS:
            m_statusCallback.onStatusChange(m_socket->getSockFd(),
                                            cynara_async_status::CYNARA_STATUS_FOR_RW);
            return CYNARA_API_SUCCESS;
        default:
            onDisconnected();
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
}

int Logic::completeConnection(bool &connectionInProgress)
{
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
            m_statusCallback.onStatusChange(m_socket->getSockFd(),
                                            cynara_async_status::CYNARA_STATUS_FOR_READ);
            connectionInProgress = false;
            return CYNARA_API_SUCCESS;
        case Socket::ConnectionStatus::CONNECTION_IN_PROGRESS:
            connectionInProgress = true;
            return CYNARA_API_SUCCESS;
        default:
            onDisconnected();
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
}

void Logic::onDisconnected(void)
{
    m_statusCallback.onDisconnected();
}

} // namespace Cynara
