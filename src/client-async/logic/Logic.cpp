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
#include <socket/socketPath.h>

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
    int ret = connect();
    switch(ret)
    {
    case CYNARA_API_SUCCESS:
    case CYNARA_API_SERVICE_NOT_AVAILABLE:
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
    return CYNARA_API_MAX_PENDING_REQUESTS;
}


int Logic::process(void) noexcept
{
    return connect();
}

int Logic::cancelRequest(cynara_check_id checkId UNUSED) noexcept
{
    return connect();
}

int Logic::connect()
{
    bool connected = false;
    try {
        if (m_socket->isConnected())
            return CYNARA_API_SUCCESS;
        connected = m_socket->connect();
    } catch (Exception &ex) {
        LOGE("Unexpected error occured");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    if (connected) {
            m_statusCallback.onStatusChange(m_socket->getSockFd(),
                                            cynara_async_status::CYNARA_STATUS_FOR_READ);
            return CYNARA_API_SUCCESS;
    } else {
            onDisconnected();
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
}

void Logic::onDisconnected()
{
    m_statusCallback.onDisconnected();
}

} // namespace Cynara
