#ifndef ANCHORPOINT_ATTRIBUTE_FIELD_TAG_H
#define ANCHORPOINT_ATTRIBUTE_FIELD_TAG_H

#include "apsync/sync_global.h"
#include "apsync/util/random.h"
#include "attribute_field_tag_color.h"

namespace apsync {
struct SYNC_EXPORT AttributeFieldTag {
    explicit AttributeFieldTag(const std::string& tagName) : AttributeFieldTag() { name = tagName; }
    AttributeFieldTag(const std::string& tagName, const TagColor& tagColor) : AttributeFieldTag()
    {
        name = tagName;
        color = tagColor;
    }
    AttributeFieldTag(const std::string& tagName, const std::string& tagColor) : AttributeFieldTag()
    {
        name = tagName;
        color = TagColor(tagColor);
    }
    AttributeFieldTag() : id(randomId32()) {}
    std::string id; // Must be unique within the scope of an AttributeField
    std::string name;
    TagColor color;
};
} // namespace apsync

#endif // ANCHORPOINT_ATTRIBUTE_FIELD_TAG_H
