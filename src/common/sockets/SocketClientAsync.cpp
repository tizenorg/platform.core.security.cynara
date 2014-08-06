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
 * @file        SocketClientAsync.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of cynara's socket asynchronous
                client
 */

#include <log/log.h>
#include <request/Request.h>
#include <request/RequestContext.h>

#include "SocketClientAsync.h"

namespace Cynara {

SocketClientAsync::SocketClientAsync(const std::string &socketPath,
                                     ProtocolPtr protocol)
        : m_socket(socketPath), m_protocol(protocol)
{
}

bool SocketClientAsync::connect(int &sockFd)
{
    return m_socket.connect(sockFd);
}

bool SocketClientAsync::askCynaraServer(RequestPtr request)
{
    //pass request to protocol
    RequestContextPtr context = std::make_shared<RequestContext>(ResponseTakerPtr(), m_writeQueue);
    request->execute(request, m_protocol, context);

    //send request to cynara
    if (!m_socket.sendToServer(m_writeQueue)) {
        LOGW("Error sending request to Cynara. Service not available.");
        return false;
    }
    return true;
}

bool SocketClientAsync::getAnswerFromCynaraServer(ResponsePtr &response)
{
    if (!m_socket.receiveFromServer(m_readQueue)) {
        LOGW("Error receiving response from Cynara. Service not available.");
        return false;
    }
    response = m_protocol->extractResponseFromBuffer(m_readQueue);
    return true;
}

bool SocketClientAsync::isConnected(void)
{
    return m_socket.isConnected() && m_socket.receiveFromServer(m_readQueue);
}

} // namespace Cynara
