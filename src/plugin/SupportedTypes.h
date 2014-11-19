#pragma once

#include <types/PolicyType.h>

namespace SupportedTypes {

namespace Service {
//service
const Cynara::PolicyType ASK_USER = 10;
}

namespace Client {
//client
const Cynara::PolicyType ALLOW_ONCE = 11;
const Cynara::PolicyType ALLOW_PER_SESSION = 12;
const Cynara::PolicyType ALLOW_PER_LIFE = 13;
}
}
