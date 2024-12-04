#ifndef DATABASE_LINEAR_VIEW_H
#define DATABASE_LINEAR_VIEW_H

#include <unordered_map>
#include <vector>

#include "apsync/models/attributes/attribute_field.h"
#include "apsync/models/attributes/database_type.h"
#include "apsync/models/attributes/sort_type.h"
#include "apsync/models/filter/base_filter.h"
#include "apsync/sync_global.h"
#include "database_base_view.h"

namespace apsync {
struct SYNC_EXPORT DatabaseLinearViewEntry {
    AttributeBaseFieldIdentifier identifier;
};
using DatabaseLinearViewEntryRef = std::shared_ptr<DatabaseLinearViewEntry>;

struct SYNC_EXPORT DatabaseLinearViewAttributeEntry {
    AttributeBaseFieldRef attribute;
    DatabaseLinearViewEntryRef entry;
};

struct SYNC_EXPORT DatabaseLinearViewSorting {
    AttributeBaseFieldIdentifier identifier;
    SortType sortType;
    std::optional<std::vector<std::string>> customSortingIds;
};

struct SYNC_EXPORT DatabaseLinearViewGrouping {
    AttributeBaseFieldIdentifier identifier;
};

class SYNC_EXPORT DatabaseLinearView: public DatabaseBaseView
{
public:
    DatabaseLinearView(const DatabaseType& type);
    DatabaseLinearView(const DatabaseType& type, const std::string& workspaceId,
                       const std::string& viewId, const std::string& databaseId);
    virtual ~DatabaseLinearView() = default;

    virtual bool addField(const DatabaseLinearViewEntryRef& field,
                          std::optional<int> insertionIndex);
    virtual bool removeField(const DatabaseLinearViewEntryRef& field);
    virtual std::vector<DatabaseLinearViewEntryRef> getFields() const;
    virtual std::vector<DatabaseLinearViewEntryRef>& getFields();

    virtual bool moveField(int from, int to);

    virtual std::vector<DatabaseLinearViewAttributeEntry> mapFields(
        const std::vector<AttributeField>& fields, bool& updateRequired,
        std::vector<std::shared_ptr<AttributeBaseField>>* unusedFields = nullptr);

    virtual void setSorting(const AttributeBaseFieldIdentifier& identifier, SortType type,
                            std::optional<std::vector<std::string>> customSortingIds);
    virtual std::optional<DatabaseLinearViewSorting> getSorting();

    virtual void setGrouping(const AttributeBaseFieldIdentifier& identifier);
    virtual std::optional<DatabaseLinearViewGrouping> getGrouping();

    virtual void setFilter(const BaseFilterRef& filter);
    virtual void clearFilter();
    virtual std::optional<BaseFilterRef> getFilter();

    void setSupportedAttributeFieldKeys(const std::unordered_set<std::string>& keys);

    void setWrapCells(bool wrapCells);
    bool getWrapCells() const;

    void setScaleFactor(float scaleFactor);
    std::optional<float> getScaleFactor() const;

protected:
    std::optional<bool> supportsAttributeFieldKeyInternal(const AttributeFieldKey& key) const;

    std::optional<DatabaseLinearViewSorting> _sorting;
    std::optional<DatabaseLinearViewGrouping> _grouping;
    DatabaseType _dbType;
    std::vector<DatabaseLinearViewEntryRef> _fields;
    std::unordered_map<AttributeBaseFieldIdentifier, DatabaseLinearViewEntryRef> _fieldMap;
    std::optional<BaseFilterRef> _filter;
    bool _wrapCells = false;
    std::optional<float> _scaleFactor;
    std::unordered_set<std::string> _supportedAttributeFieldKeys;
};


} // namespace apsync


#endif // DATABASE_LINEAR_VIEW_H
