/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
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
 * @file        frame_serializer.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       .
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <containers/BinaryQueue.h>
#include <protocol/ProtocolFrameHeader.h>
#include <protocol/ProtocolFrameSerializer.h>

using namespace Cynara;

class FakeProtocolFrameHeader : public ProtocolFrameHeader {
public:
    MOCK_METHOD0(isHeaderComplete, bool(void));
    MOCK_METHOD0(frameLength, ProtocolFrameLength(void));
    MOCK_METHOD0(setBodyComplete, void(void));

};

//class FakeBinaryQueue : public BinaryQueue {
//    MOCK_METHOD0(size, size_t(void));
//};

TEST(ProtocolFrameSerializer, deserializeHeader_emptyQueue) {
    using ::testing::Return;

    FakeProtocolFrameHeader header;
    BinaryQueue queue;

    EXPECT_CALL(header, isHeaderComplete()).WillOnce(Return(false));

    ProtocolFrameSerializer::deserializeHeader(header, queue);
}

TEST(ProtocolFrameSerializer, deserializeHeader_headerComplete) {
    using ::testing::Return;

    FakeProtocolFrameHeader header;
    BinaryQueue queue;

    EXPECT_CALL(header, isHeaderComplete()).WillOnce(Return(true));
    EXPECT_CALL(header, frameLength()).WillOnce(Return(ProtocolFrameLength(0)));

    ProtocolFrameSerializer::deserializeHeader(header, queue);
}

//TEST(ProtocolFrameSerializer, deserializeHeader_bodyComplete) {
//    using ::testing::Return;
//
//    FakeProtocolFrameHeader header;
//    FakeBinaryQueue queue;
//
//    EXPECT_CALL(queue, size()).WillOnce(Return(size_t(ProtocolFrameHeader::frameHeaderLength())));
//    EXPECT_CALL(header, isHeaderComplete()).WillOnce(Return(true));
//    EXPECT_CALL(header, frameLength()).WillOnce(Return(ProtocolFrameLength(0)));
//
//    ProtocolFrameSerializer::deserializeHeader(header, queue);
//}
