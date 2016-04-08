/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/service/monitor/EntriesManager.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines class for managing monitor entries
 */

#ifndef SRC_SERVICE_MONITOR_ENTRIESMANAGER_H_
#define SRC_SERVICE_MONITOR_ENTRIESMANAGER_H_

#include <list>
#include <map>
#include <vector>

#include <request/RequestContext.h>
#include <types/MonitorEntry.h>

#include <monitor/EntriesContainer.h>

namespace Cynara {

class EntriesManager {
public:
    EntriesManager(uint16_t maxSize) : m_maxSize(maxSize) {}
    RequestContext::ClientId getFilledClientId(void);
    std::vector<MonitorEntry> fetchEntriesForClient(RequestContext::ClientId clientId);
    void addClient(RequestContext::ClientId clientId, uint16_t bufferSize);
    void removeClient(RequestContext::ClientId clientId);
    void updateClient(RequestContext::ClientId, uint16_t bufferSize);
    void addEntry(const MonitorEntry &e);

private:
    typedef std::map<uint16_t, std::list<RequestContext::ClientId>> BufferClientsMapping;

    uint16_t m_maxSize;
    EntriesContainer m_container;

    BufferClientsMapping m_bufferClientsMap;
    std::map<RequestContext::ClientId, BufferClientsMapping::iterator> m_clientBufferMap;
};

} /* namespace Cynara */

#endif /* SRC_SERVICE_MONITOR_ENTRIESMANAGER_H_ */
