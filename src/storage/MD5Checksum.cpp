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
 * @file        src/storage/MD5Checksum.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file contains MD5Checksum implementation.
 */

#include <algorithm>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>
#include <unistd.h>

//#include <config/PathConfig.h>
#include <exceptions/ChecksumRecordCorruptedException.h>
#include <exceptions/CannotCreateFileException.h>
#include <exceptions/FileNotFoundException.h>
#include <log/log.h>
#include <storage/config/StorageConfig.h>
#include <storage/StorageSerializer.h>

#include "MD5Checksum.h"

namespace Cynara {

void MD5Checksum::load(bool isBackupValid) {
    auto suffix = std::string();
    if (isBackupValid) {
        suffix += StorageConfig::DatabaseConfig::backupFilenameSuffix;
    }
    auto inStream = openStream(suffix);

    m_lastSums.clear();

    for (std::size_t lineNum = 1; !inStream->eof(); ++lineNum) {
        std::string line;
        std::getline(*inStream, line, StorageSerializer::recordSeparator());

        if (line.empty()) {
            break;
        }

        try {
            std::size_t beginToken = 0;
            auto filename = parseFilename(line, beginToken);
            auto checksum = parseChecksum(line, beginToken);

            m_lastSums.insert({ filename, checksum });
        } catch (const ChecksumRecordCorruptedException &ex) {
            throw ex.withLineNumber(lineNum);
        }
    }
};

void MD5Checksum::save(std::unique_ptr<Checksums> newSums) const {
    const auto &outStream = openDumpStream();

    for (const auto &record : *newSums) {
        *outStream << record.first << StorageSerializer::fieldSeparator()
                   << generate(record.second) << StorageSerializer::recordSeparator();
    }

    newSums->clear();
};

const std::string MD5Checksum::generate(const std::string &data) const {
    return std::string(crypt(data.c_str(), "$1$"));
};

void MD5Checksum::compare(std::shared_ptr<std::ifstream> stream, const std::string &filename) {
    std::stringstream copyStream;
    std::string index(basename(filename.c_str()));

    std::copy(std::istreambuf_iterator<char>(*stream),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(copyStream));
    stream->seekg(0);

    if(m_lastSums[index] != generate(copyStream.str())) {
        throw ChecksumRecordCorruptedException(m_lastSums[index]);
    }
};

std::unique_ptr<std::ifstream> MD5Checksum::openStream(const std::string &filenameSuffix) const {
    auto filename(m_dbPath + StorageConfig::ChecksumConfig::checksumFilename + filenameSuffix);
    auto stream = std::unique_ptr<std::ifstream>(new std::ifstream());
    stream->open(filename);

    if (!stream->is_open()) {
        throw FileNotFoundException(filename);
    }

    return stream;
}

std::unique_ptr<std::ofstream> MD5Checksum::openDumpStream(void) const {
    auto filename(m_dbPath + StorageConfig::ChecksumConfig::checksumFilename
                           + StorageConfig::DatabaseConfig::backupFilenameSuffix);
    auto stream = std::unique_ptr<std::ofstream>(new std::ofstream());
    stream->open(filename, std::ofstream::out | std::ofstream::trunc);

    if (!stream->is_open()) {
        throw CannotCreateFileException(filename);
    }

    return stream;
}

const std::string MD5Checksum::parseFilename(const std::string &line, std::size_t &beginToken) {
    auto filenameEndToken = line.find(StorageSerializer::fieldSeparator(), beginToken);
    if (filenameEndToken != std::string::npos) {
        auto filename = line.substr(beginToken, filenameEndToken - beginToken);
        beginToken = filenameEndToken + 1;
        return filename;
    } else {
        throw ChecksumRecordCorruptedException(line);
    }
}

const std::string MD5Checksum::parseChecksum(const std::string &line, std::size_t &beginToken) {
    if (beginToken < line.size()) {
        auto checksum = line.substr(beginToken);
        beginToken = line.size();
        return checksum;
    } else {
        throw ChecksumRecordCorruptedException(line);
    }
}

} // namespace Cynara
