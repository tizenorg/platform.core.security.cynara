/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/*
 * @file        cynara-helper-credentials.h
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains common APIs for Cynara credentials helper.
 */


#ifndef CYNARA_HELPER_CREDENTIAL_H
#define CYNARA_HELPER_CREDENTIAL_H

enum cynara_client_creds {
    CLIENT_METHOD_SMACK,
    CLIENT_METHOD_PID,

    CLIENT_METHOD_UNKNOWN
};

enum cynara_user_creds {
    USER_METHOD_UID,
    USER_METHOD_GID,

    USER_METHOD_UNKNOWN
};

#ifdef __cplusplus
extern "C" {
#endif

enum cynara_client_creds cynara_helper_credential_get_default_client_method(void);

enum cynara_user_creds cynara_helper_credential_get_default_user_method(void);

#ifdef __cplusplus
}
#endif

#endif /* CYNARA_HELPER_CREDENTIAL_H */
