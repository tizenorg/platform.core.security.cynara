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
 * @file        FakeAdminApiWrapper.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       
 */

#ifndef TEST_CYAD_FAKEADMINAPIWRAPPER_H_
#define TEST_CYAD_FAKEADMINAPIWRAPPER_H_

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cyad/AdminApiWrapper.h>

class FakeAdminApiWrapper : public Cynara::AdminApiWrapper {
public:
    using AdminApiWrapper::AdminApiWrapper;

    MOCK_METHOD2(cynara_admin_set_policies, int(struct cynara_admin *p_cynara_admin,
                 const struct cynara_admin_policy *const *policies));
    MOCK_METHOD4(cynara_admin_set_bucket, int(struct cynara_admin *p_cynara_admin,
                 const char *bucket, int operation, const char *extra));
    MOCK_METHOD8(cynara_admin_check, int(struct cynara_admin *p_cynara_admin,
                 const char *start_bucket, const int recursive,
                 const char *client, const char *user, const char *privilege,
                 int *result, char **result_extra));
};

#endif /* TEST_CYAD_FAKEADMINAPIWRAPPER_H_ */
