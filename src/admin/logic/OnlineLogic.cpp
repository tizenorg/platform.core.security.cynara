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
/**
 * @file        src/admin/logic/OnlineLogic.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of online version of Logic class
 */


#include <cinttypes>
#include <memory>

#include <cynara-error.h>
#include <common.h>
#include <config/PathConfig.h>
#include <exceptions/DatabaseBusyException.h>
#include <exceptions/Exception.h>
#include <exceptions/UnexpectedErrorException.h>
#include <log/log.h>
#include <protocol/Protocol.h>
#include <protocol/ProtocolAdmin.h>
#include <request/AdminCheckRequest.h>
#include <request/InsertOrUpdateBucketRequest.h>
#include <request/pointers.h>
#include <request/RemoveBucketRequest.h>
#include <request/SetPoliciesRequest.h>
#include <response/CheckResponse.h>
#include <response/CodeResponse.h>
#include <response/pointers.h>
#include <sockets/SocketClient.h>
#include <types/ProtocolFields.h>

#include "OnlineLogic.h"

namespace Cynara {

OnlineLogic::OnlineLogic() {
    m_socketClient = std::make_shared<SocketClient>(PathConfig::SocketPath::admin,
                                                    std::make_shared<ProtocolAdmin>());
}

ProtocolFrameSequenceNumber generateSequenceNumber(void) {
    static ProtocolFrameSequenceNumber sequenceNumber = 0;
    return ++sequenceNumber;
}

bool OnlineLogic::ensureConnection(void) {
    return m_socketClient->isConnected() || m_socketClient->connect();
}

template<typename T, typename... Args>
int OnlineLogic::askCynaraAndInterpreteCodeResponse(Args... args) {
    if (!ensureConnection()) {
        LOGE("Cannot connect to cynara. Service not available.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    //Ask cynara service
    CodeResponsePtr codeResponse;

    RequestPtr request = std::make_shared<T>(args..., sequenceNumber);
    ResponsePtr response;
    while (!(response = m_socketClient->askCynaraServer(request))) {
        if (!m_socketClient->connect())
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    codeResponse = std::dynamic_pointer_cast<CodeResponse>(response);
    if (!codeResponse) {
        LOGC("Critical error. Casting Response to CodeResponse failed.");
        return CYNARA_API_UNKNOWN_ERROR;
    }

    LOGD("codeResponse: code [%" PRIu16 "]", codeResponse->m_code);
    switch (codeResponse->m_code) {
        case CodeResponse::Code::OK:
            LOGI("Policies set successfully.");
            return CYNARA_API_SUCCESS;
        case CodeResponse::Code::NOT_ALLOWED:
            LOGE("Cynara service answered: Operation not allowed.");
            return CYNARA_API_OPERATION_NOT_ALLOWED;
        case CodeResponse::Code::NO_BUCKET:
            LOGE("Trying to use unexisting bucket.");
            return CYNARA_API_BUCKET_NOT_FOUND;
        case CodeResponse::Code::FAILED:
            LOGC("Cynara service answered: Operation failed.");
            return CYNARA_API_OPERATION_FAILED;
        default:
            LOGE("Unexpected response code from server: [%d]",
                 static_cast<int>(codeResponse->m_code));
            return CYNARA_API_UNKNOWN_ERROR;
    }
}

int OnlineLogic::setPolicies(const ApiInterface::PoliciesByBucket &insertOrUpdate,
                             const ApiInterface::KeysByBucket &remove) {
    return askCynaraAndInterpreteCodeResponse<SetPoliciesRequest>(insertOrUpdate, remove);
}

int OnlineLogic::insertOrUpdateBucket(const PolicyBucketId &bucket,
                                      const PolicyResult &policyResult) {
    return askCynaraAndInterpreteCodeResponse<InsertOrUpdateBucketRequest>(bucket, policyResult);
}

int OnlineLogic::removeBucket(const PolicyBucketId &bucket) {
    return askCynaraAndInterpreteCodeResponse<RemoveBucketRequest>(bucket);
}

int OnlineLogic::adminCheck(const PolicyBucketId &startBucket, bool recursive, const PolicyKey &key,
                            PolicyResult &result) {
    if (!ensureConnection()) {
        LOGE("Cannot connect to cynara. Service not available.");
        return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    //Ask cynara service
    CheckResponsePtr checkResponse;

    RequestPtr request = std::make_shared<AdminCheckRequest>(key, startBucket, recursive,
                                                             sequenceNumber);
    ResponsePtr response;
    while (!(response = m_socketClient->askCynaraServer(request))) {
        if (!m_socketClient->connect())
            return CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    checkResponse = std::dynamic_pointer_cast<CheckResponse>(response);
    if (!checkResponse) {
        LOGC("Casting Response to CheckResponse failed.");
        return CYNARA_API_UNKNOWN_ERROR;
    }

    LOGD("checkResponse: policyType [%" PRIu16 "], metadata <%s>",
         checkResponse->m_resultRef.policyType(),
         checkResponse->m_resultRef.metadata().c_str());

    result = checkResponse->m_resultRef;
    return CYNARA_API_SUCCESS;
}

} // namespace Cynara
