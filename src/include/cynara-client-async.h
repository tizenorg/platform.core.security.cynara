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
 * @file        cynara-client.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains asynchronous client APIs of Cynara available
 *              with libcynara-client-asynchronous.
 */


#ifndef CYNARA_CLIENT_ASYNC_H
#define CYNARA_CLIENT_ASYNC_H

/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes begin with the start error code and extend into negative direction.
 * @{
*/

/*! \brief   indicating the result of the one specific API is successful or access is allowed */
#define CYNARA_ASYNC_API_SUCCESS		0

/*! \brief   indicating that access that was checked is denied */
#define CYNARA_ASYNC_API_ACCESS_DENIED		1

/*! \brief   indicating that answer will be received asynchronously */
#define CYNARA_ASYNC_API_ANSWER_ASYNC		2

/*! \brief   indicating system is running out of memory state */
#define CYNARA_ASYNC_API_OUT_OF_MEMORY		3

/*! \brief   indicating the API's parameter is malformed */
#define CYNARA_ASYNC_API_INVALID_PARAM		4

/*! \brief   service not available */
#define CYNARA_ASYNC_API_SERVICE_NOT_AVAILABLE	5

/** @}*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cynara_async cynara_async;
typedef struct cynara_async_configuration cynara_async_configuration;
typedef unsigned int cynara_check_id;

/**
 * \par Description:
 * Initialize cynara-async-client library with given configuration.
 * Create structured used in following API calls.
 *
 * \par Purpose:
 * This API must be used by prior calling cynara_async_connect function.
 *
 * \par Typical use case:
 * Once before a service can call cynara_async_connect.
 *
 * \par Method of function operation:
 * This API initializes inner library structures and in case of success creates
 * and returns cynara structure.
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * Structure cynara_async created by cynara_async_initialize call should be released
 * with cynara_async_finish.
 *
 * \param[out] pp_cynara Place holder for created cynara_async structure.
 * \param[in] p_conf Configuration for cynara-async-client library. NULL for default parameters.
 *
 * \return CYNARA_ASYNC_API_SUCCESS on success, or error code on error.
 */

int cynara_async_initialize(cynara_async **pp_cynara,
			const cynara_async_configuration *p_conf);

/**
 * \par Description:
 * Release cynara-async-client library and destroy structure created with cynara_initialize.
 *
 * \par Purpose:
 * This API should be used to clean up after usage of cynara-async-client library.
 *
 * \par Typical use case:
 * Once after all checks has been answered.
 *
 * \par Method of function operation:
 * This API releases inner library structure and destroys cynara_async structure.
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * No other call to cynara-async-client library should be made after call to cynara_async_finish.
 *
 * \param[in] p_cynara cynara_async structure.
 *
 * \return CYNARA_ASYNC_API_SUCCESS on success, or error code on error.
 */
int cynara_async_finish(cynara_async *p_cynara);

/**
 * \par Description:
 * Connect with cynara server.
 *
 * \par Purpose:
 * This API must be used by prior calling calling cynara_async_check and cynara_async_receive.
 *
 * \par Typical use case:
 * After initiating cynara_async structure and after connection with cynara server has been lost.
 *
 * \par Method of function operation:
 * This API connects to cynara server and provides socket descriptor of this connection.
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_check needs cynara_async structure to be created first by
 * cynara_async_initialize.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[out] p_sock_fd connection socket descriptor.
 *
 * \return CYNARA_ASYNC_API_SUCCESS on success, or error code on error.
 */

int cynara_async_connect(cynara_async *p_cynara, int *p_sock_fd);

/**
 * \par Description:
 * Check client, user access for given privilege.
 *
 * \par Purpose:
 * This API should be used to check if a user running application identified as client
 * has access to a privilege.
 *
 * \par Typical use case:
 * A service wants to ask Cynara daemon, if a client demanding access to some privilege
 * has proper rights. If service does not get answer in this call it may receive it on
 * cynara_async_receive call.
 * 
 * \par Method of function operation:
 * Client (a process / application) demanding access to a privilege is running as some user.
 * For such triple an access to a privilege is checked by calling cynara.
 * Depending on defined policy, an external application may be launched to ask user a question,
 * e.g. if [s]he wants to allow client to use a privilege. Additional parameter client_session
 * may be used to distinguish between client session (e.g. for allowing access only for this
 * particular application launch).
 *
 * \par Sync (or) Async:
 * This is a Asynchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * An external application may be launched to allow user interaction in granting or denying access.
 * Call cynara_async_cancel to cancel pending request. Call to cynara_async_check needs
 * cynara_async structure to be created first and connected with cynara daemon. To do that call
 * cynara_async_initialize and cynara_async_connect.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] client Application or process identifier.
 * \param[in] client_session Session of client (connection, launch).
 * \param[in] user User running client.
 * \param[in] privilege Privilege that is a subject of a check..
 * \param[out] p_check_id Check id received when final answer was not received in this call.
 *
 * \return CYNARA_ASYNC_API_SUCCESS on success (access granted), CYNARA_API_ACCESS_DENIED
 * on access denial, CYNARA_ASYNC_API_ANSWER_ASYNC on answer receive asynchronously
 * or other error code on error.
 */
int cynara_async_check(cynara_async *p_cynara,
			const char *client, const char *client_session,
			const char *user, const char *privilege,
			cynara_check_id *p_check_id);

/**
 * \par Description:
 * Receive answer of cynara_async_check call from cynara daemon.
 *
 * \par Purpose:
 * This API should be used to receive answer on single check not answered by cynara_async_check.
 *
 * \par Typical use case:
 * A service did not get final answer in cynara_async_check call. If a read event occurs on
 * socket received by cynara_async_connect, answer can be received by this call.
 *
 * \par Method of function operation:
 * Receives answer sent by cynara daemon in response for cynara_async_check call.
 *
 * \par Sync (or) Async:
 * This is a Asynchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * An external application may be launched to allow user interaction in granting or denying access.
 * Call cynara_async_cancel to cancel pending request. Call to cynara_async_receive needs
 * cynara_async structure to be created first and connected with cynara daemon. To do that call
 * cynara_async_initialize and cynara_async_connect.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[out] p_check_id Check id received
 *
 * \return CYNARA_ASYNC_API_SUCCESS on success (access granted), CYNARA_API_ACCESS_DENIED
 * on access denial or other error code on error.
 */
int cynara_async_receive(cynara_async *p_cynara, cynara_check_id *p_check_id);

/**
 * \par Description:
 * Cancel check request created by cynara_async_check.
 *
 * \par Purpose:
 * This API should be used to cancel check request created by cynara_async_check.
 *
 * \par Typical use case:
 * A service did not get final answer in cynara_async_check call and answer did not come yet
 * due to check hanging on user decision to allow or deny privilege.
 *
 * \par Method of function operation:
 * Cancels check request created by cynara_async_check call.
 *
 * \par Sync (or) Async:
 * This is a Asynchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_cancel needs cynara_async structure to be created first and connected
 * with cynara daemon. To do that call cynara_async_initialize and cynara_async_connect.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] check_id Check id to be cancelled
 *
 * \return CYNARA_ASYNC_API_SUCCESS on success or other error code on error.
 */
int cynara_async_cancel(cynara_async *p_cynara, const cynara_check_id check_id);


#ifdef __cplusplus
}
#endif

#endif /* CYNARA_CLIENT_ASYNC_H */
