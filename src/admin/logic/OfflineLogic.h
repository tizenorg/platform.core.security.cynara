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
 * @file        src/admin/logic/OfflineLogic.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of OfflineLogic class
 */

#ifndef SRC_ADMIN_LOGIC_OFFLINELOGIC_H_
#define SRC_ADMIN_LOGIC_OFFLINELOGIC_H_

#include <memory>

#include <lock/FileLock.h>

#include <storage/Storage.h>
#include <storage/StorageBackend.h>

#include <api/ApiInterface.h>

namespace Cynara {

class OfflineLogic : public ApiInterface {
public:
    OfflineLogic();

    bool acquireDatabase(void);
    void releaseDatabase(void);

    int setPolicies(const ApiInterface::PoliciesByBucket &insertOrUpdate,
                    const ApiInterface::KeysByBucket &remove);
    int insertOrUpdateBucket(const PolicyBucketId &bucket,
                             const PolicyResult &policyResult);
    int removeBucket(const PolicyBucketId &bucket);
    int adminCheck(const PolicyBucketId &startBucket, bool recursive,
                   const PolicyKey &key, PolicyResult &result);
    int listPolicies(const PolicyBucketId &bucket, const PolicyKey &filter,
                     std::vector<Policy> &policies);
    int erasePolicies(const PolicyBucketId &startBucket, bool recursive,
                      const PolicyKey &filter);

protected:
    void onPoliciesChanged(void);

private:
    typedef std::unique_ptr<Storage> StorageUniquePtr;
    typedef std::unique_ptr<StorageBackend> StorageBackendUniquePtr;

    StorageUniquePtr m_storage;
    StorageBackendUniquePtr m_storageBackend;
    FileLock m_databaseLock;
};

} /* namespace Cynara */

#endif /* SRC_ADMIN_LOGIC_OFFLINELOGIC_H_ */
