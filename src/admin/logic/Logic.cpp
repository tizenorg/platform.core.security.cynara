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
/**
 * @file        src/admin/logic/Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main libcynara-admin class
 */

#include <memory>

#include <common.h>
#include <exceptions/DatabaseBusyException.h>
#include <log/log.h>

#include "Logic.h"
#include "OfflineLogic.h"
#include "OnlineLogic.h"

namespace Cynara {

Logic::Logic() : m_onlineLogic(std::make_shared<OnlineLogic>()) {}

int Logic::setPolicies(const ApiInterface::PoliciesByBucket &insertOrUpdate,
                       const ApiInterface::KeysByBucket &remove) {
    return api()->setPolicies(insertOrUpdate, remove);
}

int Logic::insertOrUpdateBucket(const PolicyBucketId &bucket,
                                const PolicyResult &policyResult) {
    return api()->insertOrUpdateBucket(bucket, policyResult);
}

int Logic::removeBucket(const PolicyBucketId &bucket) {
    return api()->removeBucket(bucket);
}

int Logic::adminCheck(const PolicyBucketId &startBucket, bool recursive, const PolicyKey &key,
                      PolicyResult &result) {
    return api()->adminCheck(startBucket, recursive, key, result);
}

int Logic::listPolicies(const PolicyBucketId &bucket, const PolicyKey &filter,
                        std::vector<Policy> &policies) {
    return api()->listPolicies(bucket, filter, policies);
}

std::shared_ptr<ApiInterface> Logic::api(void) {
    try {
        auto offlineLogic = std::make_shared<OfflineLogic>();
        LOGI("Admin uses offline API");
        return offlineLogic;
    } catch (const DatabaseBusyException &) {
        LOGI("Admin uses online API");
        return m_onlineLogic;
    }
}

} // namespace Cynara
