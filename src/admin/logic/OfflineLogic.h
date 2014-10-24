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
 * @file        OfflineLogic.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of OfflineLogic class
 */
#ifndef SRC_ADMIN_LOGIC_OFFLINELOGIC_H_
#define SRC_ADMIN_LOGIC_OFFLINELOGIC_H_

#include <memory>

#include <api/ApiInterface.h>
#include <lock/DatabaseLock.h>
#include <storage/Storage.h>
#include <storage/StorageBackend.h>


namespace Cynara {

class OfflineLogic : public ApiInterface {
public:
    OfflineLogic();

    int setPolicies(const ApiInterface::PoliciesByBucket &insertOrUpdate,
                    const ApiInterface::KeysByBucket &remove);
    int insertOrUpdateBucket(const PolicyBucketId &bucket,
                             const PolicyResult &policyResult);
    int removeBucket(const PolicyBucketId &bucket);
    int adminCheck(const PolicyBucketId &startBucket, bool recursive,
                   const PolicyKey &key, PolicyResult &result);

protected:
    void onPoliciesChanged(void);

private:
    typedef std::unique_ptr<Storage> StoragePtr;
    typedef std::unique_ptr<StorageBackend> StorageBackendPtr;
    typedef std::unique_ptr<DatabaseLock> DatabaseLockPtr;

    StoragePtr m_storage;
    StorageBackendPtr m_storageBackend;
    DatabaseLockPtr m_databaseLock;

};

} /* namespace Cynara */

#endif /* SRC_ADMIN_LOGIC_OFFLINELOGIC_H_ */
