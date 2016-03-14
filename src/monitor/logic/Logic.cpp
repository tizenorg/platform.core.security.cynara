/*
 *  Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/monitor/logic/logic.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains implementation of Logic class -- main libcynara-monitor class
 */


#include <mutex>

#include <cynara-error.h>
#include <log/log.h>
#include <types/MonitorEntry.h>
#include <request/MonitorGetEntriesRequest.h>
#include <request/MonitorGetFlushRequest.h>

#include "Logic.h"

namespace {
Cynara::ProtocolFrameSequenceNumber generateSequenceNumber(void) {
    static Cynara::ProtocolFrameSequenceNumber sequenceNumber = 0;
    return ++sequenceNumber;
}

class RunStatus {
public:
    RunStatus(std::mutex &mutex) : m_mutex(mutex), m_isRunning(false) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_isRunning = true;
    }
    ~RunStatus() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_isRunning = false;
    }
    bool isRunning() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_isRunning;
    }
private:
    std::mutex& m_mutex;
    bool m_isRunning;
};

} // namespace anonymous

namespace Cynara {

int Logic::init() {
    if (!m_notify.init()) {
        LOGE("Couldn't initialize notification object");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    m_client.setNotifyFd(m_notify.getNotifyFd());
    return CYNARA_API_SUCCESS;
}

bool Logic::connect() {
    std::unique_lock<std::mutex> lock(m_mutexCond);
    if (!m_client.isConnected()) {
        bool connectRet = m_client.connect();
        m_connectionResolved = true;
        if (!connectRet) {
            LOGE("Cannot connect to Cynara service");
            return false;
        }
    }
    m_connectedCV.notify_one();
    return true;
}

int Logic::sendAndFetch(std::vector<MonitorEntry> &entries) {
    RunStatus status(m_mutexCond);
    if (!connect()) {
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
    if (!m_client.sendRequest(MonitorGetEntriesRequest(m_conf.getBufferSize(),
            generateSequenceNumber()))) {
        LOGE("Failed sending request to Cynara");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    MonitorSocketClient::Event event;
    if (!m_client.waitForEvent(event)) {
        LOGE("Waiting for event failed");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }
    switch(event) {
    case MonitorSocketClient::Event::FETCH_ENTRIES:
    {
        auto responsePtr = m_client.fetchEntries();
        if (!responsePtr) {
            LOGE("Error fetching response");
            return CYNARA_API_UNKNOWN_ERROR;
        }
        entries.assign(responsePtr->entries().begin(), responsePtr->entries().end());
        break;
    }
    case MonitorSocketClient::Event::NOTIFY_RETURN:
        LOGD("Got notification to stop working");
        break;
    }
    return CYNARA_API_SUCCESS;
}

int Logic::entriesGet(std::vector<MonitorEntry> &entries) {
    std::unique_lock<std::mutex> guard(m_reentrantGuard, std::defer_lock);
    if (!guard.try_lock()) {
        LOGE("Function is not reentrant");
        return CYNARA_API_OPERATION_NOT_ALLOWED;
    }
    int ret = sendAndFetch(entries);
    m_finishedCV.notify_one();
    return ret;
}

bool Logic::waitForConnectionResolved() {
    std::unique_lock<std::mutex> lock(m_mutexCond);
    m_connectedCV.wait(lock, [&] { return m_connectionResolved; });
    if (!m_client.isConnected()) {
        LOGE("Cynara monitor is not connected to service.");
        return false;
    }
    return true;
}

int Logic::entriesFlush(void) {
    if (!waitForConnectionResolved()) {
        return CYNARA_API_OPERATION_NOT_ALLOWED;
    }
    if (!m_client.sendRequest(MonitorGetFlushRequest(generateSequenceNumber()))) {
        LOGE("Error sending request to Cynara");
        return CYNARA_API_UNKNOWN_ERROR;
    }
    return CYNARA_API_SUCCESS;
}

bool Logic::isRunning() {
    std::unique_lock<std::mutex> lock(m_mutexCond);
    bool isRunning = m_isRunning;
    return isRunning;
}

void Logic::notifyFinish(void) {
    if (!isRunning()) {
        return;
    }
    if (!m_notify.notify()) {
        LOGE("Failed to notify");
        return;
    }
    std::unique_lock<std::mutex> lock(m_mutexCond);
    m_finishedCV.wait(lock, [&] {return !m_isRunning;});
}

} // namespace Cynara
