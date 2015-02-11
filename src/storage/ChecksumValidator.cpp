/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/storage/ChecksumValidator.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file contains ChecksumValidator implementation.
 */

#include <algorithm>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>
#include <unistd.h>

#include <config/PathConfig.h>
#include <exceptions/ChecksumRecordCorruptedException.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>

#include "ChecksumValidator.h"

namespace Cynara {

const std::string ChecksumValidator::m_checksumFilename(PathConfig::StoragePath::checksumFilename);
const std::string ChecksumValidator::m_backupFilenameSuffix(
        PathConfig::StoragePath::backupFilenameSuffix);

void ChecksumValidator::load(const std::shared_ptr<std::istream> &stream) {
    m_sums.clear();

    for (std::size_t lineNum = 1; !stream->eof(); ++lineNum) {
        std::string line;
        std::getline(*stream, line, PathConfig::StoragePath::recordSeparator);

        if (line.empty()) {
            break;
        }

        try {
            std::size_t beginToken = 0;
            auto filename = parseFilename(line, beginToken);
            auto checksum = parseChecksum(line, beginToken);

            m_sums.insert({ filename, checksum });
        } catch (const ChecksumRecordCorruptedException &ex) {
            throw ex.withLineNumber(lineNum);
        }
    }
};

const std::string ChecksumValidator::generate(const std::string &data) {
    auto checksum = crypt(data.c_str(), "$1$");

    if (nullptr != checksum) {
        return std::string(checksum);
    } else {
        int err = errno;
        if (err == ENOSYS) {
            LOGE("'crypt' function was not implemented; error [%d] : <%s>", err, strerror(err));
        } else {
            LOGE("'crypt' function error [%d] : <%s>", err, strerror(err));
        }
        throw UnexpectedErrorException(err, strerror(err));
    }
};

void ChecksumValidator::compare(const std::shared_ptr<std::istream> &stream,
                                const std::string &pathname, bool isBackupValid) {
    if (isChecksumIndex(pathname)) {
        return;
    }

    std::stringstream copyStream;
    std::string filename(basename(pathname.c_str()));

    if (isBackupValid) {
        filename.erase(filename.rfind(m_backupFilenameSuffix));
    }

    std::copy(std::istreambuf_iterator<char>(*stream),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(copyStream));
    stream->seekg(0);

    if (m_sums[filename] != generate(copyStream.str())) {
        throw ChecksumRecordCorruptedException(m_sums[filename]);
    }
};

const std::string ChecksumValidator::parseFilename(const std::string &line,
                                                   std::size_t &beginToken) {
    auto filenameEndToken = line.find(PathConfig::StoragePath::fieldSeparator, beginToken);
    if (filenameEndToken != beginToken && filenameEndToken != std::string::npos) {
        auto filename = line.substr(beginToken, filenameEndToken - beginToken);
        beginToken = filenameEndToken + 1;
        return filename;
    } else {
        throw ChecksumRecordCorruptedException(line);
    }
}

const std::string ChecksumValidator::parseChecksum(const std::string &line,
                                                   std::size_t &beginToken) {
    if (beginToken < line.size()) {
        auto checksum = line.substr(beginToken);
        beginToken = line.size();
        return checksum;
    } else {
        throw ChecksumRecordCorruptedException(line);
    }
}

bool ChecksumValidator::isChecksumIndex(const std::string &filename) const {
    auto checksum = m_dbPath + m_checksumFilename;
    return filename == checksum || filename == checksum + m_backupFilenameSuffix;
}

} // namespace Cynara
