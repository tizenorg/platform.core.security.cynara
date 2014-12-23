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
 * @file        src/storage/MD5Checksum.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file contains MD5Checksum header.
 */

#ifndef SRC_STORAGE_MD5CHECKSUM_H_
#define SRC_STORAGE_MD5CHECKSUM_H_

#include <storage/ChecksumInterface.h>

namespace Cynara {

class MD5Checksum : public ChecksumInterface {
public:
    MD5Checksum(const std::string &path) : m_dbPath(path) {}

    void load(bool isBackupValid);
    void save(std::unique_ptr<Checksums> newSums) const;
    void compare(std::shared_ptr<std::ifstream> stream, const std::string &data);

private:
    const std::string generate(const std::string &data) const;

    std::unique_ptr<std::ofstream> openDumpStream(void) const;
    std::unique_ptr<std::ifstream> openStream(const std::string &filenameSuffix) const;

    static const std::string parseFilename(const std::string &line, std::size_t &beginToken);
    static const std::string parseChecksum(const std::string &line, std::size_t &beginToken);

    const std::string m_dbPath;
};

} // namespace Cynara

#endif // SRC_STORAGE_MD5CHECKSUM_H_
