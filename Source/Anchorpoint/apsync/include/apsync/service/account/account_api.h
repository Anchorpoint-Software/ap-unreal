#ifndef ACCOUNT_API_H
#define ACCOUNT_API_H

#include "apsync/internal/models/subscription.h"
#include "apsync/internal/models/trial_subscription.h"
#include "apsync/sync_global.h"

namespace apsync {
class Api;
class AccountApiImpl;
class SYNC_EXPORT AccountApi
{
public:
    explicit AccountApi(std::shared_ptr<Api> api);

    Result<std::vector<Subscription>> listSubscriptions() const;

    Result<std::vector<TrialSubscription>> listTrials() const;

    Result<void> startTrial(const std::string& trialPlanName);

private:
    std::shared_ptr<AccountApiImpl> _pimpl;
    std::shared_ptr<Api> _api;
};
} // namespace apsync

#endif // ACCOUNT_API_H
