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
/**
 * @file        src/common/types/Agent.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines agent related types
 */

#ifndef SRC_COMMON_TYPES_AGENT_H_
#define SRC_COMMON_TYPES_AGENT_H_

#include <cstdint>
#include <string>

namespace Cynara {

typedef std::uint8_t AgentRequestType;
typedef std::uint8_t AgentResponseType;
typedef std::string AgentType;

}  // namespace Cynara

#endif /* SRC_COMMON_TYPES_AGENT_H_ */
