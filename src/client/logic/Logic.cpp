/*
 *  Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
/**
 * @file        src/client/logic/Logic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class - main libcynara-client class
 */

#include <cinttypes>
#include <memory>
#include <ctime>

#include <cache/CapacityCache.h>
#include <common.h>
#include <config/PathConfig.h>
#include <cynara-error.h>
#include <exceptions/Exception.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <plugins/NaiveInterpreter.h>
#include <protocol/Protocol.h>
#include <protocol/ProtocolClient.h>
#include <request/MonitorEntriesPutRequest.h>
#include <request/CheckRequest.h>
#include <request/pointers.h>
#include <request/SimpleCheckRequest.h>
#include <response/CheckResponse.h>
#include <response/pointers.h>
#include <response/SimpleCheckResponse.h>
#include <sockets/SocketClient.h>

#include <logic/Logic.h>

namespace Cynara {

static ProtocolFrameSequenceNumber generateSequenceNumber(void) {
    static ProtocolFrameSequenceNumber sequenceNumber = 0;
    return ++sequenceNumber;
}

Logic::Logic(const Configuration &conf) :
        m_socketClient(PathConfig::SocketPath::client, std::make_shared<ProtocolClient>()),
        m_cache(conf.getCacheSize()) {
    auto naiveInterpreter = std::make_shared<NaiveInterpreter>();
    for (auto &descr : naiveInterpreter->getSupportedPolicyDescr()) {
        m_cache.registerPlugin(descr, naiveInterpreter);
    }
}

Logic::~Logic() {
    flushMonitor();
}

int Logic::check(const std::string &client, const ClientSession &session, const std::string &user,
                 const std::string &privilege) {
    if (!ensureConnection())
        return CYNARA_API_SERVICE_NOT_AVAILABLE;

    PolicyKey key(client, user, privilege);
    int ret = m_cache.get(session, key);
    if (ret != CYNARA_API_CACHE_MISS) {
        updateMonitor(key, ret);
        return ret;
    }

    PolicyResult result;
    ret = requestResult(key, result);
    if (ret != CYNARA_API_SUCCESS) {
        LOGE("Error fetching new entry.");
        return ret;
    }

    updateMonitor(key, ret);

    return m_cache.update(session, key, result);
}

int Logic::simpleCheck(const std::string &client, const ClientSession &session,
                       const std::string &user, const std::string &privilege) {
    if (!ensureConnection())
        return CYNARA_API_SERVICE_NOT_AVAILABLE;

    PolicyKey key(client, user, privilege);
    int ret = m_cache.get(session, key);
    if (ret != CYNARA_API_CACHE_MISS) {
        return ret;
    }

    PolicyResult result;
    ret = requestSimpleResult(key, result);
    if (ret != CYNARA_API_SUCCESS) {
        if (ret != CYNARA_API_ACCESS_NOT_RESOLVED) {
            LOGE("Error fetching response for simpleCheck.");
        }
        return ret;
    }

    return m_cache.update(session, key, result);
}

bool Logic::ensureConnection(void) {
    if (m_socketClient.isConnected())
        return true;
    onDisconnected();
    if (m_socketClient.connect())
        return true;
    LOGW("Cannot connect to cynara. Service not available.");
    return false;
}

template <typename Req, typename Res>
std::shared_ptr<Res> Logic::requestResponse(const PolicyKey &key) {
    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    //Ask cynara service
    std::shared_ptr<Res> reqResponse;
    Req request(key, sequenceNumber);
    ResponsePtr response;
    while (!(response = m_socketClient.askCynaraServer(request))) {
        onDisconnected();
        if (!m_socketClient.connect())
            return nullptr;
    }

    reqResponse = std::dynamic_pointer_cast<Res>(response);
    return reqResponse;
}

int Logic::requestResult(const PolicyKey &key, PolicyResult &result) {
    auto checkResponse = requestResponse<CheckRequest, CheckResponse>(key);
    if (!checkResponse) {
        LOGC("Critical error. Requesting CheckResponse failed.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
    LOGD("checkResponse: policyType = %" PRIu16 ", metadata = %s",
         checkResponse->m_resultRef.policyType(),
         checkResponse->m_resultRef.metadata().c_str());
    result = checkResponse->m_resultRef;

    return CYNARA_API_SUCCESS;
}

int Logic::requestSimpleResult(const PolicyKey &key, PolicyResult &result) {
    auto simpleCheckResponse = requestResponse<SimpleCheckRequest, SimpleCheckResponse>(key);
    if (!simpleCheckResponse) {
        LOGC("Critical error. Requesting SimpleCheckResponse failed.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    if (simpleCheckResponse->getReturnValue() != CYNARA_API_SUCCESS)
        return simpleCheckResponse->getReturnValue();

    LOGD("SimpleCheckResponse: policyType = %" PRIu16 ", metadata = %s",
         simpleCheckResponse->getResult().policyType(),
         simpleCheckResponse->getResult().metadata().c_str());

    result = simpleCheckResponse->getResult();
    return CYNARA_API_SUCCESS;
}

bool Logic::requestMonitorEntriesPut() {
    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();
    MonitorEntriesPutRequest request(m_monitorEntries, sequenceNumber);
    return m_socketClient.sendAndForget(request);
}

void Logic::onDisconnected(void) {
    m_cache.clear();
}

void Logic::updateMonitor(const PolicyKey &policyKey, int result) {
    struct timespec ts;
    // https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/commit/?id=da15cfdae03351c689736f8d142618592e3cebc3
    auto ret = clock_gettime(CLOCK_REALTIME_COARSE, &ts);
    if (ret != 0) {
        LOGE("Could not update monitor entries. clock_gettime() failed with [%d]", ret);
        // TODO: Decide what to do. Something very bad must have happened, if clock_gettime() failed
        return;
    }

    if (result != CYNARA_API_ACCESS_ALLOWED)
        result = CYNARA_API_ACCESS_DENIED;

    m_monitorEntries.push_back({ policyKey, result, ts });

    // TODO: Decide on criteria on when entries are flushed
    flushMonitor();
}

void Logic::flushMonitor() {
    auto flushSuccess = requestMonitorEntriesPut();
    if (flushSuccess) {
        // TODO: Also reset all the flushing conditions
        m_monitorEntries.clear();
    }
}

} // namespace Cynara
