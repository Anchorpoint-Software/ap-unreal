#ifndef DATABASE_GRID_VIEW_H
#define DATABASE_GRID_VIEW_H

#include "database_linear_view.h"

namespace apsync {

struct DatabaseGridViewEntry: DatabaseLinearViewEntry {
};

class SYNC_EXPORT DatabaseGridView: public DatabaseLinearView
{
public:
    DatabaseGridView(const DatabaseType& type);
    DatabaseGridView(const DatabaseType& type, const std::string& workspaceId,
                     const std::string& viewId, const std::string& databaseId);

    virtual ~DatabaseGridView() = default;

    virtual std::string toJson() const override;
    virtual bool fromJson(const std::string& json) override;

    virtual DatabaseViewType getType() const override;

    virtual std::shared_ptr<DatabaseBaseView> copy(const std::string& workspaceId,
                                                   const std::string& objectId,
                                                   const std::string& databaseId) const override;

    virtual bool supportsAttributeFieldKey(const AttributeFieldKey& key) const override;

private:
    void initDefaultColumns();
};


} // namespace apsync

#endif // DATABASE_GRID_VIEW_H
