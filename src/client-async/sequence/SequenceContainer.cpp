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
 * @file        SequenceContainer.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of SequenceContainer class
 *              for storing check identifiers in libcynara-client-async
 */

#include <algorithm>
#include <cinttypes>

#include "SequenceContainer.h"

namespace Cynara {

const ProtocolFrameSequenceNumber SequenceContainer::msc_maxSequenceNumber = static_cast<uint16_t>(-1);

bool SequenceContainer::generateCheck(ProtocolFrameSequenceNumber &sequenceNumber)
{
    sequenceNumber = 0;
    for (auto it = m_sequenceSet.begin(); it != m_sequenceSet.end(); ++it) {
        if (*it != sequenceNumber) {
            m_sequenceSet.insert(it, sequenceNumber);
            return true;
        }
        ++sequenceNumber;
    }
    if (sequenceNumber == 0)
        return false;
    m_sequenceSet.insert(m_sequenceSet.end(), sequenceNumber);
    return true;
}

bool SequenceContainer::removeCheck(ProtocolFrameSequenceNumber sequenceNumber)
{
    return m_sequenceSet.erase(sequenceNumber) == 1;
}

void SequenceContainer::clear()
{
    return m_sequenceSet.clear();
}

} // namespace Cynara
