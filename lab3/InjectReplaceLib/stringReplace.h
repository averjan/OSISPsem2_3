#pragma once

#include <vector>
#include <Windows.h>
#include <string>

// extern "C" __declspec(dllexport) void __cdecl ReplaceString(DWORD pid, const char* srcString, const char* resString);
typedef struct
{
    DWORD pid;
    char srcString[20];
    char resString[20];

} loadLibraryInfo;

extern "C" __declspec(dllexport) void __cdecl ReplaceString(loadLibraryInfo* info);
