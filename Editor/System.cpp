//#include "System.h"
//using namespace gpr460;
//
//// Add your System.h include file here
//// Use windows instead of console
//// fromUTF8 taken from https://gist.github.com/xebecnan/6d070c93fb69f40c3673.
//wchar_t*
//fromUTF8(
//    const char* src,
//    size_t src_length,  /* = 0 */
//    size_t* out_length  /* = NULL */
//)
//{
//    if (!src)
//    {
//        return NULL;
//    }
//
//    if (src_length == 0) { src_length = strlen(src); }
//    int length = MultiByteToWideChar(CP_UTF8, 0, src, src_length, 0, 0);
//    wchar_t* output_buffer = (wchar_t*)malloc((length + 1) * sizeof(wchar_t));
//    if (output_buffer) {
//        MultiByteToWideChar(CP_UTF8, 0, src, src_length, output_buffer, length);
//        output_buffer[length] = L'\0';
//    }
//    if (out_length) { *out_length = length; }
//    return output_buffer;
//}
//
//void System::Initialize()
//{
//    openedLog = false;
//    openedError = false;
//    //Do the crtsetreportmode
//    if (AllocConsole() == false)
//    {
//        std::cout << "Oh no!\n";
//
//    }
//    else
//    {
//        freopen("CONOUT$", "w", stdout);
//        std::cout << "Hi there!\n";
//    }
//}
//
//void System::PreGameStartup() 
//{
//    _CrtMemCheckpoint(&checkPoint);
//}
//
//void System::PostGameShutdown()
//{
//    _CrtMemDumpAllObjectsSince(&checkPoint);
//}
//
//void System::Shutdown()
//{
//    if(openedLog)
//        CloseHandle(logFile);
//    if(openedError)
//        CloseHandle(errorFile);
//}
//
//void System::DisplayMessageBox(const string& msg)
//{
//    wchar_t* winString = fromUTF8(msg.c_str(), msg.size(), NULL);
//    MessageBox(NULL, winString, L"Message Caption", MB_OK);
//    free(winString);
//}
//
//void System::WriteToLogFile(const string& msg)
//{
//    if (openedLog == false)
//    {
//        logFile = CreateFileW(L"Log.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//        openedLog = true;
//    }
//    WriteFile(logFile, msg.c_str(), msg.length(), NULL, NULL);
//}
//
//void System::LogToErrorFile(const string& msg)
//{
//    if (openedError == false)
//    {
//        errorFile = CreateFileW(L"Errors.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//        openedError = true;
//    }
//    WriteFile(errorFile, msg.c_str(), msg.length(), NULL, NULL);
//}