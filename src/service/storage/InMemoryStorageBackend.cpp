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
 * @file        InMemoryStorageBackend.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @version     1.0
 * @brief       Implementation of InMemoryStorageBackend
 */

#include <fstream>
#include <functional>

#include <exceptions/FileNotFoundException.h>
#include <storage/BucketDeserializer.h>
#include <storage/StorageDeserializer.h>
#include <types/PolicyBucketId.h>

#include "InMemoryStorageBackend.h"

namespace Cynara {

InMemoryStorageBackend::InMemoryStorageBackend(const std::string &dbPath) : m_dbPath(dbPath) {}


InMemoryStorageBackend::InMemoryStorageBackend() {
    // Make sure, there's always default bucket
    this->buckets().insert({ defaultPolicyBucketId, PolicyBucket() });
}

InMemoryStorageBackend::~InMemoryStorageBackend() {}

void InMemoryStorageBackend::load(void) {
    std::string indexFilename = m_dbPath + "buckets";

    std::ifstream indexStream;
    openFileStream(indexStream, indexFilename);

    StorageDeserializer storageDeserializer(indexStream,
        std::bind(&InMemoryStorageBackend::bucketStreamOpener, this, std::placeholders::_1));

    storageDeserializer.initBuckets(buckets());
    storageDeserializer.loadBuckets(buckets());

    // TODO: Add default bucket if it wasn't loaded from file
}

PolicyBucket InMemoryStorageBackend::searchDefaultBucket(const PolicyKey &key) {
    return searchBucket(defaultPolicyBucketId, key);
}

PolicyBucket InMemoryStorageBackend::searchBucket(const PolicyBucketId &bucketId,
                                                  const PolicyKey &key) {
    try {
        const auto &bucket = this->buckets().at(bucketId);
        return bucket.filtered(key);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::insertPolicy(const PolicyBucketId &bucketId, PolicyPtr policy) {
    try {
        auto &bucket = buckets().at(bucketId);
        auto &policies = bucket.policyCollection();
        policies.push_back(policy);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::createBucket(const PolicyBucketId &bucketId,
                                          const PolicyResult &defaultPolicy) {
    PolicyBucket newBucket;
    newBucket.setDefaultPolicy(defaultPolicy);
    buckets().insert({ bucketId, newBucket });
}

void InMemoryStorageBackend::updateBucket(const PolicyBucketId &bucketId,
                                          const PolicyResult &defaultPolicy) {
    try {
        auto &bucket = buckets().at(bucketId);
        bucket.setDefaultPolicy(defaultPolicy);
    } catch (const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::deleteBucket(const PolicyBucketId &bucketId) {
    auto bucketErased = buckets().erase(bucketId);
    if (bucketErased == 0) {
        throw BucketNotExistsException(bucketId);
    }
}

bool InMemoryStorageBackend::hasBucket(const PolicyBucketId &bucketId) {
    return buckets().find(bucketId) != buckets().end();
}

void InMemoryStorageBackend::deletePolicy(const PolicyBucketId &bucketId, const PolicyKey &key) {
    try {
        // TODO: Move the erase code to PolicyCollection maybe?
        auto &bucket = buckets().at(bucketId);
        auto &policies = bucket.policyCollection();
        policies.erase(remove_if(policies.begin(), policies.end(),
                [key](PolicyPtr policy) -> bool {
                    return policy->key() == key;
            }), policies.end());
    } catch(const std::out_of_range &) {
        throw BucketNotExistsException(bucketId);
    }
}

void InMemoryStorageBackend::deleteLinking(const PolicyBucketId &bucketId) {
    auto bucketIdMatches = [&bucketId] (PolicyPtr policy) -> bool {
        auto policyResult = policy->result();

        // Check bucket id only if policy is a bucket policy
        // TODO: Maybe move the test to PolicyResult
        if (policyResult.policyType() == PredefinedPolicyType::BUCKET) {
            return policyResult.metadata() == bucketId;
        }
        return false;
    };

    for(auto &bucketIter : buckets()) {
        // TODO: Move the erase code to PolicyCollection maybe?
        auto &bucket = bucketIter.second;
        auto &policies = bucket.policyCollection();
        policies.erase(remove_if(policies.begin(), policies.end(), bucketIdMatches),
                policies.end());
    }
}

void InMemoryStorageBackend::openFileStream(std::ifstream &stream, const std::string &filename) {
    // TODO: Consider adding exceptions to streams and handling them:
    // stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    stream.open(filename);

    if (!stream.is_open()) {
        throw FileNotFoundException(filename);
        return;
    }
}

std::shared_ptr<BucketDeserializer> InMemoryStorageBackend::bucketStreamOpener(
        const PolicyBucketId &bucketId) {
    std::string bucketFilename = m_dbPath + "_" + bucketId;
    std::ifstream bucketStream;
    try {
        openFileStream(bucketStream, bucketFilename);
        return std::make_shared<BucketDeserializer>(bucketStream);
    } catch (const FileNotFoundException &) {
        return nullptr;
    }
}

} /* namespace Cynara */
