#! /usr/bin/python

import _cynara

class CynaraAdminPolicy(object):
    def __init__(self, bucket, client, user, privilege, result, result_extra):
        self.bucket = bucket
        self.client = client
        self.user   = user
        self.privilege = privilege
        self.result = result
        self.result_extra = result_extra

# TODO: destructor etc.
class CynaraAdmin(object):
    def __init__(self):
        ret, p_cynara = _cynara.cynara_admin_initialize()
        if ret != 0:
            raise IOError('Failed to connect to cynara daemon')
        self.p_cynara = p_cynara

    def finish(self):
        return _cynara.cynara_admin_finish(self.p_cynara)

    def set_policies(self, policies):
        return _cynara.cynara_admin_set_policies(self.p_cynara, policies)

    def set_policy(self, bucket, client, user, privilege, result, result_extra):
        return self.set_policies([ CynaraAdminPolicy(bucket, client, user, privilege, result, result_extra) ])


