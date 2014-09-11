#! /usr/bin/python

import _cynara

# TODO: destructor etc.
class CynaraClient(object):
    def __init__(self):
        ret, p_cynara = _cynara.cynara_initialize(None)
        if ret != 0:
            raise IOError('Failed to connect to cynara daemon')
        self.p_cynara = p_cynara

    def check(self, client, session, user, privilege):
        return _cynara.cynara_check(self.p_cynara, client, session, user, privilege)

    def finish(self):
        return _cynara.cynara_finish(self.p_cynara)

