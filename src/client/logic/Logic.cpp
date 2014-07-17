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
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main libcynara-client class
 */

#include <memory>

#include <common.h>
#include <exceptions/ServerConnectionErrorException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <protocol/Protocol.h>
#include <protocol/ProtocolClient.h>
#include <request/CheckRequest.h>
#include <request/pointers.h>
#include <response/CheckResponse.h>
#include <response/pointers.h>
#include <sockets/SocketClient.h>
#include <types/PolicyKey.h>

#include <cache/CapacityCache.h>
#include <cache/NaiveInterpreter.h>
#include <cache/PolicyGetter.h>
#include "Logic.h"

namespace Cynara {

const std::string clientSocketPath("/run/cynara/cynara.socket");

Logic::Logic() {
    m_socketClient = std::make_shared<SocketClient>(clientSocketPath,
                                                    std::make_shared<ProtocolClient>());
    m_cache = std::make_shared<CapacityCache>(new PolicyGetter(m_socketClient));
    m_cache->registerPlugin(PredefinedPolicyType::ALLOW, new NaiveInterpreter());
    m_cache->registerPlugin(PredefinedPolicyType::DENY, new NaiveInterpreter());
    m_cache->registerPlugin(PredefinedPolicyType::BUCKET, new NaiveInterpreter());
}

cynara_api_result Logic::check(const std::string &client, const std::string &session UNUSED,
                               const std::string &user, const std::string &privilege) noexcept
{
    PolicyKey key(client, user, privilege);

    auto ret = m_cache->get(session, key);
    if(ret == cynara_api_result::CYNARA_API_SERVICE_NOT_AVAILABLE)
        onDisconnected();
    return ret;
}

void Logic::onDisconnected(void) {
    m_cache->clear();
}

} // namespace Cynara
