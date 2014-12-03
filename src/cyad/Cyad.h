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
 * @file        src/cyad/Cyad.h
 * @author      Aleksander Zdyb <a.zdybsamsung.com>
 * @version     1.0
 * @brief       A commandline tool to manage Cynara's database
 */
#ifndef SRC_CYAD_CYAD_H_
#define SRC_CYAD_CYAD_H_

#include <cyad/CommandlineParser/CyadCommandlineParser.h>

namespace Cynara {

class Cyad {
public:
    Cyad(int argc, char **argv);
    virtual ~Cyad();

    void execute(ParsingResult &);
    void execute(HelpParsingResult &);
private:
    CyadCommandlineParser m_parser;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_CYAD_H_ */
