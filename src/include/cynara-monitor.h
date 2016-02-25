/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/include/cynara-monitor.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains client APIs of Cynara monitoring.
 */

#ifndef CYNARA_MONITOR_H
#define CYNARA_MONITOR_H

#include <stddef.h>

#include <cynara-monitor-types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cynara_monitor cynara_monitor;
typedef struct cynara_monitor_configuration cynara_monitor_configuration;

/**
 * \par Description:
 * Initializes cynara_monitor_configuration. Creates structure used in following configuration
 * API calls.
 *
 * \par Purpose:
 * For configuration parameter to be used in cynara_monitor_initialize() function, this API must be
 * called before any other Cynara monitor configuration API function.
 * It will create cynara_monitor_configuration structure, an optional parameter
 * of Cynara monitor initialization.
 *
 * \par Typical use case:
 * Once before setting parameters of Cynara monitor configuration and passing to
 * cynara_monitor_initialize().
 *
 * \par Method of function operation:
 * This API initializes inner library structures and in case of success returns pointer
 * to created cynara_monitor_configuration structure.
 *
 * \par Sync (or) Async:
 * This as a synchronous API.
 *
 * \par Thread-safety:
 * \parblock
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * Structure cynara_monitor_configuration created by cynara_monitor_configuration_create() call
 * should be released with cynara_monitor_configuration_destroy().
 * Structure cynara_monitor_configuration should be destroyed after passing it to
 * cynara_monitor_initialize().
 *
 * \param[out] pp_conf Placeholder for created cynara_monitor_configuration structure.
 *
 * \return CYNARA_API_SUCCESS on success
 *         or negative error code on error.
 */
int cynara_monitor_configuration_create(cynara_monitor_configuration **pp_conf);

/**
 * \par Description:
 * Releases cynara_monitor_configuration structure created
 * with cynara_monitor_configuration_create().
 *
 * \par Purpose:
 * This API should be used to clean up after usage of cynara_monitor_configuration.
 *
 * \par Typical use case:
 * Once cynara_monitor_configuration is not needed.
 *
 * \par Method of function operation:
 * This API releases inner library structure and destroys cynara_monitor_configuration structure.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \param[in] p_conf cynara_monitor_configuration structure. If NULL, the call has no effect.
 */
void cynara_monitor_configuration_destroy(cynara_monitor_configuration *p_conf);

/**
 * \par Description:
 * Set monitor entries buffer size.
 *
 * \par Purpose:
 * This API is used to change default number of monitor entries stored on server side
 * before returning them to client of libcynara-monitor.
 *
 * \par Typical use case:
 * Once after cynara_configuration is created with cynara_monitor_configuration_create()
 * and before passing configuration to cynara_monitor_configuration().
 *
 * \par Method of function operation:
 * This API initializes buffer with given capacity.
 *
 * \par Sync (or) Async:
 * This as a synchronous API.
 *
 * \par Thread-safety:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into protected critical section.
 *
 * \par Important notes:
 * After passing cynara_configuration to cynara_monitor_initialize() calling this API will have
 * no effect.
 *
 * If buffer size is not set with cynara_monitor_configuration_set_buffer_size(),
 * a default size will be taken.
 * \endparblock
 *
 * \param[in] p_conf cynara_monitor_configuration structure pointer.
 * \param[in] buffer_size buffer size to be set.
 *
 * \return CYNARA_API_SUCCESS on success
 *        or negative error code on error.
 */
int cynara_monitor_configuration_set_buffer_size(cynara_monitor_configuration *p_conf,
                                                 size_t buffer_size);

/**
 * \par Description:
 * Initializes cynara-monitor library with given configuration.
 * Creates structure used in following API calls.
 *
 * \par Purpose:
 * This API must be used by prior calling cynara_check() function.
 *
 * \par Typical use case:
 * Once before a service can call cynara_monitor_entries_get().
 *
 * \par Method of function operation:
 * This API initializes inner library structures and in case of success
 * creates cynara_monitor structure.
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * Structure cynara_monitor created by cynara_monitor_initialize() call should be released
 * with cynara_monitor_finish().
 *
 * \param[out] pp_cynara_monitor Placeholder for created cynara_monitor structure.
 * \param[in] p_conf Configuration for cynara-monitor library. NULL for default parameters.
 * [TODO define and describe functions for custom parameters].
 *
 * \return CYNARA_API_SUCCESS on success, or error code on error.
 */
