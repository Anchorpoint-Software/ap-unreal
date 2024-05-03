#ifndef BINDINGS_H
#define BINDINGS_H

#include <optional>
#include <string>

#include "apsync/models/project_object.h"
#include "apsync/service/settings/settings_service.h"
#include "apsync/service/settings/shared_settings_service.h"

namespace pybind11 {
class object;
class module_;
} // namespace pybind11

namespace python {
class TaskApi;
class AttributesApi;
} // namespace python

namespace apsync {

#define THROW_IF_ERROR(r) \
    if (r.has_error()) throw SyncException(r.error().message());

#define WRAPRESULT(func) \
    auto r = func;       \
    THROW_IF_ERROR(r)    \
    return r.value();

class SYNC_EXPORT BaseSettingsPy
{
public:
    pybind11::object get(const std::string& key, const pybind11::object& defaultValue) const;
    void set(const std::string& key, const pybind11::object& object);
    void remove(const std::string& key);
    void clear();
    void store();
    bool contains(const std::string& key) const;
    virtual apsync::BaseSettingsService& getSettings() = 0;
    virtual const apsync::BaseSettingsService& getSettings() const = 0;

protected:
    bool _isValid;
};

class SYNC_EXPORT SettingsPy: public BaseSettingsPy
{
public:
    SettingsPy(const std::optional<std::string>& name, const std::optional<std::string>& identifier,
               const std::optional<std::string>& location, bool user);
    apsync::BaseSettingsService& getSettings() { return _settings; }
    const apsync::BaseSettingsService& getSettings() const { return _settings; }

private:
    apsync::SettingsService _settings;
};

class SYNC_EXPORT SharedSettingsPy: public BaseSettingsPy
{
public:
    SharedSettingsPy(const std::string& workspaceId, const std::string& identifier);
    SharedSettingsPy(const std::string& projectId, const std::string& workspaceId,
                     const std::string& identifier);
    apsync::BaseSettingsService& getSettings() { return _settings; };
    const apsync::BaseSettingsService& getSettings() const { return _settings; }

private:
    apsync::SharedSettingsService _settings;
};


class PyApi;
class SYNC_EXPORT PyPublicApi
{
public:
    explicit PyPublicApi(const std::shared_ptr<PyApi>& api);

    void setWorkspaceId(const string& workspaceId);
    string getWorkspaceId() const;

    void setProject(const optional<apsync::ProjectObject>& project);
    optional<apsync::ProjectObject> getProject() const;

    shared_ptr<python::TaskApi> tasksApi;
    shared_ptr<python::AttributesApi> attributesApi;

private:
    optional<string> _workspaceId;
    optional<apsync::ProjectObject> _project;
};

class SyncException: public std::exception
{
public:
    explicit SyncException(const std::string& m) : _message{m} {}
    const char* what() const noexcept override { return _message.c_str(); }

private:
    std::string _message = "";
};

extern void bindApi(pybind11::module_& m);
extern void bindProject(pybind11::module_& m);
extern void bindDatabase(pybind11::module_& m);
//extern void bindEvent(pybind11::module_& m);
extern void bindAttachment(pybind11::module_& m);
extern void bindCopy(pybind11::module_& m);
extern void bindVersioning(pybind11::module_& m);
extern void bindSettings(pybind11::module_& m);
extern void bindTimeline(pybind11::module_& m);
//extern void bindTasks(pybind11::module_& m);
extern void bindExchange(pybind11::module_& m);
extern void bindUtility(pybind11::module_& m);
extern void bindWorkspace(pybind11::module_& m);
extern void bindIPC(pybind11::module_& m);
extern void bindObject(pybind11::module_& m);
extern void bindTasks(pybind11::module_& m);
extern void bindAttributes(pybind11::module_& m);
extern void bindLocks(pybind11::module_& m);
} // namespace apsync

#endif // BINDINGS_H
