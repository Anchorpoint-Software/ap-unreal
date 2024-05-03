#ifndef STREAM_UPDATE_MODEL_H
#define STREAM_UPDATE_MODEL_H

#include <functional>
#include <memory>
#include <unordered_map>

#include "apsync/models/event/event_model.h"
#include "apsync/models/workspace/workspace_change.h"
#include "apsync/sync_global.h"

namespace apsync {
/*
struct SYNC_EXPORT StreamUpdateModel {
    string workspaceId;
    string id;
    int64 revision;
    bool deleted;
};

struct SYNC_EXPORT StreamObjectUpdateModel: public StreamUpdateModel {
    string kind;
    string resource;
    optional<string> projectId;
};

struct SYNC_EXPORT StreamDocumentUpdateModel: public StreamUpdateModel {
    string repo;
    string objectId;
};

struct SYNC_EXPORT StreamEventUpdateModel {
    string workspaceId;
    Event event;
};

using StreamUpdateCallback =
    std::function<void(const string& sourceTag,
                       vector<StreamObjectUpdateModel> objects,
                       vector<StreamDocumentUpdateModel> documents,
                       vector<StreamEventUpdateModel> events,
                       vector<WorkspaceChangeBaseRef> workspaceChanges)>;
*/
} // namespace apsync

#endif // STREAM_UPDATE_MODEL_H
