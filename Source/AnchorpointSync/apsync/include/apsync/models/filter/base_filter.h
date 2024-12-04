#ifndef BASE_FILTER_H
#define BASE_FILTER_H

#include <memory>
#include <optional>
#include <vector>

#include "apsync/sync_global.h"
#include "apsync/util/json.hpp"
#include "filter_operation_type.h"
#include "filter_scope_type.h"
#include "filter_type.h"

namespace apsync {

struct SYNC_EXPORT BaseFilter {

    virtual ~BaseFilter() = default;
    std::string id;
    FilterType type;

    virtual nlohmann::json toJson() const;
};
using BaseFilterRef = std::shared_ptr<BaseFilter>;

struct SYNC_EXPORT AndFilter: public BaseFilter {
    AndFilter() { type = FilterType::andType(); }
    std::vector<BaseFilterRef> filters;

    nlohmann::json toJson() const override;
};
using AndFilterRef = std::shared_ptr<AndFilter>;

struct SYNC_EXPORT OrFilter: public BaseFilter {
    OrFilter() { type = FilterType::orType(); }
    std::vector<BaseFilterRef> filters;

    nlohmann::json toJson() const override;
};
using OrFilterRef = std::shared_ptr<OrFilter>;

struct SYNC_EXPORT SingleTagFilter: public BaseFilter {
    const std::vector<FilterOperationType> allowedOperations = {FilterOperationType::contains(),
                                                                FilterOperationType::notContains()};
    SingleTagFilter() { type = FilterType::singleTag(); }

    nlohmann::json toJson() const override;

    std::string fieldId;
    std::vector<std::string> tagIds;
    FilterOperationType activeOperation;
};
using SingleTagFilterRef = std::shared_ptr<SingleTagFilter>;

struct SYNC_EXPORT MultiTagFilter: public SingleTagFilter {
    const std::vector<FilterScopeType> allowedScopes = {FilterScopeType::all(),
                                                        FilterScopeType::atLeastOne()};
    MultiTagFilter() { type = FilterType::multiTag(); }

    nlohmann::json toJson() const override;

    FilterScopeType activeScope;
};
using MultiTagFilterRef = std::shared_ptr<MultiTagFilter>;

struct SYNC_EXPORT TextFilter: public BaseFilter {
    const std::vector<FilterOperationType> allowedOperations = {
        FilterOperationType::contains(),   FilterOperationType::notContains(),
        FilterOperationType::equals(),     FilterOperationType::doesNotEqual(),
        FilterOperationType::isEmpty(),    FilterOperationType::isNotEmpty(),
        FilterOperationType::equalsRegex()};
    TextFilter() { type = FilterType::text(); }

    nlohmann::json toJson() const override;

    std::string fieldId;
    std::string input;
    FilterOperationType activeOperation;
};
using TextFilterRef = std::shared_ptr<TextFilter>;

struct SYNC_EXPORT UrlFilter: TextFilter {
    UrlFilter() { type = FilterType::url(); }
};
using UrlFilterRef = std::shared_ptr<UrlFilter>;

struct SYNC_EXPORT RatingFilter: public BaseFilter {
    const std::vector<FilterOperationType> allowedOperations = {FilterOperationType::min(),
                                                                FilterOperationType::max()};
    RatingFilter() { type = FilterType::rating(); }

    nlohmann::json toJson() const override;

    std::string fieldId;
    std::optional<int> input;
    FilterOperationType activeOperation;
};
using RatingFilterRef = std::shared_ptr<RatingFilter>;

struct SYNC_EXPORT MemberFilter: public BaseFilter {
    const std::vector<FilterOperationType> allowedOperations = {FilterOperationType::contains(),
                                                                FilterOperationType::notContains()};
    const std::vector<FilterScopeType> allowedScopes = {FilterScopeType::all(),
                                                        FilterScopeType::atLeastOne()};
    MemberFilter() { type = FilterType::member(); }

    nlohmann::json toJson() const override;

    std::string fieldId;
    std::vector<std::string> userIds;
    FilterOperationType activeOperation;
    FilterScopeType activeScope;
};
using MemberFilterRef = std::shared_ptr<MemberFilter>;

struct SYNC_EXPORT BaseNumberFilter: public BaseFilter {

    nlohmann::json toJson() const override;

    std::string fieldId;
    uint64 input;
    std::optional<uint64> maxInput;
};

struct SYNC_EXPORT TimeTrackingFilter: public BaseNumberFilter {
    const std::vector<FilterOperationType> allowedOperations = {
        FilterOperationType::greaterThan(), FilterOperationType::lessThan(),
        FilterOperationType::zero(),        FilterOperationType::notZero(),
        FilterOperationType::equals(),      FilterOperationType::doesNotEqual(),
        FilterOperationType::between()};
    TimeTrackingFilter() { type = FilterType::timeTracking(); }

    nlohmann::json toJson() const override;

    FilterOperationType activeOperation;
};
using TimeTrackingFilterRef = std::shared_ptr<TimeTrackingFilter>;

struct SYNC_EXPORT FileSizeFilter: public BaseNumberFilter {
    const std::vector<FilterOperationType> allowedOperations = {
        FilterOperationType::greaterThan(), FilterOperationType::lessThan(),
        FilterOperationType::equals(), FilterOperationType::doesNotEqual(),
        FilterOperationType::between()};
    FileSizeFilter() { type = FilterType::fileSize(); }

    nlohmann::json toJson() const override;

    FilterOperationType activeOperation;
};
using FileSizeFilterRef = std::shared_ptr<FileSizeFilter>;

struct SYNC_EXPORT DateFilter: public BaseFilter {
    const std::vector<FilterOperationType> allowedOperations = {
        FilterOperationType::today(),        FilterOperationType::thisWeek(),
        FilterOperationType::thisMonth(),    FilterOperationType::yesterday(),
        FilterOperationType::lastWeek(),     FilterOperationType::lastMonth(),
        FilterOperationType::tomorrow(),     FilterOperationType::nextWeek(),
        FilterOperationType::nextMonth(),    FilterOperationType::before(),
        FilterOperationType::after(),        FilterOperationType::equals(),
        FilterOperationType::doesNotEqual(), FilterOperationType::between()};
    DateFilter() { type = FilterType::date(); }

    nlohmann::json toJson() const override;

    std::string fieldId;
    std::vector<FilterOperationType> activeOperations;
    std::optional<uint64> input;
    std::optional<uint64> maxInput;
};
using DateFilterRef = std::shared_ptr<DateFilter>;

struct SYNC_EXPORT CheckboxFilter: public BaseFilter {
    CheckboxFilter() { type = FilterType::checkbox(); }

    const std::vector<FilterOperationType> allowedOperations = {
        FilterOperationType::equals(), FilterOperationType::doesNotEqual()};

    nlohmann::json toJson() const override;

    std::string fieldId;
    FilterOperationType activeOperation;
};
using CheckboxFilterRef = std::shared_ptr<CheckboxFilter>;

SYNC_EXPORT void from_json(const nlohmann::json& json, BaseFilterRef& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, AndFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, OrFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, SingleTagFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, MultiTagFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, TextFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, UrlFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, RatingFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, MemberFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, BaseNumberFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, TimeTrackingFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, FileSizeFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, DateFilter& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, CheckboxFilter& instance);

SYNC_EXPORT void to_json(nlohmann::json& json, const BaseFilterRef& instance);

} // namespace apsync

#endif // BASE_FILTER_H
