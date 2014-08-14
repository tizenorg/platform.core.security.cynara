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
 * @file        dbus-credentials.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-helper-dbus-credentials API
 */

#include <map>
#include <new>
#include <string>
#include <vector>

#include <common.h>
#include <log/log.h>
#include <types/Policy.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

#include <dbus-credentials-helper.h>



CYNARA_API
int dbus_credentials_get_user_id(const DBusConnection *connection, const char *unique_name, void* user_id, unsigned int *length) {
  (void) connection;
  (void) unique_name;
  (void) user_id;
  (void) length;
  return 0;
}

CYNARA_API
int dbus_credentials_get_client_id(const DBusConnection *connection, const char *unique_name, void* client_id, unsigned int *length) {
  (void) connection;
  (void) unique_name;
  (void) client_id;
  (void) length;
  return 0;
}
