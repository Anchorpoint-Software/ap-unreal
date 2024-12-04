#ifndef DATABASE_API_HELPER_H
#define DATABASE_API_HELPER_H

#include "apsync/models/attributes/attribute_cell.h"
#include "apsync/models/attributes/attribute_field.h"
#include "apsync/models/attributes/attribute_field_tag.h"
#include "apsync/models/attributes/attribute_type.h"
#include "apsync/models/attributes/database_object.h"
#include "apsync/models/attributes/database_type.h"
#include "apsync/sync_global.h"

namespace apsync {

class Api;
class ObjectApi;
class DatabaseApi;
class ProjectsApi;
class AccountApi;
class SYNC_EXPORT DatabaseApiHelper
{
public:
    DatabaseApiHelper(shared_ptr<ObjectApi> objectApi, shared_ptr<DatabaseApi> databaseApi,
                      shared_ptr<ProjectsApi> projectsApi);

    Result<optional<string>> getCellLink(
        const string& absolutePath, const string& fieldName,
        const optional<string>& activeWorkspaceIdOpt = nullopt) const;
    Result<void> setCellLink(const string& absolutePath, const string& fieldName,
                             const string& link,
                             const optional<string>& activeWorkspaceIdOpt = nullopt,
                             bool autoCreate = true, bool updateTimeline = false);

    Result<optional<string>> getCellText(
        const string& absolutePath, const string& fieldName,
        const optional<string>& activeWorkspaceIdOpt = nullopt) const;
    Result<void> setCellText(const string& absolutePath, const string& fieldName,
                             const string& text,
                             const optional<string>& activeWorkspaceIdOpt = nullopt,
                             bool autoCreate = true, bool updateTimeline = false);

    Result<bool> getCellChecked(const string& absolutePath, const string& fieldName,
                                const optional<string>& activeWorkspaceIdOpt = nullopt) const;
    Result<void> setCellChecked(const string& absolutePath, const string& fieldName,
                                const bool checked,
                                const optional<string>& activeWorkspaceIdOpt = nullopt,
                                bool autoCreate = true, bool updateTimeline = false);

    Result<uint64> getCellDate(const string& absolutePath, const string& fieldName,
                               const optional<string>& activeWorkspaceIdOpt = nullopt) const;
    Result<void> setCellDate(const string& absolutePath, const string& fieldName,
                             const uint64 secsSinceEpoch,
                             const optional<string>& activeWorkspaceIdOpt = nullopt,
                             bool autoCreate = true, bool updateTimeline = false);

    Result<int> getCellRating(const string& absolutePath, const string& fieldName,
                              const optional<string>& activeWorkspaceIdOpt = nullopt) const;
    Result<void> setCellRating(const string& absolutePath, const string& fieldName,
                               const int rating,
                               const optional<string>& activeWorkspaceIdOpt = nullopt,
                               bool autoCreate = true, bool updateTimeline = false);

    Result<optional<AttributeFieldTag>> getCellTag(
        const string& absolutePath, const string& fieldName,
        const optional<string>& activeWorkspaceIdOpt = nullopt) const;
    Result<vector<AttributeFieldTag>> getCellTags(
        const string& absolutePath, const string& fieldName,
        const optional<string>& activeWorkspaceIdOpt = nullopt) const;
    Result<void> setCellTag(const string& absolutePath, const string& fieldName,
                            const string& tagName,
                            const AttributeType& type = AttributeType::singleChoiceTag(),
                            const optional<string>& activeWorkspaceIdOpt = nullopt,
                            bool autoCreate = true, const optional<TagColor>& tagColor = nullopt,
                            bool updateTimeline = false) const;
    Result<void> setCellTags(const string& absolutePath, const string& fieldName,
                             const vector<string>& tagNames,
                             const AttributeType& type = AttributeType::multipleChoiceTag(),
                             const optional<string>& activeWorkspaceIdOpt = nullopt,
                             bool autoCreate = true, bool updateTimeline = false) const;
    Result<void> addCellTag(const string& absolutePath, const string& fieldName,
                            const string& tagName,
                            const AttributeType& type = AttributeType::multipleChoiceTag(),
                            const optional<string>& activeWorkspaceIdOpt = nullopt,
                            bool autoCreate = true, const optional<TagColor>& tagColor = nullopt,
                            bool updateTimeline = false) const;
    Result<void> addCellTags(const string& absolutePath, const string& fieldName,
                             const vector<string>& tagNames,
                             const AttributeType& type = AttributeType::multipleChoiceTag(),
                             const optional<string>& activeWorkspaceIdOpt = nullopt,
                             bool autoCreate = true, bool updateTimeline = false) const;

    Result<void> removeCellTag(const string& absolutePath, const string& fieldName,
                               const string& tagName,
                               const AttributeType& type = AttributeType::multipleChoiceTag(),
                               const optional<string>& activeWorkspaceIdOpt = nullopt) const;

    Result<vector<AttributeField>> getAttributes(const string& workspaceId,
                                                 const optional<string>& projectId) const;
    Result<AttributeField> getAttribute(const string& workspaceId,
                                        const optional<string>& projectId, const string& name,
                                        optional<AttributeType> type) const;
    Result<void> addAttribute(AttributeField& field, const string& workspaceId,
                              const optional<string>& projectId);
    Result<void> removeAttribute(const AttributeField& field);
    Result<void> updateAttribute(const AttributeField& field);

private:
    shared_ptr<ObjectApi> _objectApi;
    shared_ptr<DatabaseApi> _databaseApi;
    shared_ptr<ProjectsApi> _projectsApi;
};
} // namespace apsync

#endif // DATABASE_API_HELPER_H
