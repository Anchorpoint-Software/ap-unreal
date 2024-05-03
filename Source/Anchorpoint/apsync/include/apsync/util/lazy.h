#ifndef LAZY_H
#define LAZY_H

#include <functional>
#include <memory>
#include <mutex>

#define INIT_LAZY(Class, ...) Lazy<Class>([=] { return std::make_shared<Class>(__VA_ARGS__); })

static std::recursive_mutex __lazy__mutex;

template<typename T>
class Lazy
{
public:
    Lazy() = default;
    explicit Lazy(std::function<std::shared_ptr<T>()> initializer) : _initializer(initializer) {}
    T* operator->()
    {
        auto p = get();
        if (p) return p.get();
        return nullptr;
    }

    T const* const operator->() const
    {
        auto p = get();
        if (p) return p.get();
        return nullptr;
    }

    std::shared_ptr<T> get()
    {
        std::scoped_lock lock(__lazy__mutex);
        if (!_initializer) return nullptr;
        if (!_object) _object = _initializer();
        return _object;
    }

    std::shared_ptr<const T> get() const
    {
        std::scoped_lock lock(__lazy__mutex);
        if (!_initializer) return nullptr;
        if (!_object) _object = _initializer();
        return _object;
    }

private:
    std::function<std::shared_ptr<T>()> _initializer;
    mutable std::shared_ptr<T> _object = nullptr;
};
#endif // LAZY_H
