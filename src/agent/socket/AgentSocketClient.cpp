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
 * @file        src/agent/socket/AgentSocket.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of agent socket client
 */

#include <memory>
#include <string>

#include <log/log.h>
#include <protocol/Protocol.h>
#include <request/pointers.h>
#include <request/Request.h>
#include <request/RequestContext.h>
#include <response/pointers.h>
#include <sockets/Socket.h>

#include "AgentSocketClient.h"

namespace Cynara {

AgentSocketClient::AgentSocketClient(const std::string &socketPath, ProtocolPtr protocol)
        : m_socket(socketPath), m_protocol(protocol) {
}

ResponsePtr AgentSocketClient::askCynaraServer(RequestPtr request) {
    //pass request to protocol
    RequestContextPtr context = std::make_shared<RequestContext>(ResponseTakerPtr(), m_writeQueue);
    request->execute(request, m_protocol, context);

    //send request to cynara
    if (m_socket.sendToServer(m_writeQueue) == Socket::SendStatus::CONNECTION_LOST) {
        LOGW("Error sending request to Cynara. Service not available.");
        return nullptr;
    }

    // receive response from cynara
    while (true) {
        if (!m_socket.receiveFromServer(m_readQueue)) {
            LOGW("Error receiving response from Cynara. Service not available.");
            return nullptr;
        }
        ResponsePtr response = m_protocol->extractResponseFromBuffer(m_readQueue);
        if (response) {
            return response;
        }
    }
}

RequestPtr AgentSocketClient::receiveRequestFromServer(void) {
    while (true) {
        if (!m_socket.receiveFromServer(m_readQueue)) {
            LOGW("Error receiving data from Cynara. Service not available.");
            return nullptr;
        }

        RequestPtr request = m_protocol->extractRequestFromBuffer(m_readQueue);
        if (request) {
            return request;
        }
    }
}

bool AgentSocketClient::sendDataToServer(BinaryQueue &data) {
    if (m_socket.sendToServer(data) == Socket::SendStatus::CONNECTION_LOST) {
        LOGW("Error sending data to Cynara. Service not available.");
        return false;
    }

    return true;
}

bool AgentSocketClient::isConnected(void) {
    return m_socket.isConnected() && m_socket.receiveFromServer(m_readQueue);
}

AgentSocketState AgentSocketClient::connect(void) {
    if (isConnected())
        return ASS_CONNECTED;

    if (m_socket.connect() == Socket::ConnectionStatus::CONNECTION_SUCCEEDED)
        return ASS_RECONNECTED;

    return ASS_DISCONNECTED;
}

} // namespace Cynara
