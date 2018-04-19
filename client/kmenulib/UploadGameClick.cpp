
#include "stdafx.h"
#include <vector>
#include <string>
#include "UploadGameClick.h"
#include "winhttp\win_http.hpp"
#include "json\json.h"
#include "aes\AES.h"
#include "localconfig/localconfigfun.h"

using namespace std;

#define SOFTWARE_KEY				TEXT("SOFTWARE\\{3798BE84-4E13-4b81-B8CF-5063730FF905}")
#define HTTP_TIME_OUT				3					// HTTP超时秒为单位
#define WAIT_TIME					1000
#define OBJECT_CLIENT					2			//客户端服务


#define SystemTimeInformation 3  

typedef struct
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG uCurrentTimeZoneId;
	DWORD dwReserved;
} SYSTEM_TIME_INFORMATION;

typedef long(__stdcall *fnNtQuerySystemInformation)(\
	IN  UINT SystemInformationClass, \
	OUT PVOID SystemInformation, \
	IN  ULONG SystemInformationLength, \
	OUT PULONG ReturnLength OPTIONAL);

CUpLoadGameClick::CUpLoadGameClick()
{
	m_hExitEvent = NULL;
	m_hThread = NULL;
	m_appKey = "210_0";
	m_bInitHw = false;
	m_pHwmonitor_Init = NULL;
	m_pHwmonitor_UnInit = NULL;
	m_pHw_GetMainBoardInfo = NULL;
	m_sck = INVALID_SOCKET;

	fnNtQuerySystemInformation NtQuerySystemInformation = NULL;

	NtQuerySystemInformation = (fnNtQuerySystemInformation)\
		GetProcAddress(LoadLibrary(L"ntdll.dll"), \
		"NtQuerySystemInformation");

	if (NtQuerySystemInformation != NULL)
	{
		LONG status;
		SYSTEM_TIME_INFORMATION sti;

		status = NtQuerySystemInformation(SystemTimeInformation, \
			&sti, sizeof(sti), 0);

		FILETIME ft;

		memcpy(&ft, &sti.liKeBootTime, sizeof(ft));
		FileTimeToLocalFileTime(&ft, &ft);
		
		INT64 timeStamp = ((INT64)ft.dwHighDateTime << 32) + ft.dwLowDateTime;
		timeStamp = (timeStamp - 116444736000000000) / 10000000;

		char szBootTime[16] = { 0 };
		_ltoa_s((long)timeStamp, szBootTime, 16, 10);

		m_bootTime = szBootTime;
	}


}

CUpLoadGameClick::~CUpLoadGameClick()
{
	if(m_sck != INVALID_SOCKET)
		AfxSocketClose(m_sck);
}
void CUpLoadGameClick::addItem(vector<ClickInfo>& clickVect)
{
	ObjectLock<CLock>	guard(m_lock);
	m_click_info.insert(m_click_info.begin(),clickVect.begin(),clickVect.end());
}
void CUpLoadGameClick::addItem(std::string type, std::string pos, std::string dstid, std::string row_id,
	std::string column_id, std::string per_row_cnt, std::string tag_id, std::string effect_id)
{
	ClickInfo clickinfo;
	clickinfo.agency = m_agency;
	clickinfo.netbar = m_netbar;
	clickinfo.userId = m_userId;
	clickinfo.mac = m_mac;
	clickinfo.udid = m_udid;
	clickinfo.type = type;
	clickinfo.location = pos;
	clickinfo.tartget = dstid;
	clickinfo.row_id = row_id;
	clickinfo.column_id = column_id;
	clickinfo.per_row_cnt = per_row_cnt;
	clickinfo.tag_id = tag_id;
	clickinfo.effect_id = effect_id;
	ObjectLock<CLock>	guard(m_lock);
	m_click_info.push_back(clickinfo);
}
void CUpLoadGameClick::addItemSvr( DWORD gid,DWORD nclick )
{
	ObjectLock<CLock> guard(m_lockSvr);
	std::map<DWORD, DWORD>::iterator it = m_GameClick.find(gid);
	if (it != m_GameClick.end())
		it->second++;
	else
		m_GameClick.insert(std::make_pair(gid, 1));
}

