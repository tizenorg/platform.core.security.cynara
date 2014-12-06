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
 * @file        test/common/protocols/admin/admincheckrequest.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::AdminCheckRequest usage in Cynara::ProtocolAdmin
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <protocol/ProtocolAdmin.h>
#include <request/AdminCheckRequest.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>

#include "../RequestResponseTestHelper.h"

using namespace Cynara;

template<>
void RequestResponseComparator<AdminCheckRequest>::compare(const AdminCheckRequest &req1,
                                                           const AdminCheckRequest &req2) {
    EXPECT_EQ(req1.key(), req2.key());
    EXPECT_EQ(req1.startBucket(), req2.startBucket());
    EXPECT_EQ(req1.recursive(), req2.recursive());
}

typedef PolicyKeyFeature PKF;

TEST(ProtocolAdmin, AdminCheckRequest01) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk0(PKF::create(""), PKF::create("u"), PKF::create(""));
    bool recursive = true;
    helper.testRequest(pk0, bucket, recursive, 01);
}

TEST(ProtocolAdmin, AdminCheckRequest02) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk1(PKF::create("c"), PKF::create("u"), PKF::create("p"));
    bool recursive = true;
    helper.testRequest(pk1, bucket, recursive, 02);
}

TEST(ProtocolAdmin, AdminCheckRequest03) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk2(PKF::createWildcard(), PKF::createWildcard(), PKF::createWildcard());
    bool recursive = false;
    helper.testRequest(pk2, bucket, recursive, 0);
}

TEST(ProtocolAdmin, AdminCheckRequest04) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk3(PKF::createWildcard(), PKF::create("u"), PKF::createWildcard());
    bool recursive = false;
    helper.testRequest(pk3, bucket, recursive, 0xFFFF);
}

TEST(ProtocolAdmin, AdminCheckRequest05) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk4(PKF::createAny(), PKF::createAny(), PKF::createAny());
    bool recursive = false;
    helper.testRequest(pk4, bucket, recursive, 05);
}

TEST(ProtocolAdmin, AdminCheckRequest06) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk5(PKF::createWildcard(), PKF::create("u"), PKF::createAny());
    bool recursive = true;
    helper.testRequest(pk5, bucket, recursive, 06);
}

TEST(ProtocolAdmin, AdminCheckRequest07) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk6(PKF::create(""), PKF::create("u"), PKF::createAny());
    bool recursive = true;
    helper.testRequest(pk6, bucket, recursive, 07);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary01) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk0(PKF::create(""), PKF::create("u"), PKF::create(""));
    bool recursive = true;
    helper.binaryTestRequest(pk0, bucket, recursive, 01);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary02) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk1(PKF::create("c"), PKF::create("u"), PKF::create("p"));
    bool recursive = true;
    helper.binaryTestRequest(pk1, bucket, recursive, 02);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary03) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk2(PKF::createWildcard(), PKF::createWildcard(), PKF::createWildcard());
    bool recursive = false;
    helper.binaryTestRequest(pk2, bucket, recursive, 0);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary04) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk3(PKF::createWildcard(), PKF::create("u"), PKF::createWildcard());
    bool recursive = false;
    helper.binaryTestRequest(pk3, bucket, recursive, 0xFFFF);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary05) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk4(PKF::createAny(), PKF::createAny(), PKF::createAny());
    bool recursive = false;
    helper.binaryTestRequest(pk4, bucket, recursive, 05);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary06) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "";
    PolicyKey pk5(PKF::createWildcard(), PKF::create("u"), PKF::createAny());
    bool recursive = true;
    helper.binaryTestRequest(pk5, bucket, recursive, 06);
}

TEST(ProtocolAdmin, AdminCheckRequestBinary07) {
    RequestResponseTestHelper<ProtocolAdmin, AdminCheckRequest> helper;
    PolicyBucketId bucket = "test_bucket";
    PolicyKey pk6(PKF::create(""), PKF::create("u"), PKF::createAny());
    bool recursive = true;
    helper.binaryTestRequest(pk6, bucket, recursive, 07);
}
