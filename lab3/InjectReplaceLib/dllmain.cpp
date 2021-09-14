// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "stringReplace.h"

void ConsoleLoad();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
    }
    case DLL_THREAD_ATTACH:
        // ConsoleLoad();
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


void ConsoleLoad()
{
    const char* srcString = "Hello, world";
    const char* resString = "dlrow ,olleH";

    DWORD pid = GetCurrentProcessId();
    printf("%li", pid);
    //ReplaceString(pid, srcString, resString);
}