void CUpLoadGameClick::addItemSvr( std::map<DWORD,DWORD>& clickMap )
{
	ObjectLock<CLock> guard(m_lockSvr);
	for (std::map<DWORD, DWORD>::iterator it= clickMap.begin();it != clickMap.end();it++)
	{
		std::map<DWORD, DWORD>::iterator itTemp = m_GameClick.find(it->first);
		if (itTemp != m_GameClick.end())
			itTemp->second++;
		else
			m_GameClick.insert(std::make_pair(it->first, 1));
	}
}
std::string CUpLoadGameClick::ComposeJsonData(vector<ClickInfo>& clickVect)
{
	Json::Value root;
	Json::Value	app_evts_array;
	Json::Value app_evts;

	{
		ObjectLock<CLock>	guard(m_lock);
		clickVect = m_click_info;
		m_click_info.clear();
	}
	for (size_t i=0;i<clickVect.size();i++)
	{
		Json::Value param;

		param["client_version"] = clickVect[i].strMenuVer;
		param["agency"] = clickVect[i].agency;
		param["netbar"] = clickVect[i].netbar;
		param["udid"] = clickVect[i].udid;
		param["mac"] = clickVect[i].mac;
		param["userId"] = clickVect[i].userId;
		param["type"] = clickVect[i].type;
		param["location"] = clickVect[i].location;
		param["target"] = clickVect[i].tartget;
		app_evts["id"] = clickVect[i].id;
		param["row_id"] = clickVect[i].row_id;
		param["column_id"] = clickVect[i].column_id;
		param["per_row_cnt"] = clickVect[i].per_row_cnt;
		param["tag_id"] = clickVect[i].tag_id;
		param["effect_id"] = clickVect[i].effect_id;
		

		param["boot_time"] = m_bootTime;
		app_evts["startTime"] = (unsigned int)clickVect[i].startTime;
		app_evts["parameters"] = param;
		app_evts_array.append(app_evts);
	}
	
	root["appKey"] = "210_0";
	root["appEvents"] = app_evts_array;

	Json::FastWriter	jwriter;
	std::string sTmp = jwriter.write(root);

	return sTmp;
}
bool CUpLoadGameClick::SendDataToServer(char* pData, int len, bool bAck, char* buf)
{
	//send data to server.
	int nSend = 0;
	while(nSend < len)
	{
		int nlen = send(m_sck, pData+nSend, len-nSend, 0);
		if(nlen <= 0)
		{
			AfxSocketClose(m_sck);
			return false;
		}
		nSend += nlen;
	}
	if (!bAck)
		return true;

	kmenu::PkgHeader* pheader = (kmenu::PkgHeader*)buf;
	int   nRecvLen = 0;
	while (1)
	{
		FD_SET fdset;
		timeval tv = {3, 0};
		FD_ZERO(&fdset);
		FD_SET(m_sck, &fdset);
		int ret = select(0, &fdset, NULL, NULL, &tv);
		if (ret <= 0)
		{
			AfxSocketClose(m_sck);
			return false;
		}

		if (FD_ISSET(m_sck, &fdset))
		{
			int len = recv(m_sck, buf + nRecvLen, PKG_MAXLENGTH - nRecvLen, 0);
			if (len <= 0)
			{
				AfxSocketClose(m_sck);
				return false;
			}
			nRecvLen += len;

			if (nRecvLen >= sizeof(kmenu::PkgHeader) && nRecvLen >= (int)pheader->length)
				return true;
		}
	}
	return false;
}

