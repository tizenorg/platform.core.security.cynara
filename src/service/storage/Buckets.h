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
 * @file        Buckets.h
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Headers for Buckets
 */

#ifndef SRC_SERVICE_STORAGE_BUCKETS_H_
#define SRC_SERVICE_STORAGE_BUCKETS_H_

#include <unordered_map>

#include <types/PolicyBucket.h>
#include <types/PolicyBucketId.h>

namespace Cynara {

typedef std::unordered_map<PolicyBucketId, PolicyBucket> Buckets;

} /* namespace Cynara */

#endif /* SRC_SERVICE_STORAGE_BUCKETS_H_ */
