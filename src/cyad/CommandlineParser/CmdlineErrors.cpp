/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/cyad/CommandlineParser/CommandlineErrors.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Command-line error messages
 */

#include <cyad/CommandlineParser/CmdlineOpts.h>
#include "CmdlineErrors.h"

namespace Cynara {

namespace CmdlineErrors {

std::string unknownError() {
    return "Unknown error";
};

std::string noOption() {
    return "No option specified";
};

std::string unknownOption() {
    return "Unknown option";
};

std::string unknownOption(CmdlineOpts::CmdlineOpt option) {
    const auto &opt = CmdlineOpts::commandlineOptions.at(option);
    return std::string("Unknown option in --")
            + opt.longOption
            + " (-" + opt.shortOption + ")";
};

std::string noType() {
    return "No --policy specified";
};

std::string noBucket() {
    return "No bucket specified";
};

std::string invalidType() {
    return "Invalid --policy option";
};

std::string optionMissing(CmdlineOpts::CmdlineOpt option) {
    const auto &opt = CmdlineOpts::commandlineOptions.at(option);
    return std::string("One or more option missing in --")
            + opt.longOption
            + " (-" + opt.shortOption + ")";
};

std::string argumentMissing(CmdlineOpts::CmdlineOpt option) {
    const auto &opt = CmdlineOpts::commandlineOptions.at(option);
    return std::string("One or more argument missing in ")
            + opt.longOption
            + " (-" + opt.shortOption + ")";
};

} /* namespace CmdlineErrors */

} /* namespace Cynara */
