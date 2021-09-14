﻿// Prog3Injector.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <windows.h>
#include <string>
#include <TlHelp32.h>
#include <tchar.h>

void InjectDll(DWORD pid, std::string path);
DWORD GetProcessIdByName(std::string procName);
// void LibraryLoadThread(loadLibraryInfo info);

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
    InjectDll(pid, "InjectReplaceLib.dll");
    getchar();
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
                /*
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                // Do stuff..

                CloseHandle(hProcess);
                */
            }
        }
    }
}

void InjectDll(DWORD pid, std::string path)
{
    HANDLE hProc, hThread;
    PCSTR virtPath;

    hProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);
    if (hProc == NULL)
    {
        return;
    }

    int pathParamLength = 1 + path.size();
    int pathParamSize = pathParamLength * sizeof(CHAR);
    virtPath = (PCSTR)VirtualAllocEx(hProc, NULL, pathParamSize, MEM_COMMIT, PAGE_READWRITE);
    if (virtPath == NULL)
    {
        CloseHandle(hProc);
        return;
    }

    if (!WriteProcessMemory(hProc, (LPVOID)virtPath, path.c_str(), pathParamSize, NULL))
    {
        VirtualFreeEx(hProc, (LPVOID)virtPath, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return;
    }

    /*
    loadLibraryInfo* info;
    (*info).pid = pid;
    (*info).srcString = "Hello, world";
    (*info).resString = "dlrow ,olleH";
    */
    PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
        GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryA");
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

    loadLibraryInfo info;
    info.pid = pid;
    strcpy_s(info.srcString, "Hello, world\0");
    strcpy_s(info.resString, "dlrow ,olleH\0");
    int size = sizeof(info);
    printf("%li\n", info.pid);
    //size = sizeof(DWORD);
    loadLibraryInfo* vp2 = (loadLibraryInfo*)VirtualAllocEx(hProc, NULL, size, MEM_COMMIT, PAGE_READWRITE);
    DWORD b;
    WriteProcessMemory(hProc, (LPVOID)vp2, &info, size, &b);
    PTHREAD_START_ROUTINE repFun = (PTHREAD_START_ROUTINE)
        GetProcAddress(LoadLibraryA("InjectReplaceLib.dll"), "ReplaceString");
    hThread = CreateRemoteThread(hProc, NULL, 0, (PTHREAD_START_ROUTINE)repFun, (LPVOID)vp2, 0, NULL);

    WaitForSingleObject(hThread, INFINITE);

    if (virtPath != NULL)
        VirtualFreeEx(hProc, (LPVOID)virtPath, 0, MEM_RELEASE);
    if (hThread != NULL)
        CloseHandle(hThread);
    if (hProc != NULL)
        CloseHandle(hProc);
}


typedef void __cdecl ReplaceFunction(DWORD pid, const char* srcString, const char* resString);
void LibraryLoadThread(loadLibraryInfo info)
{
    HMODULE hDll = LoadLibrary(L"ReplaceLib.dll");
    if (hDll != NULL)
    {
        ReplaceFunction* replaceFunction = (ReplaceFunction*)GetProcAddress(hDll, "ReplaceString");
        if (replaceFunction != NULL)
        {
            replaceFunction(info.pid, info.srcString, info.resString);
        }
        else
        {
            std::cout << GetLastError();
        }

        FreeLibrary(hDll);
    }
}