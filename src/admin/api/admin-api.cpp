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
 * @file        admin-api.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of external libcynara-admin API
 */

#include <map>
#include <new>
#include <stdexcept>
#include <string>
#include <string.h>
#include <vector>

#include <common.h>
#include <log/log.h>
#include <types/Policy.h>
#include <types/PolicyBucket.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

#include <admin-common/api/ApiInterface.h>
#include <admin-common/AdminHandler.h>
#include <cynara-admin.h>
#include <cynara-admin-error.h>

#include <logic/Logic.h>

struct cynara_admin {
    Cynara::ApiInterface *impl;

    cynara_admin(Cynara::ApiInterface *_impl) : impl(_impl) {
    }
    ~cynara_admin() {
        delete impl;
    }
};

CYNARA_API
int cynara_admin_initialize(struct cynara_admin **pp_cynara_admin) {
    if (!pp_cynara_admin)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    try {
        *pp_cynara_admin = new cynara_admin(new Cynara::Logic);
    } catch (const std::bad_alloc &ex) {
        return CYNARA_ADMIN_API_OUT_OF_MEMORY;
    }

    init_log();

    LOGD("Cynara admin initialized");

    return CYNARA_ADMIN_API_SUCCESS;
}

CYNARA_API
int cynara_admin_finish(struct cynara_admin *p_cynara_admin) {
    delete p_cynara_admin;

    return CYNARA_ADMIN_API_SUCCESS;
}

CYNARA_API
int cynara_admin_set_policies(struct cynara_admin *p_cynara_admin,
                              const struct cynara_admin_policy *const *policies) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    return Cynara::AdminHandler::handlePolicies(policies, *p_cynara_admin->impl);
}

CYNARA_API
int cynara_admin_set_bucket(struct cynara_admin *p_cynara_admin, const char *bucket,
                            int operation, const char *extra) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    return Cynara::AdminHandler::handleBucket(bucket, operation, extra, *p_cynara_admin->impl);
}

CYNARA_API
int cynara_admin_check(struct cynara_admin *p_cynara_admin,
                       const char *start_bucket, const int recursive,
                       const char *client, const char *user, const char *privilege,
                       int *result, char **result_extra) {
    if (!p_cynara_admin || !p_cynara_admin->impl)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    return Cynara::AdminHandler::handleCheck(start_bucket, recursive, client, user, privilege,
                                            result, result_extra, *p_cynara_admin->impl);
}
