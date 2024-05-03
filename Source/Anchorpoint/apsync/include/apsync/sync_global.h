#ifndef INCLUDE_SYNC_GLOBAL
#define INCLUDE_SYNC_GLOBAL

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) \
    || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define AP_TARGET_WINDOWS
#elif (__APPLE__)
#define AP_TARGET_MACOS
#endif
#if defined(AP_TARGET_WINDOWS)
#define SYNC_DECL_EXPORT __declspec(dllexport)
#define SYNC_DECL_IMPORT __declspec(dllimport)
#elif defined(AP_TARGET_MACOS)
#define SYNC_DECL_EXPORT __attribute__((visibility("default")))
#define SYNC_DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(SYNC_LIBRARY)
#define SYNC_EXPORT SYNC_DECL_EXPORT
#else
#define SYNC_EXPORT SYNC_DECL_IMPORT
#endif

#include "api_types.h"

#endif /* INCLUDE_SYNC_GLOBAL */
