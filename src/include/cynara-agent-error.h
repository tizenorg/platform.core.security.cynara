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
 * @file        src/include/cynara-agent-error.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @version     1.0
 * @brief       This file contains error codes returned by agent APIs of Cynara.
 */

#ifndef CYNARA_AGENT_ERROR_H
#define CYNARA_AGENT_ERROR_H

/**
 * \name Return Codes
 * exported by the foundation API.
 * result codes begin with the start error code and extend into negative direction.
 * @{
*/

/*! \brief   function has finished successfully */
#define CYNARA_AGENT_API_SUCCESS 0

/*! \brief   not enough free memory for needed data */
#define CYNARA_AGENT_API_OUT_OF_MEMORY -1

/*! \brief   at least one function arguments is not correct */
#define CYNARA_AGENT_API_INVALID_PARAM -2

/*! \brief   cynara service is not available */
#define CYNARA_AGENT_API_SERVICE_NOT_AVAILABLE -3

/*! \brief   function has failed because of insufficient access rights */
#define CYNARA_AGENT_API_ACCESS_DENIED -4

/*! \brief   function has failed because of unknown error */
#define CYNARA_AGENT_API_UNKNOWN_ERROR -5
/** @}*/

#endif // CYNARA_AGENT_ERROR_H
