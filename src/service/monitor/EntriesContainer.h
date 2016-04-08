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
 * @brief       This file defines class for containing monitor entries
 */

#ifndef SRC_SERVICE_MONITOR_ENTRIESCONTAINER_H_
#define SRC_SERVICE_MONITOR_ENTRIESCONTAINER_H_

#include <list>
#include <map>

#include <types/MonitorEntry.h>

namespace Cynara {

class EntriesContainer {
public:
    EntriesContainer() : m_size(0) {}
    void push(const MonitorEntry &e);
    std::vector<MonitorEntry> peekTop(struct timespec from, unsigned int offset, unsigned int amount);
    void popUntil(struct timespec to);
    size_t size(void) {
        return m_size;
    }
private:
   std::map<struct timespec, std::list<MonitorEntry>> m_entries;
   size_t m_size;
};


} /* namespace Cynara */

#endif /* SRC_SERVICE_MONITOR_ENTRIESCONTAINER_H_ */
