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
 * \file        cynara-helper-session.h
 * \author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * \author      Radoslaw Bartosiak <r.bartosiak@samsung.com.pl>
 * \version     1.0
 * \brief       This file contains session string creation helper API
 */

#ifndef SESSION_HELPER_H
#define SESSION_HELPER_H

/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes begin with the start error code and extend into negative direction.
 * @{
*/

/*! \brief   indicating the result of the one specific API is successful or access is allowed */
#define SESSION_HELPER_API_SUCCESS 0

/*! \brief   indicating client process is running out of memory */
#define SESSION_HELPER_API_OUT_OF_MEMORY -1

/*! \brief   indicating the API's parameter is malformed */
#define SESSION_HELPER_API_INVALID_PARAM -2

/*! \brief  indicating an unexpected error in library */
#define SESSION_HELPER_API_UNEXPECTED_ERROR -4

/*! \brief indicating search permission is denied for one of
 the directories in the path prefix of path or the path is wrong */
#define SESSION_HELPER_API_ACCESS_DENIED -5

/*! \brief indicating length of the buffor is too small */
#define SESSION_HELPER_API_LENGTH_TOO_SMALL -6

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \par Description:
 * Creates a client session string based on pid of a client and time of creation of the client /proc/PID directory.
 *
 * \par Purpose:
 * This function shall be used to create session string identifier used.
 *
 *
 * \par Typical use case:
 * The function is called first with second parameter equal to 0.
 * The size of the needed buffer is returned to the caller
 * Next the caller allocates needed memory and sets the length
 * The function returns the session name string via its second parameter.
 *
 * //sample code
 * int ret;
 * int length = 10;
 * char* client_session;
 * char* client="1";
 *
 * if (session_helper_get_process_based_id(client, client_session, &length) != SESSION_HELPER_API_LENGTH_TOO_SMALL)
 *   exit(1);
 * client_session = (char*) malloc (length);
 * if (client_session == NULL) exit (1);
 * if (session_helper_get_process_based_id(client, client_session, &length) != SESSION_HELPER_API_SUCCESS)
 *  exit(1);
 *
 * //do something
 * free(client_session);
 * cynara_finish(cynara *p_cynara); // return value not checked!
 * exit(1);
 *
 *
 * \par Method of function operation:
 * The function generates client session based on the pid and start time of the client process.
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \param[in] client application or process identifier (PID).
 * \param[out] client_session string.
 * \param[out/in] the length of the buffer needed to store client_session (including terminating NULL)
 *
 *
 * \return SESSION_HELPER_API_SUCCESS on success, or error code otherwise.
 *
 * \brief Return session id string based on PID and client process creation time.
 */

int session_helper_get_process_based_id(pid_t client_pid, char *client_session, unsigned int *length);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* SESSION_HELPER_H */
