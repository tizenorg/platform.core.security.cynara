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

#include <cstring>
#include <vector>

#include <cynara-admin-types.h>
#include <cynara-policy-types.h>

#include <common/exceptions/BucketRecordCorruptedException.h>

#include <cyad/CynaraAdminPolicies.h>
#include <cyad/AdminPolicyParser.h>

#include "CommandsDispatcher.h"

namespace Cynara {

CommandsDispatcher::CommandsDispatcher(BaseDispatcherIO &io, BaseAdminApiWrapper &adminApiWrapper)
    : m_io(io), m_adminApiWrapper(adminApiWrapper), m_cynaraAdmin(nullptr)
{
    m_adminApiWrapper.cynara_admin_initialize(&m_cynaraAdmin);
}

CommandsDispatcher::~CommandsDispatcher() {
    m_adminApiWrapper.cynara_admin_finish(m_cynaraAdmin);
}

void CommandsDispatcher::execute(CyadCommand &) {
    m_io.cout() << "Whatever you wanted, it's not implemented" << std::endl;
}

void CommandsDispatcher::execute(HelpCyadCommand &) {
    m_io.cout() << helpMessage << std::endl;
}

void CommandsDispatcher::execute(ErrorCyadCommand &result) {
    m_io.cout() << "There was an error in commandline options:" << std::endl;
    m_io.cout() << result.message() << std::endl;

    m_io.cout() << std::endl << helpMessage << std::endl;
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

void CommandsDispatcher::execute(SetPolicyCyadCommand &result) {
    CynaraAdminPolicies policies;

    policies.add(result.bucketId(), result.policyType(), result.metadata(), result.policyKey());
    policies.seal();

    m_adminApiWrapper.cynara_admin_set_policies(m_cynaraAdmin, policies.data());

}

void CommandsDispatcher::execute(SetPolicyBulkCyadCommand &result) {
    auto input = m_io.openFile(result.filename());

    AdminPolicyParser policiesParser(input);

    try {
        auto policies = policiesParser.parse();
        m_adminApiWrapper.cynara_admin_set_policies(m_cynaraAdmin, policies.data());
    } catch (const BucketRecordCorruptedException &ex) {
        // TODO: Format better message
        // TODO: return proper error code
        m_io.cerr() << ex.message();
    }
}

} /* namespace Cynara */
