#pragma once

#include <deque>
#include <type_traits>

#include "apsync/sync_global.h"
#include "apsync/util/filesystem.hpp"

namespace apsync {

namespace fs = ghc::filesystem;

class SYNC_EXPORT FileUtils
{
public:
    static void makeHidden(const std::string& path);

    static std::error_code createDirectory(const std::string& path);
    static uint64 getFileModificationDate(const fs::path& path, bool milliseconds = false);
    static string getAllowedFilename(const string& filename);

    static bool isVirtualFile(const string& filePath);

    static bool isHidden(const string& path);
    static bool isDotfile(const string& path);

    inline static bool isHiddenOrDotfile(const string& path)
    {
        return isDotfile(path) || isHidden(path);
    }

    static int64_t getLastWriteTime(const fs::directory_entry& entry);
    static int64_t getLastWriteTime(const fs::path& path);

#ifdef AP_TARGET_WINDOWS
    static constexpr char notAllowedChars[] = "\\/:*?\"<>|";
#else
    static constexpr char notAllowedChars[] = ":/\\";
#endif
};

enum class WalkDir { next, stop };

class SYNC_EXPORT WalkDirContext
{
public:
    void addNext(fs::path p) { _next.push_back({_depth + 1, std::move(p)}); }

    const fs::path& path() const { return _path; }

    int64 depth() const { return _depth; }

    fs::directory_iterator& entries() { return _it; }

private:
    explicit WalkDirContext(fs::path p) : _next{{0, std::move(p)}} {}

    std::deque<std::pair<int64, fs::path>> _next;

    fs::directory_iterator _it;
    fs::path _path;
    int64 _depth = 0;

    template<typename F>
    friend Result<void> walkDir(fs::path path, F&& func);
};

template<typename F>
Result<void> walkDir(fs::path path, F&& func)
{
    // Called with f(ctx, entry).
    // f controls recursion via ctx.
    WalkDirContext ctx(std::move(path));

    while (!ctx._next.empty()) {
        auto [d, p] = ctx._next.front();
        ctx._next.pop_front();

        ctx._it = fs::directory_iterator(p);
        ctx._path = std::move(p);
        ctx._depth = d;

        WalkDir c = func(ctx) err_return;
        if (c == WalkDir::stop) break;
    }

    return success();
}

} // namespace apsync
