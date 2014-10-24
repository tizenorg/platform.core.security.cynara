/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file        src/service/agent/AgentManager.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Definition of AgentManager class
 */

#include <cstdint>

#include <attributes/attributes.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>

#include "AgentManager.h"

namespace Cynara {

AgentRegisterResponse::Code AgentManager::registerAgent(const AgentType &agentType,
                                                        BinaryQueue &writeQueue) {
    if (m_agents.find(agentType) != m_agents.end()) {
        return AgentRegisterResponse::REJECTED;
    }

    try {
        if (m_agents.insert(std::make_pair(agentType, std::ref(writeQueue))).second) {
            LOGI("Registered agent: <%s>", agentType.c_str());
            return AgentRegisterResponse::DONE;
        }
    } catch (const std::exception &ex) {
        LOGE("Exception caught while registering agent: <%s>", ex.what());
    }

    LOGE("Error in registering agent: <%s>", agentType.c_str());
    return AgentRegisterResponse::ERROR;
}

bool AgentManager::getAgentType(const AgentId agentId, AgentType &agentType) const {
    for (const auto &x : m_agents) {
        if (&x.second.get() == &agentId) {
           agentType = x.first;
           return true;
        }
    }
    return false;
}

void AgentManager::unregisterAgent(const LinkId linkId) {
    AgentType agentType;
    if (!getAgentType(linkId, agentType)) {
        LOGD("Trying to unregister not registered agent");
        return;
    }
    m_agents.erase(agentType);
    LOGI("Unregistered agent: <%s>", agentType.c_str());
}

bool AgentManager::agentExists(const AgentType &agentType) const {
    return m_agents.find(agentType) != m_agents.end();
}

AgentTalkerPtr AgentManager::createTalker(const AgentType &agentType) {
    try {
        ProtocolFrameSequenceNumber checkId = generateSequenceNumber(agentType);
        LinkId linkId = m_agents.at(agentType);
        auto mapKey = TalkerContainerKey(std::ref(linkId), checkId);

        auto talkerIt = m_talkers.find(mapKey);
        if (talkerIt != m_talkers.end())
            return talkerIt->second;

        AgentTalkerPtr talker = std::make_shared<AgentTalker>(agentType, linkId, checkId);
        if (m_talkers.insert(std::make_pair(mapKey, talker)).second) {
            LOGD("Created talker for: <%s>:[%" PRIu16 "]", agentType.c_str(), checkId);
            return talker;
        }
    } catch (std::exception &ex) {
        LOGE("Exception caught while creating agent talker: <%s>", ex.what());
    }

    return AgentTalkerPtr();
}

ProtocolFrameSequenceNumber AgentManager::generateSequenceNumber(const AgentType &agentType UNUSED)
{
    // TODO: implement smart sequence number generation, maybe unique per agent
    return m_sequenceNumber++;
}

AgentTalkerPtr AgentManager::getTalker(const LinkId linkId, ProtocolFrameSequenceNumber requestId)
                             const {
    const auto &talker = m_talkers.find(TalkerContainerKey(linkId, requestId));
    return talker != m_talkers.end() ? talker->second : AgentTalkerPtr();
}

void AgentManager::removeTalker(const AgentTalkerPtr &agentTalker) {
    for (const auto &talker : m_talkers) {
         if (talker.second == agentTalker) {
             m_talkers.erase(talker.first);
             LOGD("Talker removed: <%s>:[%" PRIu16 "]", talker.second->agentType().c_str(),
                  talker.second->checkId());
             return;
         }
     }
}

} // namespace Cynara