bool CUpLoadGameClick::report_game_click_toserver()
{
	bool bResult = false;
	std::map<DWORD, DWORD> GameClick;
	{
		ObjectLock<CLock>	guard(m_lockSvr);
		GameClick = m_GameClick;
		m_GameClick.clear();
	}
	if(GameClick.size() == 0)
		return true;

	if(m_sck == INVALID_SOCKET)
		m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);

	if (m_sck != INVALID_SOCKET)
	{
		char buf[PKG_MAXLENGTH] = {0};
		kmenu::CPkgHelper pkg(buf, CMD_CLI_RPTGAMEMENUCLICK, OBJECT_CLIENT);

		pkg.Push(GameClick.size());
		std::map<DWORD, DWORD>::iterator iter = GameClick.begin();
		for (iter; iter != GameClick.end(); iter++)
		{
			pkg.Push(iter->first);
			pkg.Push(iter->second);
		}
		
		//压菜单点击数
		{
			//第一次上报就报了菜单运行了，以后就不需要报了，即上报为0.
			static bool bFirst = true;
			pkg.Push(bFirst ? 1ul : 0ul);

			//压入计算机名
			TCHAR Computer[MAX_PATH] = {0};
			DWORD dwSize = _countof(Computer);
			GetComputerName(Computer, &dwSize);
			_tcsupr_s(Computer);
			pkg.PushString(Computer);
			bFirst = false;
		}

		//清空己报过的游戏点击数。
		if (SendDataToServer(buf, pkg.GetLength(), false, NULL))
		{
			GameClick.clear();
			bResult = true;
		}
		else
			addItemSvr(GameClick);
	}

	return bResult;
}
bool CUpLoadGameClick::report_game_click()
{
	bool bResult = false;
	vector<ClickInfo> clickVect;
	try
	{
		struct handler_t
		{
			void operator()(const std::string &msg)
			{
				throw std::runtime_error(msg);
			}
		};

		TCHAR szValue[MAX_PATH] = { 0 };
		kcore::GetLocalStringFun(szValue, MAX_PATH, L"REPORT_URL", L"http://ykdc.hzyoka.com/bfrd/json");

		//http::win_http_t win_http(REPORT_URL, http::post, handler_t());
		http::win_http_t win_http(szValue, http::post, handler_t());
		win_http.timeout(HTTP_TIME_OUT);

		std::string str = ComposeJsonData(clickVect);
		if(clickVect.size() == 0)
			return true;

		unsigned char key[] = "yungengxin201603";
		AES aes(key);
		int length =(str.length()+15)/16 *16;
		unsigned char* pBuffer = new unsigned char[length];
		memset(pBuffer,0,length);
		memcpy(pBuffer,str.c_str(),str.length());
		aes.Cipher(pBuffer,length);
		std::string params((char*)pBuffer,length);
		delete []pBuffer;

		std::map<std::wstring, std::wstring> header;
		header[L"Connection"] = L"close";
		header[L"Content-type"] = L"application/x-www-form-urlencoded";
		header[L"Encrypt-Version"] = L"1.0";
		std::vector<char> rdata = win_http.send(header, params);
		std::string strReponse(rdata.begin(), rdata.end());
		Json::Reader reader;
		Json::Value root;
		if(reader.parse(strReponse,root,false))
		{
			int nResult = root["result"].asInt();
			if(nResult == 0)
			{
				bResult = true;
				AfxWriteLogger(g_Logger, _T("upload game click success."));
			}
			else
			{
				CString msg = (_bstr_t)root["msg"].asCString();
				AfxWriteLogger(g_Logger, _T("upload game click.result:%d,msg:%s"), nResult,msg);
			}
		}
	}
	catch( const std::exception &e )
	{
		std::wstring msg = CA2W(e.what());
		AfxWriteLogger(g_Logger, TEXT("upload game click Error:%s"), msg.c_str());
	}
	if(!bResult)
		addItem(clickVect);

	return bResult;
}

void CUpLoadGameClick::StartUploadGameClick()
{
	if(m_hThread != NULL)
		return;

	m_hExitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, WorkThread, this, 0, NULL);
}

