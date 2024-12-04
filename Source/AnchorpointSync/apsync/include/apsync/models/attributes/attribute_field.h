#ifndef ANCHORPOINT_ATTRIBUTE_FIELD_H
#define ANCHORPOINT_ATTRIBUTE_FIELD_H

#include <memory>
#include <vector>

#include "apsync/models/base_object.h"
#include "attribute_cell.h"
#include "attribute_field_key.h"
#include "attribute_field_tag.h"
#include "attribute_type.h"

namespace apsync {
struct SYNC_EXPORT AttributeFieldProps {
    std::string name;
    AttributeType type;

    std::optional<int> maximum;                        // Used by AttributeType::Rating
    std::optional<std::vector<AttributeFieldTag>> tags; // Used by AttributeType::SingleChoiceTag and
                                                        // AttributeType::MultipleChoiceTag
    std::optional<AttributeCellValue> defaultValue;

    std::optional<std::string> projectId;
};

struct SYNC_EXPORT AttributeBaseField {
    AttributeFieldKey key = AttributeFieldKey::attribute();
};
using AttributeBaseFieldRef = std::shared_ptr<AttributeBaseField>;

struct SYNC_EXPORT AttributeField: public AttributeBaseField, public BaseObject {
    AttributeFieldProps properties;
    bool operator==(const AttributeField& other) const { return id == other.id; }
};

struct SYNC_EXPORT AttributeBaseFieldIdentifier {
    AttributeFieldKey key;
    std::optional<std::string> fieldId = std::nullopt;
    bool operator==(const AttributeBaseFieldIdentifier& other) const
    {
        return key == other.key && fieldId == other.fieldId;
    }
};

}// namespace apsync

namespace std {
template<>
struct hash<apsync::AttributeField> {
    std::size_t operator()(const apsync::AttributeField& o) const noexcept
    {
        return hash<std::string>{}(o.id);
    }
};

template<>
struct hash<apsync::AttributeBaseFieldIdentifier> {
    std::size_t operator()(const apsync::AttributeBaseFieldIdentifier& o) const noexcept
    {
        auto h1 = hash<std::string>{}(o.key);
        if (o.fieldId) {
            auto h2 = hash<std::string>{}(*o.fieldId);
            return h1 ^ (h2 << 1);
        }
        return h1;
    }
};
} // namespace std

#endif// ANCHORPOINT_ATTRIBUTE_FIELD_H
