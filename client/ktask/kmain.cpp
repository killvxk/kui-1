#include "stdafx.h"
#include "resource.h"

HANDLE ghLogger = NULL;
HANDLE ghExited = NULL;
HANDLE ghModule = NULL;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	{
		if (ghModule == NULL)
			ghModule = hModule;

		// …Ë÷√”Ô—‘
		DWORD dwLanguageId = 0;
		DWORD dwRegType = 0, dwSize = sizeof(dwLanguageId);
		if (ERROR_SUCCESS == ::SHGetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet"),
			TEXT("languageid"), &dwRegType, &dwLanguageId, &dwSize) && dwRegType == REG_DWORD)
		{
			AfxSetLangId((LANGID)dwLanguageId);
		}
	}
	break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

DWORD __stdcall AfxGetServiceInfo(LPTSTR SvrName, LPTSTR SvrDescription, LPTSTR guid)
{
	lstrcpy(SvrName, TEXT("knbclient"));
	TCHAR szDesc[MAX_PATH] = {0};
	AfxGetLangStringEx(ghModule, IDS_CLIENT_SERVICE_NAME, szDesc, MAX_PATH);
	lstrcpy(SvrDescription, szDesc);
	lstrcpy(guid, KNBCLIENT_SINGLE_NAME);
	return ERROR_SUCCESS;
}

void ModifyDeviceName(LPCTSTR DevClass, LPCTSTR DevClassName, LPCTSTR DevName)
{
	HKEY hRoot = NULL, hSubRoot = NULL, hDevice = NULL;
	TCHAR szKey[MAX_PATH] = {0};
	_stprintf_s(szKey, _countof(szKey), L"SYSTEM\\CurrentControlSet\\Enum\\%s", DevClass);
	if (ERROR_SUCCESS != RegOpenKey(HKEY_LOCAL_MACHINE, szKey, &hRoot))
		return ;

	for (DWORD Index=0; ; Index++)
	{
		TCHAR szDevId[MAX_PATH] = {0};
		DWORD dwDeviIdSize = sizeof(szDevId);
		if (SHEnumKeyEx(hRoot, Index, szDevId, &dwDeviIdSize) != ERROR_SUCCESS)
			break;

		if (ERROR_SUCCESS != RegOpenKey(hRoot, szDevId, &hSubRoot))
			continue;

		for (DWORD Index2=0; ; Index2++)
		{
			TCHAR szInstance[MAX_PATH] = {0};
			DWORD dwSize = sizeof(szInstance);
			if (SHEnumKeyEx(hSubRoot, Index2, szInstance, &dwSize) != ERROR_SUCCESS)
				break;

			if (ERROR_SUCCESS != RegOpenKey(hSubRoot, szInstance, &hDevice))
				continue;

			TCHAR szDevClass[MAX_PATH] = {0};
			DWORD dwRegType = 0, dwSize2 = sizeof(szDevClass);
			if (ERROR_SUCCESS == SHGetValue(hDevice, NULL, TEXT("Class"), &dwRegType, szDevClass, &dwSize2) && 
				lstrcmpi(szDevClass, DevClassName) == 0)
			{
				if (DevName != NULL)
				{
					dwSize = (lstrlen(DevName)+1) * sizeof(TCHAR);
					SHSetValue(hDevice, NULL, TEXT("FriendlyName"), REG_SZ, DevName, dwSize);
				}
				else
				{
					TCHAR PciDevDesc[MAX_PATH] = {0}, ClassDevDesc[MAX_PATH] = {0}, szDriver[MAX_PATH] = {0};
					dwSize = sizeof(PciDevDesc);
					SHGetValue(hDevice, NULL, TEXT("DeviceDesc"), &dwRegType, PciDevDesc, &dwSize);
					dwSize = sizeof(szDriver);
					SHGetValue(hDevice, NULL, TEXT("Driver"), &dwRegType, szDriver, &dwSize);

					dwSize = sizeof(ClassDevDesc);
					_stprintf_s(szKey, _countof(szKey), TEXT("SYSTEM\\CurrentControlSet\\Control\\Class\\%s"), szDriver);
					SHGetValue(HKEY_LOCAL_MACHINE, szKey, L"DriverDesc", &dwRegType, ClassDevDesc, &dwSize);

					//if (lstrcmpi(ClassDevDesc, PciDevDesc) != 0)
					{
						dwSize = (lstrlen(ClassDevDesc)+1) * sizeof(TCHAR);
						SHSetValue(hDevice, NULL, L"DeviceDesc", REG_SZ, ClassDevDesc, dwSize);		
					}
				}
			}
			RegCloseKey(hDevice);
		}
		RegCloseKey(hSubRoot);
	}
	RegCloseKey(hRoot);
}

DWORD __stdcall AfxSeviceProc(HANDLE hExit)
{
	
	ghExited = hExit;
	ghLogger = AfxCreateLogger(TEXT("ktask"));
	AfxWriteLogger(ghLogger, TEXT("=============================================="));
	AfxWriteLogger(ghLogger, TEXT("service running..."));
	//Sleep(10000);
	//modify device registry.
	/*
	ModifyDeviceName(TEXT("PCI"), TEXT("Display"), NULL);
	ModifyDeviceName(TEXT("PCI"), TEXT("Net"), NULL);
	CString strTmp;
	TCHAR	szTmp[MAX_PATH] = {0};
	strTmp.LoadString(IDS_CAMERA_DEVICE_SUFFIX);
	strTmp = AfxGetLangStringEx(_T("KTASK"), _T("IDS_KTASK_CAMERA_PREFIX"), szTmp, MAX_PATH, strTmp);
	ModifyDeviceName(TEXT("USB"), TEXT("Image"), strTmp);
	*/

	//initialize plug.
	if (!AfxCreateTaskServer())
		goto _do_clean;

	//main loop
	while (WaitForSingleObject(hExit, 3000))
	{
		//loop force close disk modify&check windows.
		//HWND hWnd = FindWindow(TEXT("[class:#32770]"), TEXT("…®√Ë≤¢–ﬁ∏¥"));
		//if (hWnd != NULL)
		//	CloseWindow(hWnd);
	}

_do_clean:
	//clean all plug.
	AfxStopTaskServer();
	AfxWriteLogger(ghLogger, TEXT("service already exit."));
	AfxWriteLogger(ghLogger, TEXT("==============================================\r\n\r\n"));
	AfxCloseLogger(ghLogger);
	return 0;
}