//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT AccountApi
{
public:
    explicit AccountApi(std::shared_ptr<Api> api);

    Result<void> setAuthToken(
        const std::string& token
    );

    Result<UserNew> getUser(
        bool useCache
    );

    Result<std::vector<AuthenticatedUser>> getAuthenticatedUsers(
    );

    Result<std::string> getUserId(
    );

    Result<void> updateUserProfile(
        const UserProfileNew& profile
    );

    Result<apsync::Url> updateUserAvatar(
        const std::string& path
    );

    Result<std::vector<Invite>> listInvites(
    );

    Result<void> acceptInvite(
        const std::string& workspaceId
    );

    Result<void> rejectInvite(
        const std::string& workspaceId
    );

    Result<std::vector<Subscription>> listSubscriptions(
    );

    Result<std::vector<TrialSubscription>> listTrials(
    );

    Result<TrialSubscription> startTrial(
        const std::string& planName
    );

    Result<apsync::Url> startBillingPortalSession(
        const apsync::Url& returnURL
    );

    Result<apsync::Url> startCheckoutSession(
        const std::string& planId,
        const apsync::Url& successURL,
        const apsync::Url& cancelURL
    );

    Result<std::vector<SystemMessage>> listSystemMessages(
        bool showRead
    );

    Result<void> markSystemMessage(
        const std::string& msgId,
        const std::string& flag,
        bool clear
    );

    Result<UserSettingsNew> getSettings(
    );

    Result<void> updateSettings(
        const UserSettingsNew& settings
    );

    Result<void> clientInstall(
    );

    Result<void> subscribeNewsletter(
    );

private:
    std::shared_ptr<class AccountApiImpl> _impl;
};

} // namespace apsync
//$generated