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
 * @file        src/service/agent/AgentManager.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Declaration of AgentManager class
 */

#ifndef SRC_SERVICE_AGENT_AGENTMANAGER_H_
#define SRC_SERVICE_AGENT_AGENTMANAGER_H_

#include <functional>
#include <map>

#include <containers/BinaryQueue.h>
#include <response/AgentRegisterResponse.h>
#include <types/Agent.h>

#include <agent/AgentTalker.h>
#include <sockets/LinkMonitor.h>

namespace Cynara {

typedef BinaryQueue& AgentId;

class AgentManager {
public:
    typedef std::pair<std::reference_wrapper<BinaryQueue>, ProtocolFrameSequenceNumber>
                TalkerContainerKey;
    struct pairCompare {
        bool operator()(const TalkerContainerKey &a, const TalkerContainerKey &b) const {
            return ((&a.first.get() < &b.first.get()) ||
                    (&a.first.get() < &b.first.get() && a.second < b.second));
        }
    };
    typedef std::map<std::pair<std::reference_wrapper<BinaryQueue>, ProtocolFrameSequenceNumber>,
            AgentTalkerPtr, pairCompare> TalkerContainer;

    AgentManager() : m_sequenceNumber(0) {}
    ~AgentManager() {}

    typedef enum {
        RA_DONE,
        RA_REJECTED,
        RA_ERROR
    } RegisterResult;

    AgentRegisterResponse::Code registerAgent(const AgentType &agentType, BinaryQueue &writeQueue);
    void unregisterAgent(const LinkId linkId);
    bool agentExists(const AgentType &agentType) const;

    AgentTalkerPtr createTalker(const AgentType &agentType);
    void removeTalker(const AgentTalkerPtr &agentTalkerPtr);
    AgentTalkerPtr getTalker(const LinkId linkId, ProtocolFrameSequenceNumber requestId) const;
    const TalkerContainer &getAllTalkers(void) const {
        return m_talkers;
    }

private:
    std::map<AgentType, std::reference_wrapper<BinaryQueue>> m_agents;
    TalkerContainer m_talkers;
    ProtocolFrameSequenceNumber m_sequenceNumber;

    ProtocolFrameSequenceNumber generateSequenceNumber(const AgentType &agentType);
    bool getAgentType(const AgentId agentId, AgentType &agentType) const;
};

} // namespace Cynara
#endif /* SRC_SERVICE_AGENT_AGENTMANAGER_H_ */
