//$generated
#pragma once

#include "apsync/sync_global.h"
#include "apsync/api_types.h"

// External includes


// Dependencies
#include "apsync/invites/models.h"
#include "apsync/system_messages/models.h"
#include "apsync/users/models.h"

namespace apsync {

template <typename T>
struct ApiMapper;

// Forward declare models
class SubscriptionItem;
class Subscription;
class TrialSubscription;

// Aliases

// Enums

// SubscriptionItem
class SYNC_EXPORT SubscriptionItem
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getProductName() const { return productName; }

    void setProductName(const std::string& productName_) { this->productName = productName_; }

    int64 getQuantity() const { return quantity; }

    void setQuantity(int64 quantity_) { this->quantity = quantity_; }

    const std::string& getInterval() const { return interval; }

    void setInterval(const std::string& interval_) { this->interval = interval_; }

public:
    std::string id;
    std::string productName;
    int64 quantity = 0;
    std::string interval;
};

// Subscription
class SYNC_EXPORT Subscription
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getStatus() const { return status; }

    void setStatus(const std::string& status_) { this->status = status_; }

    int64 getStartDate() const { return startDate; }

    void setStartDate(int64 startDate_) { this->startDate = startDate_; }

    const std::vector<SubscriptionItem>& getItems() const { return items; }

    void setItems(const std::vector<SubscriptionItem>& items_) { this->items = items_; }

public:
    std::string id;
    std::string status;
    int64 startDate = 0;
    std::vector<SubscriptionItem> items;
};

// TrialSubscription
class SYNC_EXPORT TrialSubscription
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getUserId() const { return userId; }

    void setUserId(const std::string& userId_) { this->userId = userId_; }

    const std::string& getTrialName() const { return trialName; }

    void setTrialName(const std::string& trialName_) { this->trialName = trialName_; }

    int64 getStartDate() const { return startDate; }

    void setStartDate(int64 startDate_) { this->startDate = startDate_; }

    int64 getRemainingDays() const { return remainingDays; }

    void setRemainingDays(int64 remainingDays_) { this->remainingDays = remainingDays_; }

public:
    std::string id;
    std::string userId;
    std::string trialName;
    int64 startDate = 0;
    int64 remainingDays = 0;
};

} //namespace apsync
//$generated