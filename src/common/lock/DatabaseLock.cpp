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
 * @file        src/common/lock/DatabaseLock.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 */


#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <exceptions/CannotCreateFileException.h>
#include <exceptions/DatabaseLockAcquiringException.h>
#include <log/log.h>

#include "DatabaseLock.h"

namespace Cynara {

DatabaseLock::DatabaseLock(const std::string &lockFilename)
    : m_lockFilename(lockFilename), m_fd(-1)
{
    m_fd = ::open(m_lockFilename.c_str(), O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    if (m_fd < 0)
        throw CannotCreateFileException(m_lockFilename);

    LOGD("Database lock file opened");
}

DatabaseLock::~DatabaseLock() {
    ::close(m_fd);
}

bool DatabaseLock::tryLock(void) {
    int lock = TEMP_FAILURE_RETRY(::flock(m_fd, LOCK_EX | LOCK_NB));

    if (lock == 0) {
        LOGI("Database lock acquired");
        return true;
    } else if (errno == EWOULDBLOCK) {
        LOGI("Database lock NOT acquired");
        return false;
    }

    throw DatabaseLockAcquiringException(errno);
}

void DatabaseLock::lock(void) {
    int lock = TEMP_FAILURE_RETRY(::flock(m_fd, LOCK_EX));

    if (lock == -1)
        throw DatabaseLockAcquiringException(errno);

    LOGI("Database lock acquired");
}

} /* namespace Cynara */
