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

#ifndef SRC_CYAD_COMMANDLINEPARSER_PARSINGRESULT_H_
#define SRC_CYAD_COMMANDLINEPARSER_PARSINGRESULT_H_

#include <string>

#include <types/PolicyBucketId.h>
#include <types/PolicyResult.h>
#include <types/PolicyType.h>

namespace Cynara {

class Cyad;

class ParsingResult {
public:
    ParsingResult() = default;
    virtual ~ParsingResult() = default;

    virtual void run(Cyad &cyad);

    virtual bool isError(void) const {
        return false;
    }
};

class ErrorParsingResult : public ParsingResult {
public:
    ErrorParsingResult(const std::string &message) : m_message(message) {};
    virtual ~ErrorParsingResult() = default;

    virtual void run(Cyad &cyad);

    virtual bool isError(void) const {
        return true;
    }

    virtual const std::string &message(void) {
        return m_message;
    }

private:
    std::string m_message;
};

class HelpParsingResult : public ParsingResult {
public:
    using ParsingResult::ParsingResult;

    virtual void run(Cyad &cyad);
};

class AddBucketParsingResult : public ParsingResult {
public:
    AddBucketParsingResult(const PolicyBucketId &bucketId, const PolicyType &policyType,
                           const PolicyResult::PolicyMetadata &metadata)
        : m_bucketId(bucketId), m_policyType(policyType), m_metadata(metadata) {}

    virtual ~AddBucketParsingResult() = default;

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

class DeleteBucketParsingResult : public ParsingResult {
public:
    explicit DeleteBucketParsingResult(const PolicyBucketId &bucketId) : m_bucketId(bucketId) {}
    virtual ~DeleteBucketParsingResult() = default;

    const PolicyBucketId &bucketId(void) const {
        return m_bucketId;
    }

private:
    PolicyBucketId m_bucketId;
};

} /* namespace Cynara */

#endif /* SRC_CYAD_COMMANDLINEPARSER_PARSINGRESULT_H_ */
