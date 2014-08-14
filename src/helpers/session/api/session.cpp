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
 * @file        socket-credentials.cpp
 * @author      Aleksander Zdyb <a.zdyb@partner.samsung.com>
 * @author      Radoslaw Bartosiak <r.bartosiak@samsung.com>
 * @version     1.0
 * @brief       Implementation of external libhelper-dbus-credentials API
 */



#include <common.h>
#include <log/log.h>

#include <helper-session.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


CYNARA_API
int create_session_string_pid_based(const char *client, char *client_session, int *length)
{
  char path[PATH_MAX]; // buffor for path names 
  int  snp_len;    //lenght of path
  struct stat st;   //statistics for the proc entry
 
  
  if ((length == NULL) || (*length < 0))
    return HELPER_SESSION_API_INVALID_PARAM;
  
  if ((*length > 0) && (client == NULL))
    return HELPER_SESSION_API_INVALID_PARAM;
    
  if (*length > (PATH_MAX-7)) //size for "/proc/"
    return HELPER_SESSION_API_INVALID_PARAM;
    
  snp_len = snprintf(path, PATH_MAX, "/proc/%s", client);
  
  if ((snp_len < 0) || (snp_len > PATH_MAX-1))
    return HELPER_SESSION_API_UNEXPECTED_ERROR;
  
  if ((stat(path, &st) < 0)) {
    if (((errno == EACCES) || (errno == ENOENT)) || (errno == ENAMETOOLONG))
      return HELPER_SESSION_API_NO_ACCESS_WRONG_PATH;
    else 
      return HELPER_SESSION_API_UNEXPECTED_ERROR;  
  }

  if (*length == 0) {
    snp_len = snprintf(NULL, 0, "%s%ld", client, (long int) (st.st_ctime));
    if (snp_len < 0) 
      return HELPER_SESSION_API_UNEXPECTED_ERROR;
    else 
    {
      *length = snp_len+1;
      return HELPER_SESSION_API_SUCCESS;
    }  
     
  }
  else /* *length > 0*/
  {
   snp_len = snprintf(client_session, *length , "%s%ld", client, (long int) (st.st_ctime));
   if (snp_len < 0)
    return HELPER_SESSION_API_UNEXPECTED_ERROR;
   if (snp_len > (*length-1))
    return HELPER_SESSION_API_LENGTH_TOO_SMALL; 
   /*results are oK*/ 
   *length = snp_len;
   return HELPER_SESSION_API_SUCCESS;
  }
  /*we should not be here*/
  return HELPER_SESSION_API_UNEXPECTED_ERROR;
}


