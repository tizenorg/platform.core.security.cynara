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
 * @file        session.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-helper-session API
 */

#include <errno.h>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <common.h>
#include <log/log.h>

#include <helper-session.h>




CYNARA_API
int session_helper_get_process_based_id(char* client, char *client_session, unsigned int *length)
{
    const std::string PROC_PREFIX = "/proc/";
    std::string path; // buffor for path names
    unsigned int needed_length; //minimal size of *client_session buffor (*length param)
    struct stat st;   //statistics for the proc entry
    std::string result; //to be returned as client_session
    pid_t client_pid;
    
    if ((length == nullptr) || (client == nullptr))
        return SESSION_HELPER_API_INVALID_PARAM;
    if ((client_pid = static_cast<pid_t>(strtoul(client, nullptr, 0))) == 0)
        return SESSION_HELPER_API_INVALID_PARAM;
    if ((*length > 0) && (client_session == nullptr))
        return SESSION_HELPER_API_INVALID_PARAM;
    if ((kill(client_pid, 0) == -1) && (errno == ESRCH))
        return SESSION_HELPER_API_INVALID_PARAM;
    path = PROC_PREFIX + std::string(client);
    if (stat(path.c_str(), &st) < 0)
    {
        if ((errno == EACCES) || (errno == ENOENT))
            return SESSION_HELPER_API_NO_ACCESS_WRONG_PATH;
        else if (errno == ENAMETOOLONG)
            return SESSION_HELPER_API_INVALID_PARAM;
        else if (errno == ENOMEM)
            return SESSION_HELPER_API_OUT_OF_MEMORY;
        else
            return SESSION_HELPER_API_UNEXPECTED_ERROR;
    }
    result = std::string(client) + std::to_string(st.st_ctim.tv_sec);
    needed_length = static_cast<unsigned int>(result.length()) + 1;
    if (*length < needed_length)
    {
        *length = needed_length;
        return SESSION_HELPER_API_LENGTH_TOO_SMALL;
    }
    *length = needed_length;
    if (strlen(strcpy(client_session, result.c_str())) != *length - 1)
        return SESSION_HELPER_API_UNEXPECTED_ERROR;
    else
        return SESSION_HELPER_API_SUCCESS;
}


