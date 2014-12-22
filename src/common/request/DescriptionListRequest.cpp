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
 * @file        src/common/list/DescriptionListRequest.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file implements plugin description list request class
 */

#include <memory>

#include <request/RequestTaker.h>

#include "DescriptionListRequest.h"

namespace Cynara {

void DescriptionListRequest::execute(RequestPtr self, RequestTakerPtr taker,
                                     RequestContextPtr context) const {
    taker->execute(context, std::dynamic_pointer_cast<DescriptionListRequest>(self));
}

} // namespace Cynara
