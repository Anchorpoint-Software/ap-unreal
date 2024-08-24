#pragma

#include <optional>
#include <type_traits>
#include <variant>

namespace apsync {

template<typename T, typename F, typename V, typename R = std::invoke_result_t<F, T>>
auto visitIf(F&& callable, V&& var)
    -> std::conditional_t<std::is_same_v<R, void>, void, std::optional<R>>
{
    using std::holds_alternative, std::get;

    if constexpr (std::is_same_v<R, void>) {
        if (holds_alternative<T>(var)) callable(get<T>(std::forward<V>(var)));
    } else {
        if (holds_alternative<T>(var)) return callable(get<T>(std::forward<V>(var)));
        return std::nullopt;
    }
}

} // namespace apsync
