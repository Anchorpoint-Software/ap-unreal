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
class Icon;
class Task;

// Aliases

// Enums

// Icon
class SYNC_EXPORT Icon
{
public:
    const std::string& getPath() const { return _path; }

    void setPath(const std::string& path) { _path = path; }

    const std::string& getColor() const { return _color; }

    void setColor(const std::string& color) { _color = color; }

private:
    std::string _path;
    std::string _color;

    template <typename T>
    friend struct ApiMapper;
};

// Task
class SYNC_EXPORT Task
{
public:
    const std::string& getId() const { return _id; }

    void setId(const std::string& id) { _id = id; }

    bool getChecked() const { return _checked; }

    void setChecked(bool checked) { _checked = checked; }

    const std::optional<std::string>& getCheckedBy() const { return _checkedBy; }

    void setCheckedBy(const std::optional<std::string>& checkedBy) { _checkedBy = checkedBy; }

    const std::optional<int64>& getCheckedAt() const { return _checkedAt; }

    void setCheckedAt(const std::optional<int64>& checkedAt) { _checkedAt = checkedAt; }

    const std::optional<Icon>& getIcon() const { return _icon; }

    void setIcon(const std::optional<Icon>& icon) { _icon = icon; }

    int64 getAttachmentCount() const { return _attachmentCount; }

    void setAttachmentCount(int64 attachmentCount) { _attachmentCount = attachmentCount; }

private:
    std::string _id;
    bool _checked;
    std::optional<std::string> _checkedBy;
    std::optional<int64> _checkedAt;
    std::optional<Icon> _icon;
    int64 _attachmentCount;

    template <typename T>
    friend struct ApiMapper;
};

} //namespace apsync
//$generated