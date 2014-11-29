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
 * @file        test/common/protocols/RequestResponseTestHelper.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       helper class for protocol tests of requests and responses
 */

#ifndef TEST_COMMON_PROTOCOLS_REQUESTRESPONSETESTHELPER_H_
#define TEST_COMMON_PROTOCOLS_REQUESTRESPONSETESTHELPER_H_

#include <memory>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <containers/BinaryQueue.h>
#include <containers/RawBuffer.h>
#include <protocol/Protocol.h>
#include <request/pointers.h>
#include <request/RequestContext.h>
#include <response/pointers.h>
#include <response/ResponseTaker.h>

template <typename R>
class RequestResponseComparator {
public:
    static void compare(const R *r1, const R *r2) {
        (void) r1;
        (void) r2;
        ASSERT_FALSE("Specialization of compare method must be implemented!");
    }
};

template <typename P, typename R>
class RequestResponseTestHelper {
public:
    RequestResponseTestHelper() {
        m_protocol = std::make_shared<P>();
        m_queue = std::make_shared<Cynara::BinaryQueue>();
        m_context = std::make_shared<Cynara::RequestContext>(Cynara::ResponseTakerPtr(), m_queue);
    }

    template<typename... Args>
    void testRequest(Args... args) {
        auto r = std::dynamic_pointer_cast<R>(std::make_shared<R>(args...));
        r->execute(r, m_protocol, m_context);
        auto r2 = std::dynamic_pointer_cast<R>(m_protocol->extractRequestFromBuffer(m_queue));

        RequestResponseComparator<R>::compare(r.get(), r2.get());
        ASSERT_EQ(m_queue->size(), 0);
    }

    template<typename... Args>
    void testResponse(Args... args) {
        auto r = std::dynamic_pointer_cast<R>(std::make_shared<R>(args...));
        r->execute(r, m_protocol, m_context);
        auto r2 = std::dynamic_pointer_cast<R>(m_protocol->extractResponseFromBuffer(m_queue));

        RequestResponseComparator<R>::compare(r.get(), r2.get());
        ASSERT_EQ(m_queue->size(), 0);
    }

    template<typename... Args>
    void binaryTestRequest(Args... args) {
        auto r = std::dynamic_pointer_cast<R>(std::make_shared<R>(args...));
        r->execute(r, m_protocol, m_context);
        Cynara::RawBuffer data(m_queue->size());
        m_queue->flatten(data.data(), m_queue->size());

        auto r2 = std::dynamic_pointer_cast<R>(m_protocol->extractRequestFromBuffer(m_queue));
        EXPECT_EQ(m_queue->size(), 0);

        r2->execute(r2, m_protocol, m_context);
        Cynara::RawBuffer data2(m_queue->size());
        m_queue->flatten(data2.data(), m_queue->size());

        ASSERT_EQ(data.size(), data2.size());
        ASSERT_EQ(memcmp(data.data(), data2.data(), data.size()), 0);
    }

    template<typename... Args>
    void binaryTestResponse(Args... args) {
        auto r = std::dynamic_pointer_cast<R>(std::make_shared<R>(args...));
        r->execute(r, m_protocol, m_context);
        Cynara::RawBuffer data(m_queue->size());
        m_queue->flatten(data.data(), m_queue->size());

        auto r2 = std::dynamic_pointer_cast<R>(m_protocol->extractResponseFromBuffer(m_queue));
        EXPECT_EQ(m_queue->size(), 0);

        r2->execute(r2, m_protocol, m_context);
        Cynara::RawBuffer data2(m_queue->size());
        m_queue->flatten(data2.data(), m_queue->size());

        ASSERT_EQ(data.size(), data2.size());
        ASSERT_EQ(memcmp(data.data(), data2.data(), data.size()), 0);
    }

private:
    Cynara::ProtocolPtr m_protocol;
    Cynara::BinaryQueuePtr m_queue;
    Cynara::RequestContextPtr m_context;
};

#endif /* TEST_COMMON_PROTOCOLS_REQUESTRESPONSETESTHELPER_H_ */

