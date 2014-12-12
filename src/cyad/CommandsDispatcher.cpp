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
 * @file        src/cyad/CommandsDispatcher.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       CommandsDispatcher class (implementation)
 */

#include <cyad/CyadExitCode.h>

#include "CommandsDispatcher.h"

namespace Cynara {

CommandsDispatcher::CommandsDispatcher(BaseDispatcherIO &io, BaseAdminApiWrapper &adminApiWrapper)
    : m_io(io), m_adminApiWrapper(adminApiWrapper) {}

CommandsDispatcher::~CommandsDispatcher() {}

CyadExitCode CommandsDispatcher::execute(CyadCommand &) {
    m_io.cout() << "Whatever you wanted, it's not implemented" << std::endl;
    return CyadExitCode::UnknownError;
}

CyadExitCode CommandsDispatcher::execute(HelpCyadCommand &) {
    m_io.cout() << helpMessage << std::endl;
    return CyadExitCode::Success;
}

CyadExitCode CommandsDispatcher::execute(ErrorCyadCommand &result) {
    m_io.cout() << "There was an error in commandline options:" << std::endl;
    m_io.cout() << result.message() << std::endl;

    m_io.cout() << std::endl << helpMessage << std::endl;
    return CyadExitCode::InvalidCommandline;
}

} /* namespace Cynara */
