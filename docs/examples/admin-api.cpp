(...)

int ret;
cynara_admin *p_cynara_admin = NULL;

(...)

ret = cynara_admin_initialize(&p_cynara_admin);
if (ret != CYNARA_ADMIN_API_SUCCESS) { /* error */ }

(...)

ret = cynara_admin_set_bucket(p_cynara_admin, "bucket1", CYNARA_ADMIN_ALLOW);
if (ret != CYNARA_ADMIN_API_SUCCESS) {
    cynara_admin_finish(p_cynara_admin);
    /* error */
}

ret = cynara_admin_set_bucket(p_cynara_admin, "bucket2", CYNARA_ADMIN_DENY);
if (ret != CYNARA_ADMIN_API_SUCCESS) {
    cynara_admin_finish(p_cynara_admin);
    /* error */
}

(...)

cynara_admin_policy *policies1[6];
policy11 = { CYNARA_ADMIN_DEFAULT_BUCKET, CYNARA_ADMIN_WILDCARD, "user1", CYNARA_ADMIN_WILDCARD, CYNARA_ADMIN_BUCKET, "bucket1" };
policy12 = { "bucket1", "client1", "user1", "privilege1", CYNARA_ADMIN_DENY, NULL };
policy13 = { "bucket2", "client2", "user1", "privilege2", CYNARA_ADMIN_ALLOW, NULL };
policy14 = { "bucket1", CYNARA_ADMIN_WILDCARD, CYNARA_ADMIN_WILDCARD, "privilege2", CYNARA_ADMIN_BUCKET, "bucket2" };
policy15 = { CYNARA_ADMIN_DEFAULT_BUCKET, "client3", CYNARA_ADMIN_WILDCARD, CYNARA_ADMIN_WILDCARD, CYNARA_ADMIN_ALLOW, NULL };
policies[6] = {    
    &policy11,
    &policy12,
    &policy13,
    &policy14,
    &policy15,
    NULL
}

ret = cynara_admin_set_policies(p_cynara_admin, &policies1);
if (ret != CYNARA_ADMIN_API_SUCCESS) {    
    cynara_admin_finish(p_cynara_admin);
    /* error */
}

(...)

ret = cynara_admin_set_bucket(p_cynara_admin, "bucket2", CYNARA_ADMIN_DELETE);
if (ret != CYNARA_ADMIN_API_SUCCESS) {    
    cynara_admin_finish(p_cynara_admin);
    /* error */
}

(...)

cynara_admin_policy *policies2[3];
policy21 = { "bucket1", CYNARA_ADMIN_WILDCARD, CYNARA_ADMIN_WILDCARD, "privilege2", CYNARA_ADMIN_DELETE, NULL };
policy22 = { CYNARA_ADMIN_DEFAULT_BUCKET, "client3", CYNARA_ADMIN_WILDCARD, CYNARA_ADMIN_WILDCARD, CYNARA_ADMIN_DELETE, NULL };
policies2[3] = {    
    &policy21,
    &policy22,
    NULL
}

ret = cynara_admin_set_policies(p_cynara_admin, &policies2);
if (ret != CYNARA_ADMIN_API_SUCCESS) {    
    cynara_admin_finish(p_cynara_admin);
    /* error */
}

(...)

cynara_admin_finish(p_cynara_admin);

(...)
