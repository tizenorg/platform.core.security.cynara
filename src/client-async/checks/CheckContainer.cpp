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
/*
 * @file        CheckContainer.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of CheckContainer class
 *              for storing check identifiers in libcynara-client-async
 */

#include <algorithm>
#include <cinttypes>

#include "CheckContainer.h"

namespace Cynara {

bool CheckContainer::generate(cynara_check_id &checkId)
{
    checkId = 0;
    for (auto it = m_checkList.begin(); it != m_checkList.end(); ++it) {
        if (*it != checkId) {
            m_checkList.insert(it, checkId);
            return true;
        }
        ++checkId;
    }
    if (checkId == static_cast<uint16_t>(-1))
        return false;
    m_checkList.push_back(checkId);
    return true;
}

bool CheckContainer::remove(cynara_check_id checkId)
{
    auto it = std::find(m_checkList.begin(), m_checkList.end(), checkId);
    if (it != m_checkList.end()) {
        m_checkList.erase(it);
        return true;
    }
    return false;
}

bool CheckContainer::find(cynara_check_id checkId)
{
    auto it = std::find(m_checkList.begin(), m_checkList.end(), checkId);
    return it != m_checkList.end();
}

void CheckContainer::clear()
{
    return m_checkList.clear();
}

} // namespace Cynara
