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

int get_credentials(DBusConnection *connection, Creds *creds)
{
  int ret = 0;
  int pid = 0;

  if(!creds)
    return INVALID_PARAMETER;

  char * unique_name = dbus_bus_get_id(connection, &ret);
  if(!unique_name) {
    LOGE("dbus_bus_get_id() failed");
    return INVALID_OPERATION;
  }

  ret = cynara_creds_dbus_get_user(connection, unique_name, USER_METHOD_DEFAULT, &(creds->user));
  if(ret != CYNARA_API_SUCCESS) {
    LOGE("cynara_creds_dbus_get_user() failed");
    free(unique_name);
    return INVALID_OPERATION;
  }

  ret = cynara_creds_dbus_get_client(connection, unique_name, CLIENT_METHOD_DEFAULT, &(creds->client));
  if(ret != CYNARA_API_SUCCESS) {
    LOGE("cynara_creds_dbus_get_client() failed");
    free(unique_name);
    return INVALID_OPERATION;
  }

  ret = cynara_creds_dbus_get_pid(connection, unique_name, &pid);
  if(ret != CYNARA_API_SUCCESS) {
    LOGE("cynara_creds_dbus_get_pid() failed");
    free(unique_name);
    return INVALID_OPERATION;
  }

  creds->client_session = cynara_session_from_pid(pid);
  free(unique_name);
  return SUCCESS;
}
