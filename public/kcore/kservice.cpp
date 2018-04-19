#include "stdafx.h"
#include <process.h>
#include <Sensapi.h>
#pragma comment(lib, "Sensapi")

#include "kfirewall.h"
#include "CchannelMgr.h"

START_NAMESPACE_KCORE

bool __stdcall AfxIsServiceExist(LPCTSTR lpszSvrName)
{
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hSCM == NULL) 
		return false;

	SC_HANDLE hService = OpenService(hSCM, lpszSvrName, SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS);
	if (hService != NULL) 
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return true;
	}
	CloseServiceHandle(hSCM);
	return false;
}

DWORD __stdcall AfxInstallService(LPCTSTR lpszSvrName, LPCTSTR lpszSvrDicription, LPCTSTR path)
{
	TCHAR szExeName[MAX_PATH] = {0};
	if (path == NULL || lstrlen(path) == 0)
		GetModuleFileName(NULL, szExeName, _countof(szExeName));
	else
		lstrcpy(szExeName, path);
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT|SC_MANAGER_CREATE_SERVICE);
	if (schSCManager == NULL)
		return GetLastError();

	SC_HANDLE schService = CreateService(
		schSCManager,               // SCManager database
		lpszSvrName,				// name of service
		lpszSvrName,				// name to display
		SERVICE_ALL_ACCESS,         // desired access
		SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,  // service type
		SERVICE_AUTO_START,			// start type
		SERVICE_ERROR_NORMAL,       // error control type
		szExeName,					// service's binary
		NULL,                       // no load ordering group
		NULL,                       // no tag identifier
		NULL,						// dependencies
		NULL,                       // LocalSystem account
		NULL);                      // no password

	if (schService == NULL)
	{
		DWORD dwError = GetLastError();
		if (ERROR_SERVICE_EXISTS != dwError)
		{
			CloseServiceHandle(schSCManager);
			return dwError;
		}
		schService = OpenService(schSCManager, lpszSvrName,SC_MANAGER_ALL_ACCESS);
		if (schService != NULL)
		{
			ChangeServiceConfig(schService, 
				SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS, 
				SERVICE_AUTO_START,
				SERVICE_ERROR_NORMAL,
				szExeName,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				lpszSvrName);
		}
	}
	if (lpszSvrDicription != NULL)
	{
		SERVICE_DESCRIPTION descp = {(LPTSTR)lpszSvrDicription};
		ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &descp);
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	return ERROR_SUCCESS;
}

DWORD __stdcall AfxRemoveService(LPCTSTR lpszSvrName)
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;
	DWORD dwError = ERROR_SUCCESS;

	try
	{
		hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
		if (hSCM == NULL)
			throw GetLastError();
		hService = OpenService(hSCM, lpszSvrName, DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);
		if (hService == NULL)
			throw GetLastError();

		SERVICE_STATUS ss = {0};
		ss.dwCurrentState = SERVICE_STOP_PENDING;
		long ntry = 0;
		if (ControlService(hService, SERVICE_CONTROL_STOP, &ss))
		{
			while (ss.dwCurrentState == SERVICE_STOP_PENDING)
			{
				if (!QueryServiceStatus(hService,&ss))
					throw GetLastError();
				if (++ntry > 10)
					throw ERROR_TIMEOUT;
				Sleep(1000);
			}
		}
		if (!DeleteService(hService))
			throw GetLastError();
	}
	catch (DWORD dwErr) { dwError = dwErr;			}
	catch (...)			{ dwError = GetLastError(); }
	if (hService != NULL)
		CloseServiceHandle(hService);

	if (hSCM != NULL)
		CloseServiceHandle(hSCM);
	return dwError;
}

DWORD __stdcall AfxGetServiceState(LPCTSTR lpszSvrName)
{
	DWORD dwStatus = 0;
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hSCM == NULL) 
		return dwStatus;

	SC_HANDLE hService = OpenService(hSCM, lpszSvrName, SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS);
	if (hService != NULL) 
	{
		SERVICE_STATUS	serviceStatus = { 0 };
		if (QueryServiceStatus(hService, &serviceStatus))
		{
			dwStatus = serviceStatus.dwCurrentState;
		}
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return dwStatus;
	}
	CloseServiceHandle(hSCM);
	return dwStatus;
}

