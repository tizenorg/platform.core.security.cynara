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
 * @file        src/storage/config/StorageConfig.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       This file specifies StorageConfig namespace containing default cynara storage consts
 */

#ifndef SRC_STORAGE_CONFIG_STORAGECONFIG_H_
#define SRC_STORAGE_CONFIG_STORAGECONFIG_H_

#include <string>

namespace Cynara {
namespace StorageConfig {

namespace DatabaseConfig {

extern const std::string indexFilename;
extern const std::string backupFilenameSuffix;
extern const std::string bucketFilenamePrefix;

} // namespace DatabaseConfig

namespace ChecksumConfig {

extern const std::string checksumFilename;

} // namespace ChecksumConfig

} // namespace StorageConfig
} // namespace Cynara

#endif /* SRC_STORAGE_CONFIG_STORAGECONFIG_H_ */
