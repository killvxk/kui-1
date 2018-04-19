#include "stdafx.h"
#include <psapi.h>
#include <TlHelp32.h>
#include <shlwapi.h>
#include <tchar.h>

#pragma comment(lib, "psapi")
#pragma comment(lib, "shlwapi")

DWORD	GetProcIdByProcName(LPCTSTR lpszProcName);
HMODULE GetModuleFromProcess(DWORD pid, LPCTSTR lpszModuleName);
DWORD	InjectToProcess(DWORD dwProcId, LPCTSTR pszDllPath);
void	UnInjectToProcess(DWORD dwProcId, LPCTSTR pszDllPath);
void	InjectToExplorere();

DWORD GetProcIdByProcName(LPCTSTR lpszProcName)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 ProcEntry = {sizeof(ProcEntry)};
	BOOL bRes = Process32First(hSnap, &ProcEntry);
	DWORD dwPid = 0;
	while (bRes)
	{
		if (lstrcmpi(lpszProcName, ProcEntry.szExeFile) == 0)
		{
			dwPid = ProcEntry.th32ProcessID;
			break;
		}
		bRes = Process32Next(hSnap, &ProcEntry);
	}
	CloseHandle(hSnap);
	return dwPid;
}

HMODULE GetModuleFromProcess(DWORD pid, LPCTSTR lpszModuleName)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (hSnap == NULL)
		return NULL;

	HMODULE hMod = NULL;
	MODULEENTRY32 mod = {sizeof(mod)};
	BOOL bRes = Module32First(hSnap, &mod);
	while (bRes)
	{
		if (_tcsicmp(mod.szModule, lpszModuleName) == 0)
		{
			hMod = mod.hModule;
			break;
		}
		bRes = Module32Next(hSnap, &mod);
	}
	CloseHandle(hSnap);
	return hMod;
}

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) 
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return FALSE; 

	if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid ))
		return FALSE; 

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), 
		(PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL))
	{
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
		return FALSE;

	return TRUE;
}

typedef LONG /*NTSTATUS*/ (WINAPI *LPFUN_NtCreateThreadEx) 
	(
		OUT PHANDLE hThread,
		IN ACCESS_MASK DesiredAccess,
		IN LPVOID ObjectAttributes,
		IN HANDLE ProcessHandle,
		IN LPTHREAD_START_ROUTINE lpStartAddress,
		IN LPVOID lpParameter,
		IN BOOL CreateSuspended, 
		IN SIZE_T StackZeroBits,
		IN SIZE_T SizeOfStackCommit,
		IN SIZE_T SizeOfStackReserve,
		OUT LPVOID lpBytesBuffer
	);
typedef struct _NtCreateThreadExBuffer
{
	SIZE_T	Size;
	SIZE_T	Unknown1;
	SIZE_T	Unknown2;
	PSIZE_T Unknown3;
	SIZE_T	Unknown4;
	SIZE_T	Unknown5;
	SIZE_T	Unknown6;
	PSIZE_T Unknown7;
	SIZE_T	Unknown8;
}NtCreateThreadExBuffer,*PNtCreateThreadExBuffer;

NtCreateThreadExBuffer gntbuffer = {0};
SIZE_T gtemp1 = 0, gtemp2 = 0;

DWORD InjectToProcess(DWORD dwProcId, LPCTSTR pszDllPath)
{
	HANDLE hProcess = NULL;
	LPVOID lpParamter = NULL;
	HANDLE hThread = NULL;
	DWORD dwError = ERROR_SUCCESS;
	try
	{
		HMODULE hMod  = GetModuleFromProcess(dwProcId, PathFindFileName(pszDllPath));
		if (hMod != NULL)
			return ERROR_SUCCESS;

		SetPrivilege(SE_DEBUG_NAME, TRUE);
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);
		if (hProcess == NULL)
		{
			throw GetLastError();
		}

		DWORD dwSize = (lstrlen(pszDllPath) + 1) * sizeof(TCHAR);
		 lpParamter = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
		if (lpParamter == NULL)
		{
			throw GetLastError();
		}

		if (!WriteProcessMemory(hProcess, lpParamter, pszDllPath, dwSize, NULL))
		{
			throw GetLastError();
		}

#ifdef _UNICODE
		LPTHREAD_START_ROUTINE addr = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "LoadLibraryW");
#else
		LPTHREAD_START_ROUTINE addr = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "LoadLibraryA");
#endif

		OSVERSIONINFO osinfo = {sizeof(OSVERSIONINFO)};
		if (!GetVersionEx(&osinfo) || osinfo.dwMajorVersion < 6)	//vista以下系统使用方式
		{
			hThread = CreateRemoteThread(hProcess, NULL, 0, addr, lpParamter, 0, NULL);
		}
		else
		{
			LPFUN_NtCreateThreadEx funNtCreateThreadEx = (LPFUN_NtCreateThreadEx)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtCreateThreadEx");
			if(!funNtCreateThreadEx)
				throw GetLastError();
		
			gntbuffer.Size = sizeof(NtCreateThreadExBuffer);
			gntbuffer.Unknown1 = 0x10003;
			gntbuffer.Unknown2 = sizeof(SIZE_T)*2;
			gntbuffer.Unknown3 = &gtemp1;
			gntbuffer.Unknown4 = 0;
			gntbuffer.Unknown5 = 0x10004;
			gntbuffer.Unknown6 = sizeof(SIZE_T);
			gntbuffer.Unknown7 = &gtemp2;
			gntbuffer.Unknown8 = 0;
			LONG status = funNtCreateThreadEx(&hThread, 0x1FFFFF, NULL, hProcess, (LPTHREAD_START_ROUTINE)addr, lpParamter, FALSE, NULL, NULL, NULL, &gntbuffer);
		}

		if (hThread == NULL)
			throw GetLastError();
		WaitForSingleObject(hThread, INFINITE);
	}
	catch (DWORD nError)
	{ 
		dwError = nError;
	}
	if (hThread != NULL)
		CloseHandle(hThread);
	if (lpParamter != NULL)
		VirtualFreeEx(hProcess, lpParamter, 0, MEM_RELEASE);
	if (hProcess != NULL)
		CloseHandle(hProcess);
	return dwError;
}

void UnInjectToProcess(DWORD dwProcId, LPCTSTR pszDllPath)
{
	HMODULE hMod = GetModuleFromProcess(dwProcId, PathFindFileName(pszDllPath));
	if (hMod == NULL)
		return ;

	SetPrivilege(SE_DEBUG_NAME, TRUE);
	LPTHREAD_START_ROUTINE addr = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "FreeLibrary");
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);
	if (hProcess == NULL)
		return ;

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, addr, (LPVOID)hMod, 0, NULL);
	if (hThread == NULL)
	{
		CloseHandle(hProcess);
		return ;
	}
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
}

void InjectToExplorere()
{
	tstring SvrName;
	DWORD DiskLess = 0, IsAdmin = 0;
	if (ERROR_SUCCESS == ScsiDiskQueryDisklessInfo(&DiskLess, &IsAdmin, NULL) && (DiskLess != 0) && (IsAdmin == 0))
	{
		TCHAR szDllName[MAX_PATH] = {0};
		AfxGetAppPath(szDllName, _countof(szDllName), TEXT("kshut.dll"));
		InjectToProcess(GetProcIdByProcName(TEXT("explorer.exe")), szDllName);
		InjectToProcess(GetProcIdByProcName(TEXT("knbclient.exe")), szDllName);
	}	
}