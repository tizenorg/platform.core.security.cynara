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
 * @file        src/common/error/api.cpp
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Implementation of cynara_strerror
 */

#include <cstring>
#include <string>

#include <cynara-error.h>

int cynara_strerror(int errnum, char *buf, size_t buflen) {
    if (buf == nullptr)
        return CYNARA_API_INVALID_PARAM;

    const char *message = nullptr;

    switch (errnum) {
        case CYNARA_API_ACCESS_ALLOWED:
            message = "Access that was checked is allowed";
            break;
        case CYNARA_API_ACCESS_DENIED:
            message = "Access that was checked is denied";
            break;
        case CYNARA_API_SUCCESS:
            message = "Result of the one specific API is successful";
            break;
        case CYNARA_API_CACHE_MISS:
            message = "Value is not present in cache";
            break;
        case CYNARA_API_MAX_PENDING_REQUESTS:
            message = "Pending requests reached maximum";
            break;
        case CYNARA_API_OUT_OF_MEMORY:
            message = "System is running out of memory state";
            break;
        case CYNARA_API_INVALID_PARAM:
            message = "API's parameter is malformed";
            break;
        case CYNARA_API_SERVICE_NOT_AVAILABLE:
            message = "Service is not available";
            break;
        case CYNARA_API_METHOD_NOT_SUPPORTED:
            message = "Provided method is not supported by library";
            break;
        case CYNARA_API_OPERATION_NOT_ALLOWED:
            message = "Service does not allow to perform requested operation";
            break;
        case CYNARA_API_OPERATION_FAILED:
            message = "Service failed to perform requested operation";
            break;
        case CYNARA_API_BUCKET_NOT_FOUND:
            message = "Service hasn't found requested bucket";
            break;
        case CYNARA_API_UNKNOWN_ERROR:
            message = "Unknown error";
            break;
        case CYNARA_API_CONFIGURATION_ERROR:
            message = "Configuration error";
            break;
        case CYNARA_API_INVALID_COMMANDLINE_PARAM:
            message = "Invalid parameter in command-line";
            break;
        case CYNARA_API_BUFFER_TOO_SHORT:
            message = "Buffer is to short";
            break;
    }

    if (message == nullptr)
        return CYNARA_API_INVALID_PARAM;

    if (buflen < strlen(message) + 1)
        return CYNARA_API_BUFFER_TOO_SHORT;

    strcpy(buf, message);

    return CYNARA_API_SUCCESS;
}
