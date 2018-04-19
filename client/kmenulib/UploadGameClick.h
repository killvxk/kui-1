
#ifndef __UPLOAD_GAME_CLICK_H__
#define __UPLOAD_GAME_CLICK_H__
#include "khwmonitor.h"
#include "dbmgr.h"

struct ClickInfo
{
	ClickInfo()
	{
		id = "1250";
		startTime = _time32(NULL);

		TCHAR szVersizon[MAX_PATH] = { 0 };
		AfxGetFileVersion(TEXT("knbmenu.exe"), szVersizon);
		strMenuVer = CW2A(szVersizon);
	}

	std::string	  strMenuVer;
	DWORD		  startTime;
	std::string	  id;
	std::string   agency;
	std::string   netbar;
	std::string   udid;
	std::string   mac;
	std::string   userId;
	std::string   type;
	std::string   location;
	std::string   tartget;
	std::string   row_id;
	std::string   column_id;
	std::string	  per_row_cnt;
	std::string   tag_id;
	std::string   effect_id;
	//std::string   boot_time;
};

//hwmonitor
typedef bool (__stdcall *HWM_INIT)();
typedef void (__stdcall *HWM_UNINIT)();
typedef bool (__stdcall *HWM_GETMAINBOARDINFO)(LPMAINBOARD lpMainBoardInfo);

class  CUpLoadGameClick
{
public:
	CUpLoadGameClick();
	~CUpLoadGameClick();
	void StartUploadGameClick();
	void StopUploadGameClick();
	
	void SetUserid(std::string userid);
	void addItem(std::string type, std::string pos, std::string dstid, std::string row_id = "",
		std::string column_id = "", std::string per_row_cnt = "", std::string tag_id = "", std::string effect_id = "");
	void addItem(std::vector<ClickInfo>& clickVect);
	void addItemSvr(DWORD gid,DWORD nclick);
	void addItemSvr(std::map<DWORD,DWORD>& clickMap);
	bool report_game_click();
	std::string ComposeJsonData(std::vector<ClickInfo>& clickVect);
	static UINT __stdcall WorkThread(LPVOID lpVoid);
private:
	void GetUdid();
	void GetNetBarInfo();
	void GetMacAddress();
	void InitHwmonitor();
	void UnitHwmonitor();
	bool report_game_click_toserver();
	bool SendDataToServer(char* pData, int len, bool bAck, char* buf);
private:
	//hwmonitor
	bool					m_bInitHw;
	HWM_INIT				m_pHwmonitor_Init;
	HWM_UNINIT				m_pHwmonitor_UnInit;
	HWM_GETMAINBOARDINFO	m_pHw_GetMainBoardInfo;

	HANDLE						m_hExitEvent;
	HANDLE						m_hThread;
	std::string					m_agency;
	std::string					m_netbar;
	std::string					m_userId;
	std::string					m_mac;
	std::string					m_udid;
	std::string					m_appKey;
	std::string					m_bootTime;
	CLock						m_lock;
	std::vector<ClickInfo>		m_click_info;

	CLock						m_lockSvr;
	std::map<DWORD, DWORD>		m_GameClick;
	SOCKET						m_sck;
};


#endif
