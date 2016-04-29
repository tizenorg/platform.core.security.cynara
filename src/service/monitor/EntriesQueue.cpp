/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/service/monitor/EntriesQueue.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file implements class for storing monitor entries
 */

#include <algorithm>
#include <limits>

#include "EntriesQueue.h"

#include <log/log.h>
namespace Cynara {

const int EntriesQueue::MAX_QUEUE_SIZE = 2 * std::numeric_limits<uint16_t>::max();
const int EntriesQueue::EntriesBucket::MAX_BUCKET_SIZE = 2 * 255;

EntriesQueue::EntriesQueue() : m_firstBucketId(0), m_lastBucketId(0), m_size(0) {
    m_entries.resize(MAX_QUEUE_SIZE/EntriesBucket::MAX_BUCKET_SIZE);
}

void EntriesQueue::push(const MonitorEntry &e) {
    auto &lastBucket = m_entries[m_lastBucketId];
    lastBucket.entries.push_back(e);
    if (lastBucket.entries.size() == EntriesBucket::MAX_BUCKET_SIZE) {
        m_lastBucketId = nextBucketId(m_lastBucketId);
        createBucket(m_lastBucketId);
    }
    m_size++;
    checkSize();
}

void EntriesQueue::checkSize() {
    if (m_size < MAX_QUEUE_SIZE) {
        return;
    }
    LOGW("Maximum capacity reached. Removing least recently pushed entry.");
    auto &firstBucket = m_entries[m_firstBucketId];
    firstBucket.offset++;
    if (firstBucket.offset >= EntriesBucket::MAX_BUCKET_SIZE) {
        eraseBucket(m_firstBucketId);
        m_firstBucketId = nextBucketId(m_firstBucketId);
    }
}
EntriesQueue::EntryId EntriesQueue::fetch(EntriesQueue::EntryId entryId, int amount,
                                          std::vector<MonitorEntry> &entries) {

    if (size(entryId) < amount) {
        LOGE("Not enough entries stored to fetch [" << amount << "]");
        return -1;
    }
    int startBucketId = countBucketId(entryId);
    int startBucketOffset = countBucketOffset(entryId);
    copyEntries(startBucketId, startBucketOffset, amount, entries);
    return (entryId + amount) % MAX_QUEUE_SIZE;
}

void EntriesQueue::popUntil(EntryId entryId) {
    int endBucketId = countBucketId(entryId);
    int endBucketOffset = countBucketOffset(entryId);
    removeEntries(endBucketId, endBucketOffset);
}

void EntriesQueue::clear(void) {
    m_lastBucketId = m_firstBucketId = 0;
    m_entries.clear();
    m_entries.resize(MAX_QUEUE_SIZE/EntriesBucket::MAX_BUCKET_SIZE);
}
int EntriesQueue::size(EntryId fromEntryId) {
    int lastEntryId = m_lastBucketId * EntriesBucket::MAX_BUCKET_SIZE
                      + m_entries[m_lastBucketId].offset;
    return (lastEntryId + MAX_QUEUE_SIZE - fromEntryId) % (MAX_QUEUE_SIZE + 1);
}

void EntriesQueue::createBucket(int bucketId) {
    m_entries[bucketId].offset = 0;
    m_entries[bucketId].entries.reserve(EntriesBucket::MAX_BUCKET_SIZE);
}
void EntriesQueue::eraseBucket(int bucketId) {
    // Trick for forcing vector to free its memory
    m_size -= m_entries[bucketId].entries.size();
    std::vector<MonitorEntry>().swap(m_entries[bucketId].entries);
    m_entries[bucketId].offset = 0;
}

int EntriesQueue::countBucketId(EntryId entryId) {
    return entryId / EntriesBucket::MAX_BUCKET_SIZE;
}

int EntriesQueue::countBucketOffset(EntryId entryId) {
    return entryId % EntriesBucket::MAX_BUCKET_SIZE;
}

int EntriesQueue::nextBucketId(int bucketId) {
    if (static_cast<unsigned int>(bucketId) + 1 >= m_entries.size())
        return 0;
    else
        return bucketId + 1;
}

void EntriesQueue::copyEntries(int bucketId, int offset, int amount,
                               std::vector<MonitorEntry> &entries) {
    while (entries.size() < static_cast<unsigned int>(amount)) {
        auto &bucket = m_entries[bucketId];
        int bucketAmount = std::min((bucket.offset + amount),
                                     static_cast<int>(bucket.entries.size()));
        entries.insert(entries.end(), bucket.entries.begin() + offset,
                                      bucket.entries.begin() + bucketAmount);
        bucketId = nextBucketId(bucketId);
        offset = 0;
    }
}

void EntriesQueue::removeEntries(int endBucketId, int endOffset) {
    int bucketId = m_firstBucketId;
    while (bucketId != endBucketId) {
        eraseBucket(bucketId);
        bucketId = nextBucketId(bucketId);
    }
    m_size -= endOffset - m_entries[bucketId].offset;
    m_entries[bucketId].offset = endOffset;
    m_firstBucketId = bucketId;
}

} /* namespace Cynara */
