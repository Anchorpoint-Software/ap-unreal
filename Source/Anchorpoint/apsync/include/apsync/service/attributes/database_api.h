#ifndef ANCHORPOINT_DATABASE_API_H
#define ANCHORPOINT_DATABASE_API_H

#include <functional>
#include <unordered_map>
#include <unordered_set>

#include "apsync/models/attributes/attribute_cell.h"
#include "apsync/models/attributes/attribute_field.h"
#include "apsync/models/attributes/database_object.h"
#include "apsync/models/base_object.h"
#include "apsync/models/project_object.h"
#include "apsync/service/attributes/database_base_view.h"
#include "apsync/sync_global.h"

namespace apsync {
class Api;
class DatabaseApiImpl;
struct Object;
class SYNC_EXPORT DatabaseApi
{
public:
    explicit DatabaseApi(std::shared_ptr<Api> api);

    Result<DatabaseObject> createDatabase(const DatabaseType& type, const BaseObject& object,
                                          const std::optional<std::string>& projectId,
                                          const std::string& name = "My Database",
                                          bool createDefaultView = true);

    Result<std::vector<DatabaseObject>> getDatabases(
        const BaseObject& object, const std::optional<DatabaseType>& databaseType);

    Result<std::vector<DatabaseObject>> getDatabases(const std::optional<ProjectObject>& projectOpt,
                                                     const std::string& workspaceId,
                                                     const DatabaseType& databaseType);

    Result<std::optional<DatabaseObject>> getDatabase(
        const BaseObject& object, const std::optional<DatabaseType>& databaseType);

    Result<DatabaseObject> getDatabase(const std::string& databaseId,
                                       const std::string& workspaceId);

    Result<void> updateDatabase(const DatabaseObject& database);

    Result<void> deleteDatabase(const DatabaseObject& database);
    Result<void> deleteDatabase(const std::string& databaseId, const std::string& workspaceId);

    Result<std::string> resolvePath(const std::optional<ProjectObject>& projectObject,
                                    const std::string& databaseId, const std::string& workspaceId);

    Result<AttributeField> createField(const std::string& workspaceId,
                                       const std::optional<std::string>& projectId,
                                       const AttributeFieldProps& content);
    Result<std::vector<AttributeField>> getFields(const std::string& workspaceId,
                                                  const std::optional<std::string>& projectId);
    Result<std::vector<AttributeField>> getFields(const std::string& workspaceId,
                                                  const std::vector<std::string>& fieldIds);
    Result<AttributeField> getField(const std::string& workspaceId, const std::string& fieldId);
    Result<void> deleteField(const std::string& workspaceId, const std::string& fieldId);

    Result<void> updateField(const AttributeField& field);
    Result<AttributeField> copyField(const AttributeField& field);

    Result<void> addFieldToDatabase(std::shared_ptr<DatabaseBaseView>& view,
                                    const AttributeField& field, std::optional<int> insertionIndex);
    Result<void> removeFieldFromDatabase(std::shared_ptr<DatabaseBaseView>& view,
                                         const AttributeField& field);

    Result<void> addFieldToDatabase(std::shared_ptr<DatabaseBaseView>& view,
                                    const AttributeBaseFieldIdentifier& identifier,
                                    std::optional<int> insertionIndex);
    Result<void> removeFieldFromDatabase(std::shared_ptr<DatabaseBaseView>& view,
                                         const AttributeBaseFieldIdentifier& identifier);

    Result<std::shared_ptr<DatabaseBaseView>> getActiveDatabaseView(const DatabaseObject& database);
    Result<void> setActiveDatabaseView(DatabaseObject& database,
                                       const std::shared_ptr<DatabaseBaseView>& view);
    Result<std::shared_ptr<DatabaseBaseView>> getDatabaseView(const DatabaseObject& database,
                                                              const std::string& viewId);
    Result<std::vector<std::shared_ptr<DatabaseBaseView>>> getDatabaseViews(
        const DatabaseObject& database);
    Result<std::shared_ptr<DatabaseBaseView>> createDatabaseView(const DatabaseObject& database,
                                                                 const DatabaseViewType& type);
    Result<void> updateDatabaseView(const DatabaseBaseView& view);

