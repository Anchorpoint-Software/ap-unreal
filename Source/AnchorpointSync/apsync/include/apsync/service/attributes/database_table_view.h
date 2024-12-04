#ifndef ANCHORPOINT_DATABASE_TABLE_VIEW_H
#define ANCHORPOINT_DATABASE_TABLE_VIEW_H

#include "database_linear_view.h"

namespace apsync {

namespace view {
static constexpr const int columnWidthDefault = 128;
[[maybe_unused]] static constexpr const int columnNameWidthDefault = 3 * columnWidthDefault;
[[maybe_unused]] static constexpr const int columnChanneNameWidthDefault = 2 * columnWidthDefault;
[[maybe_unused]] static constexpr const int columnLocationWidthDefault = 2 * columnWidthDefault;
[[maybe_unused]] static constexpr const int columnWidthMin = 32;
} // namespace view

struct SYNC_EXPORT DatabaseTableViewColumn: DatabaseLinearViewEntry {
    int width = view::columnWidthDefault;
};

class SYNC_EXPORT DatabaseTableView: public DatabaseLinearView
{
public:
    DatabaseTableView(const DatabaseType& type);
    DatabaseTableView(const DatabaseType& type, const std::string& workspaceId,
                      const std::string& viewId, const std::string& databaseId);

    virtual std::string toJson() const override;
    virtual bool fromJson(const std::string& json) override;

    virtual ~DatabaseTableView() = default;

    virtual DatabaseViewType getType() const override;

    virtual std::shared_ptr<DatabaseBaseView> copy(const std::string& workspaceId,
                                                   const std::string& objectId,
                                                   const std::string& databaseId) const override;

    virtual bool supportsAttributeFieldKey(const AttributeFieldKey& key) const override;

private:
    static constexpr const int version = 1;
    int _version = 0;
    void initDefaultColumns();
};

} // namespace apsync

#endif // ANCHORPOINT_DATABASE_TABLE_VIEW_H
