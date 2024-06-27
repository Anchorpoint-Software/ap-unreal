#ifndef INCLUDE_SYNC_CONTEXT
#define INCLUDE_SYNC_CONTEXT

#include <grpcpp/grpcpp.h>

#include "apsync/sync_global.h"

namespace apsync {
class SYNC_EXPORT SyncContext
{
public:
    SyncContext(const std::string& clientName, const std::string& address, const std::string& api,
                const std::string& grpcApi, const std::string& account);

    const std::string& getClientName() const;
    const std::string& getAddress() const;
    const std::string& getApi() const;
    const std::string& getGrpcApi() const;
    const std::string& getAccount() const;
    std::shared_ptr<grpc::Channel> getChannel();

    void setErrorCallback(std::function<void(std::error_code)> cb);
    void reportError(std::error_code ec);

    void setClientName(const std::string& clientName);
    void setAddress(const std::string& address);
    void setApi(const std::string& api);
    void setGrpcApi(const std::string& grpcApi);
    void setAccount(const std::string& acount);

private:
    std::string _clientName;
    std::string _address;
    std::string _api;
    std::string _grpcApi;
    std::string _account;

    std::function<void(std::error_code)> _cb;

    std::shared_ptr<grpc::Channel> _channel;
};
} // namespace apsync

#endif /* INCLUDE_SYNC_CONTEXT */
