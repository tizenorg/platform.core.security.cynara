/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        test/service/main/commandline.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Tests of CmdlineParser
 */

#include <stdlib.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <service/main/CmdlineParser.h>

#include "CynaraCommandlineTest.h"

namespace Parser = Cynara::CmdlineParser;

TEST_F(CynaraCommandlineTest, help) {
    std::string output;

    for (const auto &opt : { "-h", "--help" }) {
        cleanOutput();
        prepare_argv({ "./cynara", opt });

        SCOPED_TRACE(opt);
        const auto parsingSuccess = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(output);

        ASSERT_TRUE(parsingSuccess);
        ASSERT_EQ("Usage: ./cynara [OPTIONS]\n\n"
                  "  -V, --version                  print version of ./cynara and exit\n"
                  "  -h, --help                     print this help message and exit\n", output);
    }
}

TEST_F(CynaraCommandlineTest, version) {
    std::string output;

    for (const auto &opt : { "-V", "--version" }) {
        cleanOutput();
        prepare_argv({ "./cynara", opt });

        SCOPED_TRACE(opt);
        const auto parsingSuccess = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(output);

        ASSERT_TRUE(parsingSuccess);
        ASSERT_EQ(std::string(CYNARA_VERSION) + "\n", output);
    }
}

TEST_F(CynaraCommandlineTest, failure) {
    std::string output;

    for (const auto &badOpt : { "-b", "--badOption" }) {
        cleanOutput();
        prepare_argv({ "./cynara", badOpt });

        SCOPED_TRACE(badOpt);
        const auto parsingSuccess = Parser::handleCmdlineOptions(this->argc(), this->argv());
        getOutput(output);

        ASSERT_FALSE(parsingSuccess);
        ASSERT_TRUE(output.empty());
    }
}
