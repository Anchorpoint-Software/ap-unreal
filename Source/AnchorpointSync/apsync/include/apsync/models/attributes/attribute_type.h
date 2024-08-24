#ifndef ANCHORPOINT_ATTRIBUTE_TYPE_H
#define ANCHORPOINT_ATTRIBUTE_TYPE_H

#include "apsync/sync_global.h"

namespace apsync {

struct SYNC_EXPORT AttributeType {
    AttributeType() : _type(singleChoiceTag()) {}
    AttributeType(const std::string& type) : _type(type) {}
    operator const std::string&() const { return _type; }
    bool operator==(const AttributeType& other) const { return _type == other._type; }
    bool operator!=(const AttributeType& other) const { return _type != other._type; }

    static AttributeType singleChoiceTag();
    static AttributeType multipleChoiceTag();
    static AttributeType text();
    static AttributeType rating();
    static AttributeType hyperlink();
    static AttributeType date();
    static AttributeType checkbox();
    static AttributeType timestamp();
    static AttributeType user();

private:
    std::string _type;
};

} // namespace apsync

#endif // ANCHORPOINT_ATTRIBUTE_TYPE_H
