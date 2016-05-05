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
 * @file        test/service/monitor/entriesmanager.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests of EntriesManager
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ctime>

#include <service/monitor/EntriesManager.h>
#include <common/types/MonitorEntry.h>

#include "../../helpers.h"

using namespace Cynara;

TEST(EntriesManager, oneClients) {
    EntriesManager entriesManager;
    auto filledClient = entriesManager.getFilledClientId();
    ASSERT_EQ(-1, filledClient);
}

TEST(EntriesManager, oneClientNotFilled) {
    EntriesManager entriesManager;
    entriesManager.addClient(666, 3);
    auto filledClient = entriesManager.getFilledClientId();
    ASSERT_EQ(-1, filledClient);
}

TEST(EntriesManager, oneClientFilled) {
    EntriesManager entriesManager;
    entriesManager.addClient(666, 3);
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    auto filledClient = entriesManager.getFilledClientId();
    ASSERT_EQ(666, filledClient);
}

TEST(EntriesManager, oneClientModified) {
    EntriesManager entriesManager;
    entriesManager.addClient(666, 5);

    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    ASSERT_EQ(-1, entriesManager.getFilledClientId());

    entriesManager.modifyClient(666, 3);
    ASSERT_EQ(666, entriesManager.getFilledClientId());
}

TEST(EntriesManager, oneClientRemoved) {
    EntriesManager entriesManager;
    entriesManager.addClient(666, 3);

    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    entriesManager.addEntry({{"c", "u", "p"}, 0, {0, 0}});
    ASSERT_EQ(666, entriesManager.getFilledClientId());

    entriesManager.removeClient(666);
    ASSERT_EQ(-1, entriesManager.getFilledClientId());
}

TEST(EntriesManager, fetchForOne) {
    EntriesManager entriesManager;
    std::vector<MonitorEntry> expectedEntries;
    expectedEntries.push_back({{"c1", "u1", "p1"}, 0, {0, 0}});
    expectedEntries.push_back({{"c2", "u2", "p2"}, 0, {0, 0}});
    expectedEntries.push_back({{"c3", "u3", "p3"}, 0, {0, 0}});
    entriesManager.addClient(666, 3);
    entriesManager.addEntry(expectedEntries.at(0));
    entriesManager.addEntry(expectedEntries.at(1));
    entriesManager.addEntry(expectedEntries.at(2));

    auto actualEntries = entriesManager.fetchEntriesForClient(666);
    ASSERT_EQ(expectedEntries, actualEntries);
}

TEST(EntriesManager, fetchForMore) {
    using ::testing::ElementsAre;

    EntriesManager entriesManager;
    std::vector<MonitorEntry> expectedEntries;
    expectedEntries.push_back({{"c1", "u1", "p1"}, 0, {0, 0}});
    expectedEntries.push_back({{"c2", "u2", "p2"}, 0, {0, 0}});
    expectedEntries.push_back({{"c3", "u3", "p3"}, 0, {0, 0}});
    expectedEntries.push_back({{"c4", "u4", "p4"}, 0, {0, 0}});
    entriesManager.addClient(666, 3);
    entriesManager.addClient(777, 4);
    entriesManager.addEntry(expectedEntries.at(0));
    entriesManager.addEntry(expectedEntries.at(1));
    entriesManager.addEntry(expectedEntries.at(2));

    ASSERT_TRUE(entriesManager.isClientFilled(666));
    ASSERT_FALSE(entriesManager.isClientFilled(777));

    auto actualEntries666 = entriesManager.fetchEntriesForClient(666);
    ASSERT_THAT(actualEntries666, ElementsAre(expectedEntries.at(0),
                                              expectedEntries.at(1),
                                              expectedEntries.at(2)));
    ASSERT_FALSE(entriesManager.isClientFilled(666));

    entriesManager.addEntry(expectedEntries.at(3));
    ASSERT_TRUE(entriesManager.isClientFilled(777));
    auto actualEntries777 = entriesManager.fetchEntriesForClient(777);
    ASSERT_EQ(actualEntries777, expectedEntries);
    ASSERT_FALSE(entriesManager.isClientFilled(777));
}