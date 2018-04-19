#include "resource.h"
#include "OpenHMI.h"
#include <windowsx.h>
#include <Shellapi.h>
#include <atlwin.h>
CComModule   _Module;

#define UWM_MSG (WM_APP + 1)

namespace OpenHMI
{
	#define	WINDOW_CLASS		L"KUIClass"
	CScreenTouchServer	*CScreenTouchServer::s_pInstance	= NULL;

	CScreenTouchServer*	CScreenTouchServer::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CScreenTouchServer();
		}

		return s_pInstance;
	}

	void CScreenTouchServer::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	tagScreenContext::tagScreenContext() 
	{
		reset();
	}

	void tagScreenContext::reset()
	{
		uiScreenWidth = 0;
		uiScreenHeight = 0;

		hWnd = NULL;
		hDC = NULL;
	}

	tagWebBrowserContext::tagWebBrowserContext() 
	{
		hWnd = NULL;
		pWeb = NULL;
		hDC = NULL;
	}

	tagEditContext::tagEditContext() 
	{
		hWnd = NULL;
		hDC = NULL;
	}

	BOOLEAN	tagScreenContext::operator == (const tagScreenContext &src) const
	{
		if (uiScreenWidth != src.uiScreenWidth || uiScreenHeight != src.uiScreenHeight)
		{
			return FALSE;
		}

		if (hWnd != src.hWnd || hDC != src.hDC)
		{
			return FALSE;
		}

		return TRUE;
	}

	BOOLEAN	tagScreenContext::operator != (const tagScreenContext &src) const
	{
		return ! (*this == src);
	}
	
	CScreenTouchServer::CScreenTouchServer() 
		: m_pListener(NULL)
		, m_hDrop(NULL)
		, m_hIcon(NULL)
		, m_bForceHide(FALSE)
	{
	

	}

	CScreenTouchServer::~CScreenTouchServer()
	{
		;
	}

	BOOLEAN CScreenTouchServer::initialize()
	{
		//set screen touch thread priority;
		//setPriority(CConfigParser::getInstance()->getSysInfo()->iScreenTouchThreadPri);
		start();
		
		m_waitStart.wait();

		if (m_context.hWnd == NULL)
		{
			return FALSE;
		}
		
		return TRUE;
	}

	void CScreenTouchServer::unInitialize()
	{
		for (UINT i = 0; i < m_aryWebBrowserContext.size(); i++)
		{
			delete m_aryWebBrowserContext[i].pWeb;

			if (m_aryWebBrowserContext[i].hDC != NULL)
			{
				::ReleaseDC(m_aryWebBrowserContext[i].hWnd, m_aryWebBrowserContext[i].hDC);
				m_aryWebBrowserContext[i].hDC = NULL;
			}

			if (m_aryWebBrowserContext[i].hWnd != NULL)
			{
				::DestroyWindow(m_aryWebBrowserContext[i].hWnd);
				::ReleaseDC(m_aryWebBrowserContext[i].hWnd, m_aryWebBrowserContext[i].hDC);
				m_aryWebBrowserContext[i].hWnd = NULL;
			}
		}

		for (UINT i = 0; i < m_aryEditContext.size(); i++)
		{
			if (m_aryEditContext[i].hDC != NULL)
			{
				::ReleaseDC(m_aryEditContext[i].hWnd, m_aryEditContext[i].hDC);
				m_aryEditContext[i].hDC = NULL;
			}

			if (m_aryEditContext[i].hWnd != NULL)
			{
				::DestroyWindow(m_aryEditContext[i].hWnd);
				m_aryEditContext[i].hWnd = NULL;
			}
		}

		if (m_context.hDC != NULL)
		{
			::ReleaseDC(m_context.hWnd, m_context.hDC);
			m_context.hDC = NULL;
		}

		if (m_hIcon)
		{
			DestroyIcon(m_hIcon);
			m_hIcon = NULL;
		}

		if (m_context.hWnd != NULL)
		{
			::DestroyWindow(m_context.hWnd);
			::PostQuitMessage(0);
			m_context.hWnd = NULL;
		}
		m_context.reset();
	}

	const tagWebBrowserContext* CScreenTouchServer::getWebBrowserContext(UINT uiID)
	{
		if(uiID >= m_aryWebBrowserContext.size())
		{
			return NULL;
		}
		else
		{
			return &m_aryWebBrowserContext[uiID]; 
		}
	}

	const tagEditContext* CScreenTouchServer::getEditContext(UINT uiID)
	{
		if(uiID >= m_aryEditContext.size())
		{
			return NULL;
		}
		else
		{
			return &m_aryEditContext[uiID]; 
		}
	}

	static WNDPROC preEditProc;

