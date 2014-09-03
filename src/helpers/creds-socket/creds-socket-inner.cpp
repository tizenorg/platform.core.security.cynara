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
 * @file        creds-socket-inner.cpp
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of internal libcynara-creds-socket functions
 */


#include <errno.h>
#include <string>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <cynara-client-error.h>

#include "creds-socket-inner.h"

int getClientSmackLabel(int socketFd, char **client) {
    char dummy;
    int ret;
    socklen_t length = 1;
    char *result;

    ret = getsockopt(socketFd, SOL_SOCKET, SO_PEERSEC, &dummy, &length);
    if ((ret < 0) && (errno != ERANGE))
            return CYNARA_API_INVALID_PARAM;

    result = static_cast<char*>(calloc(length + 1, sizeof(char)));
    if (result == nullptr)
            return CYNARA_API_OUT_OF_MEMORY;

    ret = getsockopt(socketFd, SOL_SOCKET, SO_PEERSEC, result, &length);
    if (ret < 0) {
            free(result);
            return CYNARA_API_INVALID_PARAM;
    }

    *client = result;
    return CYNARA_API_SUCCESS;
}

int getClientPid(int socketFd, char **client) {
    struct ucred credentials;
    int ret = getCredentials(socketFd, &credentials);
    if (ret < 0)
        return ret;

    *client = strdup(std::to_string(credentials.pid).c_str());
    if (*client == nullptr)
            return CYNARA_API_OUT_OF_MEMORY;

    return CYNARA_API_SUCCESS;
}

int getUserId(int socketFd, char **user) {
    struct ucred credentials;
    int ret = getCredentials(socketFd, &credentials);
    if (ret < 0)
        return ret;

    *user = strdup(std::to_string(credentials.uid).c_str());
    if (*user == nullptr)
            return CYNARA_API_OUT_OF_MEMORY;

    return CYNARA_API_SUCCESS;
}

int getUserGid(int socketFd, char **user) {
    struct ucred credentials;
    int ret = getCredentials(socketFd, &credentials);
    if (ret < 0)
        return ret;

    *user = strdup(std::to_string(credentials.gid).c_str());
    if (*user == nullptr)
            return CYNARA_API_OUT_OF_MEMORY;

    return CYNARA_API_SUCCESS;
}

int getCredentials(int socketFd, struct ucred *credentials) {
    if (credentials == nullptr)
        return CYNARA_API_UNKNOWN_ERROR;

    int ret;
    socklen_t length = sizeof(struct ucred);
    ret = getsockopt(socketFd, SOL_SOCKET, SO_PEERCRED, credentials, &length);
    if (ret < 0) {
        switch (errno) {
            case EBADF:
            case ENOTSOCK:
                return CYNARA_API_INVALID_PARAM;
            default:
                return CYNARA_API_UNKNOWN_ERROR;
        }
    }
    return CYNARA_API_SUCCESS;
}

pid_t getPid(int socketFd) {
    struct ucred credentials;
    int ret = getCredentials(socketFd, &credentials);
    if (ret < 0)
        return static_cast<pid_t>(ret);

    return credentials.pid;
}
