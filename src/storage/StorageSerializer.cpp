/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/storage/StorageSerializer.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Implementation of Cynara::StorageSerializer methods
 */

#include <fstream>
#include <ios>
#include <memory>
#include <sstream>

#include <exceptions/BucketSerializationException.h>
#include <types/Policy.h>
#include <types/PolicyBucket.h>
#include <types/PolicyBucketId.h>
#include <types/PolicyCollection.h>
#include <types/PolicyType.h>

#include "StorageSerializer.h"

namespace Cynara {

template<typename T>
StorageSerializer<T>::StorageSerializer(std::shared_ptr<T> os) : m_outStream(os) {
}

template<typename T>
void StorageSerializer<T>::dump(const Buckets &buckets, BucketStreamOpener streamOpener) {
    for (const auto bucketIter : buckets) {
        const auto &bucket = bucketIter.second;

        dumpFields(bucket.id(), bucket.defaultPolicy().policyType(),
                   bucket.defaultPolicy().metadata());
    }

    for (const auto bucketIter : buckets) {
        const auto &bucketId = bucketIter.first;
        const auto &bucket = bucketIter.second;
        auto bucketSerializer = streamOpener(bucketId);

        if (bucketSerializer != nullptr) {
            bucketSerializer->dump(bucket);
        } else {
            throw BucketSerializationException(bucketId);
        }
    }
}

template<typename T>
void StorageSerializer<T>::dump(const PolicyBucket& bucket) {
    for (auto it = std::begin(bucket); it != std::end(bucket); ++it) {
        const auto &policy = *it;
        dump(policy);
    }
}

template<typename T>
void StorageSerializer<T>::dump(const PolicyKeyFeature &keyFeature) {
    *m_outStream << keyFeature.toString();
}

template<typename T>
void StorageSerializer<T>::dump(const PolicyType &policyType) {
    auto oldFormat = m_outStream->flags();
    *m_outStream << "0x" << std::uppercase <<  std::hex << policyType;
    m_outStream->flags(oldFormat);
}

template<typename T>
void StorageSerializer<T>::dump(const PolicyResult::PolicyMetadata &metadata) {
    *m_outStream << metadata;
}

template<typename T>
void StorageSerializer<T>::dump(const PolicyCollection::value_type &policy) {
    const auto &key = policy->key();
    const auto &result = policy->result();

    dumpFields(key.client(), key.user(), key.privilege(), result.policyType(), result.metadata());
}

template class StorageSerializer<std::ofstream>;
template class StorageSerializer<std::ostringstream>;
template class StorageSerializer<std::stringstream>;

} /* namespace Cynara */
