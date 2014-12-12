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

#include <cynara-error.h>
#include <cynara-policy-types.h>

#include <common/exceptions/BucketRecordCorruptedException.h>

#include <cyad/AdminLibraryInitializationFailedException.h>
#include <cyad/CynaraAdminPolicies.h>
#include <cyad/AdminPolicyParser.h>
#include <cyad/CyadExitCode.h>

#include "CommandsDispatcher.h"

namespace Cynara {

CommandsDispatcher::CommandsDispatcher(BaseDispatcherIO &io, BaseAdminApiWrapper &adminApiWrapper)
    : m_io(io), m_adminApiWrapper(adminApiWrapper), m_cynaraAdmin(nullptr)
{
    auto ret = m_adminApiWrapper.cynara_admin_initialize(&m_cynaraAdmin);
    if (ret != CYNARA_API_SUCCESS)
        throw AdminLibraryInitializationFailedException(ret);
}

CommandsDispatcher::~CommandsDispatcher() {
    m_adminApiWrapper.cynara_admin_finish(m_cynaraAdmin);
}

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

CyadExitCode CommandsDispatcher::execute(DeleteBucketCyadCommand &result) {
    auto ret = m_adminApiWrapper.cynara_admin_set_bucket(m_cynaraAdmin, result.bucketId().c_str(),
                                                         CYNARA_ADMIN_DELETE, nullptr);
    return static_cast<CyadExitCode>(ret);
}

CyadExitCode CommandsDispatcher::execute(SetBucketCyadCommand &result) {
    const auto &policyResult = result.policyResult();
    const char *metadata = policyResult.metadata().empty() ? nullptr
                                                           : policyResult.metadata().c_str();
    auto ret = m_adminApiWrapper.cynara_admin_set_bucket(m_cynaraAdmin,
                                                         result.bucketId().c_str(),
                                                         policyResult.policyType(), metadata);

    return static_cast<CyadExitCode>(ret);
}

CyadExitCode CommandsDispatcher::execute(SetPolicyCyadCommand &result) {
    CynaraAdminPolicies policies;

    policies.add(result.bucketId(), result.policyResult(), result.policyKey());
    policies.seal();

    auto ret = m_adminApiWrapper.cynara_admin_set_policies(m_cynaraAdmin, policies.data());
    return static_cast<CyadExitCode>(ret);
}

CyadExitCode CommandsDispatcher::execute(SetPolicyBulkCyadCommand &result) {
    auto input = m_io.openFile(result.filename());

    try {
        auto policies = Cynara::AdminPolicyParser::parse(input);
        auto ret = m_adminApiWrapper.cynara_admin_set_policies(m_cynaraAdmin, policies.data());
        return static_cast<CyadExitCode>(ret);
    } catch (const BucketRecordCorruptedException &ex) {
        m_io.cerr() << ex.message();
        return CyadExitCode::InvalidInput;
    }
}

} /* namespace Cynara */
