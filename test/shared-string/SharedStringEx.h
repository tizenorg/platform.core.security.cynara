#include <containers/SharedString.h>

namespace Cynara {

class StringStorageEx : public StringStorage {
public:
    static StringStorageEx& getInstance();
    int getRefCount(const std::string &key) {
        auto it = m_sharedStringMap.find(key);
        if (it == m_sharedStringMap.end())
            return -1;

        return it->second->refCount;
    }
};

} // namespace Cynara


