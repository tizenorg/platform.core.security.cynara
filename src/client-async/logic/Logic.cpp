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

#include <common.h>
#include <protocol/ProtocolClientAsync.h>

#include "Logic.h"

namespace Cynara {

const std::string clientAsyncSocketPath("/run/cynara/cynara-async.socket");

Logic::Logic()
{
    m_socket = std::make_shared<SocketClientAsync>(
        clientAsyncSocketPath, std::make_shared<ProtocolClientAsync>());
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

int Logic::check(const std::string &client UNUSED, const std::string &session UNUSED,
                 const std::string &user UNUSED, const std::string &privilege UNUSED,
                 cynara_check_id &checkId UNUSED) noexcept
{
    return CYNARA_ASYNC_API_SUCCESS;
}

int Logic::receive(cynara_check_id &checkId UNUSED) noexcept
{
    return CYNARA_ASYNC_API_SUCCESS;
}

int Logic::cancel(const cynara_check_id checkId UNUSED) noexcept
{
    return CYNARA_ASYNC_API_SUCCESS;
}

void Logic::onDisconnected(void)
{

}

} // namespace Cynara
