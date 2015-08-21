(...)

//create client session
char *client_session;
client_session = cynara_session_from_pid(client_pid);

if (!client_session) {
    //use another way to create session or abandon request sending
}

//check access (details of this function can be found in cynara-client.h)
int ret = cynara_check(p_cynara, client, client_session, user, privilege);

//release client_session memory
free(client_session);

//handle check answer
if (ret < 0) {
    //handle error
} else {
    //handle response
}

(...)

