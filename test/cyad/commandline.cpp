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
/*
 * @file        commandline.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Tests for CyadCommandlineParser
 */

#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cyad/CommandlineParser/CyadCommand.h>
#include <cyad/CommandlineParser/CyadCommandlineParser.h>

#include "CyadCommandlineTest.h"

TEST_F(CyadCommandlineTest, help) {
    prepare_argv({ "./cyad", "--help" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::HelpCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
}

TEST_F(CyadCommandlineTest, deleteBucket) {
    prepare_argv({ "./cyad", "--delete-bucket=bucket" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::DeleteBucketCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("bucket", result->bucketId());
}

TEST_F(CyadCommandlineTest, addBucket) {
    prepare_argv({ "./cyad", "--add-bucket=bucket", "--policy=42" });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::AddBucketCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("bucket", result->bucketId());
    ASSERT_EQ(42, result->policyType());
    ASSERT_TRUE(result->metadata().empty());
}

TEST_F(CyadCommandlineTest, addBucketWithMetadata) {
    const std::string ultimateAnswer = "Answer to The Ultimate Question of Life,"
                                       " the Universe, and Everything";

    prepare_argv({ "./cyad", "--add-bucket=adams", "--policy=42", "--metadata=" + ultimateAnswer });
    Cynara::CyadCommandlineParser parser(this->argc(), this->argv());

    auto result = std::dynamic_pointer_cast<Cynara::AddBucketCyadCommand>(parser.parseMain());
    ASSERT_NE(nullptr, result);
    ASSERT_EQ("adams", result->bucketId());
    ASSERT_EQ(42, result->policyType());
    ASSERT_EQ(ultimateAnswer, result->metadata());
}
