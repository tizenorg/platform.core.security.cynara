/**
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
 * \file        cynara-admin-policy.h
 * \author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * \author      Aleksander Zdyb <a.zdyb@samsung.com>
 * \version     1.0
 * \brief       This file contains cynara_admin_policy struct.
 */

#ifndef CYNARA_STORAGE_H
#define CYNARA_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name cynara_admin_policy
 * defines single policy
 * bucket - is the name of bucket, in which policy is placed
 * client, user, privilege - defines policy key
 * result - defines result of policy
 * result_extra - not always used, may contain some additional result data
 *                like e.g. name of bucket in case result == CYNARA_ADMIN_BUCKET
 */
struct cynara_admin_policy {
    char *bucket;

    char *client;
    char *user;
    char *privilege;

    int result;
    char *result_extra;
};


#ifdef __cplusplus
}
#endif

#endif /* CYNARA_STORAGE_H */
