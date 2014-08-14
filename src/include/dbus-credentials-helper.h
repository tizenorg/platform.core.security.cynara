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
/**
 * \file        dbus-credentials-helper.h
 * \author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * \author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * \version     1.0
 * \brief       This file contains API of cynara which returns an application context (UID/SMACK).
 */
//#include <gio/gio.h>

#ifndef DBUS_CREDENTIALS_H
#define DBUS_CREDENTIALS_H

/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes begin with the start error code and extend into negative direction.
 * @{
*/

/*! \brief   indicating the result of the one specific API is successful or access is allowed */
#define DBUS_CREDENTIALS_API_SUCCESS 0

/*! \brief   indicating client process is running out of memory */
#define DBUS_CREDENTIALS_API_OUT_OF_MEMORY -1

/*! \brief   indicating the API's parameter is malformed */
#define DBUS_CREDENTIALS_API_INVALID_PARAM -2

/*! \brief   service not available (cannot connect to cynara service) */
#define DBUS_CREDENTIALS_API_SERVICE_NOT_AVAILABLE -3

/*! \brief   unexpected error in client library */
#define DBUS_CREDENTIALS_API_UNEXPECTED_CLIENT_ERROR -4

/*! \brief   cynara service does not allow to perform requested operation */
#define DBUS_CREDENTIALS_API_OPERATION_NOT_ALLOWED -5

/** @}*/

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \name get_user_id_from_dbus
 * \par Description:
 * Provides the caller with the user_id of a procces owner, the caller communicates with via a dbus connection. 
 *
 * \par Typical use casej
 * Function should be called when identification of the process owner on the other side of dbus communication is needed.
 * The out param user_id is allocated and freed by the caller.
 *
 * \param[in] connection GDBus connection descriptor 
 * \param[in] method a string defining the type of ID which shall be returned (i.e. UID) or "SYSTEM DEFAULT",
 *              "SYSTEM DEFAULT" means a value defined in SOCKET_SYSTEM_DEFAULT_FILE 
 * \param[out] user_id allocated and freed by the caller
 *          
 */
int get_user_id_from_dbus(const /*GDBusConnection*/ int *connection, const char *unique_name, void* user_id);

/**
 * \name get_user_id_from_socket
 * \par Description:
 * Provides the caller with the user_id of a procces owner, the caller communicates with via a dbus connection. 
 *
 * \par Typical use case:
 * Function should be called when identification of the process owner on the other side of socket communication is needed.
 * The out param user_id is allocated and freed by the caller.
 *
 * \param[in] connection GDBus connection descriptor 
 * \param[in] method a string defining the type of ID which shall be returned (i.e. PID) or "SYSTEM DEFAULT",
 *              "SYSTEM DEFAULT" means a value defined in SOCKET_SYSTEM_DEFAULT_FILE 
 * \param[out] user_id allocated and freed by the caller
 *          
 */
int get_client_id_from_dbus(const /*GDBusConnection*/ int *connection, const char *unique_name, void* client_id);

#ifdef __cplusplus
}
#endif
#endif

