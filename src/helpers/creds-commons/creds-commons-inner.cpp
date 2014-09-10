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
 /**
 * @file        src/helpers/creds-commons/creds-commons-inner.cpp
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       Implementation of internal credential commons functions
 */

#include <algorithm>
#include <cctype>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>

#include <cynara-client-error.h>
#include <cynara-creds-commons.h>

#include "creds-commons-inner.h"

// trim from start
static inline std::string &ltrim(std::string &s, std::locale &loc) {
    s.erase(s.begin(), std::find_if(s.begin(),
                                    s.end(), [loc](char c) { return not(std::isspace(c, loc)); }));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s, std::locale &loc) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [loc](char c) { return not(std::isspace(c, loc)); }).base(),
                         s.end());
    return s;
}

int getMethodFromConfigurationFile(const CredentialsMap &methodCodeMap, const char *methodName,
                                   int &method) {
    try {
        std::ifstream f(std::string(DEF_CREDS_CONF_FILE));
        std::locale loc = f.getloc();
        if (!f.is_open())
            return CYNARA_API_CONFIGURATION_ERROR;

        std::string rawLine;
        std::string key;
        std::string value;
        bool occurred = false;

        while (std::getline(f, rawLine)) {
            std::istringstream rawLineStream(rawLine);
            std::string uncommentedLine;
            if (std::getline(rawLineStream, uncommentedLine, '#')) {
                std::istringstream uncommentedLineStream(uncommentedLine);
                if (std::getline(uncommentedLineStream, key, '=')) {
                    if (std::getline(uncommentedLineStream, value)) {
                         key = ltrim(rtrim(key, loc), loc);
                         value = ltrim(rtrim(value, loc), loc);
                         if (key == std::string(methodName)) {
                             if (occurred) //two entries in conf file are treated as error
                                 return CYNARA_API_CONFIGURATION_ERROR;

                             auto it = methodCodeMap.find(value.c_str());
                             if (it == methodCodeMap.end()) //value is not valid
                                 return CYNARA_API_CONFIGURATION_ERROR;

                             method = it->second;
                             occurred = true;
                        }
                    }
                }
            }
        }
        if (occurred)
            return CYNARA_API_SUCCESS;
        return CYNARA_API_CONFIGURATION_ERROR;

    } catch (const std::bad_alloc &ex) {
        return CYNARA_API_OUT_OF_MEMORY;
    } catch (const std::exception &ex) {
        return CYNARA_API_UNKNOWN_ERROR;
    }
}
