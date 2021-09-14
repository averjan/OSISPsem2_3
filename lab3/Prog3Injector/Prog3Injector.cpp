#include <iostream>
#include <windows.h>
#include <string>
#include <TlHelp32.h>
#include <tchar.h>

void InjectDll(DWORD pid, LPCVOID param, DWORD paramSize, const char* libName, const char* functionName);
DWORD GetProcessIdByName(std::string procName);
void LoadLibToThread(DWORD targetProcess, const char* loadingLibName);
void CallReplaceString(DWORD targetProcess, const char* srcString, const char* resString);

typedef struct
{
    DWORD pid;
    char srcString[20];
    char resString[20];

} loadLibraryInfo;

int main()
{
    std::string progName;
    std::cout << "ProgName: ";
    std::cin >> progName;
    DWORD pid = GetProcessIdByName(progName);
    const char* srcString = "Hello, world";
    const char* resString = "Foo";
    LoadLibToThread(pid, "InjectReplaceLib.dll");
    CallReplaceString(pid, srcString, resString);
    std::cin.get();
}

void LoadLibToThread(DWORD targetProcess, const char* loadingLibName)
{
    int paramLength = strlen(loadingLibName) + 1;
    int paramSize = paramLength * sizeof(CHAR);
    InjectDll(targetProcess, loadingLibName, paramSize, "Kernel32.dll", "LoadLibraryA");
}

void CallReplaceString(DWORD targetProcess, const char* srcString, const char* resString)
{
    loadLibraryInfo info;
    info.pid = targetProcess;
    strcpy_s(info.srcString, srcString);
    strcpy_s(info.resString, resString);
    InjectDll(targetProcess, &info, sizeof(info), "InjectReplaceLib.dll", "ReplaceString");
}

DWORD GetProcessIdByName(std::string procName)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            std::wstring wname(procName.begin(), procName.end());
            if (wcscmp(entry.szExeFile, wname.c_str()) == 0)
            {

                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        }
    }
}

void InjectDll(DWORD pid, LPCVOID param, DWORD paramSize, const char* libName, const char* functionName)
{
    HANDLE hProc, hThread;
    PCSTR virtPath;

    hProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);
    if (hProc == NULL)
    {
        return;
    }

    virtPath = (PCSTR)VirtualAllocEx(hProc, NULL, paramSize, MEM_COMMIT, PAGE_READWRITE);
    if (virtPath == NULL)
    {
        CloseHandle(hProc);
        return;
    }

    if (!WriteProcessMemory(hProc, (LPVOID)virtPath, param, paramSize, NULL))
    {
        VirtualFreeEx(hProc, (LPVOID)virtPath, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return;
    }

    PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
        GetProcAddress(LoadLibraryA(libName), functionName);
    if (pfnThreadRtn == NULL)
    {
        VirtualFreeEx(hProc, (LPVOID)virtPath, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return;
    };

    hThread = CreateRemoteThread(hProc, NULL, 0, (PTHREAD_START_ROUTINE)pfnThreadRtn, (LPVOID)virtPath, 0, NULL);
    if (hThread == NULL)
    {
        VirtualFreeEx(hProc, (LPVOID)virtPath, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return;
    }

    WaitForSingleObject(hThread, INFINITE);

    if (virtPath != NULL)
        VirtualFreeEx(hProc, (LPVOID)virtPath, 0, MEM_RELEASE);
    if (hThread != NULL)
        CloseHandle(hThread);
    if (hProc != NULL)
        CloseHandle(hProc);
}