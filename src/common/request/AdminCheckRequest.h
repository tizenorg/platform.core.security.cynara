/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/request/AdminCheckRequest.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines admin check request class
 */

#ifndef SRC_COMMON_REQUEST_ADMINCHECKREQUEST_H_
#define SRC_COMMON_REQUEST_ADMINCHECKREQUEST_H_

#include <types/PolicyBucketId.h>
#include <types/PolicyKey.h>

#include <request/pointers.h>
#include <request/Request.h>

namespace Cynara {

class AdminCheckRequest : public Request {
private:
    PolicyKey m_key;
    PolicyBucketId m_startBucket;
    bool m_recursive;

public:
    AdminCheckRequest(const PolicyKey &key, const PolicyBucketId &startBucket, bool recursive,
                      ProtocolFrameSequenceNumber sequenceNumber) :
        Request(sequenceNumber), m_key(key), m_startBucket(startBucket), m_recursive(recursive) {
    }

    virtual ~AdminCheckRequest() {};

    const PolicyKey &key(void) const {
        return m_key;
    }

    const PolicyBucketId &startBucket(void) const {
        return m_startBucket;
    }

    bool recursive(void) const {
        return m_recursive;
    }

    virtual void execute(RequestTaker &taker, const RequestContext &context) const;
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_ADMINCHECKREQUEST_H_ */
