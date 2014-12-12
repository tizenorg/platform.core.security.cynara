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
 * @file        src/cyad/CommandsDispatcher.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       CommandsDispatcher class
 */

#ifndef SRC_CYAD_COMMANDSDISPATCHER_H_
#define SRC_CYAD_COMMANDSDISPATCHER_H_

#include <cyad/BaseAdminApiWrapper.h>
#include <cyad/CommandlineParser/CyadCommand.h>
#include <cyad/DispatcherIO.h>

struct cynara_admin;
struct cynara_admin_policy;

namespace Cynara {

class CommandsDispatcher {
public:
    CommandsDispatcher(BaseDispatcherIO &io, BaseAdminApiWrapper &adminApiWrapper);
    virtual ~CommandsDispatcher();

    virtual CyadExitCode execute(CyadCommand &);
    virtual CyadExitCode execute(HelpCyadCommand &);
    virtual CyadExitCode execute(ErrorCyadCommand &);
    virtual CyadExitCode execute(DeleteBucketCyadCommand &);
    virtual CyadExitCode execute(SetBucketCyadCommand &);
    virtual CyadExitCode execute(SetPolicyCyadCommand &);
    virtual CyadExitCode execute(SetPolicyBulkCyadCommand &);

private:
    const std::string helpMessage = "Usage:\n"
                                    "--help             print help message\n"
                                    "--set-bucket       add bucket or set policy of existing one\n"
                                    "--delete-bucket    delete existing bucket\n"
                                    "--set-policy       add policy or update existing one";
    BaseDispatcherIO &m_io;
    BaseAdminApiWrapper &m_adminApiWrapper;
    struct cynara_admin *m_cynaraAdmin;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDSDISPATCHER_H_ */
