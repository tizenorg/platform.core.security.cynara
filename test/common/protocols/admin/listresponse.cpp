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
 * @file        test/common/protocols/admin/listresponse.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::ListResponse usage in Cynara::ProtocolAdmin
 */

#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <protocol/ProtocolAdmin.h>
#include <response/ListResponse.h>
#include <types/Policy.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>

#include "../RequestResponseTestHelper.h"

using namespace Cynara;

template<>
void RequestResponseComparator<ListResponse>::compare(const ListResponse &resp1,
                                                      const ListResponse &resp2) {
    EXPECT_EQ(resp1.policies().size(), resp2.policies().size());
    for (size_t i = 0; i < resp1.policies().size() && i < resp2.policies().size(); ++i)
        EXPECT_EQ(resp1.policies()[i], resp2.policies()[i]);
    EXPECT_EQ(resp1.isBucketValid(), resp2.isBucketValid());
}

typedef PolicyKeyFeature PKF;

TEST(ProtocolAdmin, ListResponse01) {
    PolicyKey pk0(PKF::create(""), PKF::create("u"), PKF::create(""));
    PolicyKey pk1(PKF::create("c"), PKF::create("u"), PKF::create("p"));
    PolicyKey pk2(PKF::createWildcard(), PKF::createWildcard(), PKF::createWildcard());
    PolicyKey pk3(PKF::createWildcard(), PKF::create("u"), PKF::createWildcard());
    PolicyKey pk4(PKF::createAny(), PKF::createAny(), PKF::createAny());
    PolicyKey pk5(PKF::createWildcard(), PKF::create("u"), PKF::createAny());
    PolicyKey pk6(PKF::create(""), PKF::create("u"), PKF::createAny());
    PolicyResult pr0(1234, "test_meta");
    PolicyResult pr1(0, "test_meta");
    PolicyResult pr2(0xFFFF, "test_meta");
    PolicyResult pr3(1234, "");
    PolicyResult pr4(0, "");
    PolicyResult pr5(0xFFFF, "");
    std::vector<Policy> policies = {
        Policy(pk0, pr0),
        Policy(pk1, pr1),
        Policy(pk2, pr2),
        Policy(pk3, pr3),
        Policy(pk4, pr4),
        Policy(pk5, pr5),
        Policy(pk6, pr0),
    };
    RequestResponseTestHelper<ProtocolAdmin, ListResponse> helper;
    helper.testResponse(policies, true, 123);
}

TEST(ProtocolAdmin, ListResponse02) {
    std::vector<Policy> policies;
    RequestResponseTestHelper<ProtocolAdmin, ListResponse> helper;
    helper.testResponse(policies, true, 234);
}

TEST(ProtocolAdmin, ListResponse03) {
    std::vector<Policy> policies;
    RequestResponseTestHelper<ProtocolAdmin, ListResponse> helper;
    helper.testResponse(policies, false, 345);
}

TEST(ProtocolAdmin, ListResponseBinary01) {
    PolicyKey pk0(PKF::create(""), PKF::create("u"), PKF::create(""));
    PolicyKey pk1(PKF::create("c"), PKF::create("u"), PKF::create("p"));
    PolicyKey pk2(PKF::createWildcard(), PKF::createWildcard(), PKF::createWildcard());
    PolicyKey pk3(PKF::createWildcard(), PKF::create("u"), PKF::createWildcard());
    PolicyKey pk4(PKF::createAny(), PKF::createAny(), PKF::createAny());
    PolicyKey pk5(PKF::createWildcard(), PKF::create("u"), PKF::createAny());
    PolicyKey pk6(PKF::create(""), PKF::create("u"), PKF::createAny());
    PolicyResult pr0(1234, "test_meta");
    PolicyResult pr1(0, "test_meta");
    PolicyResult pr2(0xFFFF, "test_meta");
    PolicyResult pr3(1234, "");
    PolicyResult pr4(0, "");
    PolicyResult pr5(0xFFFF, "");
    std::vector<Policy> policies = {
        Policy(pk0, pr0),
        Policy(pk1, pr1),
        Policy(pk2, pr2),
        Policy(pk3, pr3),
        Policy(pk4, pr4),
        Policy(pk5, pr5),
        Policy(pk6, pr0),
    };
    RequestResponseTestHelper<ProtocolAdmin, ListResponse> helper;
    helper.binaryTestResponse(policies, true, 456);
}

TEST(ProtocolAdmin, ListResponseBinary02) {
    std::vector<Policy> policies;
    RequestResponseTestHelper<ProtocolAdmin, ListResponse> helper;
    helper.binaryTestResponse(policies, true, 567);
}

TEST(ProtocolAdmin, ListResponseBinary03) {
    std::vector<Policy> policies;
    RequestResponseTestHelper<ProtocolAdmin, ListResponse> helper;
    helper.binaryTestResponse(policies, false, 678);
}
