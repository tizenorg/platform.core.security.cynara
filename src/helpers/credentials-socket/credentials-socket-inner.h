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
 * @file        credentials-socket-inner.h
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Definition of internal external libcynara-helper-credentials-socket functions
 */

#ifndef SRC_HELPERS_CREDENTIALSSOCKET_CREDENTIALSSOCKETINNER_H_
#define SRC_HELPERS_CREDENTIALSSOCKET_CREDENTIALSSOCKETINNER_H_

#include <sys/socket.h>

int getSmackClient(int socketFd, char **client);

int getPidClient(int socketFd, char **client);
int getUidUser(int socketFd, char **user);
int getGidUser(int socketFd, char **user);

int getCredentials(int socketFd, struct ucred *credentials);

#endif /* SRC_HELPERS_CREDENTIALSSOCKET_CREDENTIALSSOCKETINNER_H_ */
