#pragma once


#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

#include "System.h"

using namespace gpr460;

class System_Win : public System
{
    _CrtMemState checkPoint;
    HANDLE logFile;
    HANDLE errorFile;
    bool openedLog;
    bool openedError;
public:
    void Initialize() override;

    void PreGameStartup() override;

    void PostGameShutdown() override
    {
        _CrtMemDumpAllObjectsSince(&checkPoint);
    }

    void Shutdown() override
    {
        if (openedLog)
            CloseHandle(logFile);
        if (openedError)
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