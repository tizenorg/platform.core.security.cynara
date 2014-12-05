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
 * @file        src/storage/ChecksumInterface.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file contains checksum interface definitions.
 */

#ifndef SRC_STORAGE_CHECKSUMINTERFACE_H_
#define SRC_STORAGE_CHECKSUMINTERFACE_H_

#include <fstream>
#include <map>
#include <memory>
#include <string>

namespace Cynara {

class ChecksumInterface;
typedef std::unique_ptr<ChecksumInterface> ChecksumUniquePtr;

class ChecksumInterface {
public:
    typedef std::unordered_map<std::string, std::string> Checksums;

    virtual void load(bool isBackupValid) = 0;
    virtual void save(std::unique_ptr<Checksums> newSums) const = 0;
    virtual void compare(std::shared_ptr<std::ifstream> stream, const std::string &data) = 0;

    virtual void clear(void) {
        m_lastSums.clear();
    }

    virtual ~ChecksumInterface() {}

protected:
    virtual const std::string generate(const std::string &data) const = 0;

    virtual std::unique_ptr<std::ofstream> openDumpStream(void) const = 0;
    virtual std::unique_ptr<std::ifstream> openStream(const std::string &filenameSuffix) const = 0;

    Checksums m_lastSums;
};

} // namespace Cynara

#endif // SRC_STORAGE_CHECKSUMINTERFACE_H_
