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
 * @file        CheckContainer.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains declaration of CheckContainer class
 *              for storing check identifiers in libcynara-client-async
 */

#ifndef SRC_CLIENT_ASYNC_CHECKS_CHECKCONTAINER_H_
#define SRC_CLIENT_ASYNC_CHECKS_CHECKCONTAINER_H_

#include <set>

#include <cynara-client-async.h>

namespace Cynara {

class CheckContainer {
private:
    std::set<cynara_check_id> m_checkSet;

public:
    CheckContainer() = default;
    virtual ~CheckContainer() {}

    bool generate(cynara_check_id &checkId);
    bool remove(cynara_check_id checkId);
    void clear();
};

} // namespace Cynara

#endif // SRC_CLIENT_ASYNC_CHECKS_CHECKCONTAINER_H_
