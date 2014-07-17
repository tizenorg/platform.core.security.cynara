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
 * @file        CapacityCache.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains capacity cache implementation.
 */


#include <cache/CacheInterface.h>
#include <map>
#include <list>
#include <log/log.h>

namespace Cynara {
class CapacityCache : public PluginCacheInterface {
public:
    const std::size_t CACHE_DEFAULT_SIZE = 10000;

    CapacityCache(ResultGetterInterface *getter) :
        PluginCacheInterface(getter),
        m_capacity(CACHE_DEFAULT_SIZE) {}

    cynara_api_result get(const std::string &session,
                                const PolicyKey &key) {

        //This can be very time heavy. This part is welcomed to be optimized.
        if(session != m_session) {
            LOGD("Session changed from %s to %s.", m_session.c_str(), session.c_str());
            m_keyValue.clear();
            m_keyUsage.clear();
            m_session = session;
        }
        auto result_it = m_keyValue.find(keyToString(key));
        //Do we have entry in cache?
        if(result_it == m_keyValue.end()) {
            LOGD("No entry for client=%s user=%s privilege=%s.",
                    key.client().toString().c_str(),
                    key.user().toString().c_str(),
                    key.privilege().toString().c_str());
            return update(key);
        } else {
            LOGD("Entry available for client=%s user=%s privilege=%s",
                    key.client().toString().c_str(),
                    key.user().toString().c_str(),
                    key.privilege().toString().c_str());
            auto plugin_it = m_plugins.find(result_it->second.first.policyType());
            if(plugin_it == m_plugins.end()) {
                LOGE("No plugin registered for given PolicyType");
                return cynara_api_result::CYNARA_API_ACCESS_DENIED;
            }
            //Is it still usable?
            InterpreterInterfacePtr plugin = plugin_it->second;
            if(plugin->isUsable(result_it->second.first)) {
                LOGD("Entry usable.");
                return plugin->toResult(result_it->second.first);
            } else {
                //remove from list and update
                LOGD("Entry not usable.");
                auto usage_it = result_it->second.second;
                m_keyUsage.erase(usage_it);

                return update(key);
            }
        }
        return cynara_api_result::CYNARA_API_SUCCESS;
    }

    void clear() {
        m_keyUsage.clear();
        m_keyValue.clear();
        m_session.clear();
        PluginCacheInterface::clear();
    }
private:
    typedef std::list<std::string> KeyUsageList;
    typedef std::map<std::string,
        std::pair<PolicyResult,
                  KeyUsageList::iterator>> KeyValueMap;

    std::string keyToString(const PolicyKey &key) {
        return key.client().toString() + key.privilege().toString() + key.user().toString();
    }

    void evict() {
        auto lastUsedKey = m_keyUsage.back();
        m_keyUsage.pop_back();

        auto value_it = m_keyValue.find(lastUsedKey);
        m_keyValue.erase(value_it);
    }

    cynara_api_result update(const PolicyKey &key) {
        cynara_api_result ret;
        PolicyResult result;
        if((ret = m_getter->requestResult(key, result)) != cynara_api_result::CYNARA_API_SUCCESS) {
            LOGE("Error fetching new entry.");
            return ret;
        }
        LOGD("Fetched new entry.");
        auto plugin_it = m_plugins.find(result.policyType());

        //No registered plugin for returned type of policy
        if(plugin_it == m_plugins.end()) {
            LOGE("No plugin registered for given PolicyType");
            return cynara_api_result::CYNARA_API_ACCESS_DENIED;
        }
        auto plugin = plugin_it->second;

        if(plugin->isCacheable(result)){
            LOGD("Entry cacheable");
            if(m_keyValue.size() == m_capacity) {
                LOGD("Capacity reached.");
                evict();
            }
            m_keyUsage.push_front(keyToString(key));
            m_keyValue[keyToString(key)] = std::make_pair(result, m_keyUsage.begin());
        }

        return plugin->toResult(result);
    }
    std::size_t m_capacity;
    std::string m_session;

    KeyUsageList m_keyUsage;
    KeyValueMap m_keyValue;
};
} //namespace Cynara




