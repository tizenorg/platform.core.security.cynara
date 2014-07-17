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
 * @file        CacheInterface.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains cache interface definitions.
 */

#ifndef SRC_CLIENT_CACHE_CACHEINTERFACE_H_
#define SRC_CLIENT_CACHE_CACHEINTERFACE_H_

#include <map>
#include <memory>
#include <string>

#include <cynara-client.h>
#include <types/PolicyKey.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

namespace Cynara {

class InterpreterInterface;
typedef std::shared_ptr<InterpreterInterface> InterpreterInterfacePtr;

class PluginCacheInterface;
typedef std::shared_ptr<PluginCacheInterface> PluginCacheInterfacePtr;

class ResultGetterInterface;
typedef std::shared_ptr<ResultGetterInterface> ResultGetterInterfacePtr;

class ResultGetterInterface {
public:
    virtual cynara_api_result requestResult(const PolicyKey &key, PolicyResult &result) noexcept = 0;
    virtual ~ResultGetterInterface() = default;
};

class InterpreterInterface {
public:
    virtual bool isCacheable(const PolicyResult &result) noexcept = 0;
    virtual bool isUsable(const PolicyResult &result) noexcept = 0;
    virtual cynara_api_result toResult(const PolicyResult &result) noexcept = 0;

    virtual ~InterpreterInterface() = default;
};

class PluginCacheInterface {
public:
    PluginCacheInterface(ResultGetterInterfacePtr getter) : m_getter(getter) {}
    virtual cynara_api_result get(const std::string &session, const PolicyKey &key) = 0;
    void registerPlugin (const PolicyType policyType, InterpreterInterfacePtr plugin) {
        m_plugins[policyType] = plugin;
    }
    virtual void clear(void) {
        m_plugins.clear();
    }
    virtual ~PluginCacheInterface() = default;
protected:
    std::map<PolicyType, InterpreterInterfacePtr> m_plugins;
    ResultGetterInterfacePtr m_getter;
};

} // namespace Cynara

#endif // SRC_CLIENT_CACHE_CACHEINTERFACE_H_
