

#include <exceptions/ServerConnectionErrorException.h>
#include <log/log.h>
#include <request/CheckRequest.h>
#include <request/pointers.h>
#include <response/CheckResponse.h>
#include <response/pointers.h>

#include <cache/PolicyGetter.h>

namespace Cynara {
cynara_api_result PolicyGetter::requestResult(const PolicyKey &key, PolicyResult &result) noexcept {
    ProtocolFrameSequenceNumber sequenceNumber = generateSequenceNumber();

    //Ask cynara service
    CheckResponsePtr checkResponse;
    try {
        RequestPtr request = std::make_shared<CheckRequest>(key, sequenceNumber);
        ResponsePtr response = m_socketClient->askCynaraServer(request);
        if (!response) {
            LOGW("Disconnected by cynara server.");
            return cynara_api_result::CYNARA_API_SERVICE_NOT_AVAILABLE;
        }
        checkResponse = std::dynamic_pointer_cast<CheckResponse>(response);
        if (!checkResponse) {
            LOGC("Critical error. Casting Response to CheckResponse failed.");
            return cynara_api_result::CYNARA_API_ACCESS_DENIED;
        }

        LOGD("checkResponse: policyType = %d, metadata = %s",
             (int)checkResponse->m_resultRef.policyType(),
             checkResponse->m_resultRef.metadata().c_str());
    } catch (const ServerConnectionErrorException &ex) {
        LOGE("Cynara service not available.");
        return cynara_api_result::CYNARA_API_SERVICE_NOT_AVAILABLE;
    }

    result = checkResponse->m_resultRef;
    return cynara_api_result::CYNARA_API_SUCCESS;
}
}
