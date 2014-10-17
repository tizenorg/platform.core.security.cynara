#include <cynara-plugin.h>

#include "SupportedTypes.h"



using namespace Cynara;

const std::vector<PolicyType> serviceTypes = {SupportedTypes::Service::ASK_USER};

class ServiceDummyPlugin : public ServicePluginInterface {
public:
    const std::vector<PolicyType> &getSupportedPolicyTypes() {
        return serviceTypes;
    }
    PluginStatus check(const std::string &client, const std::string &user,
                       const std::string &privilege, PolicyResult &result,
                       AgentType &requiredAgent, PluginData &pluginData) noexcept {
        (void) client;
        (void) privilege;
        (void) requiredAgent;
        (void) pluginData;
        if (user == "admin")
            result = PolicyResult(SupportedTypes::Client::ALLOW_PER_LIFE);

        if (user == "client")
            result = PolicyResult(SupportedTypes::Client::ALLOW_PER_SESSION);

        if (user == "amanda")
            result = PolicyResult(SupportedTypes::Client::ALLOW_ONCE);

        if (user == "paszczak")
            return PluginStatus::ERROR;

        return PluginStatus::ANSWER_READY;
    }

    PluginStatus update(const std::string &client, const std::string &user,
                        const std::string &privilege, const PluginData &agentData,
                        PolicyResult &result) noexcept {
        (void) user;
        (void) client;
        (void) privilege;
        (void) agentData;
        (void) result;
        return PluginStatus::ANSWER_READY;
    }

};

extern "C" {
ExternalPluginInterface *create(void) {
    return new ServiceDummyPlugin();
}

void destroy(ExternalPluginInterface *ptr) {
    delete ptr;
}

}
