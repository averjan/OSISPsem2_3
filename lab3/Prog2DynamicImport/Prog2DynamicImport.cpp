#include <iostream>
#include <Windows.h>


typedef void __cdecl ReplaceFunction(DWORD pid, const char* srcString, const char* resString);
void ReplaceStringDynamic(DWORD pid, const char* srcString, const char* resString);

int main()
{
	DWORD pid = GetCurrentProcessId();

	const char srcString[] = "Hello, world";
	const char resString[] = "Foo";

	ReplaceStringDynamic(pid, srcString, resString);

	std::cout << srcString << "\n";
	std::cin.get();
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