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
 * @file        src/service/monitor/EntriesQueue.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file defines class for storing monitor entries
 */

#ifndef SRC_SERVICE_MONITOR_ENTRIESQUEUE_H_
#define SRC_SERVICE_MONITOR_ENTRIESQUEUE_H_

#include <list>
#include <map>
#include <vector>

#include <types/MonitorEntry.h>

namespace Cynara {

class EntriesQueue {
public:
    typedef int EntryId;

    EntriesQueue();

    /*
     * Add entry to queue, entries are stored in order they arrive. If capacity is reached,
     * least recently pushed entry is removed. Returns true, when overflow occured.
     */
    bool push(const MonitorEntry &e);

    /*
     * Fetch given amount including given entryId, if not enough entries stored, returns empty
     * vector.
     */
    EntryId fetch(EntryId entryId, int amount, std::vector<MonitorEntry> &entries);

    /*
     * Remove entries from queue pushed before given entryId excluding given entryId.
     */
    void popUntil(EntryId entryId);

    size_t size(void) {
        return m_size;
    }

    void clear(void);

    /*
     * Return elements stored counted from given fromEntryId inclusively
     */
    int size(EntryId fromEntryId);

    bool empty(void) {
        return m_size == 0;
    }

    /*
     * Returns first valid stored entryId. Mind that first entryId can be bigger than last entryId.
     */
    int getFrontEntryId(void) {
        return m_firstBucketId * EntriesBucket::MAX_BUCKET_SIZE + m_entries[m_firstBucketId].offset;
    }

    /*
     * Returns last valid stored entryId. Mint that last entryId can be smaller than first entryId.
     */
    int getBackEntryId(void) {
        return m_lastBucketId * EntriesBucket::MAX_BUCKET_SIZE + m_entries[m_lastBucketId].offset;
    }

private:
    /*
     * Maximum size of whole queue.
     */
    static const int MAX_QUEUE_SIZE;

    /*
     * Structure for keeping buffer on monitor entries. Buffer never gets smaller, so offset
     * indicates if any entries were fetched from the beginning.
     */
    struct EntriesBucket {
        EntriesBucket() : offset(0) {}
        static const int MAX_BUCKET_SIZE;
        std::vector<MonitorEntry> entries;
        int offset;
    };
    void createBucket(int bucketId);
    void eraseBucket(int bucketId);
    int countBucketId(EntryId entryId);
    int countBucketOffset(EntryId entryId);
    int nextBucketId(int bucketId);
    void copyEntries(int bucket, int offset, int amount, std::vector<MonitorEntry> &entries);
    void removeEntries(int endBucket, int endOffset);
    bool checkSize();

    std::vector<EntriesBucket> m_entries;
    int m_firstBucketId;
    int m_lastBucketId;
    int m_size;
};


} /* namespace Cynara */

#endif /* SRC_SERVICE_MONITOR_ENTRIESQUEUE_H_ */
