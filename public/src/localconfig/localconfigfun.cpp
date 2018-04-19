#include "localconfigfun.h"

#include <Shlwapi.h>
#pragma comment(lib, "shlwapi")

#include <tchar.h>
#include <mutex>

START_NAMESPACE_KCORE

PGETLOCALSTRINGFUN g_pGetLocalString = NULL;

PGETLOCALLONGFUN g_pGetLocalLong = NULL;

bool g_bInitLocalConfigModule = false;

std::mutex g_lockInitLocalConfigModule;

#ifdef _WIN64
#define LOCAL_CONFIG_MODULE_NAME			TEXT("local64.dat")
#else
#define LOCAL_CONFIG_MODULE_NAME			TEXT("local.dat")
#endif

#define GETLOCALSTRING						"GetLocalString"
#define GETLOCALLONG						"GetLocalLong"

#define LOCALCONFIG_DIRECTORY				TEXT("config\\")

DWORD LoadLocalConfigFun()
{
	std::lock_guard<std::mutex> lock(g_lockInitLocalConfigModule);

	if (g_bInitLocalConfigModule)
		return ERROR_SUCCESS;

	g_bInitLocalConfigModule = true;
	TCHAR szLocalConfigModulePath[MAX_PATH] = { 0 };
	TCHAR szLocalConfigLightningdll[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szLocalConfigModulePath, MAX_PATH);
	if (::PathRemoveFileSpec(szLocalConfigModulePath))
		PathAddBackslash(szLocalConfigModulePath);
	_stprintf_s(szLocalConfigLightningdll, TEXT("%s%s%s"), szLocalConfigModulePath,
		LOCALCONFIG_DIRECTORY, LOCAL_CONFIG_MODULE_NAME);
	HMODULE hModule = ::LoadLibrary(szLocalConfigLightningdll);
	if (hModule == NULL)
		return ::GetLastError();

	g_pGetLocalString = (PGETLOCALSTRINGFUN)::GetProcAddress(hModule, GETLOCALSTRING);
	if (g_pGetLocalString == NULL)
		return ::GetLastError();

	g_pGetLocalLong = (PGETLOCALLONGFUN)::GetProcAddress(hModule, GETLOCALLONG);
	if (g_pGetLocalLong == NULL)
		return ::GetLastError();

	return ERROR_SUCCESS;
}

void UnLoadLocalConfigFun()
{
	std::lock_guard<std::mutex> lock(g_lockInitLocalConfigModule);

	if (g_pGetLocalString != NULL)
		g_pGetLocalString = NULL;
	if (g_pGetLocalLong != NULL)
		g_pGetLocalLong = NULL;
}

LPTSTR __stdcall GetLocalStringFun(LPTSTR lpBuffer, DWORD dwSize, LPCTSTR szUrlKey, LPCTSTR szDefault)
{
	if (lpBuffer == NULL || dwSize <= 0 || szDefault == NULL)
		return lpBuffer;

	if (ERROR_SUCCESS != LoadLocalConfigFun() || g_pGetLocalString == NULL)
	{
		if (dwSize > (DWORD)lstrlen(szDefault))
			lstrcpy(lpBuffer, szDefault);

		return lpBuffer;
	}

	return g_pGetLocalString(lpBuffer, dwSize, szUrlKey, szDefault);
}

long __stdcall GetLocalLongFun(LPCTSTR szUrlKey, long lDefault)
{
	if (ERROR_SUCCESS != LoadLocalConfigFun() || g_pGetLocalLong == NULL)
		return lDefault;

	return g_pGetLocalLong(szUrlKey, lDefault);
}

END_NAMESPACE_KCORE