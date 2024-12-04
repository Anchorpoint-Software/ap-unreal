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
class QuotaExceededDetails;
class SystemMessage;

// Aliases
using SystemMessageDetails = std::variant<QuotaExceededDetails>;

// Enums

// QuotaExceededDetails
class SYNC_EXPORT QuotaExceededDetails
{
public:
    const std::vector<std::string>& getQuotaNames() const { return quotaNames; }

    void setQuotaNames(const std::vector<std::string>& quotaNames_) { this->quotaNames = quotaNames_; }

public:
    std::vector<std::string> quotaNames;
};

// SystemMessage
class SYNC_EXPORT SystemMessage
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    bool getIsRead() const { return isRead; }

    void setIsRead(bool isRead_) { this->isRead = isRead_; }

    bool getIsAccepted() const { return isAccepted; }

    void setIsAccepted(bool isAccepted_) { this->isAccepted = isAccepted_; }

    bool getIsDeclined() const { return isDeclined; }

    void setIsDeclined(bool isDeclined_) { this->isDeclined = isDeclined_; }

    const SystemMessageDetails& getDetails() const { return details; }

    void setDetails(const SystemMessageDetails& details_) { this->details = details_; }

public:
    std::string id;
    bool isRead = false;
    bool isAccepted = false;
    bool isDeclined = false;
    SystemMessageDetails details;
};

} //namespace apsync
//$generated