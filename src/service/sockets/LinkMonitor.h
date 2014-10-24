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
 * @file        src/service/sockets/LinkMonitor.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       Declaration of LinkMonitor class
 */

#ifndef SRC_SERVICE_SOCKETS_LINKMONITOR_H_
#define SRC_SERVICE_SOCKETS_LINKMONITOR_H_

#include <functional>
#include <map>

#include <containers/BinaryQueue.h>

namespace Cynara {

typedef BinaryQueue& LinkId;
typedef std::string LinkName;

class LinkMonitor {
public:
    typedef std::function<void(const LinkId)> DisconnectionCallback;

    LinkMonitor() {}
    ~LinkMonitor();

    void registerLink(const LinkId linkId, const LinkName &linkName);
    void unregisterLink(const LinkId linkId);
    bool linkExists(const LinkId linkId) const;

    bool registerDisconnectionCallback(DisconnectionCallback callback);

private:
    struct refCompare {
        bool operator()(const LinkId a, const LinkId b) const {
            return &a < &b;
        }
    };

    std::map<std::reference_wrapper<BinaryQueue>, LinkName, refCompare> m_links;
    std::list<DisconnectionCallback> m_disconnectionCallbacks;
};

} // namespace Cynara
#endif /* SRC_SERVICE_SOCKETS_LINKMONITOR_H_ */
