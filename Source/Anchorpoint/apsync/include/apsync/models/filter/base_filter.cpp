#include "base_filter.h"

nlohmann::json apsync::BaseFilter::toJson() const
{
    nlohmann::json json;
    json["id"] = id;
    json["type"] = type;
    return json;
}

nlohmann::json apsync::AndFilter::toJson() const
{
    auto json = BaseFilter::toJson();
    json["filters"] = nlohmann::json(filters);
    return json;
}

nlohmann::json apsync::OrFilter::toJson() const
{
    auto json = BaseFilter::toJson();
    json["filters"] = nlohmann::json(filters);
    return json;
}

nlohmann::json apsync::SingleTagFilter::toJson() const
{
    auto json = BaseFilter::toJson();
    json["fieldId"] = fieldId;
    json["tagIds"] = nlohmann::json(tagIds);
    json["activeOp"] = activeOperation;
    return json;
}

void apsync::from_json(const nlohmann::json& json, SingleTagFilter& instance)
{
    json.at("fieldId").get_to(instance.fieldId);
    json.at("tagIds").get_to(instance.tagIds);
    instance.activeOperation = json.at("activeOp").get<std::string>();
}

nlohmann::json apsync::MultiTagFilter::toJson() const
{
    auto json = SingleTagFilter::toJson();
    json["activeScope"] = activeScope;
    return json;
}

void apsync::from_json(const nlohmann::json& json, MultiTagFilter& instance)
{
    auto& casted = static_cast<SingleTagFilter&>(instance);
    from_json(json, casted);
    instance.activeScope = json.at("activeScope").get<std::string>();
}

nlohmann::json apsync::TextFilter::toJson() const
{
    auto json = BaseFilter::toJson();
    json["fieldId"] = fieldId;
    json["input"] = input;
    json["activeOp"] = activeOperation;
    return json;
}

void apsync::from_json(const nlohmann::json& json, TextFilter& instance)
{
    json.at("fieldId").get_to(instance.fieldId);
    json.at("input").get_to(instance.input);
    instance.activeOperation = json.at("activeOp").get<std::string>();
}

void apsync::from_json(const nlohmann::json& json, UrlFilter& instance)
{
    auto& casted = static_cast<TextFilter&>(instance);
    from_json(json, casted);
}

nlohmann::json apsync::RatingFilter::toJson() const
{
    auto json = BaseFilter::toJson();
    json["fieldId"] = fieldId;
    if (input.has_value()) { json["input"] = *input; }
    json["activeOp"] = activeOperation;
    return json;
}

void apsync::from_json(const nlohmann::json& json, RatingFilter& instance)
{
    json.at("fieldId").get_to(instance.fieldId);
    if (json.contains("input")) {
        instance.input = 0;
        json.at("input").get_to(*instance.input);
    }
    instance.activeOperation = json.at("activeOp").get<std::string>();
}

nlohmann::json apsync::MemberFilter::toJson() const
{
    auto json = BaseFilter::toJson();
    json["fieldId"] = fieldId;
    json["activeOp"] = activeOperation;
    json["activeScope"] = activeScope;
    json["userIds"] = nlohmann::json(userIds);
    return json;
}

void apsync::from_json(const nlohmann::json& json, MemberFilter& instance)
{
    json.at("fieldId").get_to(instance.fieldId);
    json.at("userIds").get_to(instance.userIds);
    instance.activeOperation = json.at("activeOp").get<std::string>();
    instance.activeScope = json.at("activeScope").get<std::string>();
}

nlohmann::json apsync::BaseNumberFilter::toJson() const
{
    auto json = BaseFilter::toJson();
    json["fieldId"] = fieldId;
    json["input"] = input;
    if (maxInput.has_value()) { json["maxInput"] = *maxInput; }
    return json;
}

void apsync::from_json(const nlohmann::json& json, BaseNumberFilter& instance)
{
    json.at("fieldId").get_to(instance.fieldId);
    json.at("input").get_to(instance.input);
    if (json.contains("maxInput")) {
        instance.maxInput = 0;
        json.at("maxInput").get_to(*instance.maxInput);
    }
}

nlohmann::json apsync::TimeTrackingFilter::toJson() const
{
    auto json = BaseNumberFilter::toJson();
    json["activeOp"] = activeOperation;
    return json;
}

