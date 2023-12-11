#pragma once



#include <stdlib.h>
//#include <crtdbg.h>
#include <iostream>

#include <SDL.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif



namespace gpr460
{
    typedef std::string string;

    class System
    {
        //_CrtMemState checkPoint;
        //HANDLE logFile;
        //HANDLE errorFile;
        //bool openedLog;
        //bool openedError;
    public:
        static System* Create();

        // This call is already added to main().
        virtual void Initialize() = 0;

        // TODO: Call this before the game loop starts.
        virtual void PreGameStartup() = 0;

        // TODO: Call this after the game loop exits.
        virtual void PostGameShutdown() = 0;

        // TODO: Call this just before or after the
        //      SDL_Destroy*() group of calls.
        virtual void Shutdown() = 0;


        //wchar_t* winString = fromUTF8(msg.c_str(), msg.size(), NULL);
            //MessageBox(NULL, winString, L"Message Caption", MB_OK);
            //free(winString);
        virtual void DisplayMessageBox(const string& msg) = 0;

        //// This is already called in main()

        //LPDWORD loadData;
            //logFile = CreateFileW(L"Log.txt", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
            //WriteFile(logFile, msg.c_str(), sizeof(string), NULL, NULL);
        virtual void WriteToLogFile(const string& msg) = 0;

        //make sure you keep the opening
            //std::wstring temp = std::wstring(msg.begin(), msg.end());
            //errorFile = CreateFileW(L"Errors.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            //WriteFile(errorFile, msg.c_str(), msg.length(), NULL, NULL);
        virtual void LogToErrorFile(const string& msg) = 0;


    };


}