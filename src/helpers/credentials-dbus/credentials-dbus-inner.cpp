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
 * @file        credentials-dbus-inner.cpp
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of internal libcynara-helper-credentials-dbus functions
 */

#include <attributes/attributes.h>

#include <cynara-client-error.h>

#include "credentials-dbus-inner.h"

int getSmackClient(DBusConnection *connection UNUSED, const char *uniqueName UNUSED,
                   char **client UNUSED) {
    //todo
    return CYNARA_API_METHOD_NOT_SUPPORTED;
}

int getPidClient(DBusConnection *connection UNUSED, const char *uniqueName UNUSED,
                 char **client UNUSED) {
    //todo
    return CYNARA_API_METHOD_NOT_SUPPORTED;
}

int getUidUser(DBusConnection *connection UNUSED, const char *uniqueName UNUSED,
               char **user UNUSED) {
    //todo
    return CYNARA_API_METHOD_NOT_SUPPORTED;
}

int getGidUser(DBusConnection *connection UNUSED, const char *uniqueName UNUSED,
               char **user UNUSED) {
    //todo
    return CYNARA_API_METHOD_NOT_SUPPORTED;
}

pid_t getPid(DBusConnection *connection UNUSED, const char *uniqueName UNUSED) {
    //todo
    return CYNARA_API_METHOD_NOT_SUPPORTED;
}
