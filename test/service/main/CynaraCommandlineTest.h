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
 * @file        test/service/main/CynaraCommandlineTest.h
 * @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
 * @version     1.0
 * @brief       Fixture for CmdlineParser tests
 */

#ifndef TEST_SERVICE_MAIN_CYNARACOMMANDLINETEST_H_
#define TEST_SERVICE_MAIN_CYNARACOMMANDLINETEST_H_

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include <BaseCommandlineTest.h>

class CynaraCommandlineTest : public BaseCommandlineTest {
public:
    void cleanOutput(void) {
        m_cout.str(std::string());
    }

    void getOutput(std::string &output) const {
        output = m_cout.str();
    }

protected:
    virtual void SetUp(void) {
        m_backup.reset(std::cout.rdbuf());
        std::cout.rdbuf(m_cout.rdbuf());
    }

    virtual void TearDown(void) {
        destroy_argv();
        std::cout.rdbuf(m_backup.release());
    }

private:
    std::unique_ptr<std::streambuf> m_backup;
    std::stringstream m_cout;
};

#endif /* TEST_SERVICE_MAIN_CYNARACOMMANDLINETEST_H_ */
