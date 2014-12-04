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

#include <cynara-policy-types.h>

#include <cyad/AdminApiWrapper.h>
#include <cyad/CommandlineParser/ParsingResult.h>
#include <cyad/CommandsDispatcher.h>

#include "FakeAdminApiWrapper.h"

/**
 * @brief   Dispatcher should not touch admin API on help or error
 * @test    Scenario:
 * - Prepare some parsing results not requiring API calls
 * - Check if no API calls were made
 */
TEST(CommandsDispatcher, noApi) {
    std::stringstream devNull;
    FakeAdminApiWrapper adminApi;

    Cynara::CommandsDispatcher dispatcher(devNull, adminApi);

    Cynara::ParsingResult result;
    Cynara::HelpParsingResult helpResult;
    Cynara::ErrorParsingResult errorResult("Fake error");

    dispatcher.execute(result);
    dispatcher.execute(helpResult);
    dispatcher.execute(errorResult);
}
