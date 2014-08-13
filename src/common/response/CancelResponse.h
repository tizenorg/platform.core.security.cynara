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
/*
 * @file        CancelResponse.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file declares response class for cancel request
 */

#ifndef SRC_COMMON_RESPONSE_CANCELRESPONSE_H_
#define SRC_COMMON_RESPONSE_CANCELRESPONSE_H_

#include <response/pointers.h>
#include <response/Response.h>
#include <types/ProtocolFields.h>

namespace Cynara {

class CancelResponse : public Response {
public:
    const ProtocolFrameSequenceNumber m_checkIdRef;

    CancelResponse(const ProtocolFrameSequenceNumber checkId,
                   ProtocolFrameSequenceNumber sequenceNumber)
        : Response(sequenceNumber), m_checkIdRef(checkId) {}

    virtual ~CancelResponse() {};

    virtual void execute(ResponsePtr self, ResponseTakerPtr taker,
                         RequestContextPtr context) const;
};

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_CANCELRESPONSE_H_ */
