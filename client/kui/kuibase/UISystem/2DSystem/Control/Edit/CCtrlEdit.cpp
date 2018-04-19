#include "OpenHMI.h"
#include <Richedit.h>

namespace OpenHMI
{
	CCtrlEdit::CCtrlEdit()
		: m_pContext(NULL)
		, m_bSetFocus(FALSE)
	{
		
	}

	CCtrlEdit::~CCtrlEdit()
	{
		DeleteObject((HGDIOBJ)(HFONT)(m_hFont));
	}

	void CCtrlEdit::refresh()
	{
		if(m_pContext == NULL)
			return;
		
		::MoveWindow(m_pContext->hWnd, getParent()->getX() + getX(), getParent()->getY()+ getY(), getPreferredSize().getX(), getPreferredSize().getY(), TRUE);		
		::InvalidateRect(m_pContext->hWnd, NULL, true);
		::UpdateWindow(m_pContext->hWnd);
	}

	void CCtrlEdit::show(BOOLEAN bValue)	
	{
		if(m_pContext == NULL)
			return;

		setVisible(bValue);

		if(bValue == TRUE)
		{
			::ShowWindow(m_pContext->hWnd, SW_SHOW);
		}
		else
		{
			::ShowWindow(m_pContext->hWnd, SW_HIDE);
		}
	}
	
	void CCtrlEdit::onTouchPress(INT iX, INT iY)
	{
		if (getEnable())
		{
			setTouchFocus(TRUE);
		}
	}

	void CCtrlEdit::setEditID(UINT uiID)
	{
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

		if(uiID < 1)
		{
			return;
		}

		m_pContext = pServer->getEditContext(uiID-1);
	}

	void CCtrlEdit::setFont(INT yHeight, COLORREF color, const String& strName)
	{
		if(m_pContext == NULL)
			return;

		CHARFORMAT cf;
		ZeroMemory(&cf, sizeof(CHARFORMAT));
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE |
			CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;
		cf.dwEffects = 0;

		cf.crTextColor = color;
		
		cf.yHeight = yHeight * yHeight;
		wcscpy_s(cf.szFaceName, (const CHAR*)strName);

		::SendMessage(m_pContext->hWnd, EM_SETCHARFORMAT, 0, (LPARAM)&cf);
	}

	void CCtrlEdit::setFontBkClr(COLORREF color)
	{
		if (m_pContext == NULL)
			return;
		CHARFORMAT2 cf;
		ZeroMemory(&cf, sizeof(CHARFORMAT2));
		cf.cbSize = sizeof(CHARFORMAT2);
		cf.dwMask = CFM_BACKCOLOR;        
		cf.crBackColor = color;
		::SendMessage(m_pContext->hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	}

	void CCtrlEdit::setBkColor(COLORREF color)
	{
		if (m_pContext == NULL)
			return;

		::SendMessage(m_pContext->hWnd, EM_SETBKGNDCOLOR, 0, color);
	}

	void CCtrlEdit::setContent(const CHAR* str)
	{
		if (m_pContext == NULL)
			return;

		SetWindowText(m_pContext->hWnd, str);
	}

	void CCtrlEdit::setLimitText(INT num)
	{
		if (m_pContext == NULL)
			return;

		SendMessage(m_pContext->hWnd, EM_LIMITTEXT, num, 0);
	}

	const String& CCtrlEdit::getContent()
	{
		if (m_pContext == NULL)
		{
			m_strContent = L"";
			return m_strContent;
		}

		CHAR buf[MAX_PATH] = {0};

		GetWindowText(m_pContext->hWnd, buf, MAX_PATH);

		m_strContent = buf;

		return m_strContent;
	}

	void CCtrlEdit::setFocus(BOOLEAN bValue)
	{		
		if (m_pContext == NULL)
		{
			return;
		}	

		if (!IsWindowVisible(m_pContext->hWnd))
			return;

		if (bValue == TRUE)
		{
			AttachThreadInput(
				::GetWindowThreadProcessId(::GetForegroundWindow(), NULL),  //当前焦点窗口的线程ID
				::GetCurrentThreadId(),  //自己的线程ID
				TRUE);

			::SetFocus(m_pContext->hWnd);

			AttachThreadInput(::GetWindowThreadProcessId(::GetForegroundWindow(), NULL), GetCurrentThreadId(), FALSE);
		}		
	}

	BOOLEAN CCtrlEdit::getFocus()
	{
		if (m_pContext == NULL)
		{
			return FALSE;
		}
		AttachThreadInput(
			::GetWindowThreadProcessId(::GetForegroundWindow(), NULL),  //当前焦点窗口的线程ID
			::GetCurrentThreadId(),  //自己的线程ID
			TRUE);
		m_bSetFocus = ::GetFocus() == m_pContext->hWnd;
		AttachThreadInput(::GetWindowThreadProcessId(::GetForegroundWindow(), NULL), GetCurrentThreadId(), FALSE);
		return m_bSetFocus;
	}
}
