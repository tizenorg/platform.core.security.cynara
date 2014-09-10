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
 * @file        src/helpers/creds-commons/CredsCommonsInner.h
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       Declaration of internal credential commons functions
 */

#ifndef SRC_HELPERS_CREDS_COMMONS_CREDSCOMMONSINNER_H_
#define SRC_HELPERS_CREDS_COMMONS_CREDSCOMMONSINNER_H_

#define OK_FOUND_OTHER_KEY 1
#define BAD_FOUND_THE_KEY_AGAIN 2
#define OK_FOUND_THE_KEY_FIRST_TIME_WITH_CORRECT_VALUE 3
#define BAD_FOUND_THE_KEY_FIRST_TIME_WITH_WRONG_VALUE 4

#include <map>
#include <string>

typedef std::map<std::string, int> CredentialsMap;

class CredsCommonsInnerBackend {
    public:
        static std::string credsConfigurationFile(void);
        virtual bool getKeyAndValue(std::istream &f, std::locale &loc, std::string &key,
                                    std::string &value);
        virtual int interpretKeyAndValue(const CredentialsMap &methodCodeMap,
                                         const std::string &methodName, int &method,
                                         const std::string &key, const std::string &value,
                                         bool &occurred);
        virtual int getMethodFromConfigurationFile(const CredentialsMap &methodCodeMap,
                                                   const std::string &methodName, int &method);
    private:
        // trim from the start
        static std::string &ltrim(std::string &s, const std::locale &loc);
        // trim from the end
        static std::string &rtrim(std::string &s, const std::locale &loc);
        static std::string &toLower(std::string &s, const std::locale &loc);
        static std::string &normalizeString(std::string &s, const std::locale &loc);
};

static CredsCommonsInnerBackend credsBackend;
#endif /* SRC_HELPERS_CREDS_COMMONS_CREDSCOMMONSINNER_H_ */
