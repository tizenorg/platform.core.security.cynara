/*
 * MonitorSocketClient.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: zos
 */

#include <fcntl.h>
#include <poll.h>
#include <stdio.h>

#include <config/PathConfig.h>
#include <log/log.h>
#include <request/Request.h>
#include <request/RequestContext.h>

#include "MonitorSocketClient.h"

namespace Cynara {

MonitorSocketClient::MonitorSocketClient()
    : m_socket(PathConfig::SocketPath::monitorGet),
      m_notifyFd(-1)
{

}

void MonitorSocketClient::setNotifyFd(int notifyFd) {
    m_notifyFd = notifyFd;
}

bool MonitorSocketClient::isConnected() {
    return m_socket.isConnected();
}

bool MonitorSocketClient::connect() {
    switch (m_socket.connect()) {
    case Socket::ConnectionStatus::CONNECTION_SUCCEEDED:
        return true;
    case Socket::ConnectionStatus::CONNECTION_IN_PROGRESS:
        return m_socket.completeConnection() != Socket::ConnectionStatus::CONNECTION_SUCCEEDED;
    default:
        return false;
    }
}

bool MonitorSocketClient::sendRequest(const Request &request) {
    //pass request to protocol
    BinaryQueuePtr totallyUnnecessarySharedPtr(&m_writeQueue, [](BinaryQueue *) {});
    RequestContext context(ResponseTakerPtr(), totallyUnnecessarySharedPtr);
    request.execute(m_protocol, context);

    //send request to cynara
    if (m_socket.sendToServer(m_writeQueue) == Socket::SendStatus::CONNECTION_LOST) {
        LOGW("Disconnected while sending request to Cynara.");
        return false;
    }
    return true;
}

bool MonitorSocketClient::waitForEvent(MonitorSocketClient::Event &event) {
    int cynaraFd = m_socket.getSockFd();
    pollfd desc[2];
    desc[0].fd = cynaraFd;
    desc[0].events = POLLIN;

    desc[1].fd = m_notifyFd;
    desc[1].events = POLLIN;

    int ret = poll(desc, 2, -1);
    if (ret == -1) {
        LOGE("Poll returned with error: " << strerror(errno));
        return false;
    }
    if (desc[1].revents & POLLIN) {
        LOGD("Poll returned with notification to return");
        event = Event::NOTIFY_RETURN;
    }

    if (desc[0].revents & POLLIN) {
        LOGD("Poll returned with fetch entries");
        event = Event::FETCH_ENTRIES;
    }
    return true;
}

MonitorGetEntriesResponsePtr MonitorSocketClient::fetchEntries() {

    while (true) {
        if (!m_socket.receiveFromServer(m_readQueue)) {
            LOGW("Disconnected while receiving response from Cynara.");
            return nullptr;
        }
        BinaryQueuePtr totallyUnnecessarySharedPtr(&m_readQueue, [](BinaryQueue *) {});
        ResponsePtr response = m_protocol.extractResponseFromBuffer(totallyUnnecessarySharedPtr);
        if (response) {
            MonitorGetEntriesResponsePtr monRes =
                    std::dynamic_pointer_cast<MonitorGetEntriesResponse>(response);
            return monRes;
        }
    }
}

} /* namespace Cynara */
