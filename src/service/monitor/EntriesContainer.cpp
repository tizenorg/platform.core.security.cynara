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
 * @file        src/service/monitor/EntriesContainer.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file implements class for containing monitor entries
 */

#include "EntriesContainer.h"

bool operator<(const struct timespec &t1, const struct timespec &t2) {
    if (t1.tv_sec < t2.tv_sec)
        return true;
    if (t1.tv_sec == t2.tv_sec && t1.tv_nsec < t2.tv_nsec)
        return true;
    return false;
}

namespace Cynara {

void EntriesContainer::push(const MonitorEntry &e) {
    m_entries[e.timestamp()].push_back(e);
    m_size++;
}

std::vector<MonitorEntry> EntriesContainer::peekTop(unsigned int amount) {
    if (amount > m_size) {
        // TODO: tttttthrooow
        return {};
    }
    std::vector<MonitorEntry> entries;
    entries.reserve(amount);
    unsigned int fetched = 0;

    const auto &storedEntriesIt = m_entries.begin();
    auto listIt = storedEntriesIt->second.begin();

    while (fetched < amount) {
        if (listIt == storedEntriesIt->second.end()) {
            storedEntriesIt++;
            if (storedEntriesIt == m_entries.end()) {
                break;
            }
            listIt = storedEntriesIt->second.begin();
            continue;
        }
        entries.push_back(*listIt);
        listIt++;
        fetched++;
    }

    return entries;
}

void EntriesContainer::popTop(unsigned int amount) {
    if (amount > m_size) {
        // TODO : ttttthroooow
        return;
    }
    const auto &storedEntriesIt = m_entries.begin();
    unsigned int count = 0;

    while (count < amount) {
        int tmp_count = count + storedEntriesIt->second.size();
        if (tmp_count > amount) {
            auto &listToCrop = storedEntriesIt->second;
            listToCrop.erase(listToCrop.begin(), listToCrop.begin() + amount - count);
            break;
        }
        count = tmp_count;
        storedEntriesIt++;
    }
    m_size -= amount;
    m_entries.erase(m_entries.begin(), storedEntriesIt);
}

} /* namespace Cynara */
