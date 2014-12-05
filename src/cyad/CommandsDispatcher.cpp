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

#include <cynara-admin-types.h>
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

// TODO: Implement a bulk version, so user doesn't have to insert policies one by one
void CommandsDispatcher::execute(SetPolicyCyadCommand &result) {
    cynara_admin_policy policy;
    policy.bucket = strdup(result.bucketId().c_str());
    policy.client = strdup(result.policyKey().client().toString().c_str());
    policy.user = strdup(result.policyKey().user().toString().c_str());
    policy.privilege = strdup(result.policyKey().privilege().toString().c_str());
    policy.result = result.policyType();
    policy.result_extra = result.metadata().empty() ? nullptr : strdup(result.metadata().c_str());

    const cynara_admin_policy *policies[2] = { &policy, nullptr };
    m_adminApiWrapper.cynara_admin_set_policies(m_cynaraAdmin, policies);

    free(policy.bucket);
    free(policy.client);
    free(policy.user);
    free(policy.privilege);
    free(policy.result_extra);
}

} /* namespace Cynara */
