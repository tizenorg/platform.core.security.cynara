/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test/client-async/sequence/sequencecontainer.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       Tests for Cynara::SequenceContainer
 */


#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <client-async/sequence/SequenceContainer.h>

using namespace Cynara;

TEST(SequenceContainer, get_linear) {

    SequenceContainer seqBox;
    constexpr size_t seqMax = 100U;
    ProtocolFrameSequenceNumber seq[seqMax];

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.get(seq[i]);
        SCOPED_TRACE(std::string("get : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
        ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
    }

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.release(seq[i]);
        SCOPED_TRACE(std::string("release : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
    }
}

TEST(SequenceContainer, get_zig_zag) {

    SequenceContainer seqBox;
    constexpr size_t seqMax = 100U;
    constexpr size_t zigzags = 100U;
    ProtocolFrameSequenceNumber seq[seqMax];

    for (size_t j = 0; j < zigzags; ++j) {
        for (size_t i = 0; i < seqMax; ++i) {
            bool ret = seqBox.get(seq[i]);
            SCOPED_TRACE(std::string("get : ") + std::to_string(i) + std::string(", zigzag : ") + std::to_string(j));
            ASSERT_EQ(true, ret);
            ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
        }

        for (size_t i = 0; i < seqMax; ++i) {
            bool ret = seqBox.release(seq[seqMax - i - 1]);
            SCOPED_TRACE(std::string("release : ") + std::to_string(i) + std::string(", zigzag : ") + std::to_string(j));
            ASSERT_EQ(true, ret);
        }
    }
}

TEST(SequenceContainer, get_odd) {

    SequenceContainer seqBox;
    constexpr size_t seqMax = 100U;
    ProtocolFrameSequenceNumber seq[seqMax];

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.get(seq[i]);
        SCOPED_TRACE(std::string("get : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
        ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
    }

    for (size_t i = 1; i < seqMax; ++++i) {
        bool ret = seqBox.release(seq[i]);
        SCOPED_TRACE(std::string("release : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
    }

    for (size_t i = 1; i < seqMax; ++++i) {
        bool ret = seqBox.get(seq[i]);
        SCOPED_TRACE(std::string("get odd : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
        ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
    }
}

TEST(SequenceContainer, clear) {

    SequenceContainer seqBox;
    constexpr size_t seqMax = 100U;
    ProtocolFrameSequenceNumber seq[seqMax];

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.get(seq[i]);
        SCOPED_TRACE(std::string("get : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
        ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
    }

    seqBox.clear();

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.get(seq[i]);
        SCOPED_TRACE(std::string("get : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
        ASSERT_EQ(static_cast<ProtocolFrameSequenceNumber>(i), seq[i]);
    }

    for (size_t i = 0; i < seqMax; ++i) {
        bool ret = seqBox.release(seq[i]);
        SCOPED_TRACE(std::string("release : ") + std::to_string(i));
        ASSERT_EQ(true, ret);
    }
}
