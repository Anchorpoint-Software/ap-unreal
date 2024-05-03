#pragma once

#include <optional>

#include "util/outcome.hpp"

namespace apsync {

namespace out = OUTCOME_V2_NAMESPACE;

template<class R, class S = std::error_code>
using Result = out::result<R, S>;

inline constexpr out::success_type<void> success() noexcept { return out::success(); }

template<typename E = std::error_code>
struct ErrorScope {
    std::optional<E> err;
};

struct DeferScope {
    bool shouldRun = false;

    ~DeferScope() { shouldRun = false; }
};

struct NullDeferScope {
};

template<typename F>
struct DeferGuard {
    DeferGuard(DeferScope* scope, F func) : scope(scope), func(func) {}

    DeferScope* scope;
    F func;

    ~DeferGuard()
    {
        if (scope->shouldRun) { func(); }
    }
};

template<typename F>
DeferGuard<F> makeDeferGuard(DeferScope& scope, F func)
{
    return {&scope, func};
}

template<typename F>
int makeDeferGuard(NullDeferScope& scope, F func)
{
    static_assert(!std::is_same_v<F, F>, "missing defer_scope()");
    return 0;
}

template<typename T>
Result<T>&& operator%(Result<T>&& res, DeferScope& rhs)
{
    if (res.has_error()) { rhs.shouldRun = true; }
    return std::move(res);
}

template<typename T>
Result<T>&& operator%(Result<T>&& res, NullDeferScope&)
{
    return std::move(res);
}

template<typename E>
void operator%(Result<void>&& res, ErrorScope<E>& rhs)
{
    if (res.has_error()) { rhs.err = res.error(); }
}

template<typename T, typename E>
T operator%(Result<T>&& res, ErrorScope<E>& rhs)
{
    if (res.has_value()) {
        return std::move(res.value());
    } else {
        rhs.err = res.error();
        return T();
    }
}

} // namespace apsync

#define AP_TOKEN_PASTE(x, y) x##y
#define AP_TOKEN_CAT(x, y) AP_TOKEN_PASTE(x, y)

#define err_scope(...) ErrorScope<__VA_ARGS__> _ap_err_scope_

#define defer_scope() DeferScope _ap_defer_scope_

#define err_return                                 \
    % _ap_defer_scope_ % _ap_err_scope_;           \
    if (_ap_err_scope_.err) {                      \
        auto err = std::move(*_ap_err_scope_.err); \
        _ap_err_scope_.err.reset();                \
        return err;                                \
    }

#define err_ignore                       \
    % _ap_defer_scope_ % _ap_err_scope_; \
    if (_ap_err_scope_.err) { _ap_err_scope_.err.reset(); }

#define err_continue                     \
    % _ap_defer_scope_ % _ap_err_scope_; \
    if (_ap_err_scope_.err) {            \
        _ap_err_scope_.err.reset();      \
        continue;                        \
    }

#define err_defer(...)                           \
    auto AP_TOKEN_CAT(_ap_deferred_, __LINE__) = \
        makeDeferGuard(_ap_defer_scope_, [&]() -> void { __VA_ARGS__ });

#define err_throw(...)                             \
    % _ap_defer_scope_ % _ap_err_scope_;           \
    if (_ap_err_scope_.err) {                      \
        auto err = std::move(*_ap_err_scope_.err); \
        _ap_err_scope_.err.reset();                \
        throw __VA_ARGS__;                         \
    }

// Global thread-safe err scope.
[[maybe_unused]] static thread_local apsync::ErrorScope<> _ap_err_scope_;

[[maybe_unused]] static apsync::NullDeferScope _ap_defer_scope_;
