#ifndef ATTRIBUTE_FIELD_KEY_H
#define ATTRIBUTE_FIELD_KEY_H

#include <vector>

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT AttributeFieldKey {
    AttributeFieldKey() : _key(attribute()) {}
    AttributeFieldKey(const std::string& key) : _key(key) {}
    operator const std::string&() const { return _key; }
    bool operator==(const AttributeFieldKey& other) const { return _key == other._key; }
    bool operator!=(const AttributeFieldKey& other) const { return _key != other._key; }

    static AttributeFieldKey attribute();
    static AttributeFieldKey name();
    static AttributeFieldKey modificationDate();
    static AttributeFieldKey size();
    static AttributeFieldKey status();
    static AttributeFieldKey location();
    static AttributeFieldKey projectMember();
    static AttributeFieldKey project();

    static std::vector<AttributeFieldKey> getFieldKeys();

private:
    std::string _key;
};
} // namespace apsync

#endif // ATTRIBUTE_FIELD_KEY_H
