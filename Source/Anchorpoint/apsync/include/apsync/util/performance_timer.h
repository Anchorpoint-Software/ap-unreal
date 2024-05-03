#ifndef PERFORMANCETIMER_H
#define PERFORMANCETIMER_H

#include <chrono>

#include "apsync/sync_global.h"

#if defined(AP_TARGET_MACOS)
#include <os/log.h>
#include <os/signpost.h>
#endif

namespace apsync {
class SYNC_EXPORT PerformanceTimer
{
public:
    explicit PerformanceTimer(bool autoStart = true);

    void start();
    int getElapsedMilliseconds(bool restart = true);
    int getElapsedSeconds(bool restart = true);
    int getElapsedMicroseconds(bool restart = true);

#if defined(AP_TARGET_MACOS)
    os_log_t log_handle;
    os_signpost_id_t signpost_id = OS_SIGNPOST_ID_NULL;
#endif

private:
    std::chrono::high_resolution_clock::time_point _start;
};

#if defined(AP_TARGET_MACOS)
#define signpostEvent(timer, message)                              \
    timer.signpost_id = os_signpost_id_generate(timer.log_handle); \
    os_signpost_event_emit(timer.log_handle, timer.signpost_id, message);

#define startSignpost(timer, message)                              \
    timer.signpost_id = os_signpost_id_generate(timer.log_handle); \
    os_signpost_interval_begin(timer.log_handle, timer.signpost_id, message);

#define endSignpost(timer, message, restart)                                          \
    if (timer.signpost_id != OS_SIGNPOST_ID_NULL) {                                   \
        os_signpost_interval_end(timer.log_handle, timer.signpost_id, message);       \
                                                                                      \
        if (!restart) {                                                               \
            timer.signpost_id = OS_SIGNPOST_ID_NULL;                                  \
        } else {                                                                      \
            timer.signpost_id = os_signpost_id_generate(timer.log_handle);            \
            os_signpost_interval_begin(timer.log_handle, timer.signpost_id, message); \
        }                                                                             \
    }
#endif

} // namespace apsync

#endif// PERFORMANCETIMER_H
