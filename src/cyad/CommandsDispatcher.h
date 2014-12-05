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

#include <ostream>

#include <cyad/BaseAdminApiWrapper.h>
#include <cyad/CommandlineParser/CyadCommand.h>

struct cynara_admin;

namespace Cynara {

class CommandsDispatcher {
public:
    CommandsDispatcher(std::ostream &outStream, BaseAdminApiWrapper &adminApiWrapper);
    virtual ~CommandsDispatcher();

    virtual void execute(CyadCommand &);
    virtual void execute(HelpCyadCommand &);
    virtual void execute(ErrorCyadCommand &);
    virtual void execute(DeleteBucketCyadCommand &);
    virtual void execute(SetBucketCyadCommand &);

private:
    const std::string helpMessage = "Cyad's future help message";
    std::ostream &m_outStream;
    BaseAdminApiWrapper &m_adminApiWrapper;
    struct cynara_admin *m_cynaraAdmin;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDSDISPATCHER_H_ */
