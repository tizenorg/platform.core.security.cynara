#include "SharedStringEx.h"

namespace Cynara {

StringStorageEx& StringStorageEx::getInstance() {
    // this is dirty hack but without it we will not be able
    // to extract refCount number from StringStorage
    return static_cast<StringStorageEx&>(StringStorage::getInstance());
}

} // namespace Cynara

