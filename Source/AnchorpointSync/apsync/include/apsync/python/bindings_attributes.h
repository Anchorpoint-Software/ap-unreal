#ifndef BINDINGS_ATTRIBUTES_H
#define BINDINGS_ATTRIBUTES_H

#include "apsync/models/attributes/attribute_field.h"
#include "apsync/models/project_object.h"
#include "apsync/service/api.h"
#include "apsync/sync_global.h"

namespace pybind11 {
class object;
class list;
} // namespace pybind11

namespace python {
struct SYNC_EXPORT Attribute {
    std::string id;

    std::string name;
    apsync::AttributeType type;
    std::optional<std::string> projectId;

    std::optional<apsync::AttributeCellValue> defaultValue;

    std::optional<int> ratingsMaximum;
    std::vector<apsync::AttributeFieldTag> tags;
};

struct Task;
using AttributesTarget = std::variant<Task, std::string>;
using AttributeIdentifier = std::variant<Attribute, std::string>;
using AttributeValueType =
    std::variant<int, std::string, std::vector<std::string>, apsync::AttributeFieldTag,
                 std::vector<apsync::AttributeFieldTag>, pybind11::object, bool>;
using AttributeTagsList = std::variant<std::vector<apsync::AttributeFieldTag>, pybind11::list>;

class AttributesApiImpl;
class SYNC_EXPORT AttributesApi
{
public:
    AttributesApi(std::shared_ptr<apsync::Api> api, const std::string& workspaceId);
    void setWorkspaceId(const std::string& workspaceId);
    void setProject(const std::optional<apsync::ProjectObject>& project);

    void setAttributeValue(const AttributesTarget& target, const AttributeIdentifier& attribute,
                           const AttributeValueType& value, bool updateTimeline = false);

    pybind11::object getAttributeValue(const AttributesTarget& target,
                                       const AttributeIdentifier& attribute);

    Attribute createAttribute(const std::string& name, const apsync::AttributeType& type,
                              const std::optional<AttributeTagsList>& tags, std::optional<int> max);

    std::optional<Attribute> getAttribute(
        const std::string& name, const std::optional<apsync::AttributeType>& type = std::nullopt);
    std::vector<Attribute> getAttributes(
        const std::optional<apsync::AttributeType>& type = std::nullopt);

    Attribute getAttributeById(const std::string& id);

    void renameAttribute(Attribute& attribute, const std::string& name);
    void setAttributeTags(Attribute& attribute, const std::vector<apsync::AttributeFieldTag>& tags);
    void setAttributeRatingMax(Attribute& attribute, int max);

private:
    std::string _workspaceId;
    std::optional<apsync::ProjectObject> _project;
    std::shared_ptr<AttributesApiImpl> _impl;
};
} // namespace python

#endif // BINDINGS_ATTRIBUTES_H
