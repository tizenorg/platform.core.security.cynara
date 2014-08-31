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
 * @file        cynara-helper-credentials-dbus.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains Cynara credentials helper APIs for dbus clients.
 */


#ifndef CYNARA_HELPER_CREDENTIAL_DBUS_H
#define CYNARA_HELPER_CREDENTIAL_DBUS_H

#include <dbus/dbus.h>
#include <sys/types.h>

#include "cynara-helper-credentials.h"

#ifdef __cplusplus
extern "C" {
#endif

int cynara_helper_credentials_dbus_get_client(DBusConnection *connection, const char *uniqueName,
                                              enum cynara_helper_credentials_client_method method,
                                              char **client);

int cynara_helper_credentials_dbus_get_user(DBusConnection *connection, const char *uniqueName,
                                            enum cynara_helper_credentials_user_method method,
                                            char **user);

pid_t cynara_helper_credentials_dbus_get_pid(DBusConnection *connection, const char *uniqueName);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_HELPER_CREDENTIAL_DBUS_H */