#define NULLTEST_SE(fn, wstr) if((fn) == NULL) goto RETURN;
#define NULLTEST_E(fn, wstr) if((fn) == NULL) goto RETURN;
#define NULLTEST(fn) if((fn) == NULL) goto RETURN;

#define HRTEST_SE(fn, wstr)  if(FAILED(fn)) goto RETURN;
#define HRTEST_E(fn, wstr)  if(FAILED(fn)) goto RETURN;
	
	void CScreenTouchServer::run()
	{
		HINSTANCE hInstance= ::GetModuleHandle(NULL);

		//::CoInitialize(NULL);

		_Module.Init(0, hInstance);

		WNDCLASSEX wcex;
		wcex.cbSize			= sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wcex.lpfnWndProc	= WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= WINDOW_CLASS;
		wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
		
		::RegisterClassEx(&wcex);
		

		RECT rt;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);

		UINT sw = rt.right - rt.left;
		UINT sh = rt.bottom - rt.top;

		m_context.hWnd = ::CreateWindowEx(WS_EX_ACCEPTFILES, WINDOW_CLASS, (const CHAR*)m_strScreenName, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX,
			(sw - m_context.uiScreenWidth) / 2, (sh - m_context.uiScreenHeight) / 2,
										m_context.uiScreenWidth, 
										m_context.uiScreenHeight, 
										NULL, NULL, hInstance, NULL);


		if (NULL == m_context.hWnd)
		{		
			m_waitStart.notify();
			return;
		}

		m_context.hDC = ::GetDC(m_context.hWnd);


		m_aryWebBrowserContext.clear();		

		for(UINT i =0; i < CConfigParser::getInstance()->getSysInfo()->uiWebBrowserCount; i++)
		{
			tagWebBrowserContext context;
			CAxWindow WinContainer;

			context.hWnd = WinContainer.Create(m_context.hWnd, NULL, 0, WS_CHILD | WS_CLIPSIBLINGS );
			context.pWeb = new CWebForm(context.hWnd);
			context.pWeb->OpenWebBrowser();

			m_aryWebBrowserContext.addItem(context);
		}

		HMODULE hRichEditDll = NULL;

		hRichEditDll = LoadLibrary(_T("RICHED20.dll"));

		m_aryEditContext.clear();

		for (UINT i = 0; i<CConfigParser::getInstance()->getSysInfo()->uiEditCtrlCount; i++)
		{
			tagEditContext context;

			if (i == 1 || i == 2)
				context.hWnd = CreateWindowEx(0, TEXT("RichEdit20W"), NULL, WS_CHILD | WS_CLIPSIBLINGS | ES_LEFT | ES_OEMCONVERT | ES_PASSWORD | WS_TABSTOP , 0, 0, 0, 0, m_context.hWnd, 0, hInstance, NULL);
			else
				context.hWnd = CreateWindowEx(0, TEXT("RichEdit20W"), NULL, WS_CHILD | WS_CLIPSIBLINGS | ES_OEMCONVERT | ES_LEFT | WS_TABSTOP, 0, 0, 0, 0, m_context.hWnd, 0, hInstance, NULL);

			preEditProc = (WNDPROC)SetWindowLong(context.hWnd,
				GWL_WNDPROC,
				(LPARAM)(WNDPROC)EditProc);
			context.hDC = ::GetDC(context.hWnd);
			m_aryEditContext.addItem(context);
		}

		if (CConfigParser::getInstance()->getSysInfo()->bNeedTray == TRUE)
		{
			NOTIFYICONDATA data = { 0 };

			data.cbSize = sizeof(data);
			data.hWnd = m_context.hWnd;
			Shell_NotifyIcon(NIM_DELETE, &data);

			data.cbSize = sizeof(data);
			data.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
			data.hIcon = LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
			data.hWnd = m_context.hWnd;

			String::strcpy(data.szTip, m_strScreenName);
			data.uCallbackMessage = UWM_MSG;
			Shell_NotifyIcon(NIM_ADD, &data);

			CScreenTouchServer::getInstance()->m_hIcon = data.hIcon;
		}

		m_waitStart.notify();

		MSG msg;	

		while (GetMessage(&msg, NULL, 0, 0)) 
		{
			if (msg.message == WM_KEYDOWN && (msg.wParam == VK_RETURN || msg.wParam == VK_TAB || msg.wParam == VK_F5))
			{
				for (UINT i = 0; i < CConfigParser::getInstance()->getSysInfo()->uiWebBrowserCount; i++)
				{
					if (!IsWindowVisible(m_aryWebBrowserContext[i].hWnd))
						continue;

					IWebBrowser2* pWb2 = m_aryWebBrowserContext[i].pWeb->GetWebBrowser2();
					NULLTEST(pWb2);
					IOleInPlaceActiveObject *pOleInPlaceActiveObj = NULL;
					HRTEST_SE(pWb2->QueryInterface(IID_IOleInPlaceActiveObject, (void**)&pOleInPlaceActiveObj), _T("QueryInterface IID_IOleInPlaceActiveObject 失败"));
					NULLTEST(pOleInPlaceActiveObj);
					pOleInPlaceActiveObj->TranslateAccelerator(&msg);
					pOleInPlaceActiveObj->Release();
				}
			}
			
		RETURN:
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		UnregisterClass(WINDOW_CLASS, hInstance);
		FreeLibrary(hRichEditDll);
		_Module.Term();
		//::CoUninitialize();
	}

	void AfxSetLangId(WORD wLangID)
	{
		if (0 == wLangID)
			return;

		LANGID lgidUI = ::GetSystemDefaultUILanguage();
		LANGID lgid = ::GetSystemDefaultLangID();
		if (lgidUI == (LANGID)wLangID && lgid == (LANGID)wLangID)
			return;

#ifndef _WIN64
		OSVERSIONINFO ovs;
		ovs.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (::GetVersionEx(&ovs))
		{
			if (ovs.dwPlatformId == VER_PLATFORM_WIN32_NT && ovs.dwMajorVersion >= 6 &&
				ovs.dwMinorVersion >= 0)
			{
				::SetThreadUILanguage((LANGID)wLangID);
			}
			else
				::SetThreadLocale(MAKELCID((LANGID)wLangID, SORT_DEFAULT));
		}
#else
		::SetThreadUILanguage((LANGID)wLangID);
#endif
	}

	LRESULT CALLBACK CScreenTouchServer::WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
	{
#define IDM_OPEN 0x1001 
#define IDM_EXIT 0x1002 
#define IDC_NC 1002

		static UINT const WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated"));

		if (uiMsg == WM_TASKBARCREATED)
		{
			if (CConfigParser::getInstance()->getSysInfo()->bNeedTray == TRUE)
			{
				NOTIFYICONDATA data = { 0 };

				data.cbSize = sizeof(data);
				data.hWnd = hWnd;
				Shell_NotifyIcon(NIM_DELETE, &data);

				data.cbSize = sizeof(data);
				data.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
				data.hIcon = LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
				data.hWnd = hWnd;

				String::strcpy(data.szTip, CScreenTouchServer::getInstance()->m_strScreenName);
				data.uCallbackMessage = UWM_MSG;
				Shell_NotifyIcon(NIM_ADD, &data);

				CScreenTouchServer::getInstance()->m_hIcon = data.hIcon;
			}
			return 0;
		}

		switch (uiMsg) 
		{

		case UWM_MSG:
			switch(lParam)
			{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
				{
					ShowWindow(hWnd, SW_RESTORE);
					::SetForegroundWindow(hWnd);
				}				
				break;

			case WM_RBUTTONDOWN:
				{
					POINT pt;
					GetCursorPos(&pt);

					::SetForegroundWindow(hWnd);

					HMENU hMenu;
					hMenu = CreatePopupMenu();

					DWORD dwLanguageId = 0;
					DWORD dwRegType = 0, dwSize = sizeof(dwLanguageId);
					if (ERROR_SUCCESS == ::SHGetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet"),
						TEXT("languageid"), &dwRegType, &dwLanguageId, &dwSize) && dwRegType == REG_DWORD)
					{
						AfxSetLangId((LANGID)dwLanguageId);
					}

					TCHAR szOpen[MAX_PATH] = { 0 };
					TCHAR szQuit[MAX_PATH] = { 0 };
					::LoadString(::GetModuleHandle(NULL), IDS_OPEN_MENU, szOpen, MAX_PATH);
					::LoadString(::GetModuleHandle(NULL), IDS_QUIT_MENU, szQuit, MAX_PATH);

					AppendMenu(hMenu, MF_STRING, IDM_OPEN, szOpen);
					AppendMenu(hMenu, MF_STRING, IDM_EXIT, szQuit);

					TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, NULL, hWnd, NULL);
					::DestroyMenu(hMenu);//释放菜单资源
				}
				break;
			case WM_DESTROY:
			{
				NOTIFYICONDATA data = { 0 };

				data.cbSize = sizeof(data);
				data.hWnd = hWnd;
				Shell_NotifyIcon(NIM_DELETE, &data);
				PostQuitMessage(0);
			}
				break;

			default:
				{

				}
				break;
			}
			break;
