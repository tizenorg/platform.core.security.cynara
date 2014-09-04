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
 * @file        Logic.h
 * @author      Marcin Niesluchowski <m.niesluchow@samsung.com>
 * @version     1.0
 * @brief       This file contains declaration of Logic class - main
 *              libcynara-client-async class
 */

#ifndef SRC_CLIENT_ASYNC_LOGIC_LOGIC_H_
#define SRC_CLIENT_ASYNC_LOGIC_LOGIC_H_

#include <api/ApiInterface.h>
#include <cynara-client-async.h>

namespace Cynara {

class Logic : public ApiInterface {
public:
    Logic(cynara_status_callback callback,
          void *userStatusData);
    virtual ~Logic() {};

    virtual int checkCache(const std::string &client,
                           const std::string &session,
                           const std::string &user,
                           const std::string &privilege) noexcept;
    virtual int createRequest(const std::string &client,
                              const std::string &session,
                              const std::string &user,
                              const std::string &privilege,
                              cynara_check_id &checkId,
                              cynara_response_callback callback,
                              void *userResponseData) noexcept;
    virtual int process(void) noexcept;
    virtual int cancelRequest(cynara_check_id checkId) noexcept;
};

} // namespace Cynara

#endif /* SRC_CLIENT_ASYNC_LOGIC_LOGIC_H_ */
