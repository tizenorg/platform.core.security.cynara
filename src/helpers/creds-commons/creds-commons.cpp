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
 * @file        creds-commons.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-creds-commons API
 */
#include <algorithm>
#include <cctype>
#include <functional>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <string>
#include <sstream>

#include <attributes/attributes.h>

#include <cynara-client-error.h>
#include <cynara-creds-commons.h>

#define USER_CREDS 1
#define CLIENT_CREDS 2

typedef std::map<std::string, int> creds_map_t;

static creds_map_t client_creds_map = {{"SMACK", CLIENT_METHOD_SMACK}, {"PID", CLIENT_METHOD_PID}};
static creds_map_t user_creds_map = {{"UID", USER_METHOD_UID}, {"GID", USER_METHOD_GID}};

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

static int creads_read_file(const char* const path, creds_map_t* method_code_map, int *method) {
    creds_map_t::iterator it;
    std::string method_name;

    std::ifstream f(path); //autoclosed
    if (!f.is_open())
        return CYNARA_API_NO_SUCH_FILE;
    std::stringstream buffer;
    buffer << f.rdbuf();
    method_name = buffer.str();
    method_name = ltrim(rtrim(method_name));
    it = method_code_map->find(method_name.c_str());
    if (it == method_code_map->end())
        return CYNARA_API_ERROR_IN_CONFIGURATION_FILE;
    *method = it->second;
    return CYNARA_API_SUCCESS;
}

CYNARA_API
int cynara_creds_get_default_client_method(enum cynara_client_creds *method) {
    int m, r;
    if ((r=creads_read_file(CLIENT_DEF_CREDS_CONF_FILE, &client_creds_map, &m)) != CYNARA_API_SUCCESS)
        return r;
    *method = static_cast<enum cynara_client_creds>(m);
    return CYNARA_API_SUCCESS;
}

CYNARA_API
int cynara_creds_get_default_user_method(enum cynara_user_creds *method) {
    int m, r;
    if ((r=creads_read_file(USER_DEF_CREDS_CONF_FILE, &user_creds_map, &m)) != CYNARA_API_SUCCESS)
        return r;
    *method = static_cast<enum cynara_user_creds>(m);
    return CYNARA_API_SUCCESS;
}
