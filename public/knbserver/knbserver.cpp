#include "stdafx.h"
#include <Psapi.h>
#include <comutil.h>

#pragma comment(lib, "psapi")
#pragma comment(lib, "comsuppw")

#ifdef _WIN64
#define KNBSERVER		TEXT("knbserver64.exe")
#define KNBUPDATE		TEXT("knbupdate64.exe")
#define KNBVDISK		TEXT("knbvdisk64.exe")
#define KCONSOLE		TEXT("kconsole64.exe")
#else
#define KNBSERVER		TEXT("knbserver.exe")
#define KNBUPDATE		TEXT("knbupdate.exe")
#define KNBVDISK		TEXT("knbvdisk.exe")
#define KCONSOLE		TEXT("kconsole.exe")
#endif // _WIN64


USE_NAMESPACE_KCORE

bool __stdcall	AfxProcessAutoUpdate(TCHAR* pParam)
{
	TCHAR szExe[MAX_PATH] = {0};
	GetModuleBaseName(GetCurrentProcess(), NULL, szExe, _countof(szExe));
	if (lstrcmpi(szExe, TEXT("knbclient.exe")) == 0)
	{
		Sleep(2000);
		AfxStopService(TEXT("knbclient"));
		AfxKillProcess(TEXT("knbclient.exe"));
		bool  bStartMenu = AfxKillProcess(TEXT("knbmenu.exe"));
		Sleep(2000);

		AfxStartService(TEXT("knbclient"));
		if (bStartMenu)
		{
			Sleep(2000);
			AfxRunProcess(TEXT("knbmenu.exe"));
		}
		return true;
	}
	else if (lstrcmpi(szExe, KNBSERVER) == 0)
	{
		bool  bStartConsole = AfxKillProcess(KCONSOLE);

		AfxStopService(TEXT("knbserver"));
		AfxKillProcess(KNBSERVER);

		AfxStopService(TEXT("knbupdate"));
		AfxKillProcess(KNBUPDATE);

		if (lstrcmpi(pParam, AUTOUPDATE_EXCLUDEVDISK) != 0)
		{
			AfxStopService(TEXT("knbvdisk"));
			AfxKillProcess(KNBVDISK);
		}
	
		Sleep(2000);
		
		AfxStartService(TEXT("knbserver"));
		AfxStartService(TEXT("knbupdate"));

		if (lstrcmpi(pParam, AUTOUPDATE_EXCLUDEVDISK) != 0)
			AfxStartService(TEXT("knbvdisk"));

		if (bStartConsole)
		{
			Sleep(2000);
			AfxRunProcess(KCONSOLE);
		}
		return true;
	}

	return false;
}

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	AfxSetExptHandle();

	if (argc == 2 && (lstrcmpi(argv[1], AUTOUPDATE_PARAMETER) == 0||
		lstrcmpi(argv[1], AUTOUPDATE_EXCLUDEVDISK) == 0))
	{
		AfxProcessAutoUpdate(argv[1]);
		return ERROR_SUCCESS;
	}
	else if (argc == 5 && lstrcmpi(argv[1], TEXT("-m")) == 0)
	{
		return ScsiDiskMount(inet_addr(_bstr_t(argv[2])), htons(_ttoi(argv[3])), argv[4][0], TEXT('C'));
	}
	else if (argc == 3 && lstrcmpi(argv[1], TEXT("-u")) == 0)
	{
		return ScsiDiskUnmount(argv[2][0]);
	}

	return AfxRunAsService(NULL, argc, argv);
}