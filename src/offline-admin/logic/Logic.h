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
 * @file        src/offline-admin/api/Logic.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of Logic class - main libcynara-offline-admin class
 */

#ifndef SRC_OFFLINE_ADMIN_API_LOGIC_H_
#define SRC_OFFLINE_ADMIN_API_LOGIC_H_

#include <memory>
#include <string>

#include <admin-common/api/ApiInterface.h>
#include <storage/Storage.h>
#include <storage/StorageBackend.h>

namespace Cynara {

class Logic : public ApiInterface {
public:
    Logic();
    virtual ~Logic() {};

    virtual int setPolicies(const PoliciesByBucket &insertOrUpdate,
                            const KeysByBucket &remove) noexcept;
    virtual int insertOrUpdateBucket(const PolicyBucketId &bucket,
                                     const PolicyResult &policyResult) noexcept;
    virtual int removeBucket(const PolicyBucketId &bucket) noexcept;

    virtual int adminCheck(const PolicyBucketId &startBucket, bool recursive,
                           const PolicyKey &key, PolicyResult &result) noexcept;

protected:
    void onPoliciesChanged(void);

private:
    std::unique_ptr<Storage> m_storage;
    std::unique_ptr<StorageBackend> m_storageBackend;
};

} // namespace Cynara

#endif /* SRC_OFFLINE_ADMIN_API_LOGIC_H_ */
