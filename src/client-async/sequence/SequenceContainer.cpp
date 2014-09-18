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
 * @file        src/client-async/sequence/SequenceContainer.cpp
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains definition of SequenceContainer class for
 *              storing check requests identifiers in libcynara-client-async
 */

#include <algorithm>
#include <cinttypes>
#include <cstring>

#include "SequenceContainer.h"

namespace Cynara {

SequenceContainer::SequenceContainer()
    : m_sequenceVector(((static_cast<uint16_t>(-1)+1)/8)/sizeof(int), -1) {
}

bool SequenceContainer::generate(ProtocolFrameSequenceNumber &sequenceNumber) {
    for (size_t index = 0; index < m_sequenceVector.size(); ++index) {
        int pos = ffs(m_sequenceVector[index]);
        if (pos != 0) {
                                                // (static_cast<int>(index)*sizeof(int)*8 -1+pos)
            sequenceNumber = static_cast<uint16_t>((static_cast<int>(index)<<5) -1+pos);
            m_sequenceVector[index] ^= 1<<(pos-1);
            return true;
        }
    }
    return false;
}

bool SequenceContainer::remove(ProtocolFrameSequenceNumber sequenceNumber) {
                                                // /(8*sizeof(int)
    int index = static_cast<size_t>(sequenceNumber)>>5;
                                           // %(8*sizeof(int)
    int pos = static_cast<int>(sequenceNumber)&31;
    int i = m_sequenceVector[index] | 1<<pos;
    if (i == m_sequenceVector[index])
        return false;
    m_sequenceVector[index] = i;
    return true;
}

void SequenceContainer::clear(void) {
    memset(m_sequenceVector.data(), -1, m_sequenceVector.size()*sizeof(int));
}

} // namespace Cynara
