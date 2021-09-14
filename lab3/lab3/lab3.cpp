#include <iostream>
#include <Windows.h>

extern "C" __declspec(dllimport) void __cdecl ReplaceString(
	DWORD pid,
	const char* srcString,
	const char* resString);

int main()
{
    DWORD pid = GetCurrentProcessId();

    const char srcString[] = "Hello, world";
    const char resString[] = "dlrow ,olleH";

	ReplaceString(pid, srcString, resString);

	std::cout << srcString << "\n";
	std::cin.get();
}