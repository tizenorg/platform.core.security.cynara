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
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains asynchronous client APIs of Cynara available
 *              with libcynara-client-asynchronous.
 */


#ifndef CYNARA_CLIENT_ASYNC_H
#define CYNARA_CLIENT_ASYNC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes begin with the start error code and extend into negative direction.
 * @{
*/

/*! \brief   indicating the result of the one specific API is successful or access is allowed */
#define CYNARA_API_SUCCESS                0

/*! \brief   indicating that access that was checked is denied */
#define CYNARA_API_ACCESS_DENIED         -1

/*! \brief   indicating system is running out of memory state */
#define CYNARA_API_OUT_OF_MEMORY         -2

/*! \brief   indicating the API's parameter is malformed */
#define CYNARA_API_INVALID_PARAM         -3

/*! \brief   service not available */
#define CYNARA_API_SERVICE_NOT_AVAILABLE -4

/** @}*/


typedef uint16_t cynara_check_id;
typedef struct cynara_async cynara_async;
typedef struct cynara_async_configuration cynara_async_configuration;

/**
 * \enum cynara_async_status
 * Flags indicating the status of connected cynara socket.
 *
 * \var cynara_async_status::CYNARA_STATUS_FOR_READ
 * Wait for read events on socket
 *
 * \var cynara_async_status::CYNARA_STATUS_FOR_WRITE
 * Wait for write events on socket
 *
 * \var cynara_async_status::CYNARA_STATUS_FOR_RW
 * Wait for both read and write events on socket
 **/
typedef enum {
    CYNARA_STATUS_FOR_READ,
    CYNARA_STATUS_FOR_WRITE,
    CYNARA_STATUS_FOR_RW
} cynara_async_status;

/*! \brief */
#define CYNARA_CALLBACK_STOP 0

/*! \brief */
#define CYNARA_CALLBACK_CONTINUE 1

/**
 * \brief Callback used by cynara async API when response is available in cynara_async_process.
 * Function should return either CYNARA_CALLBACK_STOP or CYNARA_CALLBACK_CONTINUE to indicate
 * whether client wants to immediately receive another response if it's available.
 *
 * \param[in] check_id Number identifying check request
 * \param[in] response Response for check request
 * \param[in] user_data User specific data
 **/
typedef int (*cynara_response_callback) (int check_id, int response, void *user_data);

/**
 * \brief Callback used by cynara async API when status of cynara socket is changed in
 * cynara_async_initialize, cynara_async_check, cynara_async_process or cynara_async_finish.
 *
 * \param[in] old_fd Old descriptor which should be unregistered from event loop,
 *                   -1 when callback is called after first successful connect.
 * \param[in] new_fd New descriptor which should be registered in event loop,
 *                   -1 when cynara_async_finish is called and cynara is disconnected
 * \param[in] status Status indicating which events should be awaited on socket
 * \param[in] user_data User specific data
 **/
typedef void (*cynara_status_callback) (int old_fd,
                                        int new_fd,
                                        cynara_async_status status,
                                        void *user_data);

/**
 * \par Description:
 * Initialize cynara-async-client library with given configuration. Create structure used in
 * following API calls and register callback and user_data for further cynara async API calls.
 *
 * \par Purpose:
 * This API must be used prior to calling any other cynara async API function.
 * It will create cynara_async structure required for any other cynara async API calls and will
 * trigger cynara_status_callback callback giving client file descriptor and socket flags needed
 * to be passed to event loop of user choice (old_fd will be set to -1).
 *
 * \par Typical use case:
 * Once before entering event loop and before any other cynara async API is called.
 *
 * \par Method of function operation:
 * This API initializes inner library structures, estabilishes connection to cynara server and in
 * case of success returns cynara_async structure.
 *
 * \par Sync (or) Async:
 * This is an asynchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Structure cynara_async created by cynara_async_initialize call should be released
 * with cynara_async_finish.
 * This API calls cynara_status_callback callback.
 * Given callback and user_data will be used unless cynara_async_register_status_callback will be
 * used.
 * Data in user_data will be passed to cynara_status_callback callback.
 *
 * \param[out] pp_cynara Placeholder for created cynara_async structure.
 * \param[in] p_conf Configuration for cynara-async-client library.
 * \param[in] callback Function called when connection is started
 * \param[in] user_data User specific data, passed to
 *
 * \return CYNARA_API_SUCCESS on success, or error code on error.
 */

int cynara_async_initialize(cynara_async **pp_cynara,
                            const cynara_async_configuration *p_conf,
                            cynara_status_callback callback,
                            void *user_data);

/**
 * \par Description:
 * Release cynara-async-client library and destroy structure created with cynara_async_initialize.
 *
 * \par Purpose:
 * This API should be used to clean up after usage of cynara-async-client library.
 * Upon disconnecting this will trigger cynara_status_callback so client can unregister file
 * descriptor connected with cynara.
 *
 * \par Typical use case:
 * Once after connection to cynara is not needed.
 *
 * \par Method of function operation:
 * This API releases inner library structure and destroys cynara_async structure. All pending
 * requests are cancelled, connection to cynara server is closed.
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
 * Data in user_data will be passed to cynara_status_callback callback.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] user_data User specific data, passed to callbacks.
 *
 * \return CYNARA_API_SUCCESS on success, or error code on error.
 */
int cynara_async_finish(cynara_async *p_cynara);

/**
 * \par Description:
 * Register callback for socket status changes.
 *
 * \par Purpose:
 * During cynara async API usage changes on socket might be required. cynara_status_callback is
 * called in these cases and user_data is passed to this callback.
 *
 * \par Typical use case:
 * After setting callback and user_data using cynara_async_intialize, client wants to change
 * any of them.
 *
 * \par Method of function operation:
 * This API replaces already registered callback and user_data with new ones.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_register_status_callback requires initialized cynara_async structure.
 * For this call cynara_async_intialize.
 * Data in user_data will be passed to every cynara_status_callback callback.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] user_data User specific data, passed to callbacks.
 *
 * \return CYNARA_API_SUCCESS on success, or error code on error.
 */

int cynara_async_register_status_callback(cynara_async *p_cynara,
                                          cynara_status_callback callback,
                                          void *user_data);

/**
 * \par Description:
 * Check client, user access for given privilege. Set callback and user_data to be called and pass
 * when matching response is available in cynara_async_process.
 *
 * \par Purpose:
 * This API should be used to check whether client identified by a triple
 * (client, user, privilege) in custom defined session has access.
 * Response can be received with cynara_async_process.
 * Check id is returned to pair request with response for canceling purposes.
 *
 * \par Typical use case:
 * A service wants to ask Cynara daemon, if a client demanding access to some privilege
 * has proper rights. To receive matching response client sets callback and specifies arbitrary
 * data to be passed to this callback.
 *
 * \par Method of function operation:
 * Client (a process / application) demanding access to a privilege is running as some user.
 * For such triple a request event is created and added to pending requests for
 * cynara_async_process to process. Socket status will be changed to ready for write, to
 * ensure that response will be returned as soon as possible. This will trigger
 * cynara_status_callback callback.
 *
 * \par Sync (or) Async:
 * This is an asynchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_check needs cynara_async structure to be created first.
 * Use cynara_async_initialize before calling this function.
 * Call cynara_async_cancel to cancel pending request.
 * Call cynara_async_process to receive response.
 * Data in user_data will be passed to cynara_response_callback callback in cynara_async_process.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] client Application or process identifier.
 * \param[in] client_session Client defined session.
 * \param[in] user User of running client.
 * \param[in] privilege Privilege that is a subject of a check.
 * \param[out] p_check_id Placeholder for check id.
 * \param[in] callback Function called when matching response is received.
 * \param[in] user_data User specific data, passed to callback.
 *
 * \return CYNARA_API_SUCCESS on success or error code on error.
 */
int cynara_async_check(cynara_async *p_cynara,
                       const char *client,
                       const char *client_session,
                       const char *user,
                       const char *privilege,
                       cynara_check_id *p_check_id,
                       cynara_response_callback callback,
                       void *user_data);

/**
 * \par Description:
 * Process events that appeared on cynara socket.
 *
 * \par Purpose:
 * Process events after they appear on cynara socket. When status of socket should be changed,
 * cynara_status_callback callback will be triggered. When response is available
 * cynara_response_callback will be triggered. Responses will be returned until
 * cynara_response_callback returns CYNARA_CALLBACK_STOP.
 *
 * \par Typical use case:
 * After request was queued with cynara_async_check this API will return response.
 * When event loop will return readiness on cynara socket, client should use this API.
 * When cynara_async_check was called and client depends on quick response, client may use
 * cynara_async_process immediately after cynara_async_check is called.
 *
 * \par Method of function operation:
 * This API processes all pending requests, receives all responses and reacts when cynara
 * has disconnected. If response is available in cache, no further operation on socket
 * concerning this request is required. If cynara has disconnected all values in cache become
 * invalid and all responses will be taken from cynara server. During these operations
 * status of cynara socket may change, so cynara_status_callback callback will be triggered
 * to indicate these changes.
 * When response is available cynara_response_callback will be triggered.
 *
 * \par Sync (or) Async:
 * This is an asynchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_process requires initialized cynara_async structure. For this use
 * cynara_async_initialize.
 * Only this function may trigger cynara_response_callback.
 *
 * \param[in] p_cynara cynara_async structure.
 *
 * \return CYNARA_API_SUCCESS on success or error code on error.
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
 * When cynara client is no longer interested in receiving an answer.
 *
 * \par Method of function operation:
 * Cancels check request created by cynara_async_check call.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_cancel needs cynara_async structure to be created first. For this use
 * cynara_async_initialize.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] check_id Check id to be cancelled
 *
 * \return CYNARA_API_SUCCESS on success or other error code on error.
 */
int cynara_async_cancel(cynara_async *p_cynara, cynara_check_id check_id);

/*
 *  Example of usage:
 *
 *  void change_status(int old_fd, int new_fd, cynara_async_status status, void *user_data) {
 *      // unregister using old_fd
 *      ...
 *      //register using new_fd and new status
 *  }
 *
 *  int process_response(int check_id, int response, void *user_data) {
 *      //process response
 *      ...
 *      //If processing takes much time returning CYNARA_CALLBACK_STOP is suggested
 *      return CYNARA_CALLBACK_CONTINUE;
 *  }
 *
 *  // Check only queues request for processing by cynara_async_process,
 *  // so services, which
 *  void check_fast(cynara_async *p_cynara, client, client_session, user, privilege,
 *                  cynara_check_id *check_id) {
 *
 *      if(cynara_async_check(p_cynara, ..., process_response, NULL) != CYNARA_API_SUCCESS) {
 *          //process error
 *          ...
 *      }
 *      //If response is available, cynara_async_process will call process_response
 *      if(cynara_async_process(p_cynara) != CYNARA_API_SUCCESS) {
 *          //process error
 *          ...
 *      }
 *  }
 *
 *  void main_process {
 *      //initialize all required objects
 *      cynara_async *p_cynara;
 *      //change_status will be called passing file descriptor connected to cynara server
 *      if (cynara_async_initalize(&p_cynara, NULL, change_status, &fd_sets) !=
 *            CYNARA_API_ SUCCESS) {
 *         //process error
 *         ...
 *      }
 *
 *      //start event loop
 *      event_process(fd_sets, ...)
 *
 *  }
 *
 *  void event_process(fd_sets, ...) {
 *      //event loop of user choice
 *      select(..., fd_sets.read, fd_sets.write) {
 *         ...
 *         if(active_socket == cynara_socket) {
 *             if(cynara_async_process(p_cynara) != CYNARA_API_SUCCESS) {
 *                //process error
 *                ...
 *             }
 *         }
 *         ...
 *         //processing clients, which may require cynara checks
 *      }
 *
 *      if(fast_check_is_needed)
 *          check_fast(p_cynara, client, client_session, user, privilege, &check_id);
 *      else {
 *          cynara_async_check(p_cynara, client, client_session, user, privilege, &check_id,
 *                             process_response, NULL);
 *      }
 *  }
 */



#ifdef __cplusplus
}
#endif

#endif /* CYNARA_CLIENT_ASYNC_H */
