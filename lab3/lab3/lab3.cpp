#include <iostream>
#include <Windows.h>

__declspec(dllimport) void __cdecl ReplaceString(
	DWORD pid,
	const char* srcString,
	const char* resString);

typedef void __cdecl ReplaceFunction(DWORD pid, const char* srcString, const char* resString);
void ReplaceStringDynamic(DWORD pid, const char* srcString, const char* resString);
void DllInjection(DWORD pid, const char* srcString, const char* resString);

int main()
{
    DWORD pid = GetCurrentProcessId();

    const char src_str[] = "Hello, world";
    const char res_str[] = "dlrow ,olleH";

	// ReplaceString(pid, src_str, res_str);
	// ReplaceStringDynamic(pid, src_str, res_str);
	printf("%li\n", pid);
	getchar();
	printf("%s", src_str);
	getchar();
}

void ReplaceStringDynamic(DWORD pid, const char* srcString, const char* resString)
{
	HMODULE hDll = LoadLibrary(L"ReplaceLib.dll");
	if (hDll != NULL)
	{
		ReplaceFunction* replaceFunction = (ReplaceFunction*)GetProcAddress(hDll, "ReplaceString");
		if (replaceFunction != NULL)
		{
			replaceFunction(pid, srcString, resString);
		}
		else
		{
			std::cout << GetLastError();
		}

		FreeLibrary(hDll);
	}
}