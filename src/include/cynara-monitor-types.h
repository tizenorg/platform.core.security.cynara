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
 * \file        src/include/cynara-monitor-types.h
 * \author      Aleksander Zdyb <a.zdyb@samsung.com>
 * \version     1.0
 * \brief       This file contains structs and consts for Cynara monitor.
 */

#ifndef CYNARA_MONITOR_TYPES_H
#define CYNARA_MONITOR_TYPES_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct cynara_monitor_entry
 * \brief Defines single monitor entry
 */
typedef struct cynara_monitor_entry {
    char *client;               /**< Identifier of client (application) */
    char *user;                 /**< Identifier of user */
    char *privilege;            /**< Privilege name */
    int result;                 /**< Result of policy */
    struct timespec *timestamp; /**< Time of access */
} cynara_monitor_entry;

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_MONITOR_TYPES_H */
