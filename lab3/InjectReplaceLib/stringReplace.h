#pragma once

#include <vector>
#include <Windows.h>
#include <string>

typedef struct
{
    DWORD pid;
    char srcString[20];
    char resString[20];

} loadLibraryInfo;

extern "C" __declspec(dllexport) void __cdecl ReplaceString(loadLibraryInfo* info);
