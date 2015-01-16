/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/chsgen/ChecksumGenerator.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       A micro-tool for computing checksums for Cynara's database contents
 */

#include <algorithm>
#include <cstring>
#include <iostream>
#include <unistd.h>

#include <cynara-error.h>

#include <config/PathConfig.h>
#include <exceptions/FileNotFoundException.h>
#include <storage/ChecksumValidator.h>

#include "ChecksumGenerator.h"

namespace Cynara {

const char ChecksumGenerator::m_fieldSeparator(PathConfig::StoragePath::fieldSeparator);
const char ChecksumGenerator::m_recordSeparator(PathConfig::StoragePath::recordSeparator);

int ChecksumGenerator::run(void) {
    try {
        openFileStream();
    } catch (const FileNotFoundException &ex) {
        std::cerr << ex.message() << std::endl;
        return CYNARA_API_INVALID_COMMANDLINE_PARAM;
    }

    copyFileStream();
    printRecord();
    return CYNARA_API_SUCCESS;
}

void ChecksumGenerator::openFileStream(void) {
    m_inputStream.open(m_filename);

    if (!m_inputStream.is_open()) {
        throw FileNotFoundException(m_filename);
    }
}

void ChecksumGenerator::copyFileStream(void) {
    std::copy(std::istreambuf_iterator<char>(m_inputStream),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(m_copyStream));
    m_inputStream.seekg(0);
}

void ChecksumGenerator::printRecord(void) const {
    std::cout << basename(m_filename.c_str()) << m_fieldSeparator
              << Cynara::ChecksumValidator::generate(m_copyStream.str()) << m_recordSeparator;
}

} /* namespace Cynara */
