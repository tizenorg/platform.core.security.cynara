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
 * @file        FakeDispatcherIO.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       
 */

#ifndef TEST_CYAD_FAKEDISPATCHERIO_H_
#define TEST_CYAD_FAKEDISPATCHERIO_H_

#include <memory>

#include <cyad/BaseDispatcherIO.h>

class FakeDispatcherIO : public Cynara::BaseDispatcherIO {
public:

    FakeDispatcherIO() : m_inputFile(new std::stringstream()) {}
    ~FakeDispatcherIO() = default;

    BaseDispatcherIO::InputStreamPtr openFile(const BaseDispatcherIO::Filename &) {
        return m_inputFile;
    }

    std::ostream &cout() {
        return m_out;
    }

    std::istream &cin() {
        return m_cin;
    }

    std::ostream &cerr() {
        return m_err;
    }

    std::ostream &file() {
        return *m_inputFile;
    }

    std::stringstream &cerrRaw() {
        return m_err;
    }

    std::stringstream &coutRaw() {
        return m_out;
    }

    std::stringstream &cinRaw() {
        return m_cin;
    }

private:
    std::stringstream m_out;
    std::stringstream m_err;
    std::stringstream m_cin;
    std::shared_ptr<std::stringstream> m_inputFile;
};


#endif /* TEST_CYAD_FAKEDISPATCHERIO_H_ */
