#ifndef BINDINGS_OBJECT_H
#define BINDINGS_OBJECT_H

#include "apsync/api_types.h"
namespace apsync {

struct Target {
    string id;
    string workspaceId;
    optional<string> projectId;
};

extern optional<Target> getTarget(const string& path);
extern Target getOrCreateTarget(const string& path);

template<typename... Ts>
optional<Target> getTarget(const variant<Ts...>& variant)
{
    return std::visit([](auto&& v) { return getTarget(v); }, variant);
}

template<typename... Ts>
Target getOrCreateTarget(const variant<Ts...>& variant)
{
    return std::visit([](auto&& v) { return getOrCreateTarget(v); }, variant);
}
} // namespace apsync

#endif // BINDINGS_OBJECT_H
