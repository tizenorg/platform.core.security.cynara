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
 * @file        src/common/list/DescriptionListRequest.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines plugin description list request class
 */

#ifndef SRC_COMMON_REQUEST_DESCRIPTIONLISTREQUEST_H_
#define SRC_COMMON_REQUEST_DESCRIPTIONLISTREQUEST_H_

#include <request/pointers.h>
#include <request/Request.h>

namespace Cynara {

class DescriptionListRequest : public Request {
public:
    DescriptionListRequest(ProtocolFrameSequenceNumber sequenceNumber) :
        Request(sequenceNumber) {
    }

    virtual ~DescriptionListRequest() {};

    virtual void execute(RequestPtr self, RequestTakerPtr taker, RequestContextPtr context) const;
};

} // namespace Cynara

#endif /* SRC_COMMON_REQUEST_DESCRIPTIONLISTREQUEST_H_ */
