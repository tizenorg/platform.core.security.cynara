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
 * @file        test/common/protocols/TestDataCollection.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       collection of sample data for protocol tests
 */

#ifndef TEST_COMMON_PROTOCOLS_TESTDATACOLLECTION_H_
#define TEST_COMMON_PROTOCOLS_TESTDATACOLLECTION_H_

#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>
#include <types/ProtocolFields.h>

namespace {
namespace TestDataCollection {

typedef Cynara::PolicyKeyFeature PKF;

namespace Keys {
    static const Cynara::PolicyKey k_1(PKF::create(""), PKF::create("u"), PKF::create(""));
    static const Cynara::PolicyKey k_2(PKF::create("c"), PKF::create("u"), PKF::create("p"));
    static const Cynara::PolicyKey k_3(PKF::createWildcard(), PKF::createWildcard(),
                                       PKF::createWildcard());
    static const Cynara::PolicyKey k_4(PKF::createWildcard(), PKF::create("u"),
                                       PKF::createWildcard());
    static const Cynara::PolicyKey k_5(PKF::createAny(), PKF::createAny(), PKF::createAny());
    static const Cynara::PolicyKey k_6(PKF::createWildcard(), PKF::create("u"), PKF::createAny());
    static const Cynara::PolicyKey k_7(PKF::create(""), PKF::create("u"), PKF::createAny());
} /* namespace Keys */

namespace SN {
    static const Cynara::ProtocolFrameSequenceNumber min(0);
    static const Cynara::ProtocolFrameSequenceNumber min_1(1);
    static const Cynara::ProtocolFrameSequenceNumber min_2(2);
    static const Cynara::ProtocolFrameSequenceNumber max(0xFFFF);
    static const Cynara::ProtocolFrameSequenceNumber max_1(0xFFFE);
    static const Cynara::ProtocolFrameSequenceNumber max_2(0xFFFD);
    static const Cynara::ProtocolFrameSequenceNumber mid(0x0FFF);
} /* namespace SN */

namespace Buckets {
    static const Cynara::PolicyBucketId empty("");
    static const Cynara::PolicyBucketId b_1("testBucket_1");
    static const Cynara::PolicyBucketId b_2("testBucket_2");
} /* namespace Buckets */

namespace Types {
    static const Cynara::PolicyType allow(Cynara::PredefinedPolicyType::ALLOW);
    static const Cynara::PolicyType deny(Cynara::PredefinedPolicyType::DENY);
    static const Cynara::PolicyType bucket(Cynara::PredefinedPolicyType::BUCKET);
    static const Cynara::PolicyType none(Cynara::PredefinedPolicyType::NONE);
    static const Cynara::PolicyType plugin_1(100);
    static const Cynara::PolicyType plugin_2(300);
    static const Cynara::PolicyType plugin_3(500);
    static const Cynara::PolicyType plugin_4(700);
} /* namespace Types */

namespace Results {
    static const Cynara::PolicyResult allow(Types::allow, "");
    static const Cynara::PolicyResult deny(Types::deny, "");
    static const Cynara::PolicyResult bucket_1(Types::bucket, Buckets::b_1);
    static const Cynara::PolicyResult bucket_2(Types::bucket, Buckets::b_2);
    static const Cynara::PolicyResult none(Types::none, "");
    static const Cynara::PolicyResult plugin_1(Types::plugin_1, "");
    static const Cynara::PolicyResult plugin_2(Types::plugin_2, "meta_data_2");
    static const Cynara::PolicyResult plugin_3(Types::plugin_3, "");
    static const Cynara::PolicyResult plugin_4(Types::plugin_4, "meta_data_4");
} /* namespace Results */
} /* namespace TestDataCollection */
} /* namespace anonymous */

#endif /* TEST_COMMON_PROTOCOLS_TESTDATACOLLECTION_H_ */
