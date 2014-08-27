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
 * @file        src/storage/Integrity.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     0.1
 * @brief       Headers for Cynara::Integrity
 */

#ifndef SRC_STORAGE_INTEGRITY_H_
#define SRC_STORAGE_INTEGRITY_H_

#include <string>

#include <storage/Buckets.h>

namespace Cynara {

class Integrity
{
public:
    typedef std::function<bool(const PolicyBucketId &)> BucketPresenceTester;
    Integrity(const std::string &path, const std::string &index, const std::string &guard,
              const std::string &backupSuffix, const std::string &bucketPrefix)
    : m_dbPath(path), m_indexFileName(index), m_guardFileName(guard),
      m_backupFileNameSuffix(backupSuffix), m_bucketFileNamePrefix(bucketPrefix) {
    }
    virtual ~Integrity() {};

    virtual bool backupGuardExists(void) const;
    virtual void createBackupGuard(void) const;
    virtual void revalidatePrimaryDatabase(const Buckets &buckets);
    virtual void deleteNonIndexedFiles(BucketPresenceTester tester);

protected:
    void createPrimaryHardLinks(const Buckets &buckets);
    void deleteBackupHardLinks(const Buckets &buckets);

    static void createHardLink(const std::string &oldName, const std::string &newName);
    static void deleteHardLink(const std::string &filename);

private:
    std::string m_dbPath;
    std::string m_indexFileName;
    std::string m_guardFileName;
    std::string m_backupFileNameSuffix;
    std::string m_bucketFileNamePrefix;
};

} // namespace Cynara

#endif /* SRC_STORAGE_INTEGRITY_H_ */
