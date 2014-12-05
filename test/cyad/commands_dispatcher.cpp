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
 * @file        commands_dispatcher.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for CommandsDispatcher
 */

#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cynara-error.h>
#include <cynara-policy-types.h>

#include <cyad/CommandlineParser/CyadCommand.h>
#include <cyad/CommandsDispatcher.h>

#include "FakeAdminApiWrapper.h"

/**
 * @brief   Dispatcher should not touch admin API on help or error
 * @test    Scenario:
 * - Prepare some parsing results not requiring API calls
 * - Check if no API calls were made
 */
TEST(CommandsDispatcher, noApi) {
    using ::testing::_;
    using ::testing::Return;

    std::stringstream devNull;
    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(devNull, adminApi);

    Cynara::CyadCommand result;
    Cynara::HelpCyadCommand helpResult;
    Cynara::ErrorCyadCommand errorResult("Fake error");


    dispatcher.execute(result);
    dispatcher.execute(helpResult);
    dispatcher.execute(errorResult);
}

TEST(CommandsDispatcher, deleteBucket) {
    using ::testing::_;
    using ::testing::Return;
    using ::testing::StrEq;
    using ::testing::IsNull;

    std::stringstream devNull;
    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(devNull, adminApi);
    Cynara::DeleteBucketCyadCommand result("test-bucket");

    EXPECT_CALL(adminApi,
            cynara_admin_set_bucket(_, StrEq("test-bucket"), CYNARA_ADMIN_DELETE, IsNull()))
        .WillOnce(Return(CYNARA_API_SUCCESS));

    dispatcher.execute(result);
}

TEST(CommandsDispatcher, setBucket) {
    using ::testing::_;
    using ::testing::Return;
    using ::testing::StrEq;
    using ::testing::IsNull;
    using Cynara::PolicyBucketId;
    using Cynara::PolicyType;
    using Cynara::PolicyResult;

    std::stringstream devNull;
    FakeAdminApiWrapper adminApi;

    EXPECT_CALL(adminApi, cynara_admin_initialize(_)).WillOnce(Return(CYNARA_API_SUCCESS));
    EXPECT_CALL(adminApi, cynara_admin_finish(_)).WillOnce(Return(CYNARA_API_SUCCESS));

    Cynara::CommandsDispatcher dispatcher(devNull, adminApi);

    typedef std::tuple<PolicyBucketId, PolicyType, PolicyResult::PolicyMetadata> BucketData;
    typedef std::vector<BucketData> Buckets;
    const Buckets buckets = { BucketData("test-bucket-1", CYNARA_ADMIN_ALLOW, ""),
                              BucketData("test-bucket-2", CYNARA_ADMIN_DENY, ""),
                              BucketData("test-bucket-3", CYNARA_ADMIN_BUCKET, "additional-bucket"),
                              BucketData("test-bucket-2", CYNARA_ADMIN_NONE, ""),
                              BucketData("test-bucket-4", 42, "douglas-noel-adams") };

    for (const auto &bucket : buckets) {
        const auto &bucketId = std::get<0>(bucket);
        const auto &policyType = std::get<1>(bucket);
        const auto &metadata = std::get<2>(bucket);

        SCOPED_TRACE(bucketId);

        Cynara::SetBucketCyadCommand result(bucketId, policyType, metadata);

        if (metadata.empty() == false) {
            EXPECT_CALL(adminApi,
                    cynara_admin_set_bucket(_, StrEq(bucketId.c_str()), policyType,
                                            StrEq(metadata.c_str())))
                .WillOnce(Return(CYNARA_API_SUCCESS));
        } else {
            EXPECT_CALL(adminApi,
                    cynara_admin_set_bucket(_, StrEq(bucketId.c_str()), policyType, IsNull()))
                .WillOnce(Return(CYNARA_API_SUCCESS));
        }

        dispatcher.execute(result);
    }
}
