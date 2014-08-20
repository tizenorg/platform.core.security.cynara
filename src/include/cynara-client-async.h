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

typedef uint16_t cynara_check_id;
typedef struct cynara_async cynara_async;
typedef struct cynara_async_configuration cynara_async_configuration;

/**
 * \enum cynara_async_status
 * Values indicating the status of connected cynara socket.
 *
 * \var cynara_async_status::CYNARA_STATUS_FOR_READ
 * Wait for read events on socket.
 *
 * \var cynara_async_status::CYNARA_STATUS_FOR_RW
 * Wait for both read and write events on socket.
 **/
typedef enum {
    CYNARA_STATUS_FOR_READ,
    CYNARA_STATUS_FOR_RW
} cynara_async_status;

/**
 * \enum cynara_async_call_cause
 * Values indicating the reason of cynara_response_callback call.
 *
 * \var cynara_call_cause::CYNARA_CALL_CAUSE_ANSWER
 * Callback was called due to response to previous cynara_async_create_request call.
 *
 * \var cynara_call_cause::CYNARA_CALL_CAUSE_CANCEL
 * Callback was called due to response to previous cynara_async_cancel_request call.
 *
 * \var cynara_call_cause::CYNARA_CALL_CAUSE_FINISH
 * Callback was called due to cynara_async_finish call.
 *
 * \var cynara_call_cause::CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE
 * Callback was called due to service not available.
 **/
typedef enum {
    CYNARA_CALL_CAUSE_ANSWER,
    CYNARA_CALL_CAUSE_CANCEL,
    CYNARA_CALL_CAUSE_FINISH,
    CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE
} cynara_async_call_cause;

/**
 * \brief Callback used by cynara async API in response to created request. Called upon API
 * finishing request processing in cynara_async_process, cynara_async_cancel_request
 * or cynara_async_finish.
 *
 * \param[in] check_id Number identifying check request.
 * \param[in] cause Cause of triggering this callback.
 * \param[in] response Response for created request. Should be ignored if cause is not
 * an answer to request.
 * \param[in] user_data User specific data.
 **/
typedef void (*cynara_response_callback) (int check_id,
                                          cynara_async_call_cause cause,
                                          int response,
                                          void *user_data);

/**
 * \brief Callback used by cynara async API when status of cynara socket is changed in
 * cynara_async_initialize, cynara_async_create_request, cynara_async_process,
 * cynara_async_cancel_request or cynara_async_finish.
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
 *
 * \param[out] pp_cynara Placeholder for created cynara_async structure.
 * \param[in] p_conf Configuration for cynara-async-client library.
 * \param[in] callback Function called when connection is started
 * \param[in] user_data User specific data, passed to callback.
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
 *
 * \par Typical use case:
 * Once after connection to cynara is not needed.
 *
 * \par Method of function operation:
 * This API releases inner library structure and destroys cynara_async structure. Connection to
 * cynara server is closed. Upon disconnecting this will trigger cynara_status_callback callback
 * with -1 as new_fd param so client can unregister file descriptor connected with cynara. It will
 * also trigger cynara_response_callback callback for each created request with
 * cynara_async_call_cause::CYNARA_CALL_CAUSE_FINISH as cause param.
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
 *
 * \param[in] p_cynara cynara_async structure.
 */
void cynara_async_finish(cynara_async *p_cynara);

/**
 * \par Description:
 * Check client, user access for given privilege in cache.
 *
 * \par Purpose:
 * This API should be used to check whether client identified by a triple
 * (client, user, privilege) in custom defined session has access.
 *
 * \par Typical use case:
 * A service wants to check in cache, if a client demanding access to some privilege
 * has proper rights.
 *
 * \par Method of function operation:
 * Client (a process / application) demanding access to a privilege is running as some user.
 * For such triple a cache is checked. If cache is invalid it is cleared and call returns same
 * as access not found.
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
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] client Application or process identifier.
 * \param[in] client_session Client defined session.
 * \param[in] user User of running client.
 * \param[in] privilege Privilege that is a subject of a check.
 *
 * \return CYNARA_API_ACCESS_ALLOWED on checked access allowed,
 *         CYNARA_API_ACCESS_DENIED on checked access denied,
 *         CYNARA_API_CACHE_MISS on access not in cache,
 *         or other error code on error.
 */
int cynara_async_check(cynara_async *p_cynara,
                       const char *client,
                       const char *client_session,
                       const char *user,
                       const char *privilege);

/**
 * \par Description:
 * Creates request for client, user accessing given privilege. Set callback and user_data
 * to be called and passed when request processing is finished.
 *
 * \par Purpose:
 * This API should be used to create check request for client identified by a triple
 * (client, user, privilege) in custom defined session.
 * Response can be received with cynara_async_process.
 * Check id is returned to pair request with response for canceling purposes.
 *
 * \par Typical use case:
 * A service did not received answer by cynara_async_check and wants to ask Cynara daemon,
 * if a client demanding access to some privilege has proper rights. To receive matching response
 * client sets callback and specifies arbitrary data to be passed to this callback.
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
 * Call to cynara_async_create_request needs cynara_async structure to be created first.
 * Use cynara_async_initialize before calling this function.
 * Call cynara_async_cancel_request to cancel pending request.
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
 * \return CYNARA_API_SUCCESS on success,
 *         CYNARA_API_MAX_PENDING_REQUESTS on too much pending requests,
 *         or other error code on error.
 */