#if 0
		case WM_DISPLAYCHANGE:
		{
			CMsgScreen *pMessage = new CMsgScreen(DISPLAYCHANGE);
			CMain::getInstance()->pushMessage(pMessage);
		}
		break;
#endif

		case WM_DROPFILES:
		{
			CScreenTouchServer *pServer = CScreenTouchServer::getInstance(); 
			pServer->m_hDrop = (HDROP)wParam;
			CMsgScreen *pMessage = new CMsgScreen(DROPFILES);
			CMain::getInstance()->pushMessage(pMessage);
		}
		break;

		case WM_RBUTTONDOWN:
			{
				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

				pServer->m_cursor.setID(1);
				pServer->m_cursor.setX((SHORT)LOWORD(lParam));
				pServer->m_cursor.setY((SHORT)HIWORD(lParam));
				pServer->m_cursor.setXSpeed(0);
				pServer->m_cursor.setYSpeed(0);
				pServer->m_cursor.setTouchStatus(CCursor::TS_R_PRESS_DOWN);

				CMsgScreenTouch *pMessage = new CMsgScreenTouch(pServer->m_pListener, pServer->m_cursor);
				CMain::getInstance()->pushMessage(pMessage);

				SetFocus(hWnd);
				SetCapture(hWnd);
			}
			break;

		case WM_LBUTTONDOWN:
			{
				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

				pServer->m_cursor.setID(1);
				pServer->m_cursor.setX((SHORT)LOWORD(lParam));
				pServer->m_cursor.setY((SHORT)HIWORD(lParam));
				pServer->m_cursor.setXSpeed(0);
				pServer->m_cursor.setYSpeed(0);
				pServer->m_cursor.setTouchStatus(CCursor::TS_PRESS_DOWN);

				CMsgScreenTouch *pMessage = new CMsgScreenTouch(pServer->m_pListener, pServer->m_cursor);
				CMain::getInstance()->pushMessage(pMessage);

				POINT pt;
				pt.x = (short)LOWORD(lParam);
				pt.y = (short)HIWORD(lParam);
				SetFocus(hWnd);

				CRectangle rc = ENV::getScreenCaptionRect();
				if (rc.ptInRect(CPoint(pt.x, pt.y)))
				{
					::ReleaseCapture();
					//SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, false, NULL, 0);
					SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
					SendMessage(hWnd, WM_LBUTTONUP, NULL, NULL);
					break;
				}
				SetCapture(hWnd);
			}
			break;
		case WM_LBUTTONDBLCLK:
		{
			POINT pt;
			pt.x = (short)LOWORD(lParam);
			pt.y = (short)HIWORD(lParam);

			CRectangle rc = ENV::getScreenCaptionRect();
			if (rc.ptInRect(CPoint(pt.x, pt.y)))
			{
				CMsgScreen *pMessage = new CMsgScreen(CAPTION_DBLCLK);
				CMain::getInstance()->pushMessage(pMessage);
			}
		}
		break;
		case WM_MOUSEMOVE:
			if (wParam & MK_LBUTTON)
			{
				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

				pServer->m_cursor.setID(1);
				pServer->m_cursor.setX((SHORT)LOWORD(lParam));
				pServer->m_cursor.setY((SHORT)HIWORD(lParam));
				pServer->m_cursor.setXSpeed(0);
				pServer->m_cursor.setYSpeed(0);
				pServer->m_cursor.setTouchStatus(CCursor::TS_PRESS_DRAG);

				CMsgScreenTouch *pMessage = new CMsgScreenTouch(pServer->m_pListener, pServer->m_cursor);
				CMain::getInstance()->pushMessage(pMessage);
			}
			else
			{
				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

				pServer->m_cursor.setID(1);
				pServer->m_cursor.setX((SHORT)LOWORD(lParam));
				pServer->m_cursor.setY((SHORT)HIWORD(lParam));
				pServer->m_cursor.setXSpeed(0);
				pServer->m_cursor.setYSpeed(0);
				pServer->m_cursor.setTouchStatus(CCursor::TS_MOVE);

				CMsgScreenTouch *pMessage = new CMsgScreenTouch(pServer->m_pListener, pServer->m_cursor);
				CMain::getInstance()->pushMessage(pMessage);
			}
			break;

		case WM_RBUTTONUP:
			{
				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

				pServer->m_cursor.setID(1);
				pServer->m_cursor.setX((SHORT)LOWORD(lParam));
				pServer->m_cursor.setY((SHORT)HIWORD(lParam));
				pServer->m_cursor.setXSpeed(1200);
				pServer->m_cursor.setYSpeed(1200);
				pServer->m_cursor.setTouchStatus(CCursor::TS_R_PRESS_UP);

				CMsgScreenTouch *pMessage = new CMsgScreenTouch(pServer->m_pListener, pServer->m_cursor);
				CMain::getInstance()->pushMessage(pMessage);


				::ReleaseCapture();
			}
			break;
		
		case WM_LBUTTONUP:
			{
				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

				pServer->m_cursor.setID(1);
				pServer->m_cursor.setX((SHORT)LOWORD(lParam));
				pServer->m_cursor.setY((SHORT)HIWORD(lParam));
				pServer->m_cursor.setXSpeed(1200);
				pServer->m_cursor.setYSpeed(1200);
				pServer->m_cursor.setTouchStatus(CCursor::TS_PRESS_UP);

				CMsgScreenTouch *pMessage = new CMsgScreenTouch(pServer->m_pListener, pServer->m_cursor);
				CMain::getInstance()->pushMessage(pMessage);


				::ReleaseCapture();
			}
			break;
		case WM_MOUSEWHEEL:
			{
				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
				pServer->m_cursor.setID(1);

				if( (INT)wParam > 0 )
				{
					pServer->m_cursor.setWheelType(CCursor::WT_UP);
				}
				else
				{
					pServer->m_cursor.setWheelType(CCursor::WT_DOWN);
				}

				pServer->m_cursor.setTouchStatus(CCursor::TS_WHEEL);

				CMsgScreenTouch *pMessage = new CMsgScreenTouch(pServer->m_pListener, pServer->m_cursor);
				CMain::getInstance()->pushMessage(pMessage);
			}
			break;
		case	WM_ACTIVATE:
		{
			if (LOWORD(wParam) != WA_INACTIVE)
			{
				CMsgScreen *pMessage = new CMsgScreen(SETFOCUS);
				CMain::getInstance()->pushMessage(pMessage);
			}
			else
			{
				CMsgScreen *pScreenMessage = new CMsgScreen(KILLFOCUS);
				CMain::getInstance()->pushMessage(pScreenMessage);
			}
			::DefWindowProc(hWnd, uiMsg, wParam, lParam);
		}
		break;
