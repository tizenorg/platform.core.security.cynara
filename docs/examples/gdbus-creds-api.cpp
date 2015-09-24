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

int get_credentials(GDBusConnection *connection, Creds *creds)
{
  int ret = 0;
  int pid = 0;

  if(!creds)
    return INVALID_PARAMETER;

  /* Do not free this string, it is owned by connection */
  const qchar *unique_name = g_dbus_connection_get_unique_name(connection);

  ret = cynara_creds_gdbus_get_user(connection, unique_name, USER_METHOD_DEFAULT, &(creds->user));
  if(ret != CYNARA_API_SUCCESS) {
    LOGE("cynara_creds_gdbus_get_user() failed");
    return INVALID_OPERATION;
  }

  ret = cynara_creds_gdbus_get_client(connection, unique_name, CLIENT_METHOD_DEFAULT, &(creds->client));
  if(ret != CYNARA_API_SUCCESS) {
    LOGE("cynara_creds_gdbus_get_client() failed");
    return INVALID_OPERATION;
  }

  ret = cynara_creds_gdbus_get_pid(connection, unique_name, &pid);
  if(ret != CYNARA_API_SUCCESS) {
    LOGE("cynara_creds_gdbus_get_pid() failed");
    return INVALID_OPERATION;
  }

  creds->client_session = cynara_session_from_pid(pid);
  return SUCCESS;
}
