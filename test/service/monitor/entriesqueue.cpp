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
 * @file        test/service/monitor/entriesqueue.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests of EntriesQueue
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <ctime>

#include <service/monitor/EntriesQueue.h>
#include <common/types/MonitorEntry.h>

#include "../../helpers.h"

using namespace Cynara;

TEST(EntriesQueue, empty) {
    EntriesQueue queue;
    ASSERT_TRUE(queue.empty());
    ASSERT_EQ(0, queue.size());
    ASSERT_EQ(0, queue.sizeFrom(queue.getFrontEntryId()));
    ASSERT_EQ(0, queue.getFrontEntryId());
    ASSERT_EQ(1, queue.getBackEntryId());
}

TEST(EntriesQueue, pushOne) {
    EntriesQueue queue;
    queue.push({{"c", "u", "p"}, 0, {0, 0}});
    ASSERT_EQ(1, queue.size());
    ASSERT_EQ(1, queue.sizeFrom(queue.getFrontEntryId()));
    ASSERT_EQ(queue.getFrontEntryId(), queue.getBackEntryId() - 1);
}

TEST(EntriesQueue, fetchOne) {
    using ::testing::ElementsAre;

    EntriesQueue queue;
    MonitorEntry monitorEntry{{"c", "u", "p"}, 0, {0, 0}};
    queue.push(monitorEntry);
    std::vector<MonitorEntry> entries;
    auto nextId = queue.fetch(queue.getFrontEntryId(), 1, entries);
    ASSERT_EQ(1, nextId);
    ASSERT_THAT(entries, ElementsAre(monitorEntry));
}