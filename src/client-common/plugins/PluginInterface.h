
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
/*
 * @file        PluginInterface.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains plugin interface definitions.
 */

#ifndef SRC_CLIENT_COMMON_PLUGINS_PLUGININTERFACE_H_
#define SRC_CLIENT_COMMON_PLUGINS_PLUGININTERFACE_H_

#include <memory>

#include <types/ClientSession.h>
#include <types/PolicyResult.h>

namespace Cynara {

class InterpreterInterface;
typedef std::shared_ptr<InterpreterInterface> InterpreterInterfacePtr;

class InterpreterInterface {
public:
    virtual bool isCacheable(const ClientSession &session, const PolicyResult &result) noexcept = 0;
    virtual bool isUsable(const ClientSession &session, const PolicyResult &result) noexcept = 0;
    virtual int toResult(const ClientSession &session, const PolicyResult &result) noexcept = 0;

    virtual ~InterpreterInterface() {};
};

}

#endif // SRC_CLIENT_COMMON_PLUGINS_PLUGININTERFACE_H_