void CUpLoadGameClick::StopUploadGameClick()
{
	if (m_hExitEvent != NULL)
	{
		::SetEvent(m_hExitEvent);

		if (m_hThread != NULL)
		{
			::WaitForSingleObject(m_hThread, INFINITE);
			::CloseHandle(m_hThread);
			m_hThread = NULL;
		}
		::CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
	}
}
void CUpLoadGameClick::GetMacAddress()
{
	std::string strMac;
	DWORD dwSize = 0;
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	if (GetAdaptersInfo(pAdapterInfo, &dwSize) != ERROR_BUFFER_OVERFLOW)
		return ;
	pAdapterInfo = (PIP_ADAPTER_INFO)(new char[dwSize]);
	if (GetAdaptersInfo(pAdapterInfo, &dwSize) != NOERROR)
	{
		delete [] (char*)pAdapterInfo;
		return ;
	}
	PIP_ADAPTER_INFO pAdapter = pAdapterInfo, pNextAdapter = pAdapterInfo;
	while(pNextAdapter)
	{
		pAdapter = pNextAdapter;
		pNextAdapter = pAdapter->Next;

		MIB_IFROW IfRow = {0};
		IfRow.dwIndex = pAdapter->Index;
		if (GetIfEntry(&IfRow) != NO_ERROR || inet_addr(pAdapter->IpAddressList.IpAddress.String) == 0 || 
			inet_addr(pAdapter->GatewayList.IpAddress.String) == 0 ||
			IfRow.dwOperStatus != IF_OPER_STATUS_OPERATIONAL)
			continue;

		TCHAR szMac[MAX_PATH] = {0};
		_stprintf_s(szMac, _countof(szMac), TEXT("%02X-%02X-%02X-%02X-%02X-%02X"),
			pAdapter->Address[0],
			pAdapter->Address[1],
			pAdapter->Address[2],
			pAdapter->Address[3],
			pAdapter->Address[4],
			pAdapter->Address[5]);

		strMac =  (_bstr_t)szMac;
		break;
	}
	delete [] (char*)pAdapterInfo;
	m_mac = strMac.c_str();
}
UINT __stdcall CUpLoadGameClick::WorkThread( LPVOID lpVoid )
{
	CUpLoadGameClick *pThis = reinterpret_cast<CUpLoadGameClick*>(lpVoid);
	pThis->GetMacAddress();
	pThis->GetNetBarInfo();
	pThis->GetUdid();
	while(WaitForSingleObject(pThis->m_hExitEvent,WAIT_TIME))
	{
		pThis->report_game_click();
		static DWORD nTime = _time32(NULL);
		if(_time32(NULL) - nTime > 5)
		{
			pThis->report_game_click_toserver();
			nTime = _time32(NULL);
		}
	}
	return 0;
}

void CUpLoadGameClick::GetNetBarInfo()
{
	TCHAR szBuffer[MAX_PATH] = {0};
	DWORD dwType = REG_SZ, dwSize = _countof(szBuffer);
	SHGetValue(HKEY_LOCAL_MACHINE, SOFTWARE_KEY, TEXT("nbid"), &dwType, szBuffer, &dwSize);
	m_netbar = (_bstr_t)szBuffer;

	dwType = REG_SZ, dwSize = _countof(szBuffer);
	SHGetValue(HKEY_LOCAL_MACHINE, SOFTWARE_KEY, TEXT("agid"), &dwType, szBuffer, &dwSize);
	m_agency = (_bstr_t)szBuffer;
}

void CUpLoadGameClick::SetUserid( std::string userid )
{
	m_userId = userid;
}

void CUpLoadGameClick::GetUdid()
{
	InitHwmonitor();
	if(m_bInitHw && m_pHw_GetMainBoardInfo)
	{
		MAINBOARD mainboard = {0};
		if(m_pHw_GetMainBoardInfo(&mainboard))
			m_udid = (_bstr_t)mainboard.szSerialNumber;
	}
	UnitHwmonitor();
}

void CUpLoadGameClick::InitHwmonitor()
{
	HMODULE		hMod = NULL;
	TCHAR szDllName[MAX_PATH] = {0};
	AfxGetAppPath(szDllName, _countof(szDllName), TEXT("khwmonitor.dll"));
	if (hMod = LoadLibrary(szDllName))
	{
		m_pHwmonitor_Init = (HWM_INIT)GetProcAddress(hMod, "HWMONITOR_Init");
		m_pHwmonitor_UnInit = (HWM_UNINIT)GetProcAddress(hMod, "HWMONITOR_UnInit");
		m_pHw_GetMainBoardInfo = (HWM_GETMAINBOARDINFO)GetProcAddress(hMod, "HWMONITOR_GetMainBoardInfo");

		if(m_pHwmonitor_Init)
			m_bInitHw = m_pHwmonitor_Init();
	}
}
void CUpLoadGameClick::UnitHwmonitor()
{
	if(m_pHwmonitor_UnInit)
		m_pHwmonitor_UnInit();
}


