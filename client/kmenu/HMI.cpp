#include "HMI.h"
#include "ResourceSystem/ZipSkin.h"
#include "resource.h"

typedef struct tagFileMap
{
	HWND hWnd;
	TCHAR szCmdLine[1024];
}tagFileMap;
HANDLE							m_hFileMap;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	AfxSetExptHandle();
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CString							m_szCmdLine;	

	//去除参数双引号
	m_szCmdLine = lpCmdLine;
	m_szCmdLine.Trim();
	if (!m_szCmdLine.IsEmpty())
	{
		if (m_szCmdLine[0] == _T('\"'))
			m_szCmdLine = m_szCmdLine.Right(m_szCmdLine.GetLength() - 1);
		if (m_szCmdLine[m_szCmdLine.GetLength() - 1] == _T('\"'))
			m_szCmdLine = m_szCmdLine.Left(m_szCmdLine.GetLength() - 1);
	}

	tagFileMap* pFileMap = NULL;

	m_hFileMap = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(tagFileMap), TEXT("__desk_kmenu_map__"));
	DWORD dwError = GetLastError();
	if (m_hFileMap == NULL)
	{
		WriteLog(TEXT("Create FileMapp Fail.Program will exit."));
		return FALSE;
	}

	pFileMap = (tagFileMap*)MapViewOfFile(m_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE,
		0, 0, sizeof(tagFileMap));
	if (pFileMap == NULL)
	{
		WriteLog(TEXT("Map file view error.Program will exit."));
		return FALSE;
	}
	//把命令行参数写入共享内存
	lstrcpy(pFileMap->szCmdLine, m_szCmdLine);

	//如果己经运行了菜单
	if (dwError == ERROR_ALREADY_EXISTS)
	{		
		//如果带有参数，则向己运行的菜单发送消息，否则退出。

		//WriteLog(TEXT("检查是否存在窗口"));
		HWND hWnd = pFileMap->hWnd;

		if (IsWindow(hWnd))
		{
			::SendMessage(hWnd, WM_USER, 0, 0);
			
			::SetForegroundWindow(hWnd);
			::SetFocus(hWnd);
		}

		return TRUE;
	}

	DWORD dwLanguageId = 0;
	DWORD dwRegType = 0, dwSize = sizeof(dwLanguageId);
	if (ERROR_SUCCESS == ::SHGetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet"),
		TEXT("languageid"), &dwRegType, &dwLanguageId, &dwSize) && dwRegType == REG_DWORD)
	{
		AfxSetLangId((LANGID)dwLanguageId);
	}

	//WSADATA wsaData = { 0 };
	//WSAStartup(0x0202, &wsaData);

	TCHAR szTest[MAX_PATH] = { 0 };
	if (!CZipSkin::getInstance()->LoadZip(L"skin\\MenuSkin.zip"))
	{
		::LoadString(::GetModuleHandle(NULL), IDS_LOAD_SKIN_ERROR, szTest, MAX_PATH);

		::MessageBoxEx(0, szTest, 0, MB_OK | MB_ICONEXCLAMATION, (LANGID)dwLanguageId);

		return 0;
	}

	OpenHMI::CConfigParser::tagScreenInfo info;

	info.uiWidth = 1660;
	info.uiHeight = 935;
	info.uiCaptionW = 1660;
	info.uiCaptionH = 65;

	::LoadString(::GetModuleHandle(NULL), IDS_TITLE, szTest, MAX_PATH);
	info.strName = szTest;

	OpenHMI::CConfigParser::getInstance()->InitScreenInfo(info);

	OpenHMI::CConfigParser::getInstance()->setSkinPath(L"MenuSkin/");

	CClientDbMgr::getInstance()->Initialize();



	OpenHMI::CMain::getInstance()->startHMI(OpenHMI::CMainFrame::getInstance());

	CZipSkin::getInstance()->UnLoad();

	UnmapViewOfFile(pFileMap);
	if (m_hFileMap != NULL)
		CloseHandle(m_hFileMap);

	CClientDbMgr::getInstance()->unInitialize();
	return 0;
}