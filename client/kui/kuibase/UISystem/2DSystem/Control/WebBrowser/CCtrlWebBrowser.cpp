#include "OpenHMI.h"
#include <atlbase.h>
#include <atlwin.h>

namespace OpenHMI
{
	CCtrlWebBrowser::CCtrlWebBrowser()
		: m_pContext(NULL)
		, m_StrCurURL()
	{
		setCanHitTest(FALSE);
		notAcceptFocus();
	}

	CCtrlWebBrowser::~CCtrlWebBrowser()
	{
		
	}

	BOOL CCtrlWebBrowser::navigate2(const String& strURL)
	{
		if(m_pContext == NULL)
			return FALSE;

		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

		
		VARIANT url;
		VariantInit(&url);

		url.vt = VT_LPWSTR;
		url.bstrVal = SysAllocString((const CHAR*)strURL);
		::SendMessage(pServer->getScreenContext()->hWnd, WM_USER + 1, (WPARAM)m_pContext, (LPARAM)&url);

		//BOOL bRet = m_pContext->pWeb->OpenURL(&url);
		VariantClear(&url);
		SysFreeString(url.bstrVal);

		return TRUE;
	}

	void CCtrlWebBrowser::stopUrl()
	{
		if (m_pContext == NULL)
			return;

		m_pContext->pWeb->StopUrl();
	}

	void CCtrlWebBrowser::refresh()
	{
		if(m_pContext == NULL)
			return;

		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		
		::MoveWindow(m_pContext->hWnd, getParent()->getX() + getX(), getParent()->getY()+getY(), getPreferredSize().getX(), getPreferredSize().getY(), TRUE);
		RECT rc = { 0, 0, getPreferredSize().getX(), getPreferredSize().getY() };
		m_pContext->pWeb->SetWebRect(&rc);

		show(FALSE);
	}

	void CCtrlWebBrowser::onMove(INT iX, INT iY)
	{
		return;

		if (m_pContext == NULL)
			return;


		BOOLEAN bInInner = hitTest(iX, iY);

		if (getEnable())
		{
			if (bInInner)
			{
				SetFocus(m_pContext->hWnd);
			}
			else
			{
				SetFocus(CScreenTouchServer::getInstance()->getScreenContext()->hWnd);
			}
		}
	}

	void CCtrlWebBrowser::onLostHover()
	{
		if (getHoverFocus() == FALSE)
			return;

		setHoverFocus(FALSE);
	}

	void CCtrlWebBrowser::littleRefresh()
	{
		if (m_pContext == NULL)
			return;

		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

		::MoveWindow(m_pContext->hWnd, getParent()->getX() + getX(), getParent()->getY() + getY(), getPreferredSize().getX(), getPreferredSize().getY(), TRUE);
		RECT rc = { 0, 0, getPreferredSize().getX(), getPreferredSize().getY() };
		m_pContext->pWeb->SetWebRect(&rc);
	}

	void CCtrlWebBrowser::show(BOOLEAN bValue)	
	{
		if(m_pContext == NULL)
			return;

		if (getVisible() == bValue)
			return;

		setVisible(bValue);

		if(bValue == TRUE)
		{
			::ShowWindow(m_pContext->hWnd, SW_SHOW);
#if 0
			AttachThreadInput(
				::GetWindowThreadProcessId(::GetForegroundWindow(), NULL),  //当前焦点窗口的线程ID
				::GetCurrentThreadId(),  //自己的线程ID
				TRUE);

			::SetFocus(m_pContext->hWnd);

			AttachThreadInput(::GetWindowThreadProcessId(::GetForegroundWindow(), NULL), GetCurrentThreadId(), FALSE);
#endif
		}
		else
		{
			::ShowWindow(m_pContext->hWnd, SW_HIDE);
		}
	}

	BOOLEAN CCtrlWebBrowser::getFocus()
	{
		BOOLEAN bSetFocus;

		if (m_pContext == NULL)
		{
			return FALSE;
		}
		AttachThreadInput(
			::GetWindowThreadProcessId(::GetForegroundWindow(), NULL),  //当前焦点窗口的线程ID
			::GetCurrentThreadId(),  //自己的线程ID
			TRUE);
		bSetFocus = ::GetFocus() == m_pContext->hWnd;
		AttachThreadInput(::GetWindowThreadProcessId(::GetForegroundWindow(), NULL), GetCurrentThreadId(), FALSE);
		return bSetFocus;
	}

	void CCtrlWebBrowser::setWebBrsID(UINT uiID)
	{
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

		if(uiID < 1)
		{
			return;
		}

		m_pContext = pServer->getWebBrowserContext(uiID-1);

	}
}