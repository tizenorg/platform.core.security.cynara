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
 * @file        test/creds-commons/parser.cpp
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       Tests of internal implementation of credential commons functions
 */

#include <exception>
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

#include "FakeCredsCommonsInnerBackend.h"

#include <cynara-creds-commons.h>
#include "CredsCommonsInner.h"

/* Tests */
using namespace Cynara;
using ::testing::_;
using ::testing::Assign;
using ::testing::AtMost;
using ::testing::Between;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::Throw;


#define NOT_A_METHOD_CODE -13197 //number which is not a method code

/*** Test for getKeyAndValue() ***/

struct getKeyAndValueTestEntry {
    std::string testString;
    std::locale loc;
    bool expectedResult;
    std::string key;
    std::string value;
};

TEST(parser, getKeyAndValue) {

    static std::vector<struct getKeyAndValueTestEntry>  getKeyAndValueTestEntries;
    // test cases initialization key and value are checked only if expectedResult is true
    getKeyAndValueTestEntries.push_back({"", std::locale("en_US.UTF8"), false , "", ""});
    getKeyAndValueTestEntries.push_back({"#", std::locale("en_US.UTF8"), false , "", ""});
    getKeyAndValueTestEntries.push_back({"a", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"=", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"\n", std::locale("en_US.UTF8"), false , "", ""});
    getKeyAndValueTestEntries.push_back({"aa", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"=#", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"=\n", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"\n=", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"==", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"#=", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"aa", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"##", std::locale("en_US.UTF8"), false , "", ""});
    getKeyAndValueTestEntries.push_back({"a=b", std::locale("en_US.UTF8"), true, "a", "b"});
    getKeyAndValueTestEntries.push_back({"=aa", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"aa=", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"#a=", std::locale("en_US.UTF8"), false , "", ""});
    getKeyAndValueTestEntries.push_back({"a=#", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"=#a", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"==a", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"=a=", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"a=#\nb", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"a=#b\n", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"a\nb", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"\na=b", std::locale("en_US.UTF8"), true, "a", "b"});
    getKeyAndValueTestEntries.push_back({"a=#\nb", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"a=\nb", std::locale("en_US.UTF8"), false, "", ""});
    getKeyAndValueTestEntries.push_back({"a=\nb=c#", std::locale("en_US.UTF8"), true, "b", "c"});
    getKeyAndValueTestEntries.push_back({"aa\nkey=value", std::locale("en_US.UTF8"), true, "key",
        "value"});
    getKeyAndValueTestEntries.push_back({" key with spaces  = value with spaces #\n",
                                         std::locale("en_US.UTF8"), true, "key with spaces",
                                         "value with spaces"});
    getKeyAndValueTestEntries.push_back({"VeryLongKey111111111111111111111111111111111111111111111"
                                         "11111111111111111111111111111111111111111111111111111111"
                                         "111111111 = 1\nnoImportant", std::locale("en_US.UTF8"),
                                         true,
                                         "verylongkey111111111111111111111111111111111111111111111"
                                         "11111111111111111111111111111111111111111111111111111111"
                                         "111111111", "1"});
    getKeyAndValueTestEntries.push_back({"key=value", std::locale("en_US.UTF8"), true, "key",
                                         "value"});
    getKeyAndValueTestEntries.push_back({"CAPSON=CaPSon", std::locale("en_US.UTF8"), true,
                                         "capson", "capson"});
    getKeyAndValueTestEntries.push_back({" soMe_spacEs_ = vaLue# ", std::locale("en_US.UTF8"), true,
                                          "some_spaces_", "value"});
    int i = 0;
    for (auto it = getKeyAndValueTestEntries.begin(); it != getKeyAndValueTestEntries.end(); ++it) {
        i++;
        std::string keyAtStart("predefinedKey["+it->testString+"]");
        std::string valueAtStart("prdefinedValue["+it->testString+"]");
        std::string key(keyAtStart);
        std::string value(valueAtStart);
        std::stringstream ss;
        ss << it->testString;

        EXPECT_EQ(it->expectedResult, credsBackend.getKeyAndValue(ss, it->loc, key, value))
        << "Result code in case no# "<< i << " [" + it->testString + "] was wrong" <<std::endl;
        if (it->expectedResult) {
            //key and value shall be set if getKeyAndValue returns true
            EXPECT_EQ(it->key, key) << "A in case no# " << i <<std::endl;
            EXPECT_EQ(it->value, value) << "B in case no# " << i <<std::endl;
        } else {
            //key and value should not be changed if getKeyAndValue returns false
            EXPECT_EQ(key, keyAtStart) << "C in case no# " << i <<std::endl;
            EXPECT_EQ(value, valueAtStart) << "D in case no# " << i <<std::endl;
        }
    }
}

/*** Tests for interpretKeyAndValue() ***/

