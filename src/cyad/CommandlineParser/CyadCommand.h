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
/*
 * @file        src/cyad/CommandlineParser/ParsingResult.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       ParsingResult class
 */

#ifndef SRC_CYAD_COMMANDLINEPARSER_CYADCOMMAND_H_
#define SRC_CYAD_COMMANDLINEPARSER_CYADCOMMAND_H_

#include <string>

#include <types/PolicyBucketId.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

namespace Cynara {

class CommandsDispatcher;

class CyadCommand {
public:
    CyadCommand() = default;
    virtual ~CyadCommand() = default;

    virtual void run(CommandsDispatcher &dispatcher);

    virtual bool isError(void) const {
        return false;
    }
};

class ErrorCyadCommand : public CyadCommand {
public:
    ErrorCyadCommand(const std::string &message) : m_message(message) {};
    virtual ~ErrorCyadCommand() = default;

    virtual void run(CommandsDispatcher &dispatcher);

    virtual bool isError(void) const {
        return true;
    }

    virtual const std::string &message(void) const {
        return m_message;
    }

private:
    std::string m_message;
};

class HelpCyadCommand : public CyadCommand {
public:
    using CyadCommand::CyadCommand;

    virtual void run(CommandsDispatcher &dispatcher);
};

class SetBucketCyadCommand : public CyadCommand {
public:
    SetBucketCyadCommand(const PolicyBucketId &bucketId, const PolicyType &policyType,
                         const PolicyResult::PolicyMetadata &metadata)
        : m_bucketId(bucketId), m_policyType(policyType), m_metadata(metadata) {}

    virtual ~SetBucketCyadCommand() = default;

    virtual void run(CommandsDispatcher &dispatcher);

    const PolicyBucketId &bucketId(void) const {
        return m_bucketId;
    }

    const PolicyType &policyType(void) const {
        return m_policyType;
    }

    const PolicyResult::PolicyMetadata &metadata(void) {
        return m_metadata;
    }

private:
    PolicyBucketId m_bucketId;
    PolicyType m_policyType;
    PolicyResult::PolicyMetadata m_metadata;
};

class DeleteBucketCyadCommand : public CyadCommand {
public:
    explicit DeleteBucketCyadCommand(const PolicyBucketId &bucketId) : m_bucketId(bucketId) {}
    virtual ~DeleteBucketCyadCommand() = default;

    virtual void run(CommandsDispatcher &dispatcher);

    const PolicyBucketId &bucketId(void) const {
        return m_bucketId;
    }

private:
    PolicyBucketId m_bucketId;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDLINEPARSER_CYADCOMMAND_H_ */