#if 0
		case WM_SETFOCUS:
		{
			CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
			for (UINT i = 0; i < CConfigParser::getInstance()->getSysInfo()->uiWebBrowserCount; i++)
			{
				if (wParam != 0)
					break;
				if (!IsWindowVisible(pServer->m_aryWebBrowserContext[i].hWnd))
					continue;

				//pServer->m_aryWebBrowserContext[i].pWeb->SetDocFocus();
			}
			::DefWindowProc(hWnd, uiMsg, wParam, lParam);
		}
		break;
		case WM_SYSCOMMAND:
		{
			if (wParam == SC_RESTORE)
			{
				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
				for (UINT i = 0; i < CConfigParser::getInstance()->getSysInfo()->uiWebBrowserCount; i++)
				{

					if (!IsWindowVisible(pServer->m_aryWebBrowserContext[i].hWnd))
						continue;

					pServer->m_aryWebBrowserContext[i].pWeb->SetDocFocus();
				}
			}

			::DefWindowProc(hWnd, uiMsg, wParam, lParam);
		}
		break;
#endif
		case WM_KILLFOCUS:
			{

				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

				pServer->m_cursor.setID(1);
				pServer->m_cursor.setX(-1);
				pServer->m_cursor.setY(-1);
				pServer->m_cursor.setXSpeed(0);
				pServer->m_cursor.setYSpeed(0);
				pServer->m_cursor.setTouchStatus(CCursor::TS_LOSTFOCUS);

				CMsgScreenTouch *pMessage = new CMsgScreenTouch(pServer->m_pListener, pServer->m_cursor);
				CMain::getInstance()->pushMessage(pMessage);

				::ReleaseCapture();
			}
			break;

 		case WM_PAINT:
 			{
				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
				PAINTSTRUCT ps;

				::BeginPaint(pServer->m_context.hWnd, &ps);
 				CMain::getInstance()->paintHMI(); 
				::EndPaint(pServer->m_context.hWnd, &ps);
 			}
 			break;

		case WM_COMMAND:
			{
				for (UINT i = 0; i<CScreenTouchServer::getInstance()->m_aryEditContext.size(); i++)
				{
					if (CScreenTouchServer::getInstance()->m_aryEditContext[i].hWnd == (HWND)lParam)
					{
						switch(HIWORD(wParam))
						{
						case EN_SETFOCUS:
							{						
								CMsgScreen *pMessage = new CMsgScreen(EDIT_SETFOCUS);
								CMain::getInstance()->pushMessage(pMessage);
							}
							break;
						case EN_KILLFOCUS:
							{
								CMsgScreen *pMessage = new CMsgScreen(EDIT_KILLFOCUS);
								CMain::getInstance()->pushMessage(pMessage);
							}
							break;

						default:
							break;
						}
					}
				}
				if (wParam == IDM_EXIT)
				{
					NOTIFYICONDATA data = {0};

					data.cbSize = sizeof(data);
					data.hWnd = hWnd;
					Shell_NotifyIcon(NIM_DELETE, &data);

					CMain::getInstance()->quitHMI();
				}
				else if (wParam == IDM_OPEN)
				{
					::ShowWindow(hWnd, SW_RESTORE);
					::SetForegroundWindow(hWnd);
				}

				::DefWindowProc(hWnd, uiMsg, wParam, lParam);
			}
			break;

		case WM_CHAR:
		{
			switch (wParam)
			{
			case VK_TAB:
			{
				CMsgScreen *pScreenMessage = new CMsgScreen(KEY_TAB);
				CMain::getInstance()->pushMessage(pScreenMessage);
			}
			break;

			case VK_RETURN:
			{
				CMsgScreen *pScreenMessage = new CMsgScreen(KEY_ENTER);
				CMain::getInstance()->pushMessage(pScreenMessage);
			}
			break;

			case VK_ESCAPE:
			{
				CMsgScreen *pScreenMessage = new CMsgScreen(KEY_ESC);
				CMain::getInstance()->pushMessage(pScreenMessage);
			}
			break;

			default:
				break;
			}
		}
		break;

		case WM_CLOSE:
		{
			if (CConfigParser::getInstance()->getSysInfo()->bNeedTray == TRUE)
			{
				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
				::ShowWindow(pServer->getScreenContext()->hWnd, SW_HIDE);
			}
			else
			{
				CMain::getInstance()->quitHMI();
			}
		}
		break;

		case WM_ERASEBKGND:
		{
			CMain::getInstance()->paintHMI();
		}
		break;

		case WM_USER:
		{
			CMsgScreen *pMessage = new CMsgScreen(USER);
			CMain::getInstance()->pushMessage(pMessage);
		}
		break;

		case WM_USER+1:
		{
			const tagWebBrowserContext* pContext = (const tagWebBrowserContext*)wParam;
			VARIANT* pVarUrl = (VARIANT*)lParam;

			if (pContext == NULL || pVarUrl == NULL)
				break;
			pContext->pWeb->OpenURL(pVarUrl);
		}
		break;

		default:
			return ::DefWindowProc(hWnd, uiMsg, wParam, lParam);
		}

		return 0;
	}

	LRESULT CALLBACK CScreenTouchServer::EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CHAR)
		{
			switch (wParam)
			{
			case VK_TAB:
			{
				CMsgScreen *pScreenMessage = new CMsgScreen(KEY_TAB);
				CMain::getInstance()->pushMessage(pScreenMessage);
			}
			return 0;

			case VK_RETURN:
			{
				CMsgScreen *pScreenMessage = new CMsgScreen(KEY_ENTER);
				CMain::getInstance()->pushMessage(pScreenMessage);
			}
			return 0;

			case VK_ESCAPE:
			{
				CMsgScreen *pScreenMessage = new CMsgScreen(KEY_ESC);
				CMain::getInstance()->pushMessage(pScreenMessage);
			}
			return 0;
			}

			CMsgScreen *pMessage = new CMsgScreen(EDIT_UPDATE);
			CMain::getInstance()->pushMessage(pMessage);
		}

		else if (uMsg == WM_LBUTTONDOWN)
		{
			CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

			POINT pt;

			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);

			MapWindowPoints(hwnd, GetParent(hwnd), &pt, 1);
			pServer->m_cursor.setID(1);
			pServer->m_cursor.setX((SHORT)pt.x);
			pServer->m_cursor.setY((SHORT)pt.y);
			pServer->m_cursor.setXSpeed(0);
			pServer->m_cursor.setYSpeed(0);
			pServer->m_cursor.setTouchStatus(CCursor::TS_PRESS_DOWN);

			CMsgScreenTouch *pMessage = new CMsgScreenTouch(pServer->m_pListener, pServer->m_cursor);
			CMain::getInstance()->pushMessage(pMessage);

			SetCapture(hwnd);
			return CallWindowProc(preEditProc, hwnd, uMsg, wParam, lParam);
		}
		else if (uMsg == WM_LBUTTONUP)
		{
			CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

			POINT pt;

			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);

			MapWindowPoints(hwnd, GetParent(hwnd), &pt, 1);

			pServer->m_cursor.setID(1);
			pServer->m_cursor.setX((SHORT)pt.x);
			pServer->m_cursor.setY((SHORT)pt.y);
			pServer->m_cursor.setXSpeed(0);
			pServer->m_cursor.setYSpeed(0);
			pServer->m_cursor.setTouchStatus(CCursor::TS_PRESS_UP);

			CMsgScreenTouch *pMessage = new CMsgScreenTouch(pServer->m_pListener, pServer->m_cursor);
			CMain::getInstance()->pushMessage(pMessage);

			::ReleaseCapture();
			return CallWindowProc(preEditProc, hwnd, uMsg, wParam, lParam);
		}
		else if (uMsg == WM_IME_ENDCOMPOSITION)
		{
			CMsgScreen *pMessage = new CMsgScreen(EDIT_UPDATE);
			CMain::getInstance()->pushMessage(pMessage);
		}

		return CallWindowProc(preEditProc, hwnd, uMsg, wParam, lParam);
	}
}