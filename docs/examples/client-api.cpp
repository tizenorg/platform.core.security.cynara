(...)

int ret;
cynara *p_cynara = NULL;
cynara_configuration *p_conf;
size_t cache_size = 100U;

(...)

ret = cynara_configuration_create(&p_conf)
if (ret != CYNARA_API_SUCCESS) { /* error */ }

ret = cynara_configuration_set_cache_size(p_conf, cache_size);
if (ret != CYNARA_API_SUCCESS) { /* error */ }

ret = cynara_initialize(&p_cynara, p_conf);
if (ret != CYNARA_API_SUCCESS) { /* error */ }

cynara_configuration_destroy(p_conf);

(...)

ret = cynara_check(p_cynara, "client", "client_session", "user", "privilege");
switch(ret)
{
case CYNARA_API_ACCESS_ALLOWED:
        /* access allowed */
        break;

case CYNARA_API_ACCESS_DENIED:
        /* access denied */
        break;

default:
        /* error */

}

(...)

cynara_finish(p_cynara);

(...)
