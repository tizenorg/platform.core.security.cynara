/*
 *  Copyright(c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0(the "License");
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
#include <string>
#include <string.h>

#include <cynara-client-error.h>

#include "credentials-dbus-inner.h"

static const std::string dbus_systemd_name = "org.freedesktop.DBus";
static const std::string dbus_systemd_object = "/org/freedesktop/DBus";
static const std::string dbus_systemd_interface = "org.freedesktop.DBus";


int getIdFromConnection(DBusConnection *connection,
                        const char *uniqueName,
                        const char *type,
                        unsigned int *id) {
    DBusMessage *message;
    DBusPendingCall *reply = NULL;
    DBusMessageIter iter;
    if((connection == NULL) ||(uniqueName == NULL))
        return CYNARA_API_INVALID_PARAM;
    //user id or process pid
    if  (strcmp("GetConnectionUnixUser", type) * strcmp("GetConnectionUnixProcessID", type) != 0)
        return CYNARA_API_INVALID_PARAM;

    message = dbus_message_new_method_call(dbus_systemd_name.c_str(),
                                           dbus_systemd_object.c_str(),
                                           dbus_systemd_interface.c_str(),
                                           type);
    if(message == NULL)
        return CYNARA_API_OUT_OF_MEMORY;

    if(!dbus_message_append_args(message, DBUS_TYPE_STRING, &uniqueName, DBUS_TYPE_INVALID)) {
        dbus_message_unref(message);
        return CYNARA_API_OUT_OF_MEMORY;
    }
    if(!dbus_connection_send_with_reply(connection, message, &reply, DBUS_TIMEOUT_USE_DEFAULT)) {
        dbus_message_unref(message);
        return CYNARA_API_OUT_OF_MEMORY;
    }
    //the connection is disconnected or one tries to send unix file descriptor
    if(reply == NULL)
        return CYNARA_API_INVALID_PARAM;
    dbus_connection_flush(connection);
    dbus_pending_call_block(reply);
    dbus_message_unref(message);
    if((message = dbus_pending_call_steal_reply(reply)) == NULL) //no reply received
        return CYNARA_API_UNKNOWN_ERROR;

    if(dbus_message_iter_init(message, &iter) == 0) {
        dbus_message_unref(message);
        return CYNARA_API_UNKNOWN_ERROR;
    }
    if(DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&iter)) {
        dbus_message_unref(message);
        return CYNARA_API_UNKNOWN_ERROR;
    }

    dbus_message_iter_get_basic(&iter, id);
    dbus_message_unref(message);
    dbus_pending_call_unref(reply);
    return CYNARA_API_SUCCESS;
}

int getClientSmackLabel(DBusConnection *connection,
                        const char *uniqueName,
                        char **client) {
    DBusMessage *message;
    DBusPendingCall *reply = NULL;
    DBusMessageIter iter;
    char *answ;

    if((connection == NULL) ||(uniqueName == NULL))
        return CYNARA_API_INVALID_PARAM;
    message = dbus_message_new_method_call(dbus_systemd_name.c_str(),
                                           dbus_systemd_object.c_str(),
                                           dbus_systemd_interface.c_str(),
                                           "SmackLabel");
    if(message == NULL)
        return CYNARA_API_OUT_OF_MEMORY;
    if(!dbus_message_append_args(message, DBUS_TYPE_STRING, &uniqueName, DBUS_TYPE_INVALID)) {
        dbus_message_unref(message);
        return CYNARA_API_OUT_OF_MEMORY;
    }
    if(!dbus_connection_send_with_reply(connection, message, &reply, DBUS_TIMEOUT_USE_DEFAULT)) {
        dbus_message_unref(message);
        return CYNARA_API_OUT_OF_MEMORY;
    }
    //the connection is disconnected or one tries to send unix file descriptor
    if(reply == NULL)
        return CYNARA_API_INVALID_PARAM;

    dbus_connection_flush(connection);
    dbus_pending_call_block(reply);
    dbus_message_unref(message);

    if((message = dbus_pending_call_steal_reply(reply)) == NULL) //no reply received
        return CYNARA_API_UNKNOWN_ERROR;

    if(dbus_message_iter_init(message, &iter) == 0) {
        dbus_message_unref(message);
        return CYNARA_API_UNKNOWN_ERROR;
    }

    if(DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&iter)) {
        dbus_message_unref(message);
        return CYNARA_API_UNKNOWN_ERROR;
    }
    dbus_message_iter_get_basic(&iter, &answ);
    dbus_message_unref(message);
    dbus_pending_call_unref(reply);
    if ((*client = strdup(answ)) == NULL)
        return CYNARA_API_OUT_OF_MEMORY;
    return CYNARA_API_SUCCESS;
}

int getClientPid(DBusConnection *connection,
                 const char *uniqueName,
                 char **client) {
    unsigned int pid;
    int ret;
    //return CYNARA_API_SUCCESS;
    if((ret = getIdFromConnection(connection, uniqueName, "GetConnectionUnixProcessID", &pid))
       != CYNARA_API_SUCCESS) {
        return ret;
    }
    if((*client = strdup(std::to_string(pid).c_str())) == NULL)
      return CYNARA_API_OUT_OF_MEMORY;
    return CYNARA_API_SUCCESS;
}

int getUserId(DBusConnection *connection,
              const char *uniqueName,
              char **user) {
    unsigned int uid;
    int ret;
    if((ret = getIdFromConnection(connection, uniqueName, "GetConnectionUnixUser", &uid))
       != CYNARA_API_SUCCESS)
        return ret;
    if((*user = strdup(std::to_string(uid).c_str())) == NULL)
      return CYNARA_API_OUT_OF_MEMORY;
    return CYNARA_API_SUCCESS;
}

int getUserGid(DBusConnection *connection UNUSED,
               const char *uniqueName UNUSED,
               char **user UNUSED) {
    //todo
    return CYNARA_API_METHOD_NOT_SUPPORTED;
}

pid_t getPid(DBusConnection *connection, const char *uniqueName) {
    unsigned int pid;
    int ret;
    if((ret = getIdFromConnection(connection, uniqueName, "GetConnectionUnixProcessID", &pid))
       != CYNARA_API_SUCCESS)
        return ret;
    return pid;
}
