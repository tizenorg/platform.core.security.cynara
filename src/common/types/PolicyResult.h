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
 * @file        PolicyResult.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       Definitions of PolicyResult and friends
 */

#ifndef POLICYRESULT_H_
#define POLICYRESULT_H_

#include "types/PolicyType.h"

#include <tuple>

namespace Cynara {

class PolicyResult {
public:
    typedef std::string PolicyMetadata;

    PolicyResult();
    PolicyResult(const PolicyType &policyType);
    PolicyResult(const PolicyType &policyType, const PolicyMetadata &metadata);

    const PolicyType &policyType() const;
    const PolicyMetadata& metadata() const;
    bool operator <(const PolicyResult &other) const;
    bool operator ==(const PolicyResult &other) const;
    bool operator !=(const PolicyResult &other) const;
    bool operator ==(const PolicyType &policyType) const;
    bool operator !=(const PolicyType &policyType) const;

private:
    PolicyType m_type;
    PolicyMetadata m_metadata;

};

} // namespace Cynara


#endif /* POLICYRESULT_H_ */
