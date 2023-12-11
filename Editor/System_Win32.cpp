#pragma once


#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

#include "System.h"

using namespace gpr460;

wchar_t*
fromUTF8(
    const char* src,
    size_t src_length,  /* = 0 */
    size_t* out_length  /* = NULL */
)
{
    if (!src)
    {
        return NULL;
    }

    if (src_length == 0) { src_length = strlen(src); }
    int length = MultiByteToWideChar(CP_UTF8, 0, src, src_length, 0, 0);
    wchar_t* output_buffer = (wchar_t*)malloc((length + 1) * sizeof(wchar_t));
    if (output_buffer) {
        MultiByteToWideChar(CP_UTF8, 0, src, src_length, output_buffer, length);
        output_buffer[length] = L'\0';
    }
    if (out_length) { *out_length = length; }
    return output_buffer;
}

class System_Win : public System
{
    _CrtMemState checkPoint;
    HANDLE logFile;
    HANDLE errorFile;
    bool openedLog;
    bool openedError;
public:
    void Initialize() override
    {
        openedLog = false;
        openedError = false;
        //Do the crtsetreportmode
        if (AllocConsole() == false)
        {
            std::cout << "Oh no!\n";

        }
        else
        {
            freopen("CONOUT$", "w", stdout);
            std::cout << "Hi there!\n";
        }
    }
    
    void PreGameStartup() override
    {
        _CrtMemCheckpoint(&checkPoint);
    }

    void PostGameShutdown() override
    {
        _CrtMemDumpAllObjectsSince(&checkPoint);
    }

    void Shutdown() override
    {
        if(openedLog)
            CloseHandle(logFile);
        if(openedError)
            CloseHandle(errorFile);
    }

    void DisplayMessageBox(const string& msg) override
    {
        wchar_t* winString = fromUTF8(msg.c_str(), msg.size(), NULL);
        MessageBox(NULL, winString, L"Message Caption", MB_OK);
        free(winString);
    }

    void WriteToLogFile(const string& msg) override
    {
        if (openedLog == false)
        {
            logFile = CreateFileW(L"Log.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            openedLog = true;
        }
        WriteFile(logFile, msg.c_str(), msg.length(), NULL, NULL);
    }

    void LogToErrorFile(const string& msg) override
    {
        if (openedError == false)
        {
            errorFile = CreateFileW(L"Errors.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            openedError = true;
        }
        WriteFile(errorFile, msg.c_str(), msg.length(), NULL, NULL);
    }
};

System* System::Create()
{
    return (new System_Win);
}