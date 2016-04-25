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

#include <log/log.h>

#include "EntriesContainer.h"

bool operator<(const struct timespec &t1, const struct timespec &t2) {
    if (t1.tv_sec < t2.tv_sec)
        return true;
    if (t1.tv_sec == t2.tv_sec && t1.tv_nsec < t2.tv_nsec)
        return true;
    return false;
}

namespace Cynara {

EntriesContainer::EntryInfo EntriesContainer::peekFrom(const EntriesContainer::EntryInfo &from,
                                                       unsigned int amount,
                                                       std::vector<MonitorEntry> &entries)
{
    if (size(from) < amount) {
        LOGE("Not enough data in container, requested: " << amount << ", got " << size(from));
        return {{-1, -1}, 0};
    }

    auto fromIt = m_entries.find(from.time);
    if (fromIt == m_entries.end()) {
        // This could be handled by refering to lower bound and ignoring absence of specified time
        LOGE("No entries for specified time {" << from.time.tv_sec << ", " << from.time.tv_nsec
             << "}");
        return {{-1, -1}, 0};
    }

    std::vector<MonitorEntry> tmp;
    unsigned int offset = from.offset;

    // Iterate over timespec
    for (; fromIt != m_entries.end(); ++fromIt) {

        // Iterate over monitor entries for given timespec
        for (auto it = std::next(fromIt->second.begin(), offset);
             it != fromIt->second.end() && tmp.size() <= amount; ++it) {
            tmp.push_back(*it);
            offset++;
        }
        // This allows valid fromIt and offset values to return
        if (tmp.size() == amount) {
            break;
        }
        offset = 0;
    }

    entries = std::move(tmp);
    return {{fromIt->first.tv_sec, fromIt->first.tv_nsec}, offset};

}
void EntriesContainer::popUntil(const EntriesContainer::EntryInfo &info) {
    auto toIt = m_entries.find(info.time);
    if (toIt == m_entries.end()) {
        LOGW("No entries to pop");
        return;
    }
    auto &lastList = toIt->second;
    auto listIt = lastList.begin();
    lastList.erase(listIt, std::next(listIt, info.offset));

    if (lastList.empty()) {
        ++toIt;
    }
    m_entries.erase(m_entries.begin(), toIt);
}
size_t EntriesContainer::size(const EntriesContainer::EntryInfo &from) {
    auto fromIt = m_entries.find(from.time);

    size_t partialSize = 0;
    partialSize = fromIt->second.size() - from.offset;
    ++fromIt;
    for (auto it = fromIt; it != m_entries.end(); ++it) {
        partialSize += it->second.size();
    }

    return partialSize;
}
} /* namespace Cynara */
