#pragma once

#include <functional>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "result.h"

namespace apsync {

using int64 = long long int;
using uint64 = unsigned long long int;
using EnumId = uint64;

using std::initializer_list;
using std::nullopt;
using std::optional;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::unique_ptr;
using std::unordered_map;
using std::unordered_set;
using std::variant;
using std::vector;

struct Url {
    std::string s;
    bool isLocalFile = false;
};

} // namespace apsync
