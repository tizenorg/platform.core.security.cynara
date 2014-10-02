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
 * @file        src/agent/logic/Logic.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains definition of Logic class - main libcynara-agent class
 */

#ifndef SRC_AGENT_LOGIC_LOGIC_H_
#define SRC_AGENT_LOGIC_LOGIC_H_

#include <string>

#include <api/ApiInterface.h>

namespace Cynara {

typedef std::string AgentType;

class Logic : public ApiInterface {
public:
    Logic(const AgentType &agentType);
    virtual ~Logic() {};

    virtual int getRequest(AgentActionRequestPtr &actionRequestPtr) noexcept;

    virtual int putResponse(const AgentRequestType requestType,
                            const ProtocolFrameSequenceNumber sequenceNumber,
                            const RawBuffer &pluginData) noexcept;

private:
    AgentType m_agentType;
};

} // namespace Cynara

#endif /* SRC_AGENT_LOGIC_LOGIC_H_ */
