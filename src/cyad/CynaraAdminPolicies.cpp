/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/cyad/CynaraAdminPolicies.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Collection of cynara_admin_policy structs
 */

#include <algorithm>
#include <cstring>
#include <stdexcept>

#include <cynara-admin-types.h>

#include "CynaraAdminPolicies.h"

namespace Cynara {

CynaraAdminPolicies::CynaraAdminPolicies() : m_sealed(false) {}

CynaraAdminPolicies::~CynaraAdminPolicies() {
    auto freePolicy = [] (cynara_admin_policy *admin_policy) {
        if (admin_policy == nullptr)
            return;

        free(admin_policy->bucket);
        free(admin_policy->client);
        free(admin_policy->user);
        free(admin_policy->privilege);
        free(admin_policy->result_extra);

        delete admin_policy;
    };

    std::for_each(m_policies.begin(), m_policies.end(), freePolicy);
}

void CynaraAdminPolicies::add(const PolicyBucketId &bucketId, const PolicyResult &policyResult,
                              const PolicyKey &policyKey)
{
    if (sealed()) {
        throw std::logic_error("Collection is sealed");
    }

    // TODO: Optimize -- try not to malloc every item
    // TODO: Could we use a vector of objects?
    cynara_admin_policy *policy = new cynara_admin_policy();

    policy->bucket = strdup(bucketId.c_str());
    policy->client = strdup(policyKey.client().toString().c_str());
    policy->user = strdup(policyKey.user().toString().c_str());
    policy->privilege = strdup(policyKey.privilege().toString().c_str());
    policy->result = policyResult.policyType();
    if (policyResult.metadata().empty())
        policy->result_extra = nullptr;
    else
        policy->result_extra = strdup(policyResult.metadata().c_str());

    m_policies.push_back(policy);
}
void CynaraAdminPolicies::seal() {
    m_policies.push_back(nullptr);
    m_sealed = true;
}

cynara_admin_policy* const *CynaraAdminPolicies::data() const {
    if (sealed() == false) {
        throw std::logic_error("Collection is not sealed");
    }

    return m_policies.data();
}

} /* namespace Cynara */
