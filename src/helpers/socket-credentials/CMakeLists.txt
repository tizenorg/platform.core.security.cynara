# Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @file        CMakeLists.txt
# @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
# @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
#

SET(COMMON_DEPS
    libsystemd-daemon
    libsystemd-journal
    smack
    )

SET(LIB_HELPER_SOCKET_CREDENTIALS_VERSION_MAJOR 0)
SET(LIB_HELPER_SOCKET_CREDENTIALS_VERSION ${LIB_HELPER_SOCKET_CREDENTIALS_VERSION_MAJOR}.0.1)

SET(LIB_HELPER_SOCKET_CREDENTIALS_PATH ${CYNARA_PATH}/helpers/socket-credentials)

SET(LIB_HELPER_SOCKET_CREDENTIALS_SOURCES
    ${LIB_HELPER_SOCKET_CREDENTIALS_PATH}/socket-credentials.cpp
    )

INCLUDE_DIRECTORIES(
    ${LIB_HELPER_SOCKET_CREDENTIALS_PATH}
    )

ADD_LIBRARY(${TARGET_LIB_HELPER_SOCKET_CREDENTIALS} SHARED ${LIB_HELPER_SOCKET_CREDENTIALS_SOURCES})

SET_TARGET_PROPERTIES(
    ${TARGET_LIB_HELPER_SOCKET_CREDENTIALS}
    PROPERTIES
        COMPILE_FLAGS "-D_GNU_SOURCE -fPIC -fvisibility=hidden"
        SOVERSION ${LIB_HELPER_SOCKET_CREDENTIALS_VERSION_MAJOR}
        VERSION ${LIB_HELPER_SOCKET_CREDENTIALS_VERSION}
    )

TARGET_LINK_LIBRARIES(${TARGET_LIB_HELPER_SOCKET_CREDENTIALS}
    ${CYNARA_DEP_LIBRARIES}
    ${TARGET_CYNARA_COMMON}
    )

INSTALL(TARGETS ${TARGET_LIB_HELPER_SOCKET_CREDENTIALS} DESTINATION ${LIB_INSTALL_DIR})
