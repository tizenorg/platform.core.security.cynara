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
 * @file        src/cyad/Cyad.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       A commandline tool to manage Cynara's database
 */

#include <iostream>

#include <cyad/Cyad.h>

namespace Cynara {

Cyad::Cyad(int argc, char **argv) : m_parser(argc, argv) {
    auto result = m_parser.parseMain();
    result->run(*this);
}

Cyad::~Cyad() {}

void Cyad::execute(ParsingResult &) {
    std::cout << "Whatever you wanted, it's not implemented" << std::endl;
}

void Cyad::execute(HelpParsingResult &) {
    std::cout << "Cyad's future help message" << std::endl;
}

} /* namespace Cynara */
