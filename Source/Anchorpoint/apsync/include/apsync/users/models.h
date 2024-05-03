//$generated
#pragma once

#include "apsync/sync_global.h"
#include "apsync/api_types.h"

// External includes


// Dependencies

namespace apsync {

template <typename T>
struct ApiMapper;

// Forward declare models
class UserProfileNew;
class UserNew;
class UserSettingsNew;
class AuthenticatedUser;

// Aliases

// Enums

// UserProfileNew
class SYNC_EXPORT UserProfileNew
{
public:
    const std::string& getFirstName() const { return firstName; }

    void setFirstName(const std::string& firstName_) { this->firstName = firstName_; }

    const std::string& getLastName() const { return lastName; }

    void setLastName(const std::string& lastName_) { this->lastName = lastName_; }

    const std::string& getNickname() const { return nickname; }

    void setNickname(const std::string& nickname_) { this->nickname = nickname_; }

    const apsync::Url& getPicture() const { return picture; }

    void setPicture(const apsync::Url& picture_) { this->picture = picture_; }

public:
    std::string firstName;
    std::string lastName;
    std::string nickname;
    apsync::Url picture;
};

// UserNew
class SYNC_EXPORT UserNew
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getAuthId() const { return authId; }

    void setAuthId(const std::string& authId_) { this->authId = authId_; }

    const std::string& getUserspaceId() const { return userspaceId; }

    void setUserspaceId(const std::string& userspaceId_) { this->userspaceId = userspaceId_; }

    const std::string& getEmail() const { return email; }

    void setEmail(const std::string& email_) { this->email = email_; }

    const uint64& getFlags() const { return flags; }

    void setFlags(const uint64& flags_) { this->flags = flags_; }

    const UserProfileNew& getProfile() const { return profile; }

    void setProfile(const UserProfileNew& profile_) { this->profile = profile_; }

    const UserProfileNew& getExternalProfile() const { return externalProfile; }

    void setExternalProfile(const UserProfileNew& externalProfile_) { this->externalProfile = externalProfile_; }

public:
    std::string id;
    std::string authId;
    std::string userspaceId;
    std::string email;
    uint64 flags;
    UserProfileNew profile;
    UserProfileNew externalProfile;
};

// UserSettingsNew
class SYNC_EXPORT UserSettingsNew
{
public:
    bool getEnableOfflineMentionEmails() const { return enableOfflineMentionEmails; }

    void setEnableOfflineMentionEmails(bool enableOfflineMentionEmails_) { this->enableOfflineMentionEmails = enableOfflineMentionEmails_; }

public:
    bool enableOfflineMentionEmails = false;
};

// AuthenticatedUser
class SYNC_EXPORT AuthenticatedUser
{
public:
    const UserNew& getUser() const { return user; }

    void setUser(const UserNew& user_) { this->user = user_; }

    const std::string& getToken() const { return token; }

    void setToken(const std::string& token_) { this->token = token_; }

    const std::string& getApi() const { return api; }

    void setApi(const std::string& api_) { this->api = api_; }

public:
    UserNew user;
    std::string token;
    std::string api;
};

} //namespace apsync
//$generated