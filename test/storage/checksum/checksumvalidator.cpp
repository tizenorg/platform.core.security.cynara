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
 * @file        test/storage/checksum/checksumvalidator.cpp
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Tests of ChecksumValidator
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include <config/PathConfig.h>
#include <exceptions/ChecksumRecordCorruptedException.h>
#include <storage/ChecksumValidator.h>

#include "checksumvalidatorfixture.h"
#include "fakechecksumvalidator.h"

using namespace Cynara;

const std::string ChecksumValidatorFixture::m_dbPath("/fake/path/");
const std::string ChecksumValidatorFixture::m_filename("fakeFilename");
const std::string ChecksumValidatorFixture::m_checksum("$1$$fakeChecksum");
const std::string ChecksumValidatorFixture::m_backupFilenameSuffix(
        PathConfig::StoragePath::backupFilenameSuffix);
const char ChecksumValidatorFixture::m_fieldSeparator(PathConfig::StoragePath::fieldSeparator);
const char ChecksumValidatorFixture::m_recordSeparator(PathConfig::StoragePath::recordSeparator);
const size_t ChecksumValidatorFixture::m_firstLine(1);

/*
 * @brief   Verify if generate() can use systems' crypt(3) implementation
 * @test    Expected result: no exceptions are thrown and non-empty string is returned
 */
TEST_F(ChecksumValidatorFixture, generateChecksum) {
    for (const auto &data : { std::string("test-data"), std::string() }) {
        SCOPED_TRACE(data);

        std::string returnedChecksum;
        ASSERT_NO_THROW(returnedChecksum = ChecksumValidator::generate(data));
        ASSERT_NE(std::string(), returnedChecksum);
    }
}

/*
 * @brief   Verify if load() can successfully parse sample checksum record
 * @test    Expected result:
 * - no exceptions are thrown from load()
 * - proper checksums are stored for corresponding files
 */
TEST_F(ChecksumValidatorFixture, parseCorrectRecords) {
    auto checksums = std::make_shared<std::istringstream>(m_filename + m_fieldSeparator +
                                                          m_checksum);

    FakeChecksumValidator validator(m_dbPath);
    ASSERT_NO_THROW(validator.load(checksums));
    ASSERT_EQ(m_checksum, validator.sums().at(m_filename));
}

/*
 * @brief   Verify if load() rejects storing corrupted checksum records
 * @test    Expected result:
 * - load() throws ChecksumRecordCorruptedException with proper contents
 * - no data is stored (there was no correct data to insert)
 */
TEST_F(ChecksumValidatorFixture, parseCorruptedRecords) {
    using ::testing::StartsWith;

    const auto badLines =  { m_filename + m_fieldSeparator,
                             m_filename + m_checksum,
                             m_fieldSeparator + m_checksum };

    FakeChecksumValidator validator(m_dbPath);

    for (const auto &badLine : badLines) {
        SCOPED_TRACE(badLine);
        auto checksums = std::make_shared<std::istringstream>(badLine);

        try {
            ASSERT_THROW(validator.load(checksums), ChecksumRecordCorruptedException);
        } catch (const ChecksumRecordCorruptedException &ex) {
            ASSERT_EQ(badLine, ex.line());
            ASSERT_EQ(m_firstLine, ex.lineNumber());
            ASSERT_THAT(ex.what(), StartsWith("Checksum record"));
            ASSERT_TRUE(validator.sums().empty());
        }
    }
}

/*
 * @brief   Verify if compare() can successfully check sample database file
 * @test    Expected result: no exceptions are thrown and file contents are still readable
 */
TEST_F(ChecksumValidatorFixture, compareBasicAndBackup) {
    FakeChecksumValidator validator(m_dbPath);
    validator.load(minimalChecksumFile());

    std::unordered_map<std::string, std::string> files = { { "_", std::string() },
                                                           { "buckets", ";0x0;\n" } };

    for (const auto &file : files) {
        const auto filename = m_dbPath + file.first;
        const auto contents = file.second;
        const auto fakeFile = std::make_shared<std::istringstream>(contents);

        SCOPED_TRACE(filename);

        ASSERT_NO_THROW(validator.compare(fakeFile, filename, false));
        ASSERT_NO_THROW(validator.compare(fakeFile, filename + m_backupFilenameSuffix, true));

        ASSERT_EQ(contents, fakeFile->str());
    }
}

/*
 * @brief   Verify if compare()
 * @test    Expected result: ChecksumRecordCorruptedException with proper contents is thrown
 */
TEST_F(ChecksumValidatorFixture, checksumMismatch) {
    using ::testing::StartsWith;

    FakeChecksumValidator validator(m_dbPath);
    validator.load(minimalChecksumFile());

    // Please note that default policy is set to ALLOW instead of DENY
    std::unordered_map<std::string, std::string> files = { { "_", "client;user;privilege;0x0;" },
                                                           { "buckets", ";0xFFFF;\n" } };

    for (const auto &file : files) {
        const auto filename = m_dbPath + file.first;
        const auto contents = file.second;
        const auto fakeFile = std::make_shared<std::istringstream>(contents);

        SCOPED_TRACE(filename);

        try {
            ASSERT_THROW(validator.compare(fakeFile, filename, false),
                         ChecksumRecordCorruptedException);
        } catch (const ChecksumRecordCorruptedException &ex) {
            ASSERT_EQ(filename, ex.line());
            ASSERT_THAT(ex.what(), StartsWith("Checksum record"));
        }
    }
}
