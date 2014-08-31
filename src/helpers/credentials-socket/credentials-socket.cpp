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
 * @file        credentials-socket.cpp
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-helper-credentials-socket API
 */

#include <sys/socket.h>
#include <sys/types.h>

#include <attributes/attributes.h>

#include <credentials-socket-inner.h>

#include <cynara-client-error.h>
#include <cynara-helper-credentials.h>
#include <cynara-helper-credentials-socket.h>

CYNARA_API
int cynara_helper_credentials_socket_get_client(int socket_fd,
                                                enum cynara_helper_credentials_client_method method,
                                                char **client) {
    if (client == nullptr)
        return CYNARA_API_INVALID_PARAM;

    switch (method) {
        case cynara_helper_credentials_client_method::CLIENT_METHOD_SMACK:
            return getSmackClient(socket_fd, client);
        case cynara_helper_credentials_client_method::CLIENT_METHOD_PID:
            return getPidClient(socket_fd, client);
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
}

CYNARA_API
int cynara_helper_credentials_socket_get_user(int socket_fd,
                                              enum cynara_helper_credentials_user_method method,
                                              char **user) {
    if (user == nullptr)
        return CYNARA_API_INVALID_PARAM;

    switch (method) {
        case cynara_helper_credentials_user_method::USER_METHOD_UID:
            return getUidUser(socket_fd, user);
        case cynara_helper_credentials_user_method::USER_METHOD_GID:
            return getGidUser(socket_fd, user);
        default:
            return CYNARA_API_METHOD_NOT_SUPPORTED;
    }
}

CYNARA_API
pid_t cynara_helper_credentials_socket_get_pid(int socket_fd) {
    struct ucred credentials;
    int ret = getCredentials(socket_fd, &credentials);
    if (ret < 0)
        return static_cast<pid_t>(ret);

    return credentials.pid;
}
