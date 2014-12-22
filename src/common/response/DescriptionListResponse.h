/*
 * Copyright (c) 2014 Samsung Electronics Co.
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
 * @file        src/common/response/DescriptionListResponse.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines response class for plugins description list response
 */

#ifndef SRC_COMMON_RESPONSE_DESCRIPTIONLISTRESPONSE_H_
#define SRC_COMMON_RESPONSE_DESCRIPTIONLISTRESPONSE_H_

#include <vector>

#include <types/PolicyDescription.h>

#include <request/pointers.h>
#include <response/pointers.h>
#include <response/Response.h>

namespace Cynara {

class DescriptionListResponse : public Response {
public:
    DescriptionListResponse(const std::vector<PolicyDescription> &descriptions,
                            ProtocolFrameSequenceNumber sequenceNumber) :
        Response(sequenceNumber), m_descriptions(descriptions) {
    }

    virtual ~DescriptionListResponse() {};

    virtual void execute(ResponsePtr self, ResponseTakerPtr taker, RequestContextPtr context) const;

    const std::vector<PolicyDescription> &descriptions(void) const {
        return m_descriptions;
    }

private:
    std::vector<PolicyDescription> m_descriptions;
};

} // namespace Cynara

#endif /* SRC_COMMON_RESPONSE_DESCRIPTIONLISTRESPONSE_H_ */
