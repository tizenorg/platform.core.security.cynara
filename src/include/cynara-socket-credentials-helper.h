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
 * \file        cynara-socket-credentials-helper.h
 * \author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * \author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * \version     1.0
 * \brief       This file contains socket credentials helper API.
 */

#ifndef SOCKET_CREDENTIALS_H
#define SOCKET_CREDENTIALS_H

/*! \brief indicating configuration file location with "SYSTEM DEFAULT" value*/
#define SOCKET_SYSTEM_DEFAULT_FILE "/etc/cynara/socket_helper"

/*! \brief indicating maximal size of configuration file ("SYSTEM DEFAULT" value)*/
#define SYSTEM_DEFAULT_MAX_SIZE 128


/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes begin with the start error code and extend into negative direction.
 * @{
*/

/*! \brief   indicating the result of the one specific API is successful or access is allowed */
#define SOCKET_CREDENTIALS_API_SUCCESS 0

/*! \brief   indicating the API's parameter is malformed */
#define SOCKET_CREDENTIALS_API_INVALID_PARAM -2

/*! \brief   getsockopt error occured */
#define SOCKET_CREDENTIALS_API_GETSOCKOPT_ERROR -3

/*! \brief   unexpected error in library */
#define SOCKET_CREDENTIALS_API_UNEXPECTED_ERROR -4

/*! \brief   error opening or parsing configuration file */
#define SOCKET_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE -6

/*! \brief indicating length of the buffor is too small */
#define SOCKET_CREDENTIALS_API_LENGTH_TOO_SMALL -7


/** @}*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * \name socket_credentials_get_user_id
 * \par Description:
 * Provides the caller with the user_id of a procces owner, the caller communicates with via socket.
 *
 * \par Typical use case:
 * Function should be called when identification of the process owner on the other side of socket communication is needed.
 * The out param user_id is allocated and freed by the caller.
 *
 * \param[in] socket_fd a UNIX socket descriptor
 * \param[in] method a string defining the type of ID which shall be returned: UID, GUID or "SYSTEM DEFAULT",
 *              "SYSTEM DEFAULT" means a value defined in SOCKET_SYSTEM_DEFAULT_FILE
 * \param[out] user_id allocated and freed by the caller
 * \param[in\out] the length of the buffer needed to store user_id, passing 0 or lenght smaller than needed
 *                causes returning the function with SOCKET_CREDENTIALS_API_LENGTH_TOO_SMALL error code and
 *                the parameter set to the necessary buffer size.
 * \return SOCKET_CREDENTIALS_API_SUCCESS on success, or error code on error
 */
int socket_credentials_get_user_id(const int socket_fd, const char *method, void *user_id, unsigned int *length);

/**
 * \name socket_credentials_get_client_id
 * \par Description:
 * Provides the caller with the client_id of a procces, the caller communicates with via a socket.
 *
 * \par Typical use case:
 * Function should be called when identification of the process on the other side of socket communication is needed.
 * The out param client_id is allocated and freed by the caller.
 *
 * \param[in] socket_fd a UNIX socket descriptor
 * \param[in] method a string defining the type of ID which shall be returned: PID, SMACK_LABEL or "SYSTEM DEFAULT",
 *              "SYSTEM DEFAULT" means a value defined in SOCKET_SYSTEM_DEFAULT_FILE
 * \param[out] client_id allocated and freed by the caller
 * \param[in\out] the length of the buffer needed to store client_id, passing 0 or lenght smaller than needed
 *                causes returning the function with SOCKET_CREDENTIALS_API_LENGTH_TOO_SMALL error code and
 *                the parameter set to the necessary buffer size.
 * \return SOCKET_CREDENTIALS_API_SUCCESS on success, or error code on error
 */
int socket_credentials_get_client_id(const int socket_fd, const char *method, void *client_id, unsigned int *length);

#ifdef __cplusplus
}
#endif

#endif /* SOCKET_CREDENTIALS_H*/