int cynara_async_create_request(cynara_async *p_cynara,
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
 * Process events after they appear on cynara socket.
 *
 * \par Typical use case:
 * After request was queued with cynara_async_create_request this API will return response.
 * When event loop will return readiness on cynara socket, client should use this API.
 *
 * \par Method of function operation:
 * This API sends pending requests, receives all responses and reacts when cynara
 * has disconnected. If cynara has disconnected all values in cache become invalid. During these
 * operations status of cynara socket may change, so cynara_status_callback callback will be
 * triggered to indicate these changes. cynara_response_callback callback will be triggered with
 * with cynara_async_call_cause::CYNARA_CALL_CAUSE_ANSWER as cause param when response is available.
 * If cynara has disconnected it will be triggered with
 * cynara_async_call_cause::CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE as cause param.
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
 *
 * \param[in] p_cynara cynara_async structure.
 *
 * \return CYNARA_API_SUCCESS on success or error code on error.
 */
int cynara_async_process(cynara_async *p_cynara);

/**
 * \par Description:
 * Cancel request created by cynara_async_create_request.
 *
 * \par Purpose:
 * This API should be used to cancel check request created by cynara_async_create_request.
 *
 * \par Typical use case:
 * When cynara client is no longer interested in receiving an answer.
 *
 * \par Method of function operation:
 * Cancels request created by cynara_async_create_request call. cynara_status_callback callback may
 * be triggered to be able to send cancel to cynara. cynara_response_callback callback will
 * be triggered with with cynara_async_call_cause::CYNARA_CALL_CAUSE_CANCEL as cause param.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Call to cynara_async_cancel_request needs cynara_async structure to be created first. For this
 * use cynara_async_initialize.
 *
 * \param[in] p_cynara cynara_async structure.
 * \param[in] check_id Check id to be cancelled
 *
 * \return CYNARA_API_SUCCESS on success or other error code on error.
 */
int cynara_async_cancel_request(cynara_async *p_cynara, cynara_check_id check_id);

/*
 *  Example of usage:
 *
 *  void change_status(int old_fd, int new_fd, cynara_async_status status, void *user_data)
 *  {
 *      // unregister using old_fd
 *      ...
 *      //register using new_fd and new status
 *  }
 *
 *  int process_response(int check_id, int response, cynara_async_call_cause cause, void *user_data)
 *  {
 *      switch (cause) {
 *      case cynara_async_call_cause::CYNARA_CALL_CAUSE_ANSWER:
 *          // handle response
 *          break;
 *      case cynara_async_call_cause::CYNARA_CALL_CAUSE_CANCEL:
 *          // handle canceled request
 *          break;
 *      case cynara_async_call_cause::CYNARA_CALL_CAUSE_FINISH:
 *          // handle finish of client async
 *          break;
 *      case cynara_async_call_cause::CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE:
 *          // handle disconnection
 *      }
 *      ...
 *  }
 *
 *  void main_process
 *  {
 *      //initialize all required objects
 *      cynara_async *p_cynara;
 *      int ret;
 *      //change_status will be called passing file descriptor connected to cynara server
 *      if ((ret = cynara_async_initalize(&p_cynara, NULL, change_status, &fd_sets)) !=
 *            CYNARA_API_SUCCESS) {
 *          // handle error
 *      }
 *
 *      //start event loop
 *      event_process(fd_sets, ...)
 *  }
 *
 *  void event_process(fd_sets, ...)
 *  {
 *      //event loop of user choice
 *      select(..., fd_sets.read, fd_sets.write) {
 *         ...
 *         if(active_socket == cynara_socket) {
 *             if(cynara_async_process(p_cynara) != CYNARA_API_SUCCESS) {
 *                // handle error
 *             }
 *         } else if (active_socket == some_client_socket) {
 *             //processing clients, which may require cynara checks
 *             ...
 *             int ret = cynara_async_check(p_cynara, client, client_session, user, privilege);
 *             switch(ret) {
 *             case CYNARA_API_ACCESS_ALLOWED:
 *                 // allow client
 *                 break;
 *             case CYNARA_API_ACCESS_DENIED:
 *                 // deny client
 *                 break;
 *             case CYNARA_API_CACHE_MISS:
 *                 // not in cache - create request
 *                 ret = cynara_async_create_request(p_cynara,
 *                                                   client, client_session, user, privilege,
 *                                                   &check_id,
 *                                                   process_response, NULL);
 *                 if (ret == CYNARA_API_MAX_PENDING_REQUESTS) {
 *                     // too much pending requests - cancel some or just wait for answers
 *                 } else if(ret != CYNARA_API_SUCCESS) {
 *                     cynara_async_finish(p_cynara);
 *                     // handle error
 *                 }
 *             default:
 *                 cynara_async_finish(p_cynara);
 *                 // handle error
 *             }
 *             ...
 *             // waiting for some request too long
 *             if (ret = cynara_async_cancel_request(p_cynara, check_id)) != CYNARA_API_SUCCESS) {
 *                 cynara_async_finish(p_cynara);
 *                 // handle error
 *             }
 *         }
 *         ...
 *      }
 *      ...
 *  }
 */



#ifdef __cplusplus
}
#endif

#endif /* CYNARA_CLIENT_ASYNC_H */
