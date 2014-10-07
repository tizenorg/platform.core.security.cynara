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
 * @file        src/common/exceptions/PluginErrorException.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       Implementation of PluginErrorException
 */
#ifndef SRC_COMMON_EXCEPTIONS_PLUGINERROREXCEPTION_H_
#define SRC_COMMON_EXCEPTIONS_PLUGINERROREXCEPTION_H_

#include <exceptions/Exception.h>
#include <types/PolicyKey.h>

namespace Cynara {

class PluginErrorException : public Exception {
public:
    PluginErrorException(const PolicyKey &key) {
        m_message = "Plugin couldn't get result for user <" + key.user().toString() + ">,"
                    "client <" + key.client().toString() + ">,"
                    "privilege <" + key.privilege().toString() + ">";
    }

    const std::string message() const {
        return m_message;
    }

private:
    std::string m_message;
};

} // namespace Cynara

#endif // SRC_COMMON_EXCEPTIONS_PLUGINERROREXCEPTION_H_
