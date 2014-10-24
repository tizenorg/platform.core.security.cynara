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
 * @file        src/service/sockets/LinkMonitor.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Definition of LinkMonitor class
 */

#include <log/log.h>

#include "LinkMonitor.h"

namespace Cynara {

LinkMonitor::~LinkMonitor() {
    m_disconnectionCallbacks.clear();
}

void LinkMonitor::registerLink(const LinkId linkId, const LinkName &linkName) {
    if (m_links.find(linkId) != m_links.end()) {
        LOGE("Trying to register already registered link: <%s>", linkName.c_str());
        return;
    }

    m_links.insert(std::make_pair(std::ref(linkId), linkName));
    LOGI("Registered link: <%s>", linkName.c_str());
}

void LinkMonitor::unregisterLink(const LinkId linkId) {
    if (m_links.find(linkId) != m_links.end()) {
        LinkName linkName = m_links[linkId];
        m_links.erase(linkId);
        LOGI("Unregistered link: <%s>", linkName.c_str());
            for (auto &callback : m_disconnectionCallbacks) {
                if (callback)
                    callback(linkId);
        }
    } else {
        LOGD("Trying to unregister not registered link.");
    }
}

bool LinkMonitor::linkExists(const LinkId linkId) const {
    return m_links.find(linkId) != m_links.end();
}

bool LinkMonitor::registerDisconnectionCallback(DisconnectionCallback callback) {
    try {
        m_disconnectionCallbacks.push_back(callback);
        return true;
    } catch(const std::exception &ex) {
        LOGE("Registering disconnection callback failed: <%s>", ex.what());
    }
    return false;
}

} // namespace Cynara
