#ifndef INCLUDE_SERVICE_API
#define INCLUDE_SERVICE_API

#include <functional>

#include "apsync/service/logger.h"
#include "apsync/sync_global.h"

namespace apsync {
struct ApiImpl;
struct UserModel;
class SyncContext;
class SYNC_EXPORT Api
{
public:
    Api(const std::string& clientName, const std::string& api, const std::string& apiGrpc,
        const std::string& account);
    virtual ~Api() = default;

    static Result<std::shared_ptr<Api>> createFromAuthenticatedUser(const std::string& clientName);

    static Result<void> startAnchorpoint(
        const std::optional<std::string>& pathToAnchorpoint = std::nullopt, bool minimized = false);
    static Result<void> startDaemon(const std::string& pathToDaemon);
    void setErrorCallback(std::function<void(std::error_code)> cb);
    static void setLogger(std::shared_ptr<ILogger> logger);

    void setupContext(const std::string& api, const std::string& account);

    const std::string& getClientName() const;
    const std::string& getAddress() const;
    const std::string& getApi() const;
    const std::string& getGrpcApi() const;
    const std::string& getAccount() const;
    std::shared_ptr<SyncContext> getSyncContext() const;

    void setClientName(const std::string& name);
    void setAddress(const std::string& address);
    void setApi(const std::string& api);
    void setGrpcApi(const std::string& grpcApi);
    void setAccount(const std::string& account);

    friend class ObjectApi;
    friend class DatabaseApi;
    friend class StreamApi;
    friend class RemoteAttachmentApi;
    friend class CopyService;
    friend class IncVersionService;
    friend class ReviewService;
    friend class TaskApi;
    friend class BaseSettingsService;
    friend class SharedSettingsService;
    friend class SettingsService;
    friend class JournalService;
    friend class JournalEntryCreator;
    friend class AttachmentService;
    friend class MentionApi;
    friend class ExchangeService;
    friend class ActionsSettingsApi;
    friend class LocksApiImpl;
    friend class RemoteSettingsApi;

private:
    explicit Api(const string& clientName);
    std::shared_ptr<ApiImpl> _pimpl;
};
} // namespace apsync

#endif /* INCLUDE_SERVICE_API */
