#ifndef ANCHORPOINT_ATTRIBUTE_FIELD_TAG_COLOR_H
#define ANCHORPOINT_ATTRIBUTE_FIELD_TAG_COLOR_H

#include "apsync/sync_global.h"
#include "apsync/util/random.h"

namespace apsync {

struct SYNC_EXPORT TagColor {
    TagColor() : _color(colors()[randomNumber(0, ((int) colors().size()) - 1)]) {}
    TagColor(const std::string& color) : _color(color) {}

    operator const std::string&() const { return _color; }
    bool operator==(const TagColor& other) const { return _color == other._color; }

    static const TagColor red();
    static const TagColor orange();
    static const TagColor yellow();
    static const TagColor green();
    static const TagColor turk();
    static const TagColor blue();
    static const TagColor purple();
    static const TagColor grey();
    static const std::vector<TagColor>& colors();

private:
    std::string _color;
};


} // namespace apsync

#endif // ANCHORPOINT_ATTRIBUTE_FIELD_TAG_COLOR_H
