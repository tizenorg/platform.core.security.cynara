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
 * @file        src/common/containers/SharedString.cpp
 * @author      Bartlomiej Grzelewski <b.grzelewski@samsung.com>
 * @version     1.0
 * @brief       This file is implementation of SharedString
 */

#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "SharedString.h"

namespace Cynara {
namespace ExperiMental {

StringStorage& StringStorage::getInstance() {
    static StringStorage storage;
    return storage;
}

SharedStringInternal* StringStorage::get(const std::string &key) {
    auto it = m_sharedStringMap.find(key);
    if (it != m_sharedStringMap.end())
        return it->second.get();

    SharedStringInternal *n = new SharedStringInternal;
    n->refCount = 0;
    n->value = key;
    m_sharedStringMap.emplace(std::make_pair(key, SharedStringUPtr(n)));
    return n;
}

void StringStorage::erase(const std::string &key) {
    m_sharedStringMap.erase(key);
}

SharedString::SharedString(const std::string &key)
  : m_internal(nullptr)
{
    m_internal = StringStorage::getInstance().get(key);
    addref();
}

SharedString::SharedString(SharedStringInternal *internal)
  : m_internal(internal)
{
    addref();
}

SharedString::SharedString(const SharedString &second)
  : m_internal(second.m_internal)
{
    addref();
}

SharedString::SharedString(SharedString &&second)
  : m_internal(second.m_internal)
{
    second.m_internal = nullptr;
}

SharedString& SharedString::operator=(const SharedString &second) {
    unref();
    m_internal = second.m_internal;
    addref();
    return *this;
}

SharedString& SharedString::operator=(SharedString &&second) {
    unref();
    m_internal = second.m_internal;
    second.m_internal = nullptr;
    return *this;
}

const std::string& SharedString::toString() const {
    if (!m_internal)
        throw std::logic_error("not initialized");
    return m_internal->value;
}

SharedString::~SharedString() {
    if (m_internal)
        unref();
}

void SharedString::addref() {
    if (!m_internal)
        return;
    ++(m_internal->refCount);
}

void SharedString::unref() {
    if (!m_internal)
        return;
    --(m_internal->refCount);
    if (0 == m_internal->refCount)
        StringStorage::getInstance().erase(m_internal->value);
}

} // namespace ExperiMental
} // namespace Cynara

