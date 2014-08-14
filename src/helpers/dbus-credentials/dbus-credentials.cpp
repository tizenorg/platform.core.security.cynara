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

#include <dbus/dbus-glib.h>
#include <glib/gtypes.h>
//#include <glib/gerror.h>
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

#include <cynara-dbus-credentials-helper.h>

#define PID_METHOD 1
#define UID_METHOD 2
#define GID_METHOD 3
#define SMACK_METHOD 4

typedef std::map<std::string, std::pair<int, unsigned int>> method_data_t;
typedef std::pair<int, unsigned int> iui_pair_t;

CYNARA_API
int dbus_credentials_get_user_id(DBusGConnection *connection, const char *unique_name, const char *method, void *user_id, unsigned int *length) {
    DBusGProxy *proxy;
    GError *error = NULL;
    std::string real_method; //method given in parameter or read from configuration file
    method_data_t method_data; //maping containing info on method code and required buffer (user_id) size
    method_data_t::iterator it;
    unsigned int needed_size;
    int method_id_code;

    proxy = dbus_g_proxy_new_for_name(connection,
           "org.freedesktop.DBus",
           "/org/freedesktop/DBus",
           "org.freedesktop.DBus");
    if ((connection == nullptr) || (unique_name == nullptr) || (method == nullptr))
        return DBUS_CREDENTIALS_API_INVALID_PARAM;
    method_data["PID"] = iui_pair_t(PID_METHOD, sizeof(pid_t));
    method_data["UID"] = iui_pair_t(UID_METHOD, sizeof(uid_t));
    //method_data["GID"] = iui_pair_t(GID_METHOD, sizeof(gid_t));
    method_data["SMACK"] = iui_pair_t(SMACK_METHOD, SMACK_LABEL_LEN);
    if (!strcmp(method,"SYSTEM_DEFAULT"))
    {
        std::ifstream f(DBUS_SYSTEM_DEFAULT_FILE); //autoclosed
        if (!f.is_open())
           return DBUS_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE;
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
             return DBUS_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE;
         else
             return DBUS_CREDENTIALS_API_INVALID_PARAM;
    }
    method_id_code = (it->second).first;
    needed_size = (it->second).second;
    /*buffor to small: return so the caller might recall with bigger one*/
    if (*length < needed_size)
    {
        *length = needed_size;
        return DBUS_CREDENTIALS_API_LENGTH_TOO_SMALL;
    };
    *length = needed_size;
    if (user_id == nullptr)
        return DBUS_CREDENTIALS_API_INVALID_PARAM;
    switch (method_id_code)
    {
        case PID_METHOD:
             return dbus_g_proxy_call(proxy, "GetConnectionUnixProcessID",
                &error,G_TYPE_STRING, unique_name, G_TYPE_INVALID,
                G_TYPE_UINT, &user_id, G_TYPE_INVALID)?
                DBUS_CREDENTIALS_API_SUCCESS:DBUS_CREDENTIALS_API_DBUS_OPERATION_FAILED;
        case UID_METHOD:
            return dbus_g_proxy_call(proxy, "GetConnectionUnixUser",
                &error,G_TYPE_STRING, unique_name, G_TYPE_INVALID,
                G_TYPE_UINT, &user_id, G_TYPE_INVALID)?
                DBUS_CREDENTIALS_API_SUCCESS:DBUS_CREDENTIALS_API_DBUS_OPERATION_FAILED;
       /* case GID_METHOD:
            return dbus_g_proxy_call(proxy, "GetConnectionUnixGroupID",
                &error,DG_TYPE_STRING, unique_name, G_TYPE_INVALID,
                G_TYPE_UINT, &user_id, G_TYPE_INVALID)?
                SOCKET_CREDENTIALS_API_SUCCESS:DBUS_CREDENTIALS_API_DBUS_OPERATION_FAILED; */
        case SMACK_METHOD:
            return dbus_g_proxy_call(proxy, "GetConnectionSmackContext",
                &error,G_TYPE_STRING, unique_name, G_TYPE_INVALID,
                G_TYPE_UINT, &user_id, G_TYPE_INVALID)?
                DBUS_CREDENTIALS_API_SUCCESS:DBUS_CREDENTIALS_API_DBUS_OPERATION_FAILED;
    }
    return DBUS_CREDENTIALS_API_UNEXPECTED_ERROR;
}

