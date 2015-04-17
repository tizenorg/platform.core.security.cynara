/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
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
 * @file        src/common/log/log.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @version     1.0
 * @brief       This file defines logging utilities.
 */

#ifndef CYNARA_COMMON_LOG_H
#define CYNARA_COMMON_LOG_H

#define __LOG_NARG(...) \
         __LOG_NARG_(__VA_ARGS__, __LOG_RSEQ_N())

#define __LOG_NARG_(...) \
         __LOG_ARG_N(__VA_ARGS__)

#define __LOG_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,_64,N,...) N


#define __LOG_RSEQ_N() \
         __LOG_N, __LOG_N, __LOG_N, __LOG_N , \
         __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, \
         __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, \
         __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, \
         __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, \
         __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, \
         __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_N, __LOG_0

#ifndef CYNARA_NO_LOGS
#include <sstream>
#ifdef BUILD_WITH_SYSTEMD
#include <systemd/sd-journal.h>
#else // BUILD_WITH_SYSTEMD
#include <syslog.h>
#endif // BUILD_WITH_SYSTEMD
#endif // CYNARA_NO_LOGS

extern int __log_level;

#ifndef CYNARA_NO_LOGS
    #define __LOG(LEVEL, FORMAT, ...) \
        do { \
            if (LEVEL <= __log_level) { \
                std::stringstream __LOG_MACRO_format; \
                __LOG_MACRO_format << FORMAT; \
                __LOG_NARG(_0, ##__VA_ARGS__) (LEVEL, __LOG_MACRO_format.str().c_str(), ##__VA_ARGS__) \
            } \
        } while (0)

#ifdef BUILD_WITH_SYSTEMD
    #define __LOG_N(LEVEL, FORMAT, ...) sd_journal_print(LEVEL, FORMAT, ##__VA_ARGS__);
    #define __LOG_0(LEVEL, FORMAT, ...) sd_journal_print(LEVEL, "%s", FORMAT);
#else // BUILD_WITH_SYSTEMD
    #define __LOG_N(LEVEL, FORMAT, ...) syslog(LEVEL, FORMAT, ##__VA_ARGS__);
    #define __LOG_0(LEVEL, FORMAT, ...) syslog(LEVEL, "%s", FORMAT);
#endif // BUILD_WITH_SYSTEMD

#else // CYNARA_NO_LOGS
    #define __LOG(LEVEL, ...)
#endif

#define LOGM(...)  __LOG(LOG_EMERG, __VA_ARGS__)   /* system is unusable */
#define LOGA(...)  __LOG(LOG_ALERT, __VA_ARGS__)   /* action must be taken immediately */
#define LOGC(...)  __LOG(LOG_CRIT, __VA_ARGS__)    /* critical conditions */
#define LOGE(...)  __LOG(LOG_ERR, __VA_ARGS__)     /* error conditions */
#define LOGW(...)  __LOG(LOG_WARNING, __VA_ARGS__) /* warning conditions */
#define LOGN(...)  __LOG(LOG_NOTICE, __VA_ARGS__)  /* normal but significant condition */
#define LOGI(...)  __LOG(LOG_INFO, __VA_ARGS__)    /* informational */
#define LOGD(...)  __LOG(LOG_DEBUG, __VA_ARGS__)   /* debug-level messages */

void init_log(void);

#endif /* CYNARA_COMMON_LOG_H */
