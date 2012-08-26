#include "nuiDirectoryWatcher.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

nuiDirectoryWatcher::nuiDirectoryWatcher(const char* _watchDir) : pt::thread(false)
{
    this->watchDir = _watchDir;
    this->callbackCount = 0;
    this->requestsCount = 0;
}

void nuiDirectoryWatcher::registerCallback(nuiFilesystemEventCallback callback)
{
    nuiCallbackProcessRequest request;
    request.action = nuiCallbackProcessRequestAction::nuiRegisterCallback;
    request.callback = callback;
    requests[requestsCount++] = request;
}

void nuiDirectoryWatcher::deregisterCallback(nuiFilesystemEventCallback callback)
{
    nuiCallbackProcessRequest request;
    request.action = nuiCallbackProcessRequestAction::nuiDeregisterCallback;
    request.callback = callback;
    requests[requestsCount++] = request;
}

void nuiDirectoryWatcher::processRegisterCallback(nuiFilesystemEventCallback callback)
{
    callbacks[callbackCount++] = callback;
}

void nuiDirectoryWatcher::processDeregisterCallback(nuiFilesystemEventCallback callback)
{
    for(int i = 0 ; i<callbackCount ; i++)
        if(callbacks[i] == callback)
        {
            for(int j = i ; j<callbackCount-1 ; j++)
                callbacks[j] = callbacks[j+1];
            callbackCount--;
        }
    if((callbackCount == 1) && (callbacks[0] == callback))
    {
        callbacks[0] = 0;
        callbackCount = 0;
    }
}

void nuiDirectoryWatcher::updateCallbacks()
{
    for(int i = 0 ;i<requestsCount ; i++)
        switch(requests[i].action)
        {
            case nuiCallbackProcessRequestAction::nuiRegisterCallback:
                processRegisterCallback(requests[i].callback);
                break;
            case nuiCallbackProcessRequestAction::nuiDeregisterCallback:
                processDeregisterCallback(requests[i].callback);
                break;
        }
    requestsCount = 0;
}

bool nuiDirectoryWatcher::relax(int msec)
{
    return pt::thread::relax(msec);
}

nuiDirectoryWatcher::~nuiDirectoryWatcher()
{
}


void nuiDirectoryWatcher::stop()
{
}

void nuiDirectoryWatcher::raiseevent(char* fileName, nuiFilesystemEventType eventType)
{
    for(int i=0 ; i<callbackCount ; i++)
        (callbacks[i])(fileName, &eventType );
}

void nuiDirectoryWatcher::execute()
{
    HANDLE hDir = CreateFile( 
        this->watchDir,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
        );

    FILE_NOTIFY_INFORMATION buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    DWORD BytesReturned;
    while( ReadDirectoryChangesW(
            hDir,
            &buffer,
            sizeof(buffer),
            TRUE,
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
            &BytesReturned,
            NULL, 
            NULL))
    {
        char* pBuf = (char*)buffer;
        updateCallbacks();
        do
        {
            FILE_NOTIFY_INFORMATION* fileInfo = &(((FILE_NOTIFY_INFORMATION*)pBuf)[0]);

            char* pfileName = new char[MAX_PATH];
            wchar_t* pwfileName = new wchar_t[fileInfo->FileNameLength + 1];
            memset(pfileName, 0, sizeof(pfileName));
            memset(pwfileName, 0, sizeof(pwfileName));
            memcpy(pwfileName, fileInfo->FileName, sizeof(wchar_t) * fileInfo->FileNameLength);

            wcstombs(pfileName, pwfileName, fileInfo->FileNameLength);

            nuiFilesystemEventType eventType = getEventType(fileInfo->Action);

            this->raiseevent(pfileName, eventType);

            pBuf += fileInfo->NextEntryOffset;
        }while( ((FILE_NOTIFY_INFORMATION*)pBuf)->NextEntryOffset );
        memset(buffer, 0, sizeof(buffer));
    }
}

nuiFilesystemEventType nuiDirectoryWatcher::getEventType(DWORD action)
{
    switch(action)
    {
        case 0x00000001:
            return nuiFilesystemEventType::nuiFileAdded;
        case 0x00000002:
            return nuiFilesystemEventType::nuiFileRemoved;
        case 0x00000003:
            return nuiFilesystemEventType::nuiFileUpdate;
        default:
            return nuiFilesystemEventType::nuiFileError;
    }
}