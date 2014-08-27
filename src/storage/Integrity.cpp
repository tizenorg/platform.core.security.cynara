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
 * @file        src/storage/Integrity.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     0.1
 * @brief       Implementation of Cynara::Integrity
 */

#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <functional>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <log/log.h>
#include <exceptions/CannotCreateFileException.h>
#include <exceptions/UnexpectedErrorException.h>

#include "Integrity.h"

namespace Cynara {

bool Integrity::backupGuardExists(void) const {
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

void Integrity::createBackupGuard(void) const {
    std::string guardFileName = m_dbPath + m_guardFileName;
    std::ofstream guardStream(guardFileName, std::ofstream::out | std::ofstream::trunc);

    if (!guardStream.is_open()) {
        throw CannotCreateFileException(guardFileName);
    }

    guardStream.close();
}

void Integrity::revalidatePrimaryDatabase(const Buckets &buckets) {
    createPrimaryHardLinks(buckets);
    deleteHardLink(m_dbPath + m_guardFileName);
    deleteBackupHardLinks(buckets);
}

void Integrity::deleteNonIndexedFiles(BucketPresenceTester tester) {
    DIR *dirPtr = nullptr;
    struct dirent *direntPtr;

    if ((dirPtr = opendir(m_dbPath.c_str())) == nullptr) {
        int err = errno;
        LOGE("'opendir' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
        return;
    }

    while (errno = 0, (direntPtr = readdir(dirPtr)) != nullptr) {
        std::string filename = direntPtr->d_name;
        //ignore all special files (working dir, parent dir, index)
        if ("." == filename || ".." == filename || "buckets" == filename) {
            continue;
        }

        std::string bucketId;
        auto nameLength = filename.length();
        auto prefixLength = m_bucketFileNamePrefix.length();

        //remove if it is impossible that it is a bucket file
        if (nameLength < prefixLength) {
            deleteHardLink(m_dbPath + filename);
            continue;
        }

        //remove if there is no bucket filename prefix
        //0 is returned from string::compare() if strings are equal
        if (0 != filename.compare(0, prefixLength, m_bucketFileNamePrefix)) {
            deleteHardLink(m_dbPath + filename);
            continue;
        }

        //remove if bucket is not in index
        bucketId = filename.substr(prefixLength);
        if (!tester(bucketId)) {
            deleteHardLink(m_dbPath + filename);
        }
    }

    if (errno) {
        int err = errno;
        LOGE("'readdir' function error [%d] : <%s>", err, strerror(err));
        throw UnexpectedErrorException(err, strerror(err));
        return;
    }
}

void Integrity::createPrimaryHardLinks(const Buckets &buckets) {
    for (const auto &bucketIter : buckets) {
        const auto &bucketId = bucketIter.first;
        const auto &bucketFileName = m_dbPath + m_bucketFileNamePrefix + bucketId;

        deleteHardLink(bucketFileName);
        createHardLink(bucketFileName + m_backupFileNameSuffix, bucketFileName);
    }

    std::string indexFileName = m_dbPath + m_indexFileName;
    deleteHardLink(indexFileName);
    createHardLink(indexFileName + m_backupFileNameSuffix, indexFileName);
}

void Integrity::deleteBackupHardLinks(const Buckets &buckets) {
    for (const auto &bucketIter : buckets) {
        const auto &bucketId = bucketIter.first;
        const auto &bucketFileName = m_dbPath + m_bucketFileNamePrefix +
                                     bucketId + m_backupFileNameSuffix;

        deleteHardLink(bucketFileName);
    }

    deleteHardLink(m_dbPath + m_indexFileName + m_backupFileNameSuffix);
}

void Integrity::createHardLink(const std::string &oldName, const std::string &newName) {
    int ret = link(oldName.c_str(), newName.c_str());

    if (ret < 0) {
        int err = errno;
        throw UnexpectedErrorException(err, strerror(err));
        LOGN("Trying to link to non-existent file: <%s>", oldName.c_str());
    }
}

void Integrity::deleteHardLink(const std::string &fileName) {
    int ret = unlink(fileName.c_str());

    if (ret < 0) {
        int err = errno;
        if (err != ENOENT) {
            LOGE("'unlink' function error [%d] : <%s>", err, strerror(err));
            throw UnexpectedErrorException(err, strerror(err));
        } else {
            LOGN("Trying to unlink non-existent file: <%s>", fileName.c_str());
        }
    }
}

} /* namespace Cynara */
