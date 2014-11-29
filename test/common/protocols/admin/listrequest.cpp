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
 * @file        test/common/protocols/admin/listrequest.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::ListRequest usage in Cynara::ProtocolAdmin
 */

#include <memory>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <protocol/ProtocolAdmin.h>
#include <request/ListRequest.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>

#include "../RequestResponseTestHelper.h"

using namespace Cynara;

template<>
void RequestResponseComparator<ListRequest>::compare(const ListRequest *req1,
                                                     const ListRequest *req2) {
    EXPECT_EQ(req1->bucket(), req2->bucket());
    EXPECT_EQ(req1->filter(), req2->filter());
}

typedef PolicyKeyFeature PKF;

TEST(ProtocolAdmin, ListRequest01) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk0(PKF::create(""), PKF::create("u"), PKF::create(""));
    helper.testRequest(bucket, pk0, 01);
}

TEST(ProtocolAdmin, ListRequest02) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk1(PKF::create("c"), PKF::create("u"), PKF::create("p"));
    helper.testRequest(bucket, pk1, 02);
}

TEST(ProtocolAdmin, ListRequest03) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk2(PKF::createWildcard(), PKF::createWildcard(), PKF::createWildcard());
    helper.testRequest(bucket, pk2, 0);
}

TEST(ProtocolAdmin, ListRequest04) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk3(PKF::createWildcard(), PKF::create("u"), PKF::createWildcard());
    helper.testRequest(bucket, pk3, 0xFFFF);
}

TEST(ProtocolAdmin, ListRequest05) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk4(PKF::createAny(), PKF::createAny(), PKF::createAny());
    helper.testRequest(bucket, pk4, 05);
}

TEST(ProtocolAdmin, ListRequest06) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk5(PKF::createWildcard(), PKF::create("u"), PKF::createAny());
    helper.testRequest(bucket, pk5, 06);
}

TEST(ProtocolAdmin, ListRequest07) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk6(PKF::create(""), PKF::create("u"), PKF::createAny());
    helper.testRequest(bucket, pk6, 07);
}

TEST(ProtocolAdmin, ListRequestBinary01) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk0(PKF::create(""), PKF::create("u"), PKF::create(""));
    helper.binaryTestRequest(bucket, pk0, 01);
}

TEST(ProtocolAdmin, ListRequestBinary02) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk1(PKF::create("c"), PKF::create("u"), PKF::create("p"));
    helper.binaryTestRequest(bucket, pk1, 02);
}

TEST(ProtocolAdmin, ListRequestBinary03) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk2(PKF::createWildcard(), PKF::createWildcard(), PKF::createWildcard());
    helper.binaryTestRequest(bucket, pk2, 0);
}

TEST(ProtocolAdmin, ListRequestBinary04) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk3(PKF::createWildcard(), PKF::create("u"), PKF::createWildcard());
    helper.binaryTestRequest(bucket, pk3, 0xFFFF);
}

TEST(ProtocolAdmin, ListRequestBinary05) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk4(PKF::createAny(), PKF::createAny(), PKF::createAny());
    helper.binaryTestRequest(bucket, pk4, 05);
}

TEST(ProtocolAdmin, ListRequestBinary06) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk5(PKF::createWildcard(), PKF::create("u"), PKF::createAny());
    helper.binaryTestRequest(bucket, pk5, 06);
}

TEST(ProtocolAdmin, ListRequestBinary07) {
    RequestResponseTestHelper<ProtocolAdmin, ListRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk6(PKF::create(""), PKF::create("u"), PKF::createAny());
    helper.binaryTestRequest(bucket, pk6, 07);
}
