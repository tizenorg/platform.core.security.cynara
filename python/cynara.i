/* File : example.i */

%module cynara
%include "typemaps.i"

%{
#define GET_ATTRIBUTE_STRING(object, attr_name, out) \
    do {\
        PyObject *attr_object = PyObject_GetAttrString(object, attr_name);\
        if (attr_object == NULL)\
        {\
            PyErr_SetString(PyExc_TypeError,"policy object: " attr_name " attribute missing");\
            goto finish;\
        }\
        *out = PyString_AsString(attr_object);\
        Py_DECREF(attr_object);\
        if (*out == NULL)\
        {\
            PyErr_SetString(PyExc_TypeError,"policy object: " attr_name " attribute is not a string");\
            goto finish;\
        }\
    } while(0)

#define GET_ATTRIBUTE_INT(object, attr_name, out) \
    do {\
        PyObject *attr_object = PyObject_GetAttrString(object, attr_name);\
        if (attr_object == NULL)\
        {\
            PyErr_SetString(PyExc_TypeError,"policy object: " attr_name " attribute missing");\
            goto finish;\
        }\
        *out = (int)PyInt_AsLong(attr_object);\
        Py_DECREF(attr_object);\
        if (*out == -1 && PyErr_Occurred() != NULL)\
        {\
            PyErr_SetString(PyExc_TypeError,"policy object: error when casting " attr_name " attribute to long");\
            goto finish;\
        }\
    } while(0)
%}




%typemap(in) const struct cynara_admin_policy *const * {
    int size;
    int i;
    int success=0;
    struct cynara_admin_policy **policies;  

    if (!PyList_Check($input)) {
        PyErr_SetString(PyExc_TypeError,"not a list");
        return NULL;
    }

    size = PyList_Size($input);

    policies = (struct cynara_admin_policy**) malloc((size+1)*sizeof(struct cynara_admin_policy*));
    if (policies == NULL)
        return NULL;
 
    for (i = 0; i < size; i++) {
        policies[i] = calloc(1, sizeof(struct cynara_admin_policy));
        if (policies[i] == NULL)
        {
            PyErr_SetString(PyExc_TypeError,"out of memory");
            goto finish;
        }
        PyObject *o = PyList_GetItem($input,i);
        GET_ATTRIBUTE_STRING(o, "bucket", &policies[i]->bucket);
        GET_ATTRIBUTE_STRING(o, "client", &policies[i]->client);
        GET_ATTRIBUTE_STRING(o, "user", &policies[i]->user);
        GET_ATTRIBUTE_STRING(o, "privilege", &policies[i]->privilege);
        GET_ATTRIBUTE_STRING(o, "result_extra", &policies[i]->result_extra);
        GET_ATTRIBUTE_INT(o, "result", &policies[i]->result);
    }

    success = 1;
finish:
    if (success)
    {
        policies[size] = NULL;
        $1 = policies;
    }
    else
    {       
        while (i-- > 0)
            free(policies[i]);
        free(policies);
        return NULL;
    }

}

%typemap(python,freearg)  const struct cynara_admin_policy *const * {
    struct cynara_admin_policy **policies = $source;
    if (policies != NULL)
    {
        struct cynara_admin_policy **policy = policies;
        while (*policy != NULL)
        {
            free(*policy);
            policy++;
        }
        free(policies);
    }
}


%{
#include <cynara-client.h>
#include <cynara-admin.h>
/* Put headers and other declarations here */
extern int cynara_initialize(cynara **, const cynara_configuration *);
extern int cynara_finish(cynara *p_cynara);
extern int cynara_check(cynara *p_cynara, const char *client, const char *client_session, const char *user, const char *privilege);


extern int cynara_admin_initialize(struct cynara_admin **pp_cynara_admin);
extern int cynara_admin_finish(struct cynara_admin *p_cynara_admin);
extern int cynara_admin_set_policies(struct cynara_admin *p_cynara_admin, const struct cynara_admin_policy *const *policies);
extern int cynara_admin_set_bucket(struct cynara_admin *p_cynara_admin, const char *bucket, int operation, const char *extra);

%}

/* param mismatch. TODO: find a better way */
extern int cynara_initialize(long *OUTPUT, const cynara_configuration *p_conf);
extern int cynara_finish(long p_cynara);
extern int cynara_check(long p_cynara, const char *client, const char *client_session, const char *user, const char *privilege);


extern int cynara_admin_initialize(long *OUTPUT);
extern int cynara_admin_finish(long p_cynara_admin);
extern int cynara_admin_set_policies(long p_cynara_admin, const struct cynara_admin_policy *const *policies);
extern int cynara_admin_set_bucket(long p_cynara_admin, const char *bucket, int operation, const char *extra);


