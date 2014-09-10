/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file        creds-commons-inner.cpp
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       Implementation of internal credential commons functions
 */
#include <algorithm>
#include <cctype>
#include <climits>
#include <functional>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <sstream>
#include <string>

#include <cynara-client-error.h>
#include <cynara-creds-commons.h>

#include "creds-commons-inner.h"


int my_isspace(int c) {
    // assert that c is in the range of unsigned char [0, UCHAR_MAX]
    if (!(c <= int(UCHAR_MAX) && c >= 0))
       return 0;
    // call isspace normally
    return ::isspace(c);
}

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(),
                std::find_if(s.begin(),
                             s.end(),
                             std::not1(std::ptr_fun<int, int>(my_isspace))));                   
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(),
                             s.rend(),
                             std::not1(std::ptr_fun<int, int>(my_isspace))).base(),
                s.end());
        return s;
}

int creadsReadFile(const std::string &path, CredentialsMap &methodCodeMap, int &method) {

    try {
        std::ifstream f(path);
        if (!f.is_open())
            return CYNARA_API_NO_SUCH_FILE;
        
        std::stringstream buffer;
        buffer << f.rdbuf();
        auto methodName = buffer.str();
        methodName = ltrim(rtrim(methodName));
        auto it = methodCodeMap.find(methodName.c_str());
        if (it == methodCodeMap.end())
            return CYNARA_API_ERROR_IN_CONFIGURATION_FILE;
        
        method = it->second;
    } catch (const std::bad_alloc &ex) {
        return CYNARA_API_OUT_OF_MEMORY;
    } catch (const std::exception &ex) {
        return CYNARA_API_UNKNOWN_ERROR;
    }
    return CYNARA_API_SUCCESS;
}

