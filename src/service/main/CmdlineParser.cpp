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
 * @file        src/service/main/CmdlineParser.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Helper namespace for Cynara's command-line options parsing
 */

#include <getopt.h>
#include <iostream>

#include "CmdlineParser.h"

namespace Cynara {

namespace CmdlineParser {

bool parseCmdlineOptions(int argc, char * const *argv) {
    bool success = true;
    const std::string shortOpts(":hV");
    const struct option longOpts[] = {
        { "help",       no_argument,    NULL, 'h' },
        { "version",    no_argument,    NULL, 'V' },
        { NULL, 0, NULL, 0 }
    };


    optind = 0; // On entry to `getopt', zero means this is the first call; initialize.
    int opt;
    while ((opt = getopt_long(argc, argv, shortOpts.c_str(), longOpts, nullptr)) != -1) {
        switch (opt) {
            case CmdlineOpt::Help:
                printHelp(std::string(argv[0]));
                return success;

            case CmdlineOpt::Version:
                printVersion();
                return success;

            case '?': // Unknown option
                return !success;

            default:
                return !success;
        }
    }

    return !success;
}

void printHelp(const std::string &execName) {
    std::cout << "Usage: " << execName << " [OPTIONS]" << std::endl << std::endl;
    std::cout << "  -V, --version                  print version of " << execName << std::endl;
    std::cout << "  -h, --help                     print help message" << std::endl;
}

void printVersion(void) {
    std::cout << std::string(CYNARA_VERSION) << std::endl;
}

} /* namespace CmdlineOpts */

} /* namespace Cynara */
