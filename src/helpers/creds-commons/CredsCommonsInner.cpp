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
 * @file        src/helpers/creds-commons/CredsCommonsInner.cpp
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

#include "CredsCommonsInner.h"

// trim from start
static inline std::string &ltrim(std::string &s, const std::locale &loc) {
    s.erase(s.begin(), std::find_if(s.begin(),
                                    s.end(), [loc](char c) { return not(std::isspace(c, loc)); }));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s, const std::locale &loc) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [loc](char c) { return not(std::isspace(c, loc)); }).base(),
                         s.end());
    return s;
}

static inline std::string &toLower(std::string &s, const std::locale &loc) {
    std::transform(s.begin(), s.end(), s.begin(),[loc](char c) { return std::tolower(c, loc); });
    return s;
}

static inline std::string &normalizeString(std::string &s, const std::locale &loc) {
    return toLower(ltrim(rtrim(s, loc), loc), loc);
}

// returns true when key and value founded, false when no more lines with key and values
static inline bool getKeyAndValue(std::istream &f, std::locale &loc, std::string &key,
                                  std::string &value) {
   std::string rawLine;
    while (std::getline(f, rawLine)) {
        std::istringstream rawLineStream(rawLine);
        std::string uncommentedLine;
        if (std::getline(rawLineStream, uncommentedLine, '#')) {
            std::istringstream uncommentedLineStream(uncommentedLine);
            if (std::getline(uncommentedLineStream, key, '=')) {
               if (std::getline(uncommentedLineStream, value)) {
                  key = normalizeString(key, loc);
                  value = normalizeString(value, loc);
                  return true;
               }
           }
       }
   }
   return false;
}

static inline int interpretKayAndValue (const CredentialsMap &methodCodeMap,
                                         const char *methodName, int &method, std::string &key,
                                         std::string &value, bool &occurred) {
    if (key == std::string(methodName)) {
        if (occurred) //two entries in conf file are treated as error
            return BAD_FOUNDED_THE_KEY_AGAIN;

        auto it = methodCodeMap.find(value.c_str());
        if (it == methodCodeMap.end()) //value is not valid
            return BAD_FOUNDED_THE_KEY_FIRST_TIME_WITH_WRONG_VALUE;

        method = it->second;
        occurred = true;
        return OK_FOUNDED_THE_KEY_FIRST_TIME_WITH_CORRECT_VALUE;
    }
    return OK_FOUNDED_OTHER_KEY;
};

int getMethodFromConfigurationFile(const CredentialsMap &methodCodeMap, const char *methodName,
                        int &method) {
    std::ifstream f(std::string(DEF_CREDS_CONF_FILE));
    if (!f.is_open())
        return CYNARA_API_CONFIGURATION_ERROR;

    std::locale loc = f.getloc();
    bool occurred = false;
    std::string key, value;
    int tmpMethod;
    while (getKeyAndValue(f, loc, key, value))
        switch (interpretKayAndValue (methodCodeMap, methodName, tmpMethod, key, value,
                                      occurred)) {
        case OK_FOUNDED_OTHER_KEY:
            continue;
        case BAD_FOUNDED_THE_KEY_AGAIN:
            return CYNARA_API_CONFIGURATION_ERROR;
        case BAD_FOUNDED_THE_KEY_FIRST_TIME_WITH_WRONG_VALUE:
            return CYNARA_API_CONFIGURATION_ERROR;
        case OK_FOUNDED_THE_KEY_FIRST_TIME_WITH_CORRECT_VALUE:
            break;
        default:
            return CYNARA_API_UNKNOWN_ERROR;
        }
    if (occurred) {
        method = tmpMethod;
        return CYNARA_API_SUCCESS;
    }
    return CYNARA_API_CONFIGURATION_ERROR;
}

