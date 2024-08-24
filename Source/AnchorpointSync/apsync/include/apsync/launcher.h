#pragma once

#include "apsync/error/error.h"
#include "apsync/sync_global.h"

namespace apsync {

class SYNC_EXPORT DaemonLauncher
{
public:
    static Result<void> start();
    static Result<void> start(const string& daemonPath);

    static string getDefaultDaemonPath();

    static string getDaemonPath();
    static string getDaemonAddress();

    static void setDaemonPath(const string& path);
    static void setDaemonAddress(const string& address);

    static bool isDaemonRunning();

    template<typename F>
    static auto runWithRetry(F func) -> std::invoke_result_t<F>
    {
        auto result = func();
        if (!result && result.error() == SyncError::DaemonUnavailable) {
            auto retryResult = start();
            if (!retryResult) { return retryResult.error(); }
            result = func();
        }
        return result;
    }
};

class SYNC_EXPORT AnchorpointLauncher
{
public:
    static Result<void> start();
    static Result<void> start(const string& anchorpointPath, bool minimized);

    static string getDefaultAnchorpointPath();
};

} // namespace apsync
