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
 * @file        InMemoryStorageBackend.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of InMemoryStorageBackend
 */

#include <errno.h>
#include <dirent.h>
#include <fstream>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include <log/log.h>
#include <exceptions/BucketNotExistsException.h>
#include <exceptions/CannotCreateFileException.h>
#include <exceptions/DatabaseException.h>
#include <exceptions/FileNotFoundException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <types/PolicyBucket.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

#include <storage/BucketDeserializer.h>
#include <storage/StorageDeserializer.h>
#include <storage/StorageSerializer.h>

#include "InMemoryStorageBackend.h"

namespace Cynara {

const std::string InMemoryStorageBackend::m_indexFileName = "buckets";
const std::string InMemoryStorageBackend::m_guardFileName = "guard";
const std::string InMemoryStorageBackend::m_backupFileNameSuffix = "~";
const std::string InMemoryStorageBackend::m_bucketFileNamePrefix = "_";

void InMemoryStorageBackend::load(void) {
    bool isBackupValid = backupGuardExists();
    std::string bucketSuffix = "";
    std::string indexFilename = m_dbPath + m_indexFileName;

    if (isBackupValid) {
        bucketSuffix += m_backupFileNameSuffix;
        indexFilename += m_backupFileNameSuffix;
    }

    try {
        auto indexStream = std::make_shared<std::ifstream>();
        openFileStream(indexStream, indexFilename);

        StorageDeserializer storageDeserializer(indexStream,
            std::bind(&InMemoryStorageBackend::bucketStreamOpener, this,
                      std::placeholders::_1, bucketSuffix));

        storageDeserializer.initBuckets(buckets());
        storageDeserializer.loadBuckets(buckets());
    } catch (const DatabaseException &) {
        LOGC("Reading cynara database failed.");
        buckets().clear();
        // TODO: Implement emergency mode toggle
    }

    if (!hasBucket(defaultPolicyBucketId)) {
        LOGN("Creating defaultBucket.");
        this->buckets().insert({ defaultPolicyBucketId, PolicyBucket() });
    }

    if (isBackupValid) {
        revalidatePrimaryDatabase();
    }
    //in case there were unnecessary files in db directory
    deleteNonIndexedFiles();
}

void InMemoryStorageBackend::save(void) {

    //create directory
    int ret = mkdir(m_dbPath.c_str(), S_IRWXU);
    if (ret < 0) {
        int err = errno;
        if (err != EEXIST) {
            LOGE("Cannot create directory <%s>. Error [%d] : <%s>.",
                 m_dbPath.c_str(), err, strerror(err));
            throw UnexpectedErrorException(err, strerror(err));
        }
    }

    auto indexStream = std::make_shared<std::ofstream>();
    std::string indexFileName = m_dbPath + m_indexFileName;
    openDumpFileStream(indexStream, indexFileName + m_backupFileNameSuffix);

    StorageSerializer storageSerializer(indexStream);
    storageSerializer.dump(buckets(), std::bind(&InMemoryStorageBackend::bucketDumpStreamOpener,
                           this, std::placeholders::_1));

    createBackupGuard();
    revalidatePrimaryDatabase();
    //guard is removed during revalidation
}

PolicyBucket InMemoryStorageBackend::searchDefaultBucket(const PolicyKey &key) {
    return searchBucket(defaultPolicyBucketId, key);
}

PolicyBucket InMemoryStorageBackend::searchBucket(const PolicyBucketId &bucketId,
                                                  const PolicyKey &key) {
    try {
        const auto &bucket = this->buckets().at(bucketId);
        return bucket.filtered(key);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::insertPolicy(const PolicyBucketId &bucketId, PolicyPtr policy) {
    try {
        auto &bucket = buckets().at(bucketId);
        bucket.insertPolicy(policy);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::createBucket(const PolicyBucketId &bucketId,
                                          const PolicyResult &defaultPolicy) {
    PolicyBucket newBucket;
    newBucket.setDefaultPolicy(defaultPolicy);
    buckets().insert({ bucketId, newBucket });
}

void InMemoryStorageBackend::updateBucket(const PolicyBucketId &bucketId,
                                          const PolicyResult &defaultPolicy) {
    try {
        auto &bucket = buckets().at(bucketId);
        bucket.setDefaultPolicy(defaultPolicy);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::deleteBucket(const PolicyBucketId &bucketId) {
    auto bucketErased = buckets().erase(bucketId);
    if (bucketErased == 0) {
        throw BucketNotExistsException(bucketId);
    }
}

bool InMemoryStorageBackend::hasBucket(const PolicyBucketId &bucketId) {
    return buckets().find(bucketId) != buckets().end();
}

void InMemoryStorageBackend::deletePolicy(const PolicyBucketId &bucketId, const PolicyKey &key) {
    try {
        // TODO: Move the erase code to PolicyCollection maybe?
        auto &bucket = buckets().at(bucketId);
        bucket.deletePolicy(key);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::deleteLinking(const PolicyBucketId &bucketId) {
    auto bucketIdMatches = [&bucketId] (PolicyPtr policy) -> bool {
        auto policyResult = policy->result();

        // Check bucket id only if policy is a bucket policy
        // TODO: Maybe move the test to PolicyResult
        if (policyResult.policyType() == PredefinedPolicyType::BUCKET) {
            return policyResult.metadata() == bucketId;
        }
        return false;
    };

    for (auto &bucketIter : buckets()) {
        auto &bucket = bucketIter.second;
        bucket.deletePolicy(bucketIdMatches);
    }
}

void InMemoryStorageBackend::openFileStream(std::shared_ptr<std::ifstream> stream,
                                            const std::string &filename) {
    // TODO: Consider adding exceptions to streams and handling them:
    // stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    stream->open(filename);

    if (!stream->is_open()) {
        throw FileNotFoundException(filename);
    }
}

void InMemoryStorageBackend::openDumpFileStream(std::shared_ptr<std::ofstream> stream,
                                                const std::string &filename) {
    stream->open(filename, std::ofstream::out | std::ofstream::trunc);

    if (!stream->is_open()) {
        throw CannotCreateFileException(filename);
    }
}

std::shared_ptr<BucketDeserializer> InMemoryStorageBackend::bucketStreamOpener(
        const PolicyBucketId &bucketId, const std::string &fileNameSuffix) {
    std::string bucketFilename = m_dbPath + m_bucketFileNamePrefix + bucketId + fileNameSuffix;
    auto bucketStream = std::make_shared<std::ifstream>();
    try {
        openFileStream(bucketStream, bucketFilename);
        return std::make_shared<BucketDeserializer>(bucketStream);
    } catch (const FileNotFoundException &) {
        return nullptr;
    }
}

std::shared_ptr<StorageSerializer> InMemoryStorageBackend::bucketDumpStreamOpener(
        const PolicyBucketId &bucketId) {
    std::string bucketFilename = m_dbPath + m_bucketFileNamePrefix +
                                 bucketId + m_backupFileNameSuffix;
    auto bucketStream = std::make_shared<std::ofstream>();

    openDumpFileStream(bucketStream, bucketFilename);
    return std::make_shared<StorageSerializer>(bucketStream);
}

void InMemoryStorageBackend::createHardLink(const std::string &oldName,
                                            const std::string &newName) {
    int ret = link(oldName.c_str(), newName.c_str());

    if (ret < 0) {
        int err = errno;
        LOGE("'link' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
    }
}

void InMemoryStorageBackend::deleteHardLink(const std::string &fileName) {
    int ret = unlink(fileName.c_str());

    if (ret < 0) {
        int err = errno;
        if (err != ENOENT) {
            LOGE("'unlink' function error [%d] : <%s>", err, strerror(err));
            throw UnexpectedErrorException(err, strerror(err));
        } else {
            LOGN("Trying to unlink non-existent file.");
        }
    }
}

bool InMemoryStorageBackend::backupGuardExists(void) const {
    struct stat buffer;
    std::string guardFileName = m_dbPath + m_guardFileName;

    int ret = stat(guardFileName.c_str(), &buffer);

    if (ret == 0) {
        return true;
    } else {
        int err = errno;
        if (err != ENOENT) {
            LOGE("'stat' function error [%d] : <%s>", err, strerror(err));
            throw UnexpectedErrorException(err, strerror(err));
        }
        return false;
    }
}

void InMemoryStorageBackend::createBackupGuard(void) const {
    std::string guardFileName = m_dbPath + m_guardFileName;
    std::ofstream guardStream(guardFileName, std::ofstream::out | std::ofstream::trunc);

    if (!guardStream.is_open()) {
        throw CannotCreateFileException(guardFileName);
    }

    guardStream.close();
}

void InMemoryStorageBackend::revalidatePrimaryDatabase(void) {
    createPrimaryHardLinks();
    deleteHardLink(m_dbPath + m_guardFileName);
    deleteBackupHardLinks();
}

void InMemoryStorageBackend::deleteBackupHardLinks(void) {
    for (const auto &bucketIter : buckets()) {
        const auto &bucketId = bucketIter.first;
        const auto &bucketFileName = m_dbPath + m_bucketFileNamePrefix +
                                     bucketId + m_backupFileNameSuffix;

        deleteHardLink(bucketFileName);
    }

    deleteHardLink(m_dbPath + m_indexFileName + m_backupFileNameSuffix);
}

void InMemoryStorageBackend::createPrimaryHardLinks(void) {
    for (const auto &bucketIter : buckets()) {
        const auto &bucketId = bucketIter.first;
        const auto &bucketFileName = m_dbPath + m_bucketFileNamePrefix + bucketId;

        deleteHardLink(bucketFileName);
        createHardLink(bucketFileName + m_backupFileNameSuffix, bucketFileName);
    }

    std::string indexFileName = m_dbPath + m_indexFileName;
    deleteHardLink(indexFileName);
    createHardLink(indexFileName + m_backupFileNameSuffix, indexFileName);
}

void InMemoryStorageBackend::deleteNonIndexedFiles(void) {
    DIR *dirPtr = nullptr;
    struct dirent *direntPtr;

    if ((dirPtr = opendir(m_dbPath.c_str())) == nullptr) {
        int err = errno;
        LOGE("'opendir' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
        return;
    }

    while ((direntPtr = readdir(dirPtr)) != nullptr) {
        std::string filename = direntPtr->d_name;
        //ignore all special files (working dir, parent dir, index, guard)
        if ("." == filename || ".." == filename || "buckets" == filename || "guard" == filename) {
            continue;
        }

        std::string bucketId;
        auto nameLength = filename.length();
        auto prefixLength = m_bucketFileNamePrefix.length();
        auto suffixLength = m_backupFileNameSuffix.length();
        //ignore bucket files (backups as well, db revalidation handles this),
        //but remove all other files - these are not supposed to be here
        if(0 == filename.compare(nameLength - suffixLength, suffixLength, m_backupFileNameSuffix)) {
            bucketId = filename.substr(prefixLength, nameLength - suffixLength);
        } else {
            bucketId = filename.substr(prefixLength);
        }

        if (!hasBucket(bucketId)) {
            deleteHardLink(m_dbPath + filename);
        }
    }
}

} /* namespace Cynara */
