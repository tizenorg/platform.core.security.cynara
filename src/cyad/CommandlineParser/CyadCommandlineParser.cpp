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
 * @file        src/cyad/CommandlineParser/CyadCommandlineParser.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Commandline parser for Cyad
 */

#include <getopt.h>

#include "CyadCommandlineParser.h"

namespace Cynara {

namespace CyadCmdlineArgs {
    const char HELP = 'h';
    const char * const HELP_LONG = "help";
}

namespace CyadCmdlineErrors {
    const char * const UNKNOWN_ERROR = "Unknown error";
    const char * const NO_OPTION = "No option specified";
    const char * const UNKNOWN_OPTION = "Unknown option";
}

CyadCommandlineParser::CyadCommandlineParser(int argc, char * const *argv)
    : m_argc(argc), m_argv(argv) {}

CyadCommandlineParser::~CyadCommandlineParser() {}

std::shared_ptr<ParsingResult> CyadCommandlineParser::parseMain(void) {
    namespace Args = CyadCmdlineArgs;

    const struct option long_options[] = {
        { Args::HELP_LONG, no_argument, nullptr, Args::HELP },
        { nullptr, 0, nullptr, 0 }
    };

    optind = 0; // On entry to `getopt', zero means this is the first call; initialize.
    int opt;
    while ((opt = getopt_long(m_argc, m_argv, ":", long_options, nullptr)) != -1) {
        switch(opt) {
        case CyadCmdlineArgs::HELP:
            return std::make_shared<HelpParsingResult>();

        case '?': // Unknown option
            return std::make_shared<ErrorParsingResult>(CyadCmdlineErrors::UNKNOWN_OPTION);

        default:
            return std::make_shared<ErrorParsingResult>(CyadCmdlineErrors::UNKNOWN_OPTION);
        }
    }

    return std::make_shared<ErrorParsingResult>(CyadCmdlineErrors::NO_OPTION);
}

} /* namespace Cynara */