CYNARA_API
int dbus_credentials_get_client_id(DBusGConnection *connection, const char *unique_name, const char *method, void *client_id, unsigned int *length) {
  (void) connection;
  (void) unique_name;
  (void) client_id;
  (void) length;
  (void) method;
   DBusGProxy *proxy;
    GError *error = NULL;
    std::string real_method; //method given in parameter or read from configuration file
    method_data_t method_data; //maping containing info on method code and required buffer (client_id) size
    method_data_t::iterator it;
    unsigned int needed_size;
    int method_id_code;

    proxy = dbus_g_proxy_new_for_name(connection,
           "org.freedesktop.DBus",
           "/org/freedesktop/DBus",
           "org.freedesktop.DBus");
    if ((connection == nullptr) || (unique_name == nullptr) || (method == nullptr))
        return DBUS_CREDENTIALS_API_INVALID_PARAM;
    //method_data["PID"] = iui_pair_t(PID_METHOD, sizeof(pid_t));
    //method_data["UID"] = iui_pair_t(UID_METHOD, sizeof(uid_t));
    //method_data["GID"] = iui_pair_t(GID_METHOD, sizeof(gid_t));
    method_data["SMACK"] = iui_pair_t(SMACK_METHOD, SMACK_LABEL_LEN);
    if (!strcmp(method,"SYSTEM_DEFAULT"))
    {
        std::ifstream f(DBUS_SYSTEM_DEFAULT_FILE); //autoclosed
        if (!f.is_open())
           return DBUS_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE;
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
             return DBUS_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE;
         else
             return DBUS_CREDENTIALS_API_INVALID_PARAM;
    }
    method_id_code = (it->second).first;
    needed_size = (it->second).second;
    /*buffor to small: return so the caller might recall with bigger one*/
    if (*length < needed_size)
    {
        *length = needed_size;
        return DBUS_CREDENTIALS_API_LENGTH_TOO_SMALL;
    };
    *length = needed_size;
    if (client_id == nullptr)
        return DBUS_CREDENTIALS_API_INVALID_PARAM;
    switch (method_id_code)
    {
        /*case PID_METHOD:
             return dbus_g_proxy_call(proxy, "GetConnectionUnixProcessID",
                &error,G_TYPE_STRING, unique_name, G_TYPE_INVALID,
                G_TYPE_UINT, &client_id, G_TYPE_INVALID)?
                DBUS_CREDENTIALS_API_SUCCESS:DBUS_CREDENTIALS_API_DBUS_OPERATION_FAILED;
        case UID_METHOD:
            return dbus_g_proxy_call(proxy, "GetConnectionUnixUser",
                &error,G_TYPE_STRING, unique_name, G_TYPE_INVALID,
                G_TYPE_UINT, &client_id, G_TYPE_INVALID)?
                DBUS_CREDENTIALS_API_SUCCESS:DBUS_CREDENTIALS_API_DBUS_OPERATION_FAILED;
        */
       /* case GID_METHOD:
            return dbus_g_proxy_call(proxy, "GetConnectionUnixGroupID",
                &error,DG_TYPE_STRING, unique_name, G_TYPE_INVALID,
                G_TYPE_UINT, &client_id, G_TYPE_INVALID)?
                SOCKET_CREDENTIALS_API_SUCCESS:DBUS_CREDENTIALS_API_DBUS_OPERATION_FAILED; */
        case SMACK_METHOD:
            return dbus_g_proxy_call(proxy, "GetConnectionSmackContext",
                &error,G_TYPE_STRING, unique_name, G_TYPE_INVALID,
                G_TYPE_UINT, &client_id, G_TYPE_INVALID)?
                DBUS_CREDENTIALS_API_SUCCESS:DBUS_CREDENTIALS_API_DBUS_OPERATION_FAILED;
    }
    return DBUS_CREDENTIALS_API_UNEXPECTED_ERROR;

}