int cynara_monitor_initialize(cynara_monitor **pp_cynara_monitor,
                              const cynara_monitor_configuration *p_conf);

/**
 * \par Description:
 * Releases cynara-monitor library and destroys structure created with cynara_monitor_initialize().
 *
 * \par Purpose:
 * This API should be used to clean up after usage of cynara-monitor library.
 *
 * \par Typical use case:
 * Once after last call to cynara_monitor_entries_get().
 *
 * \par Method of function operation:
 * This API releases inner library structures and destroys cynara_monitor structure.
 *
 * \par Sync (or) Async:
 * This is a Synchronous API.
 *
 * \par Thread-safeness:
 * This function is NOT thread-safe. If functions from described API are called by multithreaded
 * application from different threads, they must be put into mutex protected critical section.
 *
 * \par Important notes:
 * No other call to libcynara-monitor should be made after call to cynara_monitor_finish().
 *
 * \param[in] p_cynara_monitor Cynara monitor structure.
 *
 * \return CYNARA_API_SUCCESS on success, or error code on error.
 */
int cynara_monitor_finish(cynara_monitor *p_cynara_monitor);

/**
 * \brief Returns monitor entries.
 *
 * \par Description:
 *
 * Returns all available monitor entries right after the size of buffer reaches set limit.
 *
 * \par Purpose:
 * This API should be used to get available monitor entries.
 *
 * \par Typical use case:
 * Gathering information about possible accesses checked in Cynara by individual services.
 *
 * \par Method of function operation:
 * \parblock
 * Each call to cynara_check() made by individual services are logged. The logs can be obtained
 * by privileged clients with usage of libcynara-monitor (this API).
 *
 * In case of successful call CYNARA_API_SUCCESS is returned and *monitor_entries points
 * to newly created array of pointers to cynara_monitor_entry. It is responsibility
 * of the caller to release entries with cynara_monitor_entries_free().
 *
 * The function blocks until the size of buffer reaches set limit or cynara_monitor_entries_flush()
 * is called from another thread.
 * \endparblock
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safeness:
 * This function is thread-safe.
 *
 * \par Important notes:
 * Although this function is thread-safe, no multiple calls are allowed.
 *
 * \param[in] p_cynara_monitor cynara_monitor structure.
 * \param[out] monitor_entries placeholder for NULL terminated array of pointers to
 *             monitor_entries structures.
 *
 * \return CYNARA_API_SUCCESS on success, or error code otherwise.
 */
int cynara_monitor_entries_get(cynara_monitor *p_cynara_monitor,
                               cynara_monitor_entry ***monitor_entries);

/**
 * \brief Makes cynara_monitor_entries_get() return immediately.
 *
 * \par Description:
 *
 * When called from another thread, makes cynara_monitor_entries_get() return immediately.
 *
 * \par Purpose:
 * This API should be used to make cynara_monitor_entries_get() return immediately.
 *
 * \par Typical use case:
 * User of libcynara-monitor wants to immediately get get monitor entries and/or make
 * cynara_monitor_entries_get() return without waiting for full batch of entries.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \par Thread-safeness:
 * This function is thread-safe.
 *
 * \param[in] p_cynara_monitor cynara_monitor structure.
 *
 * \return CYNARA_API_SUCCESS on success, or error code otherwise.
 */
int cynara_monitor_entries_flush(cynara_monitor *p_cynara_monitor);

/**
 * \brief Releases monitor entries.
 *
 * \par Description:
 *
 * Releases monitor entries obtained with cynara_monitor_entries_get().
 *
 * \par Purpose:
 * This API should be used to release array of cynara_monitor_entry obtained
 * with cynara_monitor_entries_get.
 *
 * \par Sync (or) Async:
 * This is a synchronous API.
 *
 * \param[in] monitor_entries NULL terminated array of cynara_monitor_entry to be released.
 */
void cynara_monitor_entries_free(cynara_monitor_entry **monitor_entries);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_MONITOR_H */