    Result<std::optional<AttributeCell>> getCell(const BaseObject& object,
                                                 const std::string& databaseId,
                                                 const std::string& fieldId) const;
    Result<AttributeCell> getCellById(const std::string& workspaceId, const std::string& dbId,
                                      const std::string& cellId) const;

    Result<std::unordered_map<std::string, std::vector<AttributeCell>>> getCellsById(
        const std::string& workspaceId, const std::string& dbId,
        const std::unordered_set<std::string>& cellIds) const;
    Result<void> deleteCellsById(const std::string& workspaceId, const std::string& dbId,
                                 const std::vector<std::string>& cellIds) const;

    Result<std::unordered_map<std::string, AttributeCell>> getCells(
        const std::string& databaseId, const AttributeField& field) const;

    Result<std::unordered_map<std::string, AttributeCell>> getCells(
        const std::string& databaseId, const std::string& workspaceId,
        const std::string& fieldId) const;

    Result<std::unordered_map<std::string, std::vector<AttributeCell>>> getCells(
        const DatabaseObject& database, const optional<string> objectId = nullopt) const;

    Result<std::unordered_map<std::string, std::vector<AttributeCell>>> getAllCells(
        const std::string& workspaceId, std::optional<std::string>& projectId) const;

    Result<AttributeCell> identifyCell(const BaseObject& object, const std::string& databaseId,
                                       const std::string& fieldId);
    Result<unordered_map<string, AttributeCell>> identifyCells(const vector<BaseObject>& objects,
                                                               const std::string& databaseId,
                                                               const std::string& fieldId,
                                                               const std::string& workspaceId);

    Result<void> updateCell(const AttributeCell& cell);
    Result<void> updateCells(const std::vector<AttributeCell>& cells);
    Result<void> updateCellAndTimeline(const std::optional<ProjectObject>& project,
                                       const std::string& databaseType,
                                       const std::string& fieldType, const AttributeCell& cell);

    Result<void> updateCellsAndTimeline(const std::optional<ProjectObject>& project,
                                        const std::string& databaseType,
                                        const std::string& fieldType,
                                        const std::vector<AttributeCell>& cells);

    Result<void> removeCell(const AttributeCell& cell);

    Result<void> copyCells(const DatabaseObject& dstDatabase,
                           const std::vector<AttributeCell>& cells);
    Result<void> moveCells(const DatabaseObject& dstDatabase,
                           const std::vector<AttributeCell>& cells);

    Result<DatabaseObject> copy(
        const DatabaseObject& database, const std::optional<DatabaseObject>& dstDatabase,
        const BaseObject& targetObject, const std::optional<std::string>& projectId,
        std::function<bool(const std::string& objectId, BaseObject& mappedObject)>
            mapObjectCallback,
        bool overwrite = true);

    Result<void> sync(const DatabaseObject& database);

    Result<AttributeField> unmarshalAttributeField(const Object* fieldObject,
                                                   const std::string& workspaceId) const;
    bool mergeAttributeFields(const AttributeField& source, AttributeField& target,
                              std::unordered_map<std::string, std::string>* tagIdMap) const;

    Result<void> copyAttributeCells(const BaseObject& object, const DatabaseObject& database,
                                    const BaseObject& targetParentObject, const string& targetPath,
                                    const std::optional<ProjectObject>& sourceProjectOpt,
                                    const std::optional<ProjectObject>& targetProjectOpt);


private:
    std::shared_ptr<DatabaseApiImpl> _pimpl;
    std::shared_ptr<Api> _api;
};

} // namespace apsync

#endif // ANCHORPOINT_DATABASE_API_H
