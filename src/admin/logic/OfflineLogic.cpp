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
/*
 * @file        OfflineLogic.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of OfflineLogic class
 */

#include <common.h>
#include <cynara-error.h>

#include <config/PathConfig.h>
#include <exceptions/BucketNotExistsException.h>
#include <exceptions/DatabaseBusyException.h>
#include <exceptions/DefaultBucketDeletionException.h>
#include <exceptions/DefaultBucketSetNoneException.h>
#include <storage/InMemoryStorageBackend.h>
#include <storage/Storage.h>

#include "OfflineLogic.h"

namespace Cynara {

OfflineLogic::OfflineLogic() {
    m_databaseLock = DatabaseLockPtr(new DatabaseLock(PathConfig::StoragePath::lockfile));

    if (m_databaseLock->tryLock() == false)
        throw DatabaseBusyException();

    m_storageBackend = StorageBackendPtr(new InMemoryStorageBackend(PathConfig::StoragePath::dbDir));
    m_storage = StoragePtr(new Storage(*m_storageBackend));
    m_storage->load();
}

int OfflineLogic::setPolicies(const ApiInterface::PoliciesByBucket &insertOrUpdate,
                              const ApiInterface::KeysByBucket &remove) {
    try {
        m_storage->insertPolicies(insertOrUpdate);
        m_storage->deletePolicies(remove);
        onPoliciesChanged();
    } catch (const BucketNotExistsException &ex) {
        return CYNARA_API_BUCKET_NOT_FOUND;
    }

    return CYNARA_API_SUCCESS;
}

int OfflineLogic::insertOrUpdateBucket(const PolicyBucketId &bucket,
                                       const PolicyResult &policyResult) {
    try {
        m_storage->addOrUpdateBucket(bucket, policyResult);
        onPoliciesChanged();
    } catch (const DefaultBucketSetNoneException &ex) {
        return CYNARA_API_OPERATION_NOT_ALLOWED;
    }

    return CYNARA_API_SUCCESS;
}

int OfflineLogic::removeBucket(const PolicyBucketId &bucket) {
    try {
        m_storage->deleteBucket(bucket);
        onPoliciesChanged();
    } catch (const BucketNotExistsException &ex) {
        return CYNARA_API_BUCKET_NOT_FOUND;
    } catch (const DefaultBucketDeletionException &ex) {
        return CYNARA_API_OPERATION_NOT_ALLOWED;
    }

    return CYNARA_API_SUCCESS;
}

int OfflineLogic::adminCheck(const PolicyBucketId &startBucket, bool recursive,
                             const PolicyKey &key, PolicyResult &result) {
    try {
        result = m_storage->checkPolicy(key, startBucket, recursive);
        onPoliciesChanged();
    } catch (const BucketNotExistsException &ex) {
        return CYNARA_API_BUCKET_NOT_FOUND;
    }

    return CYNARA_API_SUCCESS;
}

void OfflineLogic::onPoliciesChanged(void) {
    m_storage->save();
}

} /* namespace Cynara */
