#ifndef INCLUDE_ERROR_ERROR
#define INCLUDE_ERROR_ERROR

#include <system_error>

#include "apsync/sync_global.h"

namespace apsync {
enum class SyncError {
    OK = 0,
    Cancelled,
    Unknown,
    InvalidArgument,
    DeadlineExceeded,
    NotFound,
    AlreadyExists,
    PermissionDenied,
    ResourceExhausted,
    Aborted,
    OutOfRange,
    Unimplemented,
    Internal,
    BackendUnavailable,
    DaemonUnavailable,
    AnchorpointUnavailable,
    DataLoss,
    Unauthenticated,
    InvalidId,
    InvalidProps,
    InvalidContent,
    InvalidObject,
    InvalidDocument,
    InvalidWorkspace,
    FieldNotFound,
    Conflict,
    SettingsError,
    ClientApiOutdated,
    FreeAttrLimitExceeded,
    FreeProjLimitExceeded,
    AuthTokenNotSet,
    Retry,
    ExportError,
    ImportError,
    ReusedContextError,
    SysAdminNotAllowed,
    OfflineNotSupported
};

SYNC_EXPORT std::error_code make_error_code(SyncError);

} // namespace apsync

namespace std {
template<>
struct is_error_code_enum<apsync::SyncError>: true_type {
};
} // namespace std

#endif /* INCLUDE_ERROR_ERROR */
