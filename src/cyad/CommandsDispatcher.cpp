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

#include <cynara-policy-types.h>

#include "CommandsDispatcher.h"

namespace Cynara {

CommandsDispatcher::CommandsDispatcher(std::ostream &outStream,
                                       BaseAdminApiWrapper &adminApiWrapper)
    : m_outStream(outStream), m_adminApiWrapper(adminApiWrapper), m_cynaraAdmin(nullptr)
{
    m_adminApiWrapper.cynara_admin_initialize(&m_cynaraAdmin);
}

CommandsDispatcher::~CommandsDispatcher() {
    m_adminApiWrapper.cynara_admin_finish(m_cynaraAdmin);
}

void CommandsDispatcher::execute(CyadCommand &) {
    m_outStream << "Whatever you wanted, it's not implemented" << std::endl;
}

void CommandsDispatcher::execute(HelpCyadCommand &) {
    m_outStream << helpMessage << std::endl;
}

void CommandsDispatcher::execute(ErrorCyadCommand &result) {
    m_outStream << "There was an error in commandline options:" << std::endl;
    m_outStream << result.message() << std::endl;

    m_outStream << std::endl << helpMessage << std::endl;
}

void CommandsDispatcher::execute(DeleteBucketCyadCommand &result) {
    m_adminApiWrapper.cynara_admin_set_bucket(m_cynaraAdmin, result.bucketId().c_str(),
                                              CYNARA_ADMIN_DELETE, nullptr);
}

void CommandsDispatcher::execute(SetBucketCyadCommand &result) {
    const char *metadata = result.metadata().empty() ? nullptr : result.metadata().c_str();
    m_adminApiWrapper.cynara_admin_set_bucket(m_cynaraAdmin,
                                              result.bucketId().c_str(),
                                              result.policyType(), metadata);
}

} /* namespace Cynara */
