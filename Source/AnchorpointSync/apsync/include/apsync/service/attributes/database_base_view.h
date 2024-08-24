#ifndef __DATABASE_BASE_VIEW_H__
#define __DATABASE_BASE_VIEW_H__

#include "apsync/models/attributes/attribute_field.h"
#include "apsync/models/attributes/database_view_type.h"
#include "apsync/sync_global.h"

namespace apsync {

class SYNC_EXPORT DatabaseBaseView
{
public:
    DatabaseBaseView() = default;
    DatabaseBaseView(const std::string& workspaceId, const std::string& viewId,
                     const std::string& databaseId);
    virtual ~DatabaseBaseView() = default;

    virtual DatabaseViewType getType() const = 0;
    virtual std::string toJson() const = 0;
    virtual bool fromJson(const std::string& json) = 0;
    virtual std::shared_ptr<DatabaseBaseView> copy(const std::string& workspaceId,
                                                   const std::string& viewId,
                                                   const std::string& databaseId) const = 0;

    const std::string& getId() const;
    const std::string& getDatabaseId() const;
    const std::string& getName() const;
    const std::string& getWorkspaceId() const;

    void setName(const std::string& name);

protected:
    virtual bool supportsAttributeFieldKey(const AttributeFieldKey& key) const = 0;

    std::string _workspaceId;
    std::string _id;
    std::string _databaseId;
    std::string _name = "My Database";
};


} // namespace apsync

#endif // __DATABASE_BASE_VIEW_H__
