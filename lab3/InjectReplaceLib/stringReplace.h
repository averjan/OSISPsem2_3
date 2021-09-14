#pragma once

#include <vector>
#include <Windows.h>

extern "C" __declspec(dllexport) void __cdecl ReplaceString(DWORD pid, const char* srcString, const char* resString);

