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

#include <sockets/SocketClientAsync.h>

#include <api/ApiInterface.h>

namespace Cynara {

class Logic : public ApiInterface {
private:
    SocketClientAsyncPtr m_socket;

    void onDisconnected(void);

public:
    Logic();
    virtual ~Logic() {};

    virtual int connect(int &sockFd) noexcept;
    virtual int check(const std::string &client, const std::string &session,
                      const std::string &user, const std::string &privilege,
                      uint16_t &checkId) noexcept;
    virtual int receive(uint16_t &checkId) noexcept;
    virtual int cancel(const uint16_t checkId) noexcept;
};

} // namespace Cynara

#endif /* SRC_CLIENT_ASYNC_LOGIC_LOGIC_H_ */
