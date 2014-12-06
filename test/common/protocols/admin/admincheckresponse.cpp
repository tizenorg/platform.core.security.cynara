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
 * @file        test/common/protocols/admin/admincheckresponse.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::AdminCheckResponse usage in Cynara::ProtocolAdmin
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <protocol/ProtocolAdmin.h>
#include <response/AdminCheckResponse.h>
#include <types/PolicyResult.h>

#include "../RequestResponseTestHelper.h"

using namespace Cynara;

template<>
void RequestResponseComparator<AdminCheckResponse>::compare(const AdminCheckResponse &resp1,
                                                            const AdminCheckResponse &resp2) {
    EXPECT_EQ(resp1.result(), resp2.result());
    EXPECT_EQ(resp1.isBucketValid(), resp2.isBucketValid());
}

TEST(ProtocolAdmin, AdminCheckResponse01) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr1(1234, "test_meta");
    helper.testResponse(pr1, true, 123);
}

TEST(ProtocolAdmin, AdminCheckResponse02) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr2(0, "test_meta");
    helper.testResponse(pr2, false, 234);
}

TEST(ProtocolAdmin, AdminCheckResponse03) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr3(0xFFFF, "test_meta");
    helper.testResponse(pr3, true, 0);
}

TEST(ProtocolAdmin, AdminCheckResponse04) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr4(1234, "");
    helper.testResponse(pr4, false, 0xFFFF);
}

TEST(ProtocolAdmin, AdminCheckResponse05) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr5(0, "");
    helper.testResponse(pr5, true, 1);
}

TEST(ProtocolAdmin, AdminCheckResponse06) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr6(0xFFFF, "");
    helper.testResponse(pr6, false, 0xFFFE);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary01) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr1(1234, "test_meta");
    helper.binaryTestResponse(pr1, true, 123);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary02) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr2(0, "test_meta");
    helper.binaryTestResponse(pr2, false, 234);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary03) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr3(0xFFFF, "test_meta");
    helper.binaryTestResponse(pr3, true, 0);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary04) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr4(1234, "");
    helper.binaryTestResponse(pr4, false, 0xFFFF);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary05) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr5(0, "");
    helper.binaryTestResponse(pr5, true, 1);
}

TEST(ProtocolAdmin, AdminCheckResponseBinary06) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckResponse> helper;
    PolicyResult pr6(0xFFFF, "");
    helper.binaryTestResponse(pr6, false, 0xFFFE);
}
