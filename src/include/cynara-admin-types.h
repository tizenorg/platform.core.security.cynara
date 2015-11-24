/*
 *  Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * \file        src/include/cynara-admin-types.h
 * \author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * \author      Aleksander Zdyb <a.zdyb@samsung.com>
 * \author      Zofia Abramowska <z.abramowska@samsung.com>
 * \author      Oskar Switalski <o.switalski@samsung.com>
 * \version     1.0
 * \brief       This file contains structs and consts for cynara admin.
 */

#ifndef CYNARA_ADMIN_TYPES_H
#define CYNARA_ADMIN_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct cynara_admin_policy
 * \brief Defines single policy
 *
 * \var cynara_admin_policy::bucket
 * Name of bucket, in which policy is placed
 *
 * \var cynara_admin_policy::client
 * Identifier of client (application)
 *
 * \var cynara_admin_policy::user
 * Identifier of user
 *
 * \var cynara_admin_policy::privilege
 * Privilege name
 *
 * \var cynara_admin_policy::result
 * Result of policy
 *
 * \var cynara_admin_policy::result_extra
 * Not always used, may contain some additional result data
 * like e.g. name of bucket in case result == CYNARA_ADMIN_BUCKET
 */
struct cynara_admin_policy {
    char *bucket;

    char *client;
    char *user;
    char *privilege;

    int result;
    char *result_extra;
};

/**
 * \struct cynara_admin_policy_descr
 * \brief Describes policy of type given with result
 *
 * \var cynara_admin_policy_descr::result
 * Result of policy to describe
 *
 * \var cynara_admin_policy_descr::name
 * Name of given policy result
 */

struct cynara_admin_policy_descr {
    int result;
    char *name;
};

/**
 * \name Wildcard
 * Can replace client, user or privilege name.
 * WILDCARD matches any string during check procedure from libcynara-client.
 */
#define CYNARA_ADMIN_WILDCARD "*"

/**
 * \name Name of Default Bucket
 * Default bucket - the one that check starts in.
 * Default bucket cannot be removed, although its default policy
 * (which originally is set to DENY) can be changed.
 */
#define CYNARA_ADMIN_DEFAULT_BUCKET ""

/**
 * \name Any
 * Can replace client, user or privilege name.
 * ANY matches any string (including WILDCARD) during:
 *
 * * policy removal with cynara_admin_erase() function
 * * listing policies from a single bucket.
 *
 * Using ANY as default policy for bucket or as policy type of inserted policy record
 * is forbidden and will cause CYNARA_API_INVALID_PARAM error.
 */
#define CYNARA_ADMIN_ANY "#"

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_ADMIN_TYPES_H */
