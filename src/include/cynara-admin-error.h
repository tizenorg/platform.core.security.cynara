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
/**
 * @file        cynara-admin-error.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       This file contains error codes of administration APIs of Cynara.
 */

#ifndef CYNARA_ADMIN_ERROR_H
#define CYNARA_ADMIN_ERROR_H

/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes begin with the start error code and extend into negative direction.
 * @{
*/

/*! \brief   indicating the result of the one specific API is successful or access is allowed */
#define CYNARA_ADMIN_API_SUCCESS 0

/*! \brief   indicating client process is running out of memory */
#define CYNARA_ADMIN_API_OUT_OF_MEMORY -1

/*! \brief   indicating the API's parameter is malformed */
#define CYNARA_ADMIN_API_INVALID_PARAM -2

/*! \brief   service not available (cannot connect to cynara service) */
#define CYNARA_ADMIN_API_SERVICE_NOT_AVAILABLE -3

/*! \brief   unexpected error in client library */
#define CYNARA_ADMIN_API_UNEXPECTED_CLIENT_ERROR -4

/*! \brief   cynara service does not allow to perform requested operation */
#define CYNARA_ADMIN_API_OPERATION_NOT_ALLOWED -5

/*! \brief   cynara service hasn't found requested bucket */
#define CYNARA_ADMIN_API_BUCKET_NOT_FOUND -6
/** @}*/

/**
 * \name Wildcard
 * definition of WILDCARD, that can replace client, user or privilege name.
 * WILDCARD matches any string during check procedure from libcynara-client.
 */
#define CYNARA_ADMIN_WILDCARD "*"

/**
 * \name Name of Default Bucket
 * definition of name for default bucket - the one that check starts in.
 * default bucket cannot be removed, although its default policy
 * (which originally is set to DENY) can be changed.
 */
#define CYNARA_ADMIN_DEFAULT_BUCKET ""

/**
 * \name Operation Codes
 * operation codes that define action type to be taken in below defined functions
 * they are used mostly to define policy result
 * @{
 */

/*! \brief   a policy or bucket should be removed */
#define CYNARA_ADMIN_DELETE -1

/*! \brief   set policy result or bucket's default policy to DENY */
#define CYNARA_ADMIN_DENY 0

/*! \brief   set bucket's default policy to NONE */
#define CYNARA_ADMIN_NONE 1

/*! \brief   set policy result or bucket's default policy to ALLOW */
#define CYNARA_ADMIN_ALLOW 2

/*! \brief   set policy to point into another bucket */
#define CYNARA_ADMIN_BUCKET 3
/** @}*/

#endif // CYNARA_ADMIN_ERROR_H
