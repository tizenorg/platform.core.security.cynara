struct Creds {
  char * client;
  char * user;
  char * client_session

  ~Creds() {
    free(client);
    free(user);
    free(client_session);
  }
}

int get_credentials(int fd, Creds *creds)
{
  int ret = 0;
  int pid = 0;

  if(!creds)
    return INVALID_PARAMETER;

  ret = cynara_creds_socket_get_user(fd, USER_METHOD_DEFAULT, &(creds->user));
  if(ret != CYNARA_API_SUCCESS) {
    LOGE("cynara_creds_socket_get_user() failed");
    return INVALID_OPERATION;
  }

  ret = cynara_creds_socket_get_client(fd, CLIENT_METHOD_DEFAULT, &(creds->client));
  if(ret != CYNARA_API_SUCCESS) {
    LOGE("cynara_creds_socket_get_client() failed");
    return INVALID_OPERATION;
  }

  ret = cynara_creds_socket_get_pid(fd, &pid);
  if(ret != CYNARA_API_SUCCESS) {
    LOGE("cynara_creds_socket_get_pid() failed");
    return INVALID_OPERATION;
  }

  creds->client_session = cynara_session_from_pid(pid);
  return SUCCESS;
}
