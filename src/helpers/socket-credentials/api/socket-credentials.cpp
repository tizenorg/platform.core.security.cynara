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

/*#include <map>
#include <new>
#include <string>
#include <vector>
*/
//#include <common.h>
#include <log/log.h>

#include <socket-credentials-helper.h>

//#include <api/ApiInterface.h>
//#include <logic/Logic.h>

#include <stdio.h>
//#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
//     #include <smack/smack.h>

#define PID_METHOD 1
#define UID_METHOD 2
#define GID_METHOD 3
#define SMACK_METHOD 4

#define SMACK_LABEL_LEN 1234 //todo remove

typedef struct {
       pid_t pid;
       uid_t uid;
       gid_t gid;
} PeerCredentials;

typedef char SmackLabel[SMACK_LABEL_LEN]; /* SMACK_LABEL_LEN is defined in <sys/smack.h> */


//CYNARA_API
int get_user_id_from_socket(const int connection, const char *method, void *user_id, unsigned int *length)
{
  FILE *f;
  char buffer[SYSTEM_DEFAULT_MAX_SIZE];
  (void) connection;
  (void) method;
  (void) user_id;
  int pos; /*position in a buffer*/
  int c;
  socklen_t cred_len;
  int needed_size;
  int user_id_code;
  //SmackLabel smack_label;
 // socklen_t smack_label_len = sizeof(SmackLabel);
  PeerCredentials peer_cred;
  
  memset(buffer, '\0', SYSTEM_DEFAULT_MAX_SIZE);
  if (!strcmp(method,"SYSTEM_DEFAULT")) 
  {
    f = fopen(SOCKET_SYSTEM_DEFAULT_FILE, "r"); 
    if (!f) 
      return SOCKET_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE;
    else 
    {  /*processing the configuration file*/
       pos = 0;
       while(pos < SYSTEM_DEFAULT_MAX_SIZE)
       {
         c = fgetc(f);
         if (c == EOF) 
           buffer[pos++] = '\0'; 
         else 
           buffer[pos++] = (char) c;           
       }
       fclose(f);
       if (c != EOF) 
         return SOCKET_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE;
       else
       {/*checking whether method is known*/
         if  ((!strcmp(buffer, "PID")) &&
              (!strcmp(buffer, "UID")) &&
              (!strcmp(buffer, "GID")) &&
              (!strcmp(buffer, "SMACK")))
           return SOCKET_CREDENTIALS_API_ERROR_IN_CONFIGURATION_FILE;
       }  
     }
   }
   
   if (!strcmp(method, "PID") || !strcmp(method, "PID"))
   {
     user_id_code = PID_METHOD;
     needed_size = sizeof(pid_t);
   } 
   else if (!strcmp(method, "UID") || !strcmp(method, "UID"))
   {
     user_id_code = UID_METHOD;
     needed_size = sizeof(uid_t);
   }
   else if (!strcmp(method, "GID") || !strcmp(method, "GID"))
   {
     user_id_code = GID_METHOD;
     needed_size = sizeof(gid_t);
   }
   else if (!strcmp(method, "SMACK") || !strcmp(method, "SMACK"))
   {
     user_id_code = SMACK_METHOD;
     needed_size = SMACK_LABEL_LEN;
   }
   else /*wrong value*/
   {
     return SOCKET_CREDENTIALS_API_INVALID_PARAM;
   }

  if (*length == 0) {
    *length = needed_size;
    return SOCKET_CREDENTIALS_API_SUCCESS;
  };/* else if (*length < needed_size)
    return SOCKET_CREDENTIALS_API_LENGTH_TOO_SMALL;*/
  if (user_id_code == SMACK_METHOD)
  {
     if (!getsockopt(connection, SOL_SOCKET, SO_PEERSEC, user_id, length))
      return SOCKET_CREDENTIALS_API_GETSOCKOPT_ERROR;
     else   
      return SOCKET_CREDENTIALS_API_SUCCESS;
  } else 
  {
     cred_len = sizeof(PeerCredentials);
     if (!getsockopt(connection, SOL_SOCKET, SO_PEERCRED, &peer_cred, &cred_len))
      return SOCKET_CREDENTIALS_API_GETSOCKOPT_ERROR;  
  }
  switch (user_id_code) {
   case PID_METHOD:
     memcpy((void*) user_id, (void*) &(peer_cred.pid), sizeof(pid_t));
     return SOCKET_CREDENTIALS_API_SUCCESS;
     break;
   case UID_METHOD:
     memcpy((void*) user_id, (void*) &(peer_cred.uid), sizeof(uid_t));
     return SOCKET_CREDENTIALS_API_SUCCESS;
     break;
   case GID_METHOD:
     memcpy((void*) user_id, (void*) &(peer_cred.gid), sizeof(gid_t));
     return SOCKET_CREDENTIALS_API_SUCCESS;
     break;
   default: 
    return SOCKET_CREDENTIALS_API_UNEXPECTED_ERROR;
  
  } 
  return SOCKET_CREDENTIALS_API_UNEXPECTED_ERROR;
}

//CYNARA_API
int get_client_id_from_socket(const int connection, const char *method, void *client_id, unsigned int *length)
{
  (void) connection;
  (void) method;
  (void) client_id;
  (void) length;
  return 0;
}



