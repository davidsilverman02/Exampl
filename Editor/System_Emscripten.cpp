#pragma once
#include <SDL2/SDL.h>
#include <emscripten.h>


#include "System.h"

using namespace gpr460;

class System_Em : public System
{
    
public:
    void Initialize() override
    {
        EM_ASM
         (
             alert('hello');
         ); 
    }

    void PreGameStartup() override
    {
        
    }

    void PostGameShutdown() override
    {
        
    }

    void Shutdown() override
    {
        
    }

    void DisplayMessageBox(const string& msg) override
    {
        EM_ASM
        (
            alert(msg);
        );
    }

    void WriteToLogFile(const string& msg) override
    {
        EM_ASM
        (
            console.log(msg);
        );
    }

    void LogToErrorFile(const string& msg) override
    {
        EM_ASM
        (
            console.log(msg);
        );
    }
};

System* System::Create()
{
    return (new System_Em);
}