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
 * @file        src/service/request/CheckRequestManager.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Declaration of CheckRequestManager class
 */

#ifndef SRC_SERVICE_AGENT_CHECKREQUESTMANAGER_H_
#define SRC_SERVICE_AGENT_CHECKREQUESTMANAGER_H_

#include <functional>
#include <map>

#include <containers/BinaryQueue.h>
#include <request/CheckContext.h>
#include <types/ProtocolFields.h>

namespace Cynara {

class CheckRequestManager {
public:
    typedef std::pair<LinkId, ProtocolFrameSequenceNumber> CheckContainerKey;
    typedef std::map<CheckContainerKey, CheckContextPtr> CheckContainer;

    CheckRequestManager() {}
    ~CheckRequestManager() {}

    CheckContextPtr createContext(const PolicyKey &key, const RequestContextPtr &request,
                                  ProtocolFrameSequenceNumber checkId,
                                  const ExternalPluginPtr &plugin,
                                  const AgentTalkerPtr &agentTalkerPtr);
    void removeRequest(const CheckContextPtr &checkContextPtr);

    CheckContextPtr getContext(const LinkId &linkId, ProtocolFrameSequenceNumber checkId);
    CheckContextPtr getContext(const AgentTalkerPtr &talker);
    const CheckContainer &getAllContexts(void) const {
        return m_checks;
    }

private:
    CheckContainer m_checks;
};

} // namespace Cynara

#endif /* SRC_SERVICE_AGENT_CHECKREQUESTMANAGER_H_ */