TEST(parser, interpretKeyAndValue_OK) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    bool occurred = false;
    EXPECT_EQ(OK_FOUND_THE_KEY_FIRST_TIME_WITH_CORRECT_VALUE,
              credsBackend.interpretKeyAndValue(clientCredsMap, "methodName", method,
                                                "methodName", "smack", occurred));
    EXPECT_EQ(true, occurred);
    EXPECT_EQ(CLIENT_METHOD_SMACK, method);
}

TEST(parser, interpretKeyAndValueKeyNotInMap) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    bool occurred = false;
    EXPECT_EQ(BAD_FOUND_THE_KEY_FIRST_TIME_WITH_WRONG_VALUE,
              credsBackend.interpretKeyAndValue(clientCredsMap, "methodName", method,
                                                "methodName", "NeitherSmackNorPid",
                                                 occurred));
    EXPECT_EQ(true, occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, interpretKeyAndValueKeyAppearsAgain) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    bool occurred = true;
    EXPECT_EQ(BAD_FOUND_THE_KEY_AGAIN,
              credsBackend.interpretKeyAndValue(clientCredsMap, "methodName", method,
                                               "methodName", "Anything",
                                                occurred));
    EXPECT_EQ(true, occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, interpretKeyAndValueEmptyMap1) {
    static const CredentialsMap clientCredsMap{};
    int method = NOT_A_METHOD_CODE;
    bool occurred = false;
    EXPECT_EQ(BAD_FOUND_THE_KEY_FIRST_TIME_WITH_WRONG_VALUE,
              credsBackend.interpretKeyAndValue(clientCredsMap, "methodName", method,
                                               "methodName", "Anything",
                                                occurred));
    EXPECT_EQ(true, occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, interpretKeyAndValueEmptyMap2) {
    static const CredentialsMap clientCredsMap{};
    int method = NOT_A_METHOD_CODE;
    bool occurred = true;
    EXPECT_EQ(BAD_FOUND_THE_KEY_AGAIN,
              credsBackend.interpretKeyAndValue(clientCredsMap, "methodName", method,
                                               "methodName", "Anything",
                                                occurred));
    EXPECT_EQ(true, occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, interpretKeyAndValueOtherKey1) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    bool occurred = true;
    EXPECT_EQ(OK_FOUND_OTHER_KEY,
              credsBackend.interpretKeyAndValue(clientCredsMap, "methodName1", method,
                                                "methodName2", "entryNotInClientCredsMap",
                                                 occurred));
    EXPECT_EQ(true, occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, interpretKeyAndValueOtherKey2) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    bool occurred = false;
    EXPECT_EQ(OK_FOUND_OTHER_KEY,
              credsBackend.interpretKeyAndValue(clientCredsMap, "methodName1", method,
                                                "methodName2", "entryNotInClientCredsMap",
                                                 occurred));
    EXPECT_EQ(false, occurred);
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

/*** Tests for getMethodFromConfigurationFileOK ***/

TEST(parser, getMethodFromConfigurationFileOK2Lines) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method;
    FakeCredsCommonsInnerBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .WillOnce(Return(true))
    .WillOnce(Return(false));
    EXPECT_CALL(fakeBackend, interpretKeyAndValue(_, _, _, _,_, _))
    .WillOnce(DoAll(SetArgReferee<2>(CLIENT_METHOD_SMACK), SetArgReferee<5>(true),
              Return(OK_FOUND_THE_KEY_FIRST_TIME_WITH_CORRECT_VALUE)));

    EXPECT_EQ(CYNARA_API_SUCCESS,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(CLIENT_METHOD_SMACK, method);
}

TEST(parser, getMethodFromConfigurationFileOKManyLines) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method;
    FakeCredsCommonsInnerBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .Times(AtMost(4))
    .WillOnce(Return(true))
    .WillOnce(Return(true))
    .WillOnce(Return(true))
    .WillOnce(Return(false));
    EXPECT_CALL(fakeBackend, interpretKeyAndValue(_, _, _, _,_, _))
    .Times(AtMost(3))
    .WillOnce(DoAll(SetArgReferee<2>(CLIENT_METHOD_SMACK), SetArgReferee<5>(true),
              Return(OK_FOUND_THE_KEY_FIRST_TIME_WITH_CORRECT_VALUE)))
    .WillOnce(Return(OK_FOUND_OTHER_KEY))
    .WillOnce(Return(OK_FOUND_OTHER_KEY));

    EXPECT_EQ(CYNARA_API_SUCCESS,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(CLIENT_METHOD_SMACK, method);
}

TEST(parser, getMethodFromConfigurationFileBadManyLines) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;;
    FakeCredsCommonsInnerBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .Times(AtMost(4))
    .WillOnce(Return(true))
    .WillOnce(Return(true))
    .WillOnce(Return(true))
    .WillOnce(Return(false));
    EXPECT_CALL(fakeBackend, interpretKeyAndValue(_, _, _, _,_, _))
    .Times(AtMost(3))
    .WillOnce(DoAll(SetArgReferee<2>(CLIENT_METHOD_SMACK), SetArgReferee<5>(true),
              Return(OK_FOUND_THE_KEY_FIRST_TIME_WITH_CORRECT_VALUE)))
    .WillOnce(Return(OK_FOUND_OTHER_KEY))
    .WillOnce(Return(BAD_FOUND_THE_KEY_AGAIN));

    EXPECT_EQ(CYNARA_API_CONFIGURATION_ERROR,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, getMethodFromConfigurationFileManyStrangeKeyAndValues) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method;
    FakeGetKeyAndValueBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("key1"), SetArgReferee<3>("value1"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("key2"), SetArgReferee<3>("value2"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("key3"), SetArgReferee<3>("value3"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("key4"), SetArgReferee<3>("value4"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>(""), SetArgReferee<3>(""), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("pid"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>(""), SetArgReferee<3>(""), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("key5"), SetArgReferee<3>("value5"), Return(true)))
    .WillOnce(Return(false));
    EXPECT_EQ(CYNARA_API_SUCCESS,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(CLIENT_METHOD_PID, method);
}

TEST(parser, getMethodFromConfigurationFileFoundTheKeyAgain) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    FakeGetKeyAndValueBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("smack"),
                    Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("user_default"), SetArgReferee<3>("gid"),
                    Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("smack"),
                    Return(true)));

    EXPECT_EQ(CYNARA_API_CONFIGURATION_ERROR,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, getMethodFromConfigurationFileTheKeyNotFound) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    FakeGetKeyAndValueBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .WillOnce(DoAll(SetArgReferee<2>("something_default"), SetArgReferee<3>("smack"),
                    Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("user_default"), SetArgReferee<3>("gid"),
                    Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("other_default"), SetArgReferee<3>("smack"),
                    Return(true)))
    .WillOnce(Return(false));

    EXPECT_EQ(CYNARA_API_CONFIGURATION_ERROR,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}

TEST(parser, getMethodFromConfigurationFileFoundTheKeyWithWrongValue) {
    static const CredentialsMap clientCredsMap{{"smack", CLIENT_METHOD_SMACK},
                                               {"pid", CLIENT_METHOD_PID}};
    int method = NOT_A_METHOD_CODE;
    FakeGetKeyAndValueBackend fakeBackend;
    EXPECT_CALL(fakeBackend, getKeyAndValue(_, _, _, _))
    .Times(Between(2,4))
    .WillOnce(DoAll(SetArgReferee<2>("user_default"), SetArgReferee<3>("gid"), Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("client_default"), SetArgReferee<3>("noSmackNoPid"),
                    Return(true)))
    .WillOnce(DoAll(SetArgReferee<2>("other_default"), SetArgReferee<3>("smack"), Return(true)))
    .WillOnce(Return(false));

    EXPECT_EQ(CYNARA_API_CONFIGURATION_ERROR,
              fakeBackend.getMethodFromConfigurationFile(clientCredsMap, "client_default",
                                                         method));
    EXPECT_EQ(NOT_A_METHOD_CODE, method);
}
/*** Test for cynara_creds_get_default_[user|client]_method ***/


class CredsGetDefaultClientUserMethod : public testing::Test {
protected:
    virtual void SetUp() {
        configFilePath = CredsCommonsInnerBackend::credsConfigurationFile();
        std::ifstream configFileStream(configFilePath);
        std::stringstream ss;
        ss << configFileStream.rdbuf();
        configFileContent = ss.str();
        configFileStream.close();
    }

    void TearDown() {
        WriteFile(configFileContent);
    }
    virtual void WriteFile(std::string content) {
        std::ofstream configFileStream;
        configFileStream.open(configFilePath, std::ios::trunc);
        configFileStream << content;
        configFileStream.close();
    }
private:
    std::string configFilePath;
    std::string configFileContent;
};

TEST_F(CredsGetDefaultClientUserMethod, SmackIsDefaultClientMethod) {
    enum cynara_client_creds gotMethod;
    EXPECT_EQ(CYNARA_API_SUCCESS, cynara_creds_get_default_client_method(&gotMethod));
    EXPECT_EQ(CLIENT_METHOD_SMACK, gotMethod);
}

TEST_F(CredsGetDefaultClientUserMethod, UIDIsDefaultUserMethod) {
    enum cynara_user_creds gotMethod;
    EXPECT_EQ(CYNARA_API_SUCCESS, cynara_creds_get_default_user_method(&gotMethod));
    EXPECT_EQ(USER_METHOD_UID, gotMethod);
}

struct getDefaultClientUserMethodTestEntry {
    std::string configFileContent;
    int gotUserDefaultReturn;
    int gotClientDefaultReturn;
    enum cynara_user_creds gotUserMethod;
    enum cynara_client_creds gotClientMethod;
};

TEST_F(CredsGetDefaultClientUserMethod, DifferentContentOfConfigurationFiles) {

    static std::vector<struct getDefaultClientUserMethodTestEntry> getDefaultClientUserMethodTestEntries;
    //test cases initialization key and value are checked only if expectedResult is true
    //case #1
    getDefaultClientUserMethodTestEntries.push_back({"client_default=smack \nuser_default=uid",
                                                      CYNARA_API_SUCCESS, CYNARA_API_SUCCESS,
                                                      USER_METHOD_UID, CLIENT_METHOD_SMACK});
    getDefaultClientUserMethodTestEntries.push_back({"       client_default = smack \nuser_default"
                                                     "=       gid" , CYNARA_API_SUCCESS,
                                                              CYNARA_API_SUCCESS, USER_METHOD_GID,
                                                              CLIENT_METHOD_SMACK});
    getDefaultClientUserMethodTestEntries.push_back({"        client_default = pid #dupa \n\n"
                                                     "user_default   =       uid#ooto jest to",
                                                     CYNARA_API_SUCCESS, CYNARA_API_SUCCESS,
                                                     USER_METHOD_UID, CLIENT_METHOD_PID});
    getDefaultClientUserMethodTestEntries.push_back({" \n\nuser_default   =       uid\n   "
                                                     "client_default = smack #dup()^^^^ #ooto jest "
                                                     "to", CYNARA_API_SUCCESS, CYNARA_API_SUCCESS,
                                                      USER_METHOD_UID, CLIENT_METHOD_SMACK});
    //case #5
    getDefaultClientUserMethodTestEntries.push_back({" # #       client_defalt = gid #dpua \n\n"
                                                     "user_default   =       uid#ooto jest to\n"
                                                     "client_default = smack\n client_default=gid",
                                                     CYNARA_API_SUCCESS,
                                                     CYNARA_API_CONFIGURATION_ERROR,
                                                     USER_METHOD_UID,
                                                     CLIENT_METHOD_SMACK});
    getDefaultClientUserMethodTestEntries.push_back({" client_defalt = pid #dupa \n\nuser_default"
                                                     "=       uid\nuser_default   =       uid#ooto"
                                                     "jest to\n  client_default = smack \n"
                                                     "client_defalt = gid",
                                                     CYNARA_API_CONFIGURATION_ERROR,
                                                     CYNARA_API_SUCCESS, USER_METHOD_UID,
                                                     CLIENT_METHOD_SMACK});
    getDefaultClientUserMethodTestEntries.push_back({" # #       client_default = gid #dpau"
                                                     "\n\nuser_default   =       uid"
                                                     "#ooto jest to\n  client_default = smack"
                                                     "\n client_default = gid", CYNARA_API_SUCCESS,
                                                     CYNARA_API_CONFIGURATION_ERROR,
                                                     USER_METHOD_UID, CLIENT_METHOD_SMACK});
    getDefaultClientUserMethodTestEntries.push_back({" # #       client_default = gid #dpuo"
                                                     "\n\nuser_default   =       uid",
                                                     CYNARA_API_SUCCESS,
                                                     CYNARA_API_CONFIGURATION_ERROR,
                                                     USER_METHOD_UID, CLIENT_METHOD_SMACK});

    int i=0;
    for (auto it = getDefaultClientUserMethodTestEntries.begin();
              it != getDefaultClientUserMethodTestEntries.end(); ++it) {
        i++;
        WriteFile(it->configFileContent);
        enum cynara_client_creds gotClientMethod = CLIENT_METHOD_PID;
        enum cynara_user_creds gotUserMethod = USER_METHOD_GID;
        EXPECT_EQ(it->gotClientDefaultReturn,
                  cynara_creds_get_default_client_method(&gotClientMethod))
        << "A in case no# " << i <<std::endl;
        if ((it->gotClientDefaultReturn) == CYNARA_API_SUCCESS)
            EXPECT_EQ(it->gotClientMethod, gotClientMethod)
            << "B in case no# " << i <<std::endl;
        else
            EXPECT_EQ(CLIENT_METHOD_PID, gotClientMethod);
        EXPECT_EQ(it->gotUserDefaultReturn, cynara_creds_get_default_user_method(&gotUserMethod))
        << "C in case no# " << i <<std::endl;
        if ((it->gotUserDefaultReturn) == CYNARA_API_SUCCESS)
            EXPECT_EQ(it->gotUserMethod, gotUserMethod)
            << "D in case no# " << i <<std::endl;
        else
            EXPECT_EQ(USER_METHOD_GID, gotUserMethod)
            << "E in case no# " << i <<std::endl;
    }
}

