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
 * @file        src/offline-admin/api/Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class
 *              - main libcynara-offline-admin class
 */

#include <memory>
#include <string>

#include <common.h>
#include <cynara-admin-error.h>
#include <exceptions/BucketNotExistsException.h>
#include <exceptions/DefaultBucketDeletionException.h>
#include <exceptions/DefaultBucketSetNoneException.h>
#include <log/log.h>
#include <storage/InMemoryStorageBackend.h>
#include <storage/Storage.h>

#include "Logic.h"

namespace Cynara {

// TODO: This is copied from Cynara::Cynara. Move it to some common place.
static std::string storageDir(void) {
    std::string dir("/var/lib/cynara/");

#ifdef CYNARA_STATE_PATH
    dir = CYNARA_STATE_PATH;
#else
    LOGW("Cynara compiled without CYNARA_STATE_PATH flag. Using default database directory.");
#endif

    dir += "db/";
    LOGI("Cynara database path <%s>", dir.c_str());
    return dir;
}

Logic::Logic() {
    m_storageBackend = std::unique_ptr<StorageBackend>(new InMemoryStorageBackend(storageDir()));
    m_storage = std::unique_ptr<Storage>(new Storage(*m_storageBackend));
    m_storage->load();
}

int Logic::setPolicies(const PoliciesByBucket &insertOrUpdate,
                       const KeysByBucket &remove) noexcept {

    try {
        m_storage->insertPolicies(insertOrUpdate);
        m_storage->deletePolicies(remove);
        onPoliciesChanged();
    } catch (const BucketNotExistsException &ex) {
        return CYNARA_ADMIN_API_BUCKET_NOT_FOUND;
    }

    return CYNARA_ADMIN_API_SUCCESS;
}

int Logic::insertOrUpdateBucket(const PolicyBucketId &bucketId,
                                const PolicyResult &policy) noexcept {

    try {
        m_storage->addOrUpdateBucket(bucketId, policy);
        onPoliciesChanged();
    } catch (const DefaultBucketSetNoneException &ex) {
        return CYNARA_ADMIN_API_OPERATION_NOT_ALLOWED;
    }

    return CYNARA_ADMIN_API_SUCCESS;
}

int Logic::removeBucket(const PolicyBucketId &bucketId) noexcept {

    try {
        m_storage->deleteBucket(bucketId);
        onPoliciesChanged();
    } catch (const BucketNotExistsException &ex) {
        return CYNARA_ADMIN_API_BUCKET_NOT_FOUND;
    } catch (const DefaultBucketDeletionException &ex) {
        return CYNARA_ADMIN_API_OPERATION_NOT_ALLOWED;
    }

    return CYNARA_ADMIN_API_SUCCESS;
}

int Logic::adminCheck(const PolicyBucketId &startBucket UNUSED, bool recursive UNUSED,
                      const PolicyKey &key UNUSED, PolicyResult &result UNUSED) noexcept {

    try {
        result = m_storage->checkPolicy(key, startBucket, recursive);
        onPoliciesChanged();
    } catch (const BucketNotExistsException &ex) {
        return CYNARA_ADMIN_API_BUCKET_NOT_FOUND;
    }

    return CYNARA_ADMIN_API_SUCCESS;
}

void Logic::onPoliciesChanged(void) {
    m_storage->save();
}

} // namespace Cynara
