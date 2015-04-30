/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/config/PathConfig.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file specifies PathConfig namespace containing values of default cynara paths
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include <exceptions/CannotCreateFileException.h>
#include <log/log.h>

#include "PathConfig.h"

namespace Cynara {
namespace PathConfig {

//main paths
const std::string statePath(
#ifdef LOCAL_STATE_DIR
        LOCAL_STATE_DIR "/cynara/"
#else
        "/var/cynara/"
#endif
      );

const std::string libraryPath(
#ifdef LIB_DIR
        LIB_DIR "/cynara/"
#else
        "/usr/lib/cynara/"
#endif
      );

const std::string clientPath(
#ifdef RUN_DIR
        RUN_DIR "/cynara/"
#else
        "/run/cynara/"
#endif
      );

const std::string confPath(
#ifdef SYS_CONFIG_DIR
        SYS_CONFIG_DIR "/cynara/"
#else
        "/etc/cynara/"
#endif
      );

const std::string testsPath(
#ifdef DATA_ROOT_DIR
        DATA_ROOT_DIR "/cynara/tests/"
#else
        "/usr/share/cynara/tests/"
#endif
      );

namespace SocketPath {
const std::string client(clientPath + "cynara.socket");
const std::string admin(clientPath + "cynara-admin.socket");
const std::string agent(clientPath + "cynara-agent.socket");
} // namespace SocketPath

namespace StoragePath {
const std::string dbDir(statePath + "db/");
const std::string lockFile(statePath);
const std::string indexFilename("buckets");
const std::string guardFilename("guard");
const std::string checksumFilename("checksum");
const std::string backupFilenameSuffix("~");
const std::string bucketFilenamePrefix("_");
const char fieldSeparator(';');
const char recordSeparator('\n');
} // namespace StoragePath

namespace PluginPath {
const std::string clientDir(libraryPath + "plugin/client/");
const std::string serviceDir(libraryPath + "plugin/service/");
} // namespace PluginPath

static void makeDir(const std::string &path, mode_t mode) {
    int ret = mkdir(path.c_str(), mode);

    if (ret < 0) {
        int err = errno;
        if (err != EEXIST) {
            LOGE("'mkdir' function error [%d] : <%s>", err, strerror(err));
            throw CannotCreateFileException(path);
        }
    }
}

static void makePath(const std::string &path, mode_t mode) {
    // Create subsequent parent directories
    // Assume that path is absolute - i.e. starts with '/'
    for (size_t pos = 0; (pos = path.find("/", pos + 1)) != std::string::npos;)
        makeDir(path.substr(0, pos).c_str(), mode);

    makeDir(path, mode);
}

void makeSocketPath() {
    makePath(clientPath, 0755);
}

void makeDbPath() {
    makePath(StoragePath::dbDir, 0700);
}

} // namespace PathConfig
} // namespace Cynara



