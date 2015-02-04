/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/common/emergency/EmergencyMode.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Headers for Cynara::EmergencyMode
 */

#ifndef SRC_COMMON_EMERGENCY_EMERGENCYMODE_H_
#define SRC_COMMON_EMERGENCY_EMERGENCYMODE_H_

#include <memory>

namespace Cynara {

class EmergencyMode {
public:
    EmergencyMode() : m_active(false) {};
    EmergencyMode(const EmergencyMode &) = delete;
    const EmergencyMode &operator=(const EmergencyMode &) = delete;
    ~EmergencyMode() {};

    void setActive(void);
    bool isActive(void) const;

private:
    bool m_active;
};

} /* namespace Cynara */

#endif /* SRC_COMMON_EMERGENCY_EMERGENCYMODE_H_ */
