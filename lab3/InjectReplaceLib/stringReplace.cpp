#include "pch.h"
#include "stringReplace.h"


extern "C" __declspec(dllexport) void __cdecl ReplaceString(loadLibraryInfo* info)
{
	printf("%li\n", info->pid);
	DWORD pid = info->pid;
	const char* srcString = info->srcString;
	const char* resString = info->resString;
	HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);

	if (hProcess)
	{
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);

		MEMORY_BASIC_INFORMATION memInfo;
		std::vector<char> slice;
		char* p = 0;

		while (p < systemInfo.lpMaximumApplicationAddress)
		{
			if (VirtualQueryEx(hProcess, p, &memInfo, sizeof(memInfo)) == sizeof(memInfo))
			{
				if (memInfo.State == MEM_COMMIT && memInfo.AllocationProtect == PAGE_READWRITE)
				{
					p = (char*)memInfo.BaseAddress;
					slice.resize(memInfo.RegionSize);
					SIZE_T bytes;
					try
					{
						if (ReadProcessMemory(hProcess, p, &slice[0], memInfo.RegionSize, &bytes))
						{
							for (size_t i = 0; i < (bytes - strlen(srcString)); i++)
							{

								if (memcmp(srcString, &slice[i], strlen(srcString)) == 0)
								{
									char* ch = (char*)p + i;
									for (int j = 0; j < strlen(resString); j++)
									{
										ch[j] = resString[j];
									}

									ch[strlen(resString)] = 0;
								}
							}
						}
					}
					catch (std::bad_alloc& e)
					{
						printf("Error\n");
					}
				}
			}

			p += memInfo.RegionSize;
		}
	}
}