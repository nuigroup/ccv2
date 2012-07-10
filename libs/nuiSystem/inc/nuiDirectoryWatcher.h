/////////////////////////////////////////////////////////////////////////////
// Author:      Anatoly Lushnikov
// Purpose:     Directory watcher for WIN32 and Posix
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef NUI_DIRECTORYWATCHER
#define NUI_DIRECTORYWATCHER

#include "pasync.h"
#include <vector>

#define NUI_DIRECTORYWATCHER_MAX_CALLBACKS 20
#define NUI_DIRECTORYWATCHER_MAX_CB_REQUESTS 20

enum nuiFilesystemEventType
{
    nuiFileAdded  = 0x00000001,
    nuiFileRemoved  = 0x00000002,
    nuiFileUpdate  = 0x00000004,
    nuiFileError  = 0xFFFFFFFF,
};

class nuiDirectoryWatcher;

typedef void (*nuiFilesystemEventCallback)(char *fileFullName, nuiFilesystemEventType *filesystemEventType);

enum nuiCallbackProcessRequestAction
{
    nuiRegisterCallback = 0x00000001,
    nuiDeregisterCallback = 0x00000002,
};

struct nuiCallbackProcessRequest
{
    nuiFilesystemEventCallback callback;
    nuiCallbackProcessRequestAction action;
};

class nuiDirectoryWatcher : public pt::thread {
public:
    nuiDirectoryWatcher(const char* _watchDir);
    virtual ~nuiDirectoryWatcher();

    void registerCallback(nuiFilesystemEventCallback callback);
    void deregisterCallback(nuiFilesystemEventCallback callback);
    void execute();
    void stop();
    bool relax(int msec);

private:
    const char* watchDir;

    nuiFilesystemEventCallback callbacks[NUI_DIRECTORYWATCHER_MAX_CALLBACKS];

    int callbackCount;

    void raiseevent(char* fileName, nuiFilesystemEventType eventType);
    
    nuiCallbackProcessRequest requests[NUI_DIRECTORYWATCHER_MAX_CB_REQUESTS];
    int requestsCount;
    void updateCallbacks();
    void processRegisterCallback(nuiFilesystemEventCallback callback);
    void processDeregisterCallback(nuiFilesystemEventCallback callback);

    nuiFilesystemEventType getEventType(DWORD action);
};


#endif