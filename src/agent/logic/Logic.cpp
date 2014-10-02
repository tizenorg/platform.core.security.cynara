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
#include <request/pointers.h>
#include <response/pointers.h>
#include <types/ProtocolFields.h>

#include "Logic.h"

namespace Cynara {

const std::string agentSocketPath("/run/cynara/cynara-agent.socket");

Logic::Logic(const AgentType &agentType) : m_agentType(agentType) {
}

int Logic::getRequest(AgentActionRequestPtr &resultPtr UNUSED) noexcept {
    try {
        // TODO: implement
    } catch (const std::exception &ex) {
        LOGE("Unexpected agent error: <%s>", ex.what());
        return CYNARA_AGENT_API_UNKNOWN_ERROR;
    }

    return CYNARA_AGENT_API_SUCCESS;
}

int Logic::putResponse(const AgentRequestType requestType UNUSED,
                       const ProtocolFrameSequenceNumber sequenceNumber UNUSED,
                       const RawBuffer &pluginData UNUSED) noexcept {

    try {
        // TODO: implement
    } catch (const std::exception &ex) {
        LOGE("Unexpected agent error: <%s>", ex.what());
        return CYNARA_AGENT_API_UNKNOWN_ERROR;
    }

    return CYNARA_AGENT_API_SUCCESS;
}

} // namespace Cynara
