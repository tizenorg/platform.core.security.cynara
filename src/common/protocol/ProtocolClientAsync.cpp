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
 * @file        ProtocolClientAsync.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file implements protocol class for communication with
                asynchronous client
 */

#include <attributes/attributes.h>

#include "ProtocolClientAsync.h"

namespace Cynara {

ProtocolClientAsync::ProtocolClientAsync()
{
}

ProtocolClientAsync::~ProtocolClientAsync()
{
}

ProtocolPtr ProtocolClientAsync::clone(void)
{
    return std::make_shared<ProtocolClientAsync>();
}

RequestPtr ProtocolClientAsync::extractRequestFromBuffer(BinaryQueue &bufferQueue UNUSED)
{
    return nullptr;
}

ResponsePtr ProtocolClientAsync::extractResponseFromBuffer(BinaryQueue &bufferQueue UNUSED)
{
    return nullptr;
}

} // namespace Cynara
