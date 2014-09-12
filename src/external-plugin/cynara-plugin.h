#ifndef CYNARA_PLUGIN_H_
#define CYNARA_PLUGIN_H_

#include <memory>
#include <string>

#include <types/PolicyResult.h>
#include <types/PolicyType.h>

namespace Cynara {

class ExternalPlugin;

typedef std::shared_ptr<ExternalPlugin> ExternalPluginPtr;
typedef ExternalPluginPtr (*createPlugin)(void);

//These typedefs will be defined in external headers
typedef std::string PluginData;
typedef int AgentType;

/**
 * A class defining external plugins interface.
 * These plugins work inside of cynara and either can produce
 * response through check instantly or require communication
 * with given type of agent. Agent must be registered through
 * cynara-agent API.
 */

class ExternalPluginInterface {
public:
    /**
     * Enum indicating status of calling plugin method.
     */
    enum class PluginStatus {
        SUCCESS,                /** < update() finished successfully*/
        ANSWER_READY,           /** < check() returns answer immediately through argument*/
        ANSWER_NOTREADY,        /** < check() cannot return answer immediately,
                                    < communication with agent is required */
        ERROR                   /** < either check() or update() fails */
    };

    /**
     * Policy type supported by plugin.
     */
    virtual PolicyType getSupportedPolicyType() = 0;
    /**
     * Required agent type, when answer cannot be returned immediately,
     * and external communication is needed.
     */
    virtual AgentType getRequiredAgentType() = 0;

    /**
     * Asks plugin, what kind of permission does client, user and privilege has.
     *
     * @param[in] client
     * @param[in] user
     * @param[in] privilege
     * @param[out] result     Immediate response (if available)
     * @param[out] pluginData Additional data, that will be passed to agent
     * @return PluginStatus   In case of success - either ANSWER_READY or ANSWER_NOTREADY,
     *                        in case of error - ERROR
     */
    virtual PluginStatus check(const std::string &client,
                               const std::string &user,
                               const std::string &privilege,
                               PolicyResult &result,
                               PluginData &pluginData) noexcept = 0;

    /**
     * Updates response returned by agent
     * @param[in] client
     * @param[in] user
     * @param[in] privilege
     * @param[in] result      Response received from agent
     * @param[in] agentData   Additional data, passed from agent
     * @return PluginStatus   In case of success - SUCCESS, in case of error - ERROR
     */
    virtual PluginStatus update(const std::string &client,
                                const std::string &user,
                                const std::string &privilege,
                                const PolicyResult &result,
                                const PluginData &agentData) noexcept = 0;

    virtual ~ExternalPluginInterface() {};

};

} // namespace Cynara

#endif /* EXTERNAL_PLUGIN_H_ */