DWORD __stdcall AfxStartService(LPCTSTR lpszSvrName)
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;
	DWORD dwError = ERROR_SUCCESS;
	try
	{
		hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
		if (!hSCM)
			throw GetLastError();
		hService = OpenService(hSCM, lpszSvrName, SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_QUERY_STATUS);
		if (!hService)
			throw GetLastError();
		if (!::StartService(hService, 0, NULL))
			throw GetLastError();

		SERVICE_STATUS ss = {0};
		ss.dwCurrentState = SERVICE_START_PENDING;
		long ntry = 0;
		while (ss.dwCurrentState == SERVICE_START_PENDING)
		{
			if (!QueryServiceStatus(hService,&ss))
				throw GetLastError();
			if (++ntry > 30)
				throw ERROR_TIMEOUT;
			Sleep(1000);
		}
	}
	catch (DWORD dwErr) { dwError = dwErr;			}
	catch (...)			{ dwError = GetLastError(); }

	if (hService != NULL)
		CloseServiceHandle(hService);

	if (hSCM != NULL)
		CloseServiceHandle(hSCM);
	return dwError;
}

DWORD __stdcall AfxStopService(LPCTSTR lpszSvrName)
{
	DWORD dwError = ERROR_SUCCESS;
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;
	try 
	{
		hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
		if (hSCM == NULL) 
			throw GetLastError();
		hService = OpenService(hSCM, lpszSvrName, SERVICE_STOP | SERVICE_QUERY_STATUS);
		if (hService == NULL) 
			throw GetLastError();

		SERVICE_STATUS ss = {0};
		ss.dwCurrentState = SERVICE_STOP_PENDING;
		long ntry = 0;
		if(!::ControlService(hService, SERVICE_CONTROL_STOP, &ss))
			throw GetLastError();

		while (ss.dwCurrentState == SERVICE_STOP_PENDING)
		{
			if (!QueryServiceStatus(hService,&ss))
				throw GetLastError();
			if (++ntry > 30)
				throw ERROR_TIMEOUT;
			Sleep(1000);
		}
	}
	catch (DWORD Error)	{ dwError = Error;			}
	catch (...)			{ dwError = GetLastError(); }

	if (hService != NULL)
		CloseServiceHandle(hService);

	if (hSCM != NULL)
		CloseServiceHandle(hSCM);
	return dwError;
}

