#ifndef FINALLY_H
#define FINALLY_H

#include <utility>

namespace apsync {

// inspired from https://stackoverflow.com/questions/17356258/correctly-implement-finally-block-using-c-lambda
template<typename T>
class Sentry
{
    T o;

public:
    Sentry(T in_o) : o(std::move(in_o)) {}

    Sentry(Sentry&&) = delete;
    Sentry(Sentry const&) = delete;

    ~Sentry() noexcept
    {
        static_assert(noexcept(o()),
                      "Please check that the finally block cannot throw, "
                      "and mark the lambda as noexcept.");
        if (!_dismissed) o();
    }

    void dismiss() { _dismissed = true; };

private:
    bool _dismissed = false;
};

template<typename T>
Sentry<T> Finally(T o)
{
    return {std::move(o)};
}
} // namespace apsync

#endif // FINALLY_H
