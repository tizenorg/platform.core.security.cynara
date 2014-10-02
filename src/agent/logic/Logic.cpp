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
 * @file        src/agent/logic/Logic.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main libcynara-agent class
 */

#include <cinttypes>
#include <memory>

#include <common.h>
#include <log/log.h>
#include <protocol/Protocol.h>
#include <protocol/ProtocolAgent.h>
#include <request/AgentActionRequest.h>
#include <request/AgentRegisterRequest.h>
#include <request/pointers.h>
#include <response/AgentActionResponse.h>
#include <response/AgentRegisterResponse.h>
#include <response/pointers.h>
#include <sockets/SocketPath.h>
#include <types/ProtocolFields.h>

#include "Logic.h"

namespace {

Cynara::ProtocolFrameSequenceNumber generateSequenceNumber(void) {
    static Cynara::ProtocolFrameSequenceNumber sequenceNumber = 0;
    return ++sequenceNumber;
}

} // namespace anonymous

namespace Cynara {

Logic::Logic(const AgentType &agentType) : m_agentType(agentType), m_registered(false) {
    m_agentSocket = std::make_shared<AgentSocketClient>(SocketPath::agent,
                                                  std::make_shared<ProtocolAgent>());
}

int Logic::registerInCynara(void) {
    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    //Ask cynara service
    AgentRegisterResponsePtr registerResponsePtr;
    RequestPtr request = std::make_shared<AgentRegisterRequest>(m_agentType, sequenceNumber);
    ResponsePtr response = m_agentSocket->askCynaraServer(request);
    if (!response) {
        LOGW("Disconnected by cynara server.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    registerResponsePtr = std::dynamic_pointer_cast<AgentRegisterResponse>(response);
    if (!registerResponsePtr) {
        LOGC("Casting response to AgentRegisterResponse failed.");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    LOGD("registerResponse: answer code [%d]", static_cast<int>(registerResponsePtr->m_code));

    switch (registerResponsePtr->m_code) {
    case AgentRegisterResponse::DONE:
        return CYNARA_API_SUCCESS;
    case AgentRegisterResponse::REJECTED:
        LOGE("Registering agent of type <%s> has been rejected", m_agentType.c_str());
        return CYNARA_API_ACCESS_DENIED;
    default:
        LOGE("Registering agent of type <%s> has finished with unknown error", m_agentType.c_str());
        return CYNARA_API_UNKNOWN_ERROR;
    }
}

int Logic::ensureConnection(void) {
    int ret = m_agentSocket->connect();

    if (ret == ASS_DISCONNECTED) {
        LOGE("Agent socket disconnected.");
        return ret;
    }

    if (ret == ASS_RECONNECTED) {
        ret = registerInCynara();
    }

    return ret;
}

int Logic::getRequest(AgentActionRequestPtr &resultPtr) {
    int ret = ensureConnection();
    if (ret != CYNARA_API_SUCCESS)
        return ret;

    RequestPtr requestPtr = m_agentSocket->receiveRequestFromServer();
    if (!requestPtr) {
        LOGW("Disconnected by cynara server.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    AgentActionRequestPtr actionRequestPtr =
        std::dynamic_pointer_cast<AgentActionRequest>(requestPtr);
    if (!actionRequestPtr) {
        LOGC("Casting request to AgentActionRequest failed.");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    LOGD("agentActionRequest: type: [%" PRIu8 "], data length: [%zu]",
         actionRequestPtr->type(), actionRequestPtr->data().size());

    resultPtr = actionRequestPtr;
    return CYNARA_API_SUCCESS;
}

int Logic::putResponse(const AgentResponseType requestType,
                       const ProtocolFrameSequenceNumber sequenceNumber,
                       const RawBuffer &pluginData) {
    if(!m_agentSocket->isConnected()) {
        LOGE("Agent not connected to cynara service.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    AgentActionResponse response(requestType, pluginData, sequenceNumber);
    BinaryQueue buffer;
    ProtocolAgent::serializeResponseToBuffer(response, buffer);
    return m_agentSocket->sendDataToServer(buffer) ? CYNARA_API_SUCCESS :
                                                     CYNARA_API_SERVICE_NOT_AVAILABLE;
}

} // namespace Cynara