void apsync::from_json(const nlohmann::json& json, TimeTrackingFilter& instance)
{
    auto& casted = static_cast<BaseNumberFilter&>(instance);
    from_json(json, casted);
    instance.activeOperation = json.at("activeOp").get<std::string>();
}

nlohmann::json apsync::FileSizeFilter::toJson() const
{
    auto json = BaseNumberFilter::toJson();
    json["activeOp"] = activeOperation;
    return json;
}

void apsync::from_json(const nlohmann::json& json, FileSizeFilter& instance)
{
    auto& casted = static_cast<BaseNumberFilter&>(instance);
    from_json(json, casted);
    instance.activeOperation = json.at("activeOp").get<std::string>();
}

nlohmann::json apsync::DateFilter::toJson() const
{
    auto json = BaseFilter::toJson();
    json["fieldId"] = fieldId;
    json["activeOps"] = nlohmann::json(activeOperations);
    if (input.has_value()) { json["input"] = *input; }
    if (maxInput.has_value()) { json["maxInput"] = *maxInput; }

    return json;
}

void apsync::from_json(const nlohmann::json& json, DateFilter& instance)
{
    json.at("fieldId").get_to(instance.fieldId);
    auto ops = json.at("activeOps").get<std::vector<std::string>>();
    for (const auto& op : std::as_const(ops)) { instance.activeOperations.push_back(op); }
    if (json.contains("input")) {
        instance.input = 0;
        json.at("input").get_to(*instance.input);
    }
    if (json.contains("maxInput")) {
        instance.maxInput = 0;
        json.at("maxInput").get_to(*instance.maxInput);
    }
}

nlohmann::json apsync::CheckboxFilter::toJson() const
{
    auto json = BaseFilter::toJson();
    json["fieldId"] = fieldId;
    json["activeOp"] = activeOperation;
    return json;
}

void apsync::from_json(const nlohmann::json& json, CheckboxFilter& instance)
{
    json.at("fieldId").get_to(instance.fieldId);
    instance.activeOperation = json.at("activeOp").get<std::string>();
}

void apsync::from_json(const nlohmann::json& json, AndFilter& instance)
{
    instance.filters = json.at("filters").get<std::vector<BaseFilterRef>>();
}

void apsync::from_json(const nlohmann::json& json, OrFilter& instance)
{
    instance.filters = json.at("filters").get<std::vector<BaseFilterRef>>();
}

void apsync::from_json(const nlohmann::json& json, BaseFilterRef& instance)
{
    const FilterType type = json.at("type").get<std::string>();
    const std::string id = json.at("id").get<std::string>();

    if (type == FilterType::andType()) {
        instance = std::make_shared<AndFilter>(json.get<AndFilter>());
    } else if (type == FilterType::orType()) {
        instance = std::make_shared<OrFilter>(json.get<OrFilter>());
    } else if (type == FilterType::singleTag()) {
        instance = std::make_shared<SingleTagFilter>(json.get<SingleTagFilter>());
    } else if (type == FilterType::multiTag()) {
        instance = std::make_shared<MultiTagFilter>(json.get<MultiTagFilter>());
    } else if (type == FilterType::text()) {
        instance = std::make_shared<TextFilter>(json.get<TextFilter>());
    } else if (type == FilterType::url()) {
        instance = std::make_shared<UrlFilter>(json.get<UrlFilter>());
    } else if (type == FilterType::rating()) {
        instance = std::make_shared<RatingFilter>(json.get<RatingFilter>());
    } else if (type == FilterType::member()) {
        instance = std::make_shared<MemberFilter>(json.get<MemberFilter>());
    } else if (type == FilterType::timeTracking()) {
        instance = std::make_shared<TimeTrackingFilter>(json.get<TimeTrackingFilter>());
    } else if (type == FilterType::fileSize()) {
        instance = std::make_shared<FileSizeFilter>(json.get<FileSizeFilter>());
    } else if (type == FilterType::date()) {
        instance = std::make_shared<DateFilter>(json.get<DateFilter>());
    } else if (type == FilterType::checkbox()) {
        instance = std::make_shared<CheckboxFilter>(json.get<CheckboxFilter>());
    } else {
        instance = std::make_shared<BaseFilter>();
    }
    if (instance) {
        instance->type = type;
        instance->id = id;
    }
}

void apsync::to_json(nlohmann::json& json, const BaseFilterRef& instance)
{
    json = instance->toJson();
}
