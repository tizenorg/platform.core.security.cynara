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

#include <log/log.h>

#include <cynara-error.h>
#include <cynara-creds-commons.h>

#include "CredsCommonsInner.h"

namespace Cynara {

std::string CredsCommonsInnerBackend::credsConfigurationFile() {
    std::string confFile("/etc/cynara/creds.conf");
#ifdef CYNARA_CONFIGURATION_DIR
    confFile = std::string(CYNARA_CONFIGURATION_DIR) + std::string("/creds.conf");
#else
    LOGW("Cynara compiled without CYNARA_CONFIGURATION_DIR flag. Using default configuration directory.");
#endif
    return confFile;
}
// parses stream with configuration till line of format <non empty key>=<non empty value>
// is found. Returns true when key and value FOUND, false when no more lines
bool CredsCommonsInnerBackend::getKeyAndValue(std::istream &f, std::locale &loc, std::string &key,
                                  std::string &value) {
    std::string rawLine;
    while (std::getline(f, rawLine)) {
        std::istringstream rawLineStream(rawLine);
        std::string uncommentedLine;
        if (std::getline(rawLineStream, uncommentedLine, '#')) {
            std::istringstream uncommentedLineStream(uncommentedLine);
            std::string tempKey;
            std::string tempValue;
            if (std::getline(uncommentedLineStream, tempKey, '=')) {
               if (std::getline(uncommentedLineStream, tempValue)) {
                  tempKey = normalizeString(tempKey, loc);
                  tempValue = normalizeString(tempValue, loc);
                  if ((tempKey.length() > 0) && (tempValue.length() > 0))
                  {
                      key = tempKey;
                      value = tempValue;
                      return true;
                  }
               }
           }
       }
   }
   return false;
}

int CredsCommonsInnerBackend::interpretKeyAndValue(const CredentialsMap &methodCodeMap,
                                                   const std::string &methodName, int &method,
                                                   const std::string &key, const std::string &value,
                                                   bool &occurred) {
    if (key == std::string(methodName)) {
        if (occurred) //two entries in conf file are treated as error
            return BAD_FOUND_THE_KEY_AGAIN;
        occurred = true;
        auto it = methodCodeMap.find(value.c_str());
        if (it == methodCodeMap.end()) //value is not valid
            return BAD_FOUND_THE_KEY_FIRST_TIME_WITH_WRONG_VALUE;

        method = it->second;
        return OK_FOUND_THE_KEY_FIRST_TIME_WITH_CORRECT_VALUE;
    }
    return OK_FOUND_OTHER_KEY;
}

int CredsCommonsInnerBackend::getMethodFromConfigurationFile(const CredentialsMap &methodCodeMap,
                                                             const std::string &methodName,
                                                             int &method) {
    std::ifstream f(credsConfigurationFile());
    if (!f.is_open())
        return CYNARA_API_CONFIGURATION_ERROR;

    std::locale loc = f.getloc();
    bool occurred = false;
    std::string key, value;
    int tmpMethod;
    while (getKeyAndValue(f, loc, key, value))
        switch (interpretKeyAndValue(methodCodeMap, methodName, tmpMethod, key, value, occurred)) {
        case OK_FOUND_OTHER_KEY:
            continue;
        case BAD_FOUND_THE_KEY_AGAIN:
            return CYNARA_API_CONFIGURATION_ERROR;
        case BAD_FOUND_THE_KEY_FIRST_TIME_WITH_WRONG_VALUE:
            return CYNARA_API_CONFIGURATION_ERROR;
        case OK_FOUND_THE_KEY_FIRST_TIME_WITH_CORRECT_VALUE:
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

// trim from the start
inline std::string &CredsCommonsInnerBackend::ltrim(std::string &s, const std::locale &loc) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [loc](char c) { return not(std::isspace(c, loc)); }));
    return s;
}

// trim from the end
inline std::string &CredsCommonsInnerBackend::rtrim(std::string &s, const std::locale &loc) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [loc](char c) { return not(std::isspace(c, loc)); }).base(),
                         s.end());
    return s;
}

inline std::string &CredsCommonsInnerBackend::toLower(std::string &s, const std::locale &loc) {
    std::transform(s.begin(), s.end(), s.begin(),[loc](char c) { return std::tolower(c, loc); });
    return s;
}

inline std::string &CredsCommonsInnerBackend::normalizeString(std::string &s,
                                                              const std::locale &loc) {
    return toLower(ltrim(rtrim(s, loc), loc), loc);
}

} // namespace Cynara
