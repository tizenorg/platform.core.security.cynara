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
 * @file        socket-credentials.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-helper-socket-credentials API
 */

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/smack.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <utility>

#include <common.h>

#include <log/log.h>

#include <cynara-socket-credentials-helper.h>

#define PID_METHOD 1
#define UID_METHOD 2
#define GID_METHOD 3
#define SMACK_METHOD 4

typedef struct {
       pid_t pid;
       uid_t uid;
       gid_t gid;
} peer_credentials_t;
typedef std::map<std::string, std::pair<int, unsigned int>> method_data_t;
typedef std::pair<int, unsigned int> iui_pair_t;

CYNARA_API
int  socket_credentials_get_user_id(const int socket_fd, const char *method, void *user_id, unsigned int *length)
{
    std::string real_method; //method after translation of SYSTEM_DEFAULT when needed
    method_data_t method_data; //maping containing info on method code and required buffer (user_id) size
    method_data_t::iterator it;
    socklen_t cred_len;
    unsigned int needed_size;
    int method_id_code;
    peer_credentials_t peer_cred;

    if ((method == nullptr) || (length == nullptr))
        return SOCKET_CREDENTIALS_API_INVALID_PARAM;
    method_data["PID"] = iui_pair_t(PID_METHOD, sizeof(pid_t));
    method_data["UID"] = iui_pair_t(UID_METHOD, sizeof(uid_t));
    method_data["GID"] = iui_pair_t(GID_METHOD, sizeof(gid_t));
    method_data["SMACK"] = iui_pair_t(SMACK_METHOD, SMACK_LABEL_LEN);
    if (!strcmp(method,"SYSTEM_DEFAULT"))
    {
        std::ifstream f(SOCKET_SYSTEM_DEFAULT_FILE); //autoclosed
        if (!f.is_open())
           return SOCKET_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE;
        std::stringstream buffer;
        buffer << f.rdbuf();
        real_method = buffer.str();
    }
    else
        real_method = std::string(method);
    it = method_data.find(real_method);
    if (it == method_data.end())
    {
         if (!strcmp(method,"SYSTEM_DEFAULT"))
             return SOCKET_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE;
         else
             return SOCKET_CREDENTIALS_API_INVALID_PARAM;
    }
    method_id_code = (it->second).first;
    needed_size = (it->second).second;
    /*buffor to small: return so the caller might recall with bigger one*/
    if (*length < needed_size)
    {
        *length = needed_size;
        return SOCKET_CREDENTIALS_API_LENGTH_TOO_SMALL;
    };
    *length = needed_size;
    if (user_id == nullptr)
        return SOCKET_CREDENTIALS_API_INVALID_PARAM;
    /*do the staff for smack*/
    if (method_id_code == SMACK_METHOD)
        return getsockopt(socket_fd, SOL_SOCKET, SO_PEERSEC, user_id, length)?
            SOCKET_CREDENTIALS_API_GETSOCKOPT_ERROR:SOCKET_CREDENTIALS_API_SUCCESS;
    /*do the staff for other cases*/
    cred_len = sizeof(peer_credentials_t);
    if (!getsockopt(socket_fd, SOL_SOCKET, SO_PEERCRED, &peer_cred, &cred_len))
        return SOCKET_CREDENTIALS_API_GETSOCKOPT_ERROR;
    switch (method_id_code)
    {
        case PID_METHOD:
            memcpy(user_id, &(peer_cred.pid), *length);
            return SOCKET_CREDENTIALS_API_SUCCESS;
            break;
        case UID_METHOD:
            memcpy(user_id, &(peer_cred.uid), *length);
            return SOCKET_CREDENTIALS_API_SUCCESS;
            break;
        case GID_METHOD:
            memcpy(user_id, &(peer_cred.gid), *length);
            return SOCKET_CREDENTIALS_API_SUCCESS;
    }
    return SOCKET_CREDENTIALS_API_UNEXPECTED_ERROR;
}

CYNARA_API
int socket_credentials_get_client_id(const int socket_fd, const char *method, void *client_id, unsigned int *length)
{
    std::string real_method; //method given in parameter or read from configuration file
    method_data_t method_data; //maping containing info on method code and required buffer (client_id) size
    method_data_t::iterator it;
    unsigned int needed_size;
    int method_id_code;

    if ((method == nullptr) || (length == nullptr))
        return SOCKET_CREDENTIALS_API_INVALID_PARAM;
    method_data["SMACK"] = iui_pair_t(SMACK_METHOD, SMACK_LABEL_LEN);
    if (!strcmp(method,"SYSTEM_DEFAULT"))
    {
        std::ifstream f(SOCKET_SYSTEM_DEFAULT_FILE); //autoclosed
        if (!f.is_open())
           return SOCKET_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE;
        std::stringstream buffer;
        buffer << f.rdbuf();
        real_method = buffer.str();
    }
    else
        real_method = std::string(method);
    it = method_data.find(real_method);
    if (it == method_data.end())
    {
         if (!strcmp(method,"SYSTEM_DEFAULT"))
             return SOCKET_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE;
         else
             return SOCKET_CREDENTIALS_API_INVALID_PARAM;
    }
    method_id_code = (it->second).first;
    needed_size = (it->second).second;
    /*buffor to small: return so the caller might recall with bigger one*/
    if (*length < needed_size)
    {
        *length = needed_size;
        return SOCKET_CREDENTIALS_API_LENGTH_TOO_SMALL;
    };
    *length = needed_size;
    if (client_id == nullptr)
        return SOCKET_CREDENTIALS_API_INVALID_PARAM;
    /*do the staff for smack*/
    if (method_id_code == SMACK_METHOD)
        return getsockopt(socket_fd, SOL_SOCKET, SO_PEERSEC, client_id, length)?
            SOCKET_CREDENTIALS_API_GETSOCKOPT_ERROR:SOCKET_CREDENTIALS_API_SUCCESS;
    return SOCKET_CREDENTIALS_API_UNEXPECTED_ERROR;
}
