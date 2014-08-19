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
 * @file        cynara-client-async.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains asynchronous client APIs of Cynara available
 *              with libcynara-client-asynchronous.
 */


#ifndef CYNARA_CLIENT_ASYNC_H
#define CYNARA_CLIENT_ASYNC_H

#include <stdint.h>

/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes begin with the start error code and extend into negative direction.
 * @{
*/

/*! \brief   indicating the result of the one specific API is successful or access is allowed */
#define CYNARA_ASYNC_API_SUCCESS                0

/*! \brief   indicating that access that was checked is denied */
#define CYNARA_ASYNC_API_ACCESS_DENIED         -1

/*! \brief   indicating too many pending checks */
#define CYNARA_ASYNC_API_MAX_PENDING_CHECKS    -2

/*! \brief   indicating that answer was not yet received */
#define CYNARA_ASYNC_API_ANSWER_NOT_READY      -3

/*! \brief   indicating system is running out of memory state */
#define CYNARA_ASYNC_API_OUT_OF_MEMORY         -4

/*! \brief   indicating the API's parameter is malformed */
#define CYNARA_ASYNC_API_INVALID_PARAM         -5

/*! \brief   service not available */
#define CYNARA_ASYNC_API_SERVICE_NOT_AVAILABLE -6

/** @}*/

/**
 * \name Flags
 * Flags passed to change_status_callback.
 * Indicate which socket events should be monitored.
 * @{
 */

/*! \brief   monitor socket read events only */
#define CYNARA_ASYNC_FLAG_RDONLY 0

/*! \brief   monitor socket read and write events */
#define CYNARA_ASYNC_FLAG_RDRW   1

/** @}*/


#ifdef __cplusplus
extern "C" {
#endif

typedef struct cynara_async cynara_async;
typedef struct cynara_async_configuration cynara_async_configuration;
typedef uint16_t cynara_check_id;

struct cynara_async_configuration {

/**
 * \par Description:
 * Called when answer on check request is received.
 *
 * \param[in] check_id 0 on check received immediately in cynara_async_check.
 *                     Check id passed by cynara_async_check.
 * \param[in] response CYNARA_ASYNC_API_SUCCESS on access granted.
 *                     CYNARA_ASYNC_API_ACCESS_DENIED on access denied.
 * \param[in] data Data passed in configuration.
 */
    void (* const check_callback) (cynara_check_id check_id, int response, void * const data);

/**
 * \par Description:
 * Called when flags and/or cynara socket descriptor is changed.
 *
 * \param[in] old_fd Old cynara file descriptor - may be same as new_fd param, when only flags
 * are changed.
 * \param[in] new_fd New file descriptor - may be same as old_fd param, when only flags
 * are changed.
 * \param[in] flags CYNARA_ASYNC_FLAG_RDONLY on monitored socket read events only.
 *                  CYNARA_ASYNC_FLAG_RDRW on monitored socket read and write events.
 * \param[in] data Data passed in configuration.
 */
    void (* const change_status_callback) (int old_fd, int new_fd, int flags, void * const data);
/**
 * \par Description:
 * Placeholder for client specified structure used in callback functions. Should provide variables
 * needed by callback operations.
 */
    void * const data;
};

/**
 * \par Description:
 * Initialize cynara-async-client library with given configuration.
 * Create structure used in following API calls.
 *
 * \par Purpose:
 * This API must be used prior to calling cynara_async_check function.
 *
 * \par Typical use case:
 * Once before a service can call cynara_async_check.
 *
 * \par Method of function operation:
 * This API initializes inner library structures and in case of success creates
 * and returns cynara_async structure.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Structure cynara_async created by cynara_async_initialize call should be released
 * with cynara_async_finish. This function calls change_status_callback.
 *
 * \param[out] pp_cynara Placeholder for created cynara_async structure.
 * \param[in] p_conf Configuration for cynara-async-client library. NULL for default parameters.
 *
 * \return CYNARA_ASYNC_API_SUCCESS on success, or error code on error.
 */
int cynara_async_initialize(cynara_async **pp_cynara,
                            const cynara_async_configuration *p_conf);

/**
 * \par Description:
 * Release cynara-async-client library and destroy structure created with cynara_async_initialize.
 *
 * \par Purpose:
 * This API should be used to clean up after usage of cynara-async-client library.
 *
 * \par Typical use case:
 * Once after all checks have been answered.
 *
 * \par Method of function operation:
 * This API releases inner library structure and destroys cynara_async structure. All pending
 * requests are cancelled.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * No other call to cynara-async-client library should be made after call to cynara_async_finish.
 * This function calls change_status_callback.
 *
 * \param[in] p_cynara cynara_async structure.
 *
 * \return CYNARA_ASYNC_API_SUCCESS on success, or error code on error.
 */
int cynara_async_finish(cynara_async *p_cynara);

/**
 * \par Description:
 * Check client, user access for given privilege.
 *
 * \par Purpose:
 * This API should be used to create check request (if a user running application identified
 * as client has access to a privilege) or answer it in this call using check_callback if answer
 * is available.
 *
 * \par Typical use case:
 * A service wants to ask Cynara daemon, if a client demanding access to some privilege
 * has proper rights. If service does not get answer in this call, it may get it
 * asynchronously by calling cynara_async_process.
 *
 * \par Method of function operation:
 * Client (a process / application) demanding access to a privilege is running as some user.
 * For such triple an access to a privilege is checked by calling cynara.
 * Depending on defined policy, an external application may be launched to ask user a question,
 * e.g. if [s]he wants to allow client to use a privilege. Additional parameter client_session
 * may be used to distinguish between client session (e.g. for allowing access only for this
 * particular application launch). If final answer is not given in check_callback, id of current
 * check should be received.
 *
 * \par Sync (or) Async:
 * This is an asynchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * An external application may be launched to allow user interaction in granting or denying access.
 * Call cynara_async_cancel to cancel pending request. Call to cynara_async_check needs
 * cynara_async structure to be created first and connected with cynara daemon. To do that call
 * cynara_async_initialize. This function may call change_status_callback and check_callback.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] client Application or process identifier.
 * \param[in] client_session Session of client (connection, launch).
 * \param[in] user User running client.
 * \param[in] privilege Privilege that is a subject of a check.
 * \param[out] p_check_id Placeholder for check id.
 *
 * \return CYNARA_ASYNC_API_SUCCESS on success, CYNARA_ASYNC_API_ANSWER_NOT_READY on asynchronous
 * request sent or other error code on error.
 */
int cynara_async_check(cynara_async *p_cynara,
                       const char *client, const char *client_session,
                       const char *user, const char *privilege,
                       cynara_check_id *p_check_id);

/**
 * \par Description:
 * Receives answers of cynara_async_check call from cynara daemon and sends check requests.
 *
 * \par Purpose:
 * This API should be used to react to write and read event on cynara socket.
 *
 * \par Typical use case:
 * After receiving write or read event on socket.
 *
 * \par Method of function operation:
 * Receives answer sent by cynara daemon in response to cynara_async_check call and sends requests
 * to cynara of checks created by cynara_async_check.
 *
 * \par Sync (or) Async:
 * This is an asynchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * An external application may be launched to allow user interaction in granting or denying access.
 * Call cynara_async_cancel to cancel pending request. Call to cynara_async_process needs
 * cynara_async structure to be initialized. To do that call cynara_async_initialize. This function
 * may call change_status_callback and check_callback.
 *
 * \param[in] p_cynara cynara_async structure.
 *
 * \return CYNARA_ASYNC_API_SUCCESS on success, or other error code on error.
 */
int cynara_async_process(cynara_async *p_cynara);

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
 * This is an asynchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_cancel needs cynara_async structure to be initialized. To do that call
 * cynara_async_initialize.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] check_id Check id to be cancelled
 *
 * \return CYNARA_ASYNC_API_SUCCESS on success or other error code on error.
 */
int cynara_async_cancel(cynara_async *p_cynara, cynara_check_id check_id);


#ifdef __cplusplus
}
#endif

#endif /* CYNARA_CLIENT_ASYNC_H */
