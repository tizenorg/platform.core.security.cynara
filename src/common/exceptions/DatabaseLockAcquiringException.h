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
 * @file        src/common/exceptions/DatabaseLockAcquiringException.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file defines exception thrown when database lock cannot be acquired
 */

#ifndef SRC_COMMON_EXCEPTIONS_DATABASELOCKACQUIRINGEXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_DATABASELOCKACQUIRINGEXCEPTION_H_

#include <cstring>
#include <string>

#include <exceptions/DatabaseException.h>

namespace Cynara {

class DatabaseLockAcquiringException : public DatabaseException {
public:
    DatabaseLockAcquiringException(const int _errno) : m_errno(_errno) {};
    virtual ~DatabaseLockAcquiringException() {};

    const std::string message(void) const {
        if (m_message.empty()) {
            m_message = "Database lock acquiring error " + errorString();
        }
        return m_message;
    }

    int errorNumber(void) const {
        return m_errno;
    }

    const std::string errorString(void) const {
        return strerror(m_errno);
    }

private:
    mutable std::string m_message;
    int m_errno;
};

} /* namespace Cynara */

#endif /* SRC_COMMON_EXCEPTIONS_DATABASELOCKACQUIRINGEXCEPTION_H_ */
