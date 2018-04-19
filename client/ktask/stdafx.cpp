// stdafx.cpp : 只包括标准包含文件的源文件
// ktask.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include <WinNT.h>

typedef DWORD NTSTATUS;
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) // ntsubauth

typedef NTSTATUS(WINAPI * PFNRTLGETVERSION)(_Out_ PRTL_OSVERSIONINFOW lpVersionInformation);
bool AfxGetOSVerEx(OS_VER& os, RTL_OSVERSIONINFOEXW& osinfo)
{
	HMODULE hMod = GetModuleHandleA("ntdll.dll");
	if (!hMod)
		return FALSE;

	PFNRTLGETVERSION pfnRtlGetVersion = (PFNRTLGETVERSION)GetProcAddress(hMod, "RtlGetVersion");
	if (!pfnRtlGetVersion)
		return FALSE;

	memset(&osinfo, 0, sizeof(RTL_OSVERSIONINFOEXW));
	osinfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
	NTSTATUS status = pfnRtlGetVersion((PRTL_OSVERSIONINFOW)&osinfo);
	if (status != STATUS_SUCCESS)
		return FALSE;

	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		switch(osinfo.dwMajorVersion) {
		case 3:
			os = OS_WIN98;
			break;
		case 4:
			os = OS_WINME;
			break;
		default:
			os = OS_WIN95;
			break;
		}
	}	
	else if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		switch(osinfo.dwMajorVersion) {
		case 3:
			os = OS_NT351; //NT3.51
			break;
		case 4:
			os = OS_NT4; //NT4.0
			break;
		case 5:
			if (osinfo.dwMinorVersion == 0)
				os = OS_WIN2000; //Windows 2000
			else if (osinfo.dwMinorVersion == 1)
				os = OS_WINXP; //Windows XP
			//else if (osinfo.dwMinorVersion > 1)
			//  nResult = OS_WINXP_Higher //±?Windows XP ?ü??
			break;
		case 6:
			if (osinfo.dwMinorVersion == 0)
				os = OS_VISTA; 
			else if (osinfo.dwMinorVersion == 1)
				os = OS_WIN7; 
			else if (osinfo.dwMinorVersion == 2)
				os = OS_WIN8; 
			break;
		case 10:
			os = OS_WIN10;
			break;
		}

		if(osinfo.dwMajorVersion > 10)
			os = OS_NEWEST;
	}

	return true;
}
OS_VER AfxGetOSVer()
{
	OS_VER			nResult = OS_UNKNOWN;
	OSVERSIONINFO	osinfo;

	osinfo.dwOSVersionInfoSize = sizeof(osinfo);
	GetVersionEx(&osinfo);

	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		switch(osinfo.dwMajorVersion) {
		case 3:
			nResult = OS_WIN98;
			break;
		case 4:
			nResult = OS_WINME;
			break;
		default:
			nResult = OS_WIN95;
			break;
		}
	}	
	else if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		switch(osinfo.dwMajorVersion) {
		case 3:
			nResult = OS_NT351; //NT3.51
			break;
		case 4:
			nResult = OS_NT4; //NT4.0
			break;
		case 5:
			if (osinfo.dwMinorVersion == 0)
				nResult = OS_WIN2000; //Windows 2000
			else if (osinfo.dwMinorVersion == 1)
				nResult = OS_WINXP; //Windows XP
			//else if (osinfo.dwMinorVersion > 1)
			//  nResult = OS_WINXP_Higher //±?Windows XP ?ü??
			break;
		case 6:
			if (osinfo.dwMinorVersion == 0)
				nResult = OS_VISTA; 
			else if (osinfo.dwMinorVersion == 1)
				nResult = OS_WIN7; 
			else if (osinfo.dwMinorVersion == 2)
				nResult = OS_WIN8; 
			break;
		}

		if(osinfo.dwMajorVersion > 6)
			nResult = OS_NEWEST;
	}

	return nResult;
}