void change_status(int old_fd, int new_fd, cynara_async_status status, void *user_status_data)
{
    // unregister using old_fd
    (...)
    //register using new_fd and new status
}

int process_response(int check_id, cynara_async_call_cause cause, int response, void *user_response_data)
{
    switch (cause) {
    case cynara_async_call_cause::CYNARA_CALL_CAUSE_ANSWER:
        // handle answer from cynara service - use response value
        break;

    case cynara_async_call_cause::CYNARA_CALL_CAUSE_CANCEL:
        // handle canceled request
        break;

    case cynara_async_call_cause::CYNARA_CALL_CAUSE_FINISH:
        // handle finish of client async
        break;

    case cynara_async_call_cause::CYNARA_CALL_CAUSE_SERVICE_NOT_AVAILABLE:
        // handle disconnection
    }

    (...)

    free_user_response_data(user_response_data);

}

void main_process
{
    //initialize all required objects
    cynara_async *p_cynara;
    int ret;

    //change_status will be called passing file descriptor connected to cynara server
    if ((ret = cynara_async_initialize(&p_cynara, NULL, change_status, &fd_sets)) != CYNARA_API_SUCCESS) {
        // handle error
    }

    //start event loop
    event_process(fd_sets, ...)

    //event loop stopped, clean-up, before closing program
    cynara_async_finish(p_cynara);

}

void event_process(fd_sets, ...)
{
    //event loop of user choice
    select(..., fd_sets.read, fd_sets.write) {
        (...)
        if(active_socket == cynara_socket) {
            if(cynara_async_process(p_cynara) != CYNARA_API_SUCCESS) {
                // handle error
            }
        } else if (active_socket == some_client_socket) {
            //processing clients, which may require cynara checks
            (...)
            int ret = cynara_async_check_cache(p_cynara, client, client_session, user, privilege);
            switch(ret) {
            case CYNARA_API_ACCESS_ALLOWED:
                // allow client
                break;

            case CYNARA_API_ACCESS_DENIED:
                // deny client
                break;

            case CYNARA_API_CACHE_MISS:
                // not in cache - prepare data that will be passed to callback
                allocate_user_response_data(&user_response_data);

                // create request
                ret = cynara_async_create_request(p_cynara, client, client_session, user, privilege, &check_id, process_response, user_response_data);
                if(ret != CYNARA_API_SUCCESS) {
                    free_user_response_data(user_response_data);
                    // handle error
                }
                break;

            default:
                // handle error
            }

            (...)

            // waiting for some request too long
            if (ret = cynara_async_cancel_request(p_cynara, check_id)) != CYNARA_API_SUCCESS) {
                // handle error
            }

        }
        (...)

    }
    (...)
}
