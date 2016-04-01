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
 * @file        test/common/protocols/monitor/getentriesrequest.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::MonitorGetEntriesRequest usage in
 *              Cynara::ProtocolMonitorGet
 */

#include <vector>

#include <gtest/gtest.h>

#include <protocol/ProtocolMonitorGet.h>
#include <request/MonitorGetEntriesRequest.h>
#include <types/Policy.h>

#include <RequestTestHelper.h>

namespace {

template<>
void compare(const Cynara::MonitorGetEntriesRequest &req1,
             const Cynara::MonitorGetEntriesRequest &req2) {
    EXPECT_EQ(req1.bufferSize(), req2.isbufferSize());
}

static const uint64_t BUFF_SIZE_MIN = 0;
static const uint64_t BUFF_SIZE_1 = 1;
static const uint64_t BUFF_SIZE_MAX = std::numeric_limits<uint64_t>::max();
static const uint64_t BUFF_SIZE_HALF = (BUFF_SIZE_MAX + BUFF_SIZE_MIN) / 2;

} /* namespace anonymous */

using namespace Cynara;
using namespace ResponseTestHelper;
using namespace TestDataCollection;

/* *** compare by objects test cases *** */

/**
 * @brief   Verify if ListResponse is properly (de)serialized with buffer size set
 *          to 0
 * @test    Expected result:
 * - buffer size is 0
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesRequest01) {
    auto request = std::make_shared<MonitorGetEntriesRequest>(BUFF_SIZE_MIN, SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testRequest(request, protocol);
}

/**
 * @brief   Verify if ListResponse is properly (de)serialized with buffer size set
 *          to 1
 * @test    Expected result:
 * - buffer size is 1
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesRequest02) {
    auto request = std::make_shared<MonitorGetEntriesRequest>(BUFF_SIZE_1, SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testRequest(request, protocol);
}

/**
 * @brief   Verify if ListResponse is properly (de)serialized with buffer size set
 *          to half max value
 * @test    Expected result:
 * - buffer size is half max value
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesRequest03) {
    auto request = std::make_shared<MonitorGetEntriesRequest>(BUFF_SIZE_HALF, SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testRequest(request, protocol);
}

/**
 * @brief   Verify if ListResponse is properly (de)serialized with buffer size set
 *          to max value
 * @test    Expected result:
 * - buffer size is max value
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesRequest04) {
    auto request = std::make_shared<MonitorGetEntriesRequest>(BUFF_SIZE_MAX, SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    testRequest(request, protocol);
}

/* *** compare by serialized data test cases *** */

/**
 * @brief   Verify if ListResponse is properly (de)serialized with buffer size set
 *          to 0
 * @test    Expected result:
 * - buffer size is 0
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesRequest05) {
    auto request = std::make_shared<MonitorGetEntriesRequest>(BUFF_SIZE_MIN, SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestRequest(request, protocol);
}

/**
 * @brief   Verify if ListResponse is properly (de)serialized with buffer size set
 *          to 1
 * @test    Expected result:
 * - buffer size is 1
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesRequest06) {
    auto request = std::make_shared<MonitorGetEntriesRequest>(BUFF_SIZE_1, SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestRequest(request, protocol);
}

/**
 * @brief   Verify if ListResponse is properly (de)serialized with buffer size set
 *          to half max value
 * @test    Expected result:
 * - buffer size is half max value
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesRequest07) {
    auto request = std::make_shared<MonitorGetEntriesRequest>(BUFF_SIZE_HALF, SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestRequest(request, protocol);
}

/**
 * @brief   Verify if ListResponse is properly (de)serialized with buffer size set
 *          to max value
 * @test    Expected result:
 * - buffer size is max value
 */
TEST(ProtocolMonitorGet, MonitorGetEntriesRequest08) {
    auto request = std::make_shared<MonitorGetEntriesRequest>(BUFF_SIZE_MAX, SN::min);
    auto protocol = std::make_shared<ProtocolMonitorGet>();
    binaryTestRequest(request, protocol);
}