void AddWindowFireWall()
{
	WinXPSP2FireWall Fw;
	BOOL bOn = FALSE;
	Fw.IsWindowsFirewallOn(bOn);
	if (bOn)
	{
		TCHAR path[MAX_PATH] = {0};
		GetModuleFileName(NULL, path, MAX_PATH);
		LPTSTR szExeName = PathFindFileName(path);
		if (szExeName != NULL)
		{
			Fw.AddApplication(path, _bstr_t(szExeName));
#ifndef _WIN64
			if (lstrcmpi(szExeName, TEXT("knbserver.exe")) == 0)
			{
				PathRemoveFileSpec(path);
				lstrcat(path, TEXT("\\kconsole.exe"));
				Fw.AddApplication(path, _bstr_t(TEXT("kconsole.exe")));

				PathRemoveFileSpec(path);
				lstrcat(path, TEXT("\\lightningservice\\lightning.exe"));
				Fw.AddApplication(path, _bstr_t(TEXT("lightning.exe")));
			}
#else
			if (lstrcmpi(szExeName, TEXT("knbserver64.exe")) == 0)
			{
				PathRemoveFileSpec(path);
				lstrcat(path, TEXT("\\kconsole64.exe"));
				Fw.AddApplication(path, _bstr_t(TEXT("kconsole64.exe")));

				PathRemoveFileSpec(path);
				lstrcat(path, TEXT("\\lightningservice\\lightning.exe"));
				Fw.AddApplication(path, _bstr_t(TEXT("lightning.exe")));
			}
#endif
			if (lstrcmpi(szExeName, TEXT("knbclient.exe")) == 0)
			{
				PathRemoveFileSpec(path);
				lstrcat(path, TEXT("\\knbmenu.exe"));
				Fw.AddApplication(path, _bstr_t(TEXT("knbmenu.exe")));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void __stdcall SetSviStatus(DWORD dwCurrentState);
void __stdcall ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
void __stdcall ServiceHandler(DWORD dwControl);
BOOL __stdcall ConsoleHandler(DWORD dwCtrlType);

static TCHAR					gSvrName[MAX_PATH] = TEXT("sample");
static TCHAR					gSvrDescription[MAX_PATH] = TEXT("sample main service");
static AFXSEVICEPROC			gSvrThreadProc = NULL;
static BOOL						gIsService = FALSE;
static DWORD					gServiceType = 0;
static SERVICE_STATUS			gSvcStatus = {0};
static SERVICE_STATUS_HANDLE	gSvcStatusHandle = NULL;
static HANDLE					ghSvcStopEvent = NULL;
static HANDLE					ghSingleInstance = NULL;
static TCHAR					gszGuid[MAX_PATH]  = {0};

DWORD __stdcall AfxRunAsService(LPCTSTR SvrExePath, int argc, _TCHAR* argv[])
{
	//删除该进程防止端口占用
	AfxKillProcess(_T("werfault.exe"));

	AddWindowFireWall();

	TCHAR szSvrName[MAX_PATH] = {0}, szSvrDescription[MAX_PATH] = {0};
	HMODULE hMod = NULL;
	try
	{
		TCHAR szKey[MAX_PATH] = {0}, szFullDllPath[MAX_PATH] = {0}, szDllPath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szKey, _countof(szKey));
		LPTSTR szExeName = PathFindFileName(szKey);
		LPTSTR szExtName = PathFindExtension(szKey);
		if (szExeName == NULL || szExtName == NULL || szExtName <= szExeName)
			throw 0;
		lstrcpyn(szKey, szExeName, (int)(szExtName - szExeName + 1));
		AfxGetSysString(TEXT("Service"), szKey, szDllPath, _countof(szDllPath));
		if (lstrlen(szDllPath) == 0)
		{
			TCHAR szRootPath[MAX_PATH] = {0};

#ifndef _WIN64
			if (lstrcmpi(szKey, TEXT("knbserver")) == 0)
			{
				_stprintf_s(szFullDllPath, TEXT("%skapp.dll"), AfxGetAppPath(szRootPath));
				gServiceType = 1;
			}
			else if (lstrcmpi(szKey, TEXT("knbupdate")) == 0)
				_stprintf_s(szFullDllPath, TEXT("%skupdate.dll"), AfxGetAppPath(szRootPath));
			else if (lstrcmpi(szKey, TEXT("knbvdisk")) == 0)
				_stprintf_s(szFullDllPath, TEXT("%skvdisk.dll"), AfxGetAppPath(szRootPath));
			else if (lstrcmpi(szKey, TEXT("knbclient")) == 0)
			{
				_stprintf_s(szFullDllPath, TEXT("%sktask.dll"), AfxGetAppPath(szRootPath));
				gServiceType = 2;
			}
#else
			if (lstrcmpi(szKey, TEXT("knbserver64")) == 0)
			{
				_stprintf_s(szFullDllPath, TEXT("%skapp64.dll"), AfxGetAppPath(szRootPath));
				gServiceType = 1;
			}
			else if (lstrcmpi(szKey, TEXT("knbupdate64")) == 0)
				_stprintf_s(szFullDllPath, TEXT("%skupdate64.dll"), AfxGetAppPath(szRootPath));
			else if (lstrcmpi(szKey, TEXT("knbvdisk64")) == 0)
				_stprintf_s(szFullDllPath, TEXT("%skvdisk64.dll"), AfxGetAppPath(szRootPath));
			else if (lstrcmpi(szKey, TEXT("knbclient64")) == 0)
			{
				_stprintf_s(szFullDllPath, TEXT("%sktask64.dll"), AfxGetAppPath(szRootPath));
				gServiceType = 2;
			}
#endif
		}
		else
		{
			if (PathIsRelative(szDllPath))
				AfxGetAppPath(szFullDllPath, MAX_PATH, szDllPath);
			else
				lstrcpy(szFullDllPath, szDllPath);
		}
		hMod = LoadLibrary(szFullDllPath);
		AFXGETSERVICEINFO pSvrInfo = (AFXGETSERVICEINFO)GetProcAddress(hMod, "AfxGetServiceInfo");
		AFXSEVICEPROC     pSvrProc = (AFXSEVICEPROC)GetProcAddress(hMod, "AfxSeviceProc");
		if (pSvrInfo == NULL || pSvrProc == NULL || pSvrInfo(szSvrName, szSvrDescription, gszGuid) != ERROR_SUCCESS)
			throw 0;
		if (lstrlen(szSvrName) == 0 || lstrlen(szSvrDescription) == 0 || lstrlen(gszGuid) == 0)
			throw 0;
		gSvrThreadProc = pSvrProc;
	}
	catch (...) { return ERROR_INVALID_PARAMETER; }
	lstrcpy(gSvrName, szSvrName);
	lstrcpy(gSvrDescription, szSvrDescription);

	//handle command line.
	if (argc > 2 )
	{
		_tprintf_s(TEXT("command line parameter error.\n"));
		return ERROR_SUCCESS;
	}
	else if (argc == 2)
	{
		if (lstrcmpi(argv[1], TEXT("-i")) == 0)
			return AfxInstallService(gSvrName, gSvrDescription, SvrExePath);
		else if (lstrcmpi(argv[1], TEXT("-r")) == 0)
			return AfxRemoveService(gSvrName);
		else
			_tprintf_s(TEXT("command line parameter error.\n"));
		return ERROR_SUCCESS;
	}

	//run as dos command line.
	if (AfxGetParentProcessId() != AfxGetProcIdByName(TEXT("services.exe")))
	{
		gIsService = FALSE;
		setlocale(LC_ALL, "chs");
		ServiceMain(argc, argv);
		return ERROR_SUCCESS;
	}
	else	//run as service.
	{
		gIsService = TRUE;
		SERVICE_TABLE_ENTRY dispatchTable[] =
		{
			{ gSvrName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
			{ NULL, NULL}
		};

		if (!StartServiceCtrlDispatcher(dispatchTable))
			return GetLastError();
	}
	return ERROR_SUCCESS;
}

//set service status. no operation if not service.
void __stdcall SetSviStatus(DWORD dwCurrentState)
{
	static DWORD dwCheckPoint = 1;

	if (!gIsService)
		return ;

	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = NO_ERROR;
	gSvcStatus.dwWaitHint = 30000;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP|SERVICE_ACCEPT_SHUTDOWN;

	if (dwCurrentState == SERVICE_RUNNING || dwCurrentState == SERVICE_STOPPED)
		gSvcStatus.dwCheckPoint = 0;
	else gSvcStatus.dwCheckPoint = dwCheckPoint++;

	SetServiceStatus( gSvcStatusHandle, &gSvcStatus );
}

//service main function.service exit if the function return.
void __stdcall ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	if (NULL == (ghSingleInstance = CreateEvent(NULL, TRUE, FALSE, gszGuid)))
		goto do_exit;

	if (ERROR_ALREADY_EXISTS == GetLastError())
		goto do_exit;

	DWORD dwDelayTimeout = 0;
	if (gIsService)
	{
		gSvcStatusHandle = RegisterServiceCtrlHandler(gSvrName, ServiceHandler);
		gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
		if (schSCManager != NULL)
		{
			SC_HANDLE schService = OpenService(schSCManager, gSvrName, SERVICE_ALL_ACCESS);
			if (schService != NULL)
			{
				SERVICE_FAILURE_ACTIONS action = {0};
				action.dwResetPeriod = 3;
				action.cActions = 3;
				action.lpsaActions = new SC_ACTION[action.cActions];
				for (DWORD idx=0; idx<action.cActions; idx++)
				{
					action.lpsaActions[idx].Type = SC_ACTION_RESTART;
					action.lpsaActions[idx].Delay = 3 * 1000;
				}
				ChangeServiceConfig2(schService, SERVICE_CONFIG_FAILURE_ACTIONS, &action);
				delete []action.lpsaActions;
				CloseServiceHandle(schService);
			}
			CloseServiceHandle(schSCManager);
		}
		dwDelayTimeout = AfxGetSysLong(TEXT("config"), TEXT("delayruntime")) * 1000;
	}

	SetSviStatus(SERVICE_START_PENDING);
	ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ghSvcStopEvent == NULL)
		goto do_exit;
	SetSviStatus(SERVICE_RUNNING);
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);

	if (gIsService)
	{
		if (WAIT_OBJECT_0 == ::WaitForSingleObject(ghSvcStopEvent, dwDelayTimeout))
			goto do_exit;

		if (dwDelayTimeout <= 0)
		{
			// auto check network is alive(1 minute)
			DWORD dwTimeOut = 60 * 1000;
			while (dwTimeOut > 0)
			{
				DWORD dwFlags = NETWORK_ALIVE_LAN;
				if (::IsNetworkAlive(&dwFlags))
					break;

				if (WAIT_OBJECT_0 == ::WaitForSingleObject(ghSvcStopEvent, 5000))
					goto do_exit;

				dwTimeOut -= 5000;
			}		
		}
	}

	//start plug channel
	if(gServiceType == 1 || gServiceType == 2)
		sCchannelMgr.StartchannelMgr();

	//run service entry.
	gSvrThreadProc(ghSvcStopEvent);

	//stop plug channel
	if(gServiceType == 1 || gServiceType == 2)
		sCchannelMgr.StopchannelMgr();

	SetConsoleCtrlHandler(ConsoleHandler, FALSE);
do_exit:
	if (ghSvcStopEvent != NULL)
	{
		CloseHandle(ghSvcStopEvent);
		ghSvcStopEvent = NULL;
	}
	if (ghSingleInstance != NULL)
	{
		CloseHandle(ghSingleInstance);
		ghSingleInstance = NULL;
	}
	SetSviStatus(SERVICE_STOPPED);
}

//only handle service stop.
void __stdcall ServiceHandler(DWORD dwControl)
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_SHUTDOWN:
	case SERVICE_CONTROL_STOP:
		SetEvent(ghSvcStopEvent);
		SetSviStatus(SERVICE_STOP_PENDING);
		break;
	}
}

//console callback.
BOOL __stdcall ConsoleHandler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_BREAK_EVENT:
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		SetEvent(ghSvcStopEvent);
		SetSviStatus(SERVICE_STOP_PENDING);
		return TRUE;
	}
	return FALSE;
}

END_NAMESPACE_KCORE