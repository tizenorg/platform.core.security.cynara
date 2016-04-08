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
 * @brief       This file implements class for managing monitor entries
 */
#include "EntriesManager.h"

namespace Cynara {

RequestContext::ClientId EntriesManager::getFilledClientId(void) {
    size_t storedEntriesCount = m_container.size();
    // First entry has the lowest key -> lowest buffer size
    auto &lowestBufferIt = m_bufferClientsMap.begin();

    // No client which has requested buffer filled
    if (lowestBufferIt->first > storedEntriesCount) {
        return -1;
    }

    // Return first client, which requested lowest buffer
    return *(lowestBufferIt->second.begin());
}
std::vector<MonitorEntry> EntriesManager::fetchEntriesForClient(RequestContext::ClientId clientId) {
    // Find client iterator to buffer map
    auto clientBufferIt = m_clientBufferMap.find(clientId);
    if (clientBufferIt == m_clientBufferMap.end())
        return {};

    // Reference to buffer -> clients mapping to get requested buffer size
    auto bufferClientMapIt = clientBufferIt->second;
    uint16_t bufferSize = bufferClientMapIt->first;
    if (bufferSize > m_container.size())
        return {};
    auto entries =  m_container.peekTop(bufferSize);

    removeClient(clientId);
}
void EntriesManager::addClient(RequestContext::ClientId clientId, uint16_t bufferSize) {
    auto insertInfo = m_bufferClientsMap.insert(std::make_pair(bufferSize, clientId));
    auto bufferMapIt = insertInfo.first;

    m_clientBufferMap[clientId] = bufferMapIt;
}

void EntriesManager::removeClient(RequestContext::ClientId clientId) {
    auto clientBufferIt = m_clientBufferMap.find(clientId);
    if (clientBufferIt == m_clientBufferMap.end())
        return;

    auto bufferClientIt = clientBufferIt->second;
    auto bufferClientList = bufferClientIt->second;
    auto clientToRemoveIt = std::find(bufferClientList.begin(), bufferClientList.end(), clientId);

    bufferClientList.erase(clientToRemoveIt);
    // If it was the last client waiting for given buffer, delete the entry
    if (bufferClientList.empty())
        m_bufferClientsMap.erase(bufferClientIt);
    m_clientBufferMap.erase(clientBufferIt);
}

void EntriesManager::updateClient(RequestContext::ClientId clientId, uint16_t bufferSize) {
    removeClient(clientId);
    addClient(clientId, bufferSize);
}
void EntriesManager::addEntry(const MonitorEntry &e) {
    m_container.push(e);
}


} /* namespace Cynara */
