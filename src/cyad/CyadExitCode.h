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
 * @file        src/cyad/CyadExitCode.h
 * @author      Aleksander Zdyb <a.zdyb@samsung.com>
 * @version     1.0
 * @brief       Exit codes for Cyad
 */

#ifndef SRC_CYAD_CYADEXITCODE_H_
#define SRC_CYAD_CYADEXITCODE_H_

namespace Cynara {

enum CyadExitCode {
    Success = 0,
    UnknownError = 128, // Previous exit codes are reserved for cynara-admin
    NoMemory,
    InvalidCommandline,
    InvalidInput
};

}  // namespace Cynara



#endif /* SRC_CYAD_CYADEXITCODE_H_ */
