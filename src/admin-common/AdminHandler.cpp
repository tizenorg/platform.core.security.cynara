/* Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file        src/admin-common/ParamsParser.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 */

#include <cstring>

#include <common.h>
#include <cynara-admin-error.h>
#include <types/PolicyBucket.h>

#include "AdminHandler.h"

namespace Cynara {

namespace AdminHandler {

int handlePolicies(const struct cynara_admin_policy *const *policies, ApiInterface &logic) {

    if (!policies)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    Cynara::ApiInterface::PoliciesByBucket insertOrUpdate;
    Cynara::ApiInterface::KeysByBucket remove;

    auto key = ([](const cynara_admin_policy *policy) -> Cynara::PolicyKey {
        std::string wildcard(CYNARA_ADMIN_WILDCARD);

        auto feature = ([&wildcard] (const char *str) -> Cynara::PolicyKeyFeature {
            if (wildcard.compare(str))
                return Cynara::PolicyKeyFeature::create(str);
            else
                return Cynara::PolicyKeyFeature::createWildcard();
        });

        return Cynara::PolicyKey(feature(policy->client), feature(policy->user),
                                 feature(policy->privilege));
    });

    try {
        for (auto i = policies; *i; i++) {
            const cynara_admin_policy *policy = *i;
            if (!policy->bucket || !policy->client || !policy->user || !policy->privilege)
                return CYNARA_ADMIN_API_INVALID_PARAM;

            switch (policy->result) {
                case CYNARA_ADMIN_DELETE:
                    remove[policy->bucket].push_back(key(policy));
                    break;
                case CYNARA_ADMIN_DENY:
                    insertOrUpdate[policy->bucket].push_back(Cynara::Policy(key(policy),
                                                        Cynara::PredefinedPolicyType::DENY));
                    break;
                case CYNARA_ADMIN_ALLOW:
                    insertOrUpdate[policy->bucket].push_back(Cynara::Policy(key(policy),
                                                        Cynara::PredefinedPolicyType::ALLOW));
                    break;
                case CYNARA_ADMIN_BUCKET:
                    if (!policy->result_extra)
                        return CYNARA_ADMIN_API_INVALID_PARAM;
                    insertOrUpdate[policy->bucket].push_back(Cynara::Policy(key(policy),
                                                        Cynara::PolicyResult(
                                                        Cynara::PredefinedPolicyType::BUCKET,
                                                        policy->result_extra)));
                    break;
                case CYNARA_ADMIN_NONE:
                default:
                    return CYNARA_ADMIN_API_INVALID_PARAM;
            }
        }
    } catch (const std::bad_alloc &ex) {
        return CYNARA_ADMIN_API_OUT_OF_MEMORY;
    }

    return logic.setPolicies(insertOrUpdate, remove);;

}

int handleBucket(const char *bucket, int operation, const char *extra, ApiInterface &logic) {

    if (!bucket)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    std::string extraStr;
    try {
         extraStr = extra ? extra : "";
    } catch (const std::bad_alloc &ex) {
        return CYNARA_ADMIN_API_OUT_OF_MEMORY;
    }

    switch (operation) {
        case CYNARA_ADMIN_DELETE:
            return logic.removeBucket(bucket);
        case CYNARA_ADMIN_DENY:
            return logic.insertOrUpdateBucket(bucket,
                Cynara::PolicyResult(Cynara::PredefinedPolicyType::DENY, extraStr));
        case CYNARA_ADMIN_ALLOW:
            return logic.insertOrUpdateBucket(bucket,
                Cynara::PolicyResult(Cynara::PredefinedPolicyType::ALLOW, extraStr));
        case CYNARA_ADMIN_NONE:
            if (bucket != Cynara::defaultPolicyBucketId) {
                return logic.insertOrUpdateBucket(bucket, Cynara::PredefinedPolicyType::NONE);
            }
            return CYNARA_ADMIN_API_OPERATION_NOT_ALLOWED;
        case CYNARA_ADMIN_BUCKET:
        default:
            return CYNARA_ADMIN_API_INVALID_PARAM;
    }
}

int handleCheck(const char *start_bucket, const int recursive,
                               const char *client, const char *user, const char *privilege,
                               int *result, char **result_extra, ApiInterface &logic) {

    if (!start_bucket)
        return CYNARA_ADMIN_API_INVALID_PARAM;
    if (!client || !user || !privilege)
        return CYNARA_ADMIN_API_INVALID_PARAM;
    if (!result || !result_extra)
        return CYNARA_ADMIN_API_INVALID_PARAM;

    Cynara::PolicyResult policyResult;

    try {
        int ret = logic.adminCheck(start_bucket, recursive != 0,
                                    Cynara::PolicyKey(client, user, privilege),
                                    policyResult);
        if (ret != CYNARA_ADMIN_API_SUCCESS)
            return ret;
    } catch (const std::bad_alloc &ex) {
        return CYNARA_ADMIN_API_OUT_OF_MEMORY;
    } catch (const std::length_error &ex) {
        return CYNARA_ADMIN_API_INVALID_PARAM;
    }

    char *str = nullptr;
    if (!policyResult.metadata().empty()) {
        str = strdup(policyResult.metadata().c_str());
        if (!str)
            return CYNARA_ADMIN_API_OUT_OF_MEMORY;
    }
    *result = static_cast<int>(policyResult.policyType());
    *result_extra = str;

    return CYNARA_ADMIN_API_SUCCESS;
}

}  // namespace ParamsParser

}  // namespace Cynara
