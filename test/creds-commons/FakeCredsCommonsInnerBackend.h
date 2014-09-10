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
 * @file        test/creds-commons/FakeCredsCommonsInnerBackend.h
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       Mock of CredsCommonsInnerBackend
 */

#ifndef FAKECREDSCOMMONSINNERBACKEND_H_
#define FAKECREDSCOMMONSINNERBACKEND_H_

#include <helpers/creds-commons/CredsCommonsInner.h>

class FakeCredsCommonsInnerBackend : public CredsCommonsInnerBackend {
public:

    MOCK_METHOD4(getKeyAndValue, bool(std::istream &f, std::locale &loc, std::string &key,
                                      std::string &value));
    MOCK_METHOD6(interpretKeyAndValue, int(const CredentialsMap &methodCodeMap,
                                           const std::string &methodName, int &method,
                                           const std::string &key,const std::string &value,
                                           bool &occurred));
};

class FakeGetKeyAndValueBackend : public CredsCommonsInnerBackend {
public:
    MOCK_METHOD4(getKeyAndValue, bool(std::istream &f, std::locale &loc, std::string &key,
                                      std::string &value));
};
#endif /* FAKECREDSCOMMONSINNERBACKEND_H_ */