#if 0
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
 * @file        test/common/protocols/RequestResponseTestHelper.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       helper class for protocol tests of requests and responses
 */

#ifndef TEST_COMMON_PROTOCOLS_REQUESTRESPONSETESTHELPER_H_
#define TEST_COMMON_PROTOCOLS_REQUESTRESPONSETESTHELPER_H_

#include <memory>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <containers/BinaryQueue.h>
#include <containers/RawBuffer.h>
#include <protocol/Protocol.h>
#include <request/pointers.h>
#include <request/RequestContext.h>
#include <response/pointers.h>
#include <response/ResponseTaker.h>

template <typename R>
class RequestResponseComparator {
public:

    static void compare(const R *r1, const R *r2) {
        (void) r1;
        (void) r2;
        ASSERT_FALSE("Specialization of compare method must be implemented!");
    }
};

template <typename P, typename R>
class RequestResponseTestHelper {
public:

    RequestResponseTestHelper() {
        m_protocol = std::make_shared<P>();
        m_queue = std::make_shared<Cynara::BinaryQueue>();
        m_context = std::make_shared<Cynara::RequestContext>(Cynara::ResponseTakerPtr(), m_queue);
    }

    template<typename... Args>
    void testRequest(Args... args) {
        Cynara::RequestPtr request = std::make_shared<R>(args...);
        request->execute(request, m_protocol, m_context);
        Cynara::RequestPtr request2 = m_protocol->extractRequestFromBuffer(m_queue);

        RequestResponseComparator<R>::compare((std::dynamic_pointer_cast<R>(request)).get(),
                (std::dynamic_pointer_cast<R>(request2)).get());
        ASSERT_EQ(m_queue->size(), 0);
    }

    template<typename... Args>
    void testResponse(Args... args) {
        Cynara::ResponsePtr response = std::make_shared<R>(args...);
        response->execute(response, m_protocol, m_context);
        Cynara::ResponsePtr response2 = m_protocol->extractResponseFromBuffer(m_queue);

        RequestResponseComparator<R>::compare((std::dynamic_pointer_cast<R>(response)).get(),
                (std::dynamic_pointer_cast<R>(response2)).get());
        ASSERT_EQ(m_queue->size(), 0);
    }

    template<typename... Args>
    void binaryTestRequest(Args... args) {
        Cynara::RequestPtr request = std::make_shared<R>(args...);
        request->execute(request, m_protocol, m_context);
        Cynara::RawBuffer data(m_queue->size());
        m_queue->flatten(data.data(), m_queue->size());

        Cynara::RequestPtr request2 = m_protocol->extractRequestFromBuffer(m_queue);
        EXPECT_EQ(m_queue->size(), 0);

        request2->execute(request2, m_protocol, m_context);
        Cynara::RawBuffer data2(m_queue->size());
        m_queue->flatten(data2.data(), m_queue->size());

        ASSERT_EQ(data.size(), data2.size());
        ASSERT_EQ(memcmp(data.data(), data2.data(), data.size()), 0);
    }

    template<typename... Args>
    void binaryTestResponse(Args... args) {
        Cynara::ResponsePtr response = std::make_shared<R>(args...);
        response->execute(response, m_protocol, m_context);
        Cynara::RawBuffer data(m_queue->size());
        m_queue->flatten(data.data(), m_queue->size());

        Cynara::ResponsePtr response2 = m_protocol->extractResponseFromBuffer(m_queue);
        EXPECT_EQ(m_queue->size(), 0);

        response2->execute(response2, m_protocol, m_context);
        Cynara::RawBuffer data2(m_queue->size());
        m_queue->flatten(data2.data(), m_queue->size());

        ASSERT_EQ(data.size(), data2.size());
        ASSERT_EQ(memcmp(data.data(), data2.data(), data.size()), 0);
    }

private:
    Cynara::ProtocolPtr m_protocol;
    Cynara::BinaryQueuePtr m_queue;
    Cynara::RequestContextPtr m_context;
};

#endif /* TEST_COMMON_PROTOCOLS_REQUESTRESPONSETESTHELPER_H_ */

#endif
