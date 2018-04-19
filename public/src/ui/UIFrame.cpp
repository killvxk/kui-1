// UIFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "UIFrame.h"
#include "dwmapi.h"
#include "UI/ZipSkin.h"

IMPLEMENT_DYNCREATE(UIFrameShadow, UI_FRAME_BASE)

UIFrameShadow::UIFrameShadow()
	:UIView(NULL, 0)
{
	m_shadow_image = AfxUIGetImageEx(_T("shadow.png"));
	m_dc = NULL;
	m_parent = NULL;
}

UIFrameShadow::~UIFrameShadow()
{

}

void UIFrameShadow::Destroy()
{
	UIView::Destroy();
	if (m_shadow_image)
	{
		delete m_shadow_image;
		m_shadow_image = NULL;
	}

	if (m_dc)
	{
		delete m_dc;
		m_dc = NULL;
	}
}

BOOL UIFrameShadow::Create(CRect rc, HWND hParent)
{
	return UI_FRAME_BASE::CreateEx(WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW, NULL, _T("Shadow"), WS_POPUP, rc, CWnd::FromHandle(hParent), NULL, NULL);
}

BEGIN_MESSAGE_MAP(UIFrameShadow, UI_FRAME_BASE)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_REDRAW_WINDOW, OnRedrawWindow)
END_MESSAGE_MAP()

int UIFrameShadow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	::PostMessage(m_hWnd, WM_REDRAW_WINDOW, 0, 0);
	return 0;
}

BOOL UIFrameShadow::ShowWindow(int nCmdShow)
{
	return UI_FRAME_BASE::ShowWindow(nCmdShow);
}

LRESULT UIFrameShadow::OnRedrawWindow(WPARAM wParam, LPARAM lParam)
{
	OnPaint();
	return 0;
}

void UIFrameShadow::OnShowWindow(BOOL bShow,UINT nStatus)
{

}

void UIFrameShadow::Draw(UIDC* dc)
{
	dc->Clear(Gdiplus::Color(0,0,0,0));
	//dc->Clear(Gdiplus::Color(255,255,255,255));
	Gdiplus::Rect rc = Rect();
	Gdiplus::Image* m = m_shadow_image;
	if (m == NULL) return;
	// 绘制阴影
	// 左上角
	dc->DrawImage(m, Gdiplus::Rect(0,0,15,15), Gdiplus::Rect(0,0,15,15));
	// 上中
	dc->DrawImage(m, Gdiplus::Rect(15,0,rc.Width-15-15,15), Gdiplus::Rect(15,0,m->GetWidth()-15-15,15));
	// 右上
	dc->DrawImage(m, Gdiplus::Rect(rc.Width-15,0,15,15), Gdiplus::Rect(m->GetWidth()-15,0,15,15));
	// 右中
	dc->DrawImage(m, Gdiplus::Rect(rc.Width-15,15,15,rc.Height-15-15), Gdiplus::Rect(m->GetWidth()-15,15,15,m->GetHeight()-15-15));
	// 右下
	dc->DrawImage(m, Gdiplus::Rect(rc.Width-15,rc.Height-15,15,15), Gdiplus::Rect(m->GetWidth()-15,m->GetHeight()-15,15,15));
	// 中下
	dc->DrawImage(m, Gdiplus::Rect(15,rc.Height-15,rc.Width-15-15,15), Gdiplus::Rect(15,m->GetHeight()-15,m->GetWidth()-15-15,15));
	// 左下
	dc->DrawImage(m, Gdiplus::Rect(0,rc.Height-15,15,15), Gdiplus::Rect(0,m->GetHeight()-15,15,15));
	// 左中
	dc->DrawImage(m, Gdiplus::Rect(0,15,15,rc.Height-15-15), Gdiplus::Rect(0,15,15,m->GetHeight()-15-15));
}

void UIFrameShadow::OnPaint()
{
	{
		CPaintDC	dc(this);
	}

	//if (!IsWindowVisible())
	//{
	//	return;
	//}

	//if (m_parent && ::IsWindow(m_parent->m_hWnd))
	//{
	//	if (!m_parent->IsWindowVisible())
	//	{
	//		return;
	//	}
	//}
	//else
	//{
	//	return;
	//}

	Gdiplus::Rect rc = Rect();

	CRect rcWin;
	GetWindowRect(rcWin);

	if (m_dc == NULL)
	{
		m_dc = new UIDC(rcWin.Width(), rcWin.Height());
	}
	else
	{
		if (m_dc->m_width != rcWin.Width() || m_dc->m_height != rcWin.Height())
		{
			delete m_dc;
			m_dc = new UIDC(rcWin.Width(), rcWin.Height());
		}
	}

	m_dc->Reset();

	Draw(m_dc);

	{
		HDC hdc_dst = ::GetWindowDC(m_hWnd);
		HDC hdc_src = CreateCompatibleDC(hdc_dst);

		HBITMAP hBitmap = NULL;
		m_dc->m_image->GetHBITMAP(Gdiplus::Color(0,0,0,0), &hBitmap);

		if (hBitmap)
		{
			HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdc_src, hBitmap);

			POINT pt_dst = {rcWin.left,rcWin.top};
			POINT pt_src = {0,0};
			SIZE size = {FrameRect().Width,FrameRect().Height};
			BLENDFUNCTION func;
			func.BlendOp = AC_SRC_OVER;
			func.BlendFlags = 0;
			func.SourceConstantAlpha = 255;
			func.AlphaFormat = AC_SRC_ALPHA;
			::UpdateLayeredWindow(m_hWnd, hdc_dst, &pt_dst, &size, hdc_src, &pt_src, 0, &func, ULW_ALPHA);

			SelectObject(hdc_src, hBitmapOld);
			DeleteDC(hdc_src);
			::ReleaseDC(m_hWnd, hdc_dst);

			DeleteObject(hBitmap);
		}
	}
}

void UIFrameShadow::OnSize(UINT nType, int cx, int cy)
{
	UI_FRAME_BASE::OnSize(nType, cx, cy);

	SetFrameRect(Gdiplus::Rect(0,0,cx,cy));

	OnPaint();
}

void UIFrameShadow::OnLButtonDown(UINT nFlags, CPoint point)
{
	AddRef();
	if (m_parent)
	{
		m_parent->OnLButtonDown(nFlags, point);
	}
}

void UIFrameShadow::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_parent)
	{
		m_parent->OnLButtonUp(nFlags, point);
	}
	Release();
}

BOOL UIFrameShadow::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void UIFrameShadow::OnNcPaint()
{

}

// UIFrame

IMPLEMENT_DYNCREATE(UIFrame, UI_FRAME_BASE)

UIFrame::UIFrame()
	:UIView(NULL,0)
{
	AddRef();

	m_dc = NULL;

	m_theme_bg_image = NULL;

	m_need_draw_rc = Gdiplus::Rect(0,0,0,0);

	m_bDrawAllView = false;

	m_is_max_fullscreen = false;
	m_is_can_user_drag_size = false;
	m_is_user_drag_size = false;

	m_drag_code = HTCLIENT;

	m_is_no_zorder = false;

	m_min_width = 640;
	m_min_height = 480;

	m_client_off_x = 0;
	m_client_off_y = 0;

	m_shadow_window = NULL;
}

UIFrame::~UIFrame()
{
	m_timer_map.clear();
	if (m_dc)
	{
		delete m_dc;
		m_dc = NULL;
	}
	if (m_theme_bg_image)
	{
		delete m_theme_bg_image;
		m_theme_bg_image = NULL;
	}
}

void UIFrame::Destroy()
{
	//if (m_shadow_window)
	//{
	//	m_shadow_window->ShowWindow(SW_HIDE);
	//}

	if (m_theme_bg_image)
	{
		delete m_theme_bg_image;
		m_theme_bg_image = NULL;
	}

	if (m_shadow_window)
	{
		m_shadow_window->Destroy();
		m_shadow_window = NULL;
	}

	if (::IsWindow(m_hWnd))
	{
		::DestroyWindow(m_hWnd);
		return;
	}

	UIView::Destroy();

	std::map<DWORD, AutoRef<UIView>>::iterator iter = m_timer_map.begin();
	for (; iter != m_timer_map.end(); ++iter)
	{
		::KillTimer(m_hWnd, iter->first);
	}
	m_timer_map.clear();

	m_hover_view = NULL;
	m_capture_view = NULL;
	m_focus_view = NULL;
	m_prev_focus_view = NULL;
	m_modal_view = NULL;

	if (m_dc)
	{
		UIDC* dc = m_dc;
		m_dc = NULL;
		delete dc;
	}
}

bool UIFrame::SetTimer(AutoRef<UIView> view, DWORD timer_id, DWORD elapse_millsec)
{
	UINT_PTR ret = ::SetTimer(m_hWnd, timer_id, elapse_millsec, NULL);
	if (ret)
	{
		m_timer_map[timer_id] = view;
	}
	return ret != 0;
}

bool UIFrame::KillTimer(DWORD timer_id)
{
	BOOL ret = ::KillTimer(m_hWnd, timer_id);
	m_timer_map.erase(timer_id);
	return true;
}

bool UIFrame::KillTimer(AutoRef<UIView> view)
{
	bool is_kill = false;
	std::map<DWORD, AutoRef<UIView>>::iterator iter = m_timer_map.begin();
	for (; iter != m_timer_map.end(); )
	{
		if (iter->second == view)
		{
			std::map<DWORD, AutoRef<UIView>>::iterator iter2 = iter;
			if (::IsWindow(m_hWnd))
			{
				::KillTimer(m_hWnd, iter->first);
			}
			iter++;
			m_timer_map.erase(iter2);
			is_kill = true;
		}
		else
		{
			++iter;
		}
	}
	return is_kill;
}

BOOL UIFrame::CreateDefault(bool is_shadow, Gdiplus::Rect rc, LPCTSTR name)
{
	if (is_shadow)
	{
		return Create(is_shadow, rc, name, WS_POPUP | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE, WS_EX_ACCEPTFILES);
	}
	else
	{
		return Create(is_shadow, rc, name, WS_POPUP | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE, WS_EX_LAYERED | WS_EX_ACCEPTFILES);
	}
}

BOOL UIFrame::Create(bool is_shadow, Gdiplus::Rect rc, LPCTSTR name, DWORD style, DWORD exstyle,CWnd* pParentWnd)
{
	if (is_shadow)
	{
		//CRect rc_shadow(rc.X,rc.Y,rc.GetRight(),rc.GetBottom());

		//rc_shadow.InflateRect(9,9,15,15);

		//m_shadow_window = new UIFrameShadow();
		//m_shadow_window->Create(rc_shadow, NULL);

		//BOOL ret = UI_FRAME_BASE::CreateEx(exstyle, NULL, name ? name :_T("Main"), style, CRect(rc.X, rc.Y, rc.X+rc.Width, rc.Y+rc.Height), m_shadow_window, NULL, NULL);
		//if (ret)
		//{
		//	m_shadow_window->m_parent = this;
		//	if (IsWindowVisible())
		//	{
		//		m_shadow_window->ShowWindow(SW_SHOW);
		//	}
		//	return ret;
		//}
		//return ret;



		CRect rc_shadow(rc.X,rc.Y,rc.GetRight(),rc.GetBottom());

		rc_shadow.InflateRect(9,9,15,15);

		BOOL ret = UI_FRAME_BASE::CreateEx(exstyle, NULL, name ? name :_T("Main"), style, CRect(rc.X, rc.Y, rc.X+rc.Width, rc.Y+rc.Height), pParentWnd, NULL, NULL);

		if (ret)
		{
			m_shadow_window = new UIFrameShadow();
			m_shadow_window->Create(rc_shadow, m_hWnd);

			if (IsWindowVisible())
			{
				m_shadow_window->ShowWindow(SW_SHOW);
			}
			return ret;
		}
		return ret;
	}
	else
	{
		BOOL ret = UI_FRAME_BASE::CreateEx(exstyle, NULL, name ? name :_T("Main"), style, CRect(rc.X, rc.Y, rc.X+rc.Width, rc.Y+rc.Height), pParentWnd, NULL, NULL);
		return ret;
	}
}

void UIFrame::EnableLayeredWindow(bool is_enable)
{
	if (is_enable)
	{
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::PostMessage(m_hWnd,WM_REDRAW_WINDOW, 0,0);
	}
	else
	{
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) & (~WS_EX_LAYERED));
		Invalidate();
	}
}

void UIFrame::EnableUserDragSize(bool is_enable)
{
	m_is_can_user_drag_size = is_enable;
	//if (is_enable)
	//{
	//	::SetWindowLong(m_hWnd, GWL_STYLE, ::GetWindowLong(m_hWnd, GWL_STYLE) | WS_SIZEBOX);
	//}
	//else
	//{
	//	::SetWindowLong(m_hWnd, GWL_STYLE, ::GetWindowLong(m_hWnd, GWL_STYLE) & (~WS_SIZEBOX));
	//}
}

void UIFrame::ClientToView(CPoint& point, UIView* view)
{
	CRect rc;
	GetWindowRect(rc);

	ClientToScreen(&point);

	Gdiplus::Rect rc2 = view->WindowRect();

	point.x += -rc.left - rc2.X;
	point.y += -rc.top - rc2.Y;
}

void AddRect(Gdiplus::Rect& rc, const Gdiplus::Rect& rc2)
{
	if (rc.Width <= 0 && rc.Height <= 0)
	{
		rc = rc2;
		return;
	}
	else if (rc2.Width <= 0 && rc2.Height <= 0)
	{
		return;
	}

	Gdiplus::Rect rc3;
	Gdiplus::Rect::Union(rc3, rc, rc2);

	rc = rc3;
}

void UIFrame::RedrawView(UIView* view)
{
	if (m_hWnd == NULL) return;

	if (m_need_draw_rc.Width <= 0 || m_need_draw_rc.Height <= 0)
	{
		::PostMessage(m_hWnd,WM_REDRAW_WINDOW, 0, (LPARAM)view);
	}

	Gdiplus::Rect rc = view->WindowRect();
	AddRect(m_need_draw_rc, rc);
}
void UIFrame::RedrawAllView()
{
	if (m_hWnd == NULL) return;

	if (!m_bDrawAllView)
	{
		::PostMessage(m_hWnd,WM_REDRAW_WINDOW, 0, 0);
	}
	Gdiplus::Rect win_rc = Rect();
	AddRect(m_need_draw_rc, win_rc);
	m_bDrawAllView = true;
}
bool UIFrame::IsWindowView()
{
	return true;
}

BOOL UIFrame::ShowWindow(int nCmdShow)
{
	return UI_FRAME_BASE::ShowWindow(nCmdShow);
}

void UIFrame::DoModal()
{
	//MoveWindow(rc.X,rc.Y,rc.Width,rc.Height);
	ShowWindow(SW_SHOW);

	SetForegroundWindow();

	int uMsg = 0;
	MSG msg;

	// 主消息循环:
	while (true)
	{
		int ret = 0;
		if (::IsWindow(m_hWnd))
		{
			if (!::IsWindowVisible(m_hWnd))
			{
				break;
			}
		}
		else
		{
			break;
		}

		ret = GetMessageW(&msg, NULL, 0, 0);
		if (ret != 0)
		{
			uMsg = msg.message;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			break;
		}
	}
	return;
}

BEGIN_MESSAGE_MAP(UIFrame, UI_FRAME_BASE)
	ON_WM_NCHITTEST()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_NCCALCSIZE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_WINDOWPOSCHANGING()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave2)
	ON_MESSAGE(WM_IME_CHAR, OnImeChar2)
	ON_MESSAGE(WM_IME_COMPOSITION, OnImeComposition)
	ON_MESSAGE(WM_REDRAW_WINDOW, OnRedrawWindow)
	ON_MESSAGE(WM_WINDOW_INIT, OnInitWindow)
	ON_MESSAGE(WM_TIMER, OnTimer2)
	ON_MESSAGE(WM_CLOSE, OnCloseWindow)
END_MESSAGE_MAP()


// UIFrame 消息处理程序

//#pragma comment(lib, "dwmapi.lib")

int UIFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ((::GetWindowLong(m_hWnd, GWL_STYLE) & WS_SIZEBOX) == WS_SIZEBOX)
	{
		// 可以拉伸的窗口，必须设成LAYERED，这样才不会有白边
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	}

	::SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) & (~WS_EX_CLIENTEDGE));

	//if ((::GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) == WS_EX_LAYERED)
	{
		::PostMessage(m_hWnd,WM_REDRAW_WINDOW, 0, 0);
	}
	::PostMessage(m_hWnd,WM_WINDOW_INIT, 0, 0);

	return 0;
}

static void DoInit(UIView* view)
{
	std::vector< AutoRef<UIView> > childs = view->m_childs;
	std::vector< AutoRef<UIView> >::iterator iter = childs.begin();
	for (; iter != childs.end(); ++iter)
	{
		(*iter)->DoInit();
		DoInit(*iter);
	}
}

LRESULT UIFrame::OnInitWindow(WPARAM wParam, LPARAM lParam)
{
	::DoInit(this);
	//PostMessage(WM_TIMER, 0, 0);
	return 0;
}

BOOL UIFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_TIMER)
	{
		int i = 0;
	}
	//if (pMsg->message == WM_IME_CHAR)
	//{
	//	OnImeChar2(pMsg->wParam, pMsg->lParam);
	//	return TRUE;
	//}
	return UI_FRAME_BASE::PreTranslateMessage(pMsg);
}

LRESULT UIFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_IME_ENDCOMPOSITION)
	{
		HIMC hImc = ImmGetContext(m_hWnd);
		WCHAR buffer[4096] = {0};
		LONG ret = ImmGetCompositionString(hImc, GCS_RESULTSTR, buffer, 4096-1);
		int len = ret / 2;
		for (int i = 0; i < len; ++i)
		{
			OnImeChar2((WPARAM)buffer[i], 0);
		}
		ImmReleaseContext(m_hWnd, hImc);
	}

/*
	if (message == WM_IME_NOTIFY && wParam == IMN_CHANGECANDIDATE)
	{
		HIMC hImc = ImmGetContext(m_hWnd);
		WCHAR buffer[4096] = {0};
		LONG ret = ImmGetCompositionString(hImc, GCS_RESULTSTR, buffer, 4096-1);
		int len = ret / 2;
		m_ime_string.SetString(buffer, len);
		ImmReleaseContext(m_hWnd, hImc);
	}
	else if (message == WM_IME_NOTIFY && wParam == IMN_CLOSECANDIDATE)
	{
		if (m_ime_string.GetLength() > 0)
		{
			for (int i = 0; i < m_ime_string.GetLength(); ++i)
			{
				OnImeChar2((WPARAM)m_ime_string.GetAt(i), 0);
			}
			m_ime_string = _T("");
		}
	}
*/

	return UI_FRAME_BASE::WindowProc(message, wParam, lParam);
}

void UIFrame::OnSize(UINT nType, int cx, int cy)
{
	UI_FRAME_BASE::OnSize(nType, cx, cy);

	CRect rc;
	GetWindowRect(rc);

	if ((GetKeyState(VK_SHIFT) & 0x8000) == 0x8000)
	{
		int kk = 0;
	}

	SetFrameRect(Gdiplus::Rect(0,0,rc.Width(),rc.Height()));

	if (m_dc)
	{
		delete m_dc;
		m_dc = NULL;
	}
	m_dc = new UIDC(rc.Width(), rc.Height());

	m_need_draw_rc = Rect();

	this->OnSizer();

	if ((::GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) == WS_EX_LAYERED)
	{
		DoPaint();
	}
	else
	{
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
}

BOOL UIFrame::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void UIFrame::OnPaint()
{
	CRect rc;
	GetWindowRect(rc);

	m_need_draw_rc = Gdiplus::Rect(0,0,rc.Width(), rc.Height());

	DoPaint();

	{
		CPaintDC dc(this);
	}
}

void UIFrame::DoPaint(UIView* view)
{
	if (m_dc == NULL)
	{
		return;
	}

	if (!::IsWindow(m_hWnd))
	{
		return;
	}

	CRect rcWin;
	GetWindowRect(rcWin);

	if ((GetKeyState(VK_SHIFT) & 0x8000) == 0x8000)
	{
		int kk = 0;
	}

	{
		Gdiplus::Rect rc = FrameRect();
		CRect rc_client;
		GetClientRect(rc_client);

		if (m_dc->m_width != rcWin.Width() || m_dc->m_height != rcWin.Height())
		{
			delete m_dc;
			m_dc = new UIDC(rcWin.Width(), rcWin.Height());
		}

		if (m_need_draw_rc.Width == 0 || m_need_draw_rc.Height == 0)
		{
			m_need_draw_rc = Gdiplus::Rect(0,0,rcWin.Width(),rcWin.Height());
		}
	}

	//{
	//	TCHAR buffer[MAX_PATH];
	//	//wsprintf(buffer, _T("%i x %i\r\n"), rcWin.Width(), rcWin.Height());
	//	wsprintf(buffer, _T("%i x %i\r\n"), FrameRect().Width, FrameRect().Height);
	//	OutputDebugString(buffer);
	//}

	Gdiplus::Rect rc = m_need_draw_rc;

	if (rc.X < 0) rc.X = 0;
	if (rc.Y < 0) rc.Y = 0;
	if (rc.GetRight() > FrameRect().GetRight()) rc.Width = FrameRect().GetRight() - rc.X;
	if (rc.GetBottom() > FrameRect().GetBottom()) rc.Height = FrameRect().GetBottom() - rc.Y;

	//m_dc->Reset();

	if ((::GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) == WS_EX_LAYERED)
	{
		rc = Gdiplus::Rect(0,0,FrameRect().Width,FrameRect().Height);
		m_dc->Clear(Gdiplus::Color(0,0,0,0));
		DrawView(m_dc,this);
	}
	else
	{
		if(view == NULL )
			DrawView(m_dc,this);
		else
			DrawView(m_dc,view, this);
	}
	//m_dc->SaveImage(CString(_T("222.png")));

	if ((::GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) == WS_EX_LAYERED)
	{
		HDC hdc_dst = ::GetWindowDC(m_hWnd);
		HDC hdc_src = CreateCompatibleDC(hdc_dst);

		HBITMAP hBitmap = NULL;
		m_dc->m_image->GetHBITMAP(Gdiplus::Color(0,0,0,0), &hBitmap);

		if (hBitmap)
		{
			HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdc_src, hBitmap);

			POINT pt_dst = {rcWin.left,rcWin.top};
			POINT pt_src = {0,0};
			SIZE size = {FrameRect().Width,FrameRect().Height};
			BLENDFUNCTION func;
			func.BlendOp = AC_SRC_OVER;
			func.BlendFlags = 0;
			func.SourceConstantAlpha = 255;
			func.AlphaFormat = AC_SRC_ALPHA;
			::UpdateLayeredWindow(m_hWnd, hdc_dst, &pt_dst, &size, hdc_src, &pt_src, 0, &func, ULW_ALPHA);

			SelectObject(hdc_src, hBitmapOld);
			DeleteDC(hdc_src);
			::ReleaseDC(m_hWnd, hdc_dst);

			DeleteObject(hBitmap);
		}
		else
		{
			DeleteDC(hdc_src);
			::ReleaseDC(m_hWnd, hdc_dst);
		}
	}
	else
	{
		HDC hdc_dst = ::GetWindowDC(m_hWnd);
		/*{
			Gdiplus::Graphics g(hdc_dst);

			//{
			//	TCHAR buffer[MAX_PATH];
			//	wsprintf(buffer, _T("%i %i %i x %i\r\n"), rc.X, rc.Y, rc.Width, rc.Height);
			//	OutputDebugString(buffer);
			//}

			HWND hWndChild = ::GetWindow(m_hWnd, GW_CHILD);
			while (hWndChild)
			{
				if (::IsWindowVisible(hWndChild))
				{
					CRect rc_child;
					::GetWindowRect(hWndChild, rc_child);

					int x = rc_child.left - rcWin.left;
					int y = rc_child.top - rcWin.top;
					int w = rc_child.Width();
					int h = rc_child.Height();
					g.SetClip(Gdiplus::Rect(x,y,w,h), Gdiplus::CombineModeExclude);
				}
				hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
			}

			Gdiplus::Rect rc = Rect();
			g.DrawImage(m_dc->m_image, rc, rc.X, rc.Y, rc.Width, rc.Height, Gdiplus::UnitPixel);
		}*/
		{
			{
				CDC dc;
				dc.Attach(hdc_dst);
				HWND hWndChild = ::GetWindow(m_hWnd, GW_CHILD);
				while (hWndChild)
				{
					if (::IsWindowVisible(hWndChild))
					{
						CRect rc_child;
						::GetWindowRect(hWndChild, rc_child);

						int l = rc_child.left - rcWin.left;
						int t = rc_child.top - rcWin.top;
						int r = l + rc_child.Width();
						int b = t + rc_child.Height(); 
						CRgn rgn;
						rgn.CreateRectRgn(l,t,r,b);
						dc.SelectClipRgn(&rgn,RGN_DIFF);
					}
					hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
				}
				dc.Detach();
			}
			Gdiplus::Rect rc = Rect();

			HBITMAP hBitmap = NULL;
			m_dc->m_image->GetHBITMAP(Gdiplus::Color(0,0,0,0), &hBitmap);

			if (hBitmap)
			{
				HDC hdc_src = CreateCompatibleDC(hdc_dst);

				HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdc_src, hBitmap);					

				BitBlt(hdc_dst,0,0,rc.Width,rc.Height,hdc_src,0,0, SRCCOPY);

				SelectObject(hdc_src, hBitmapOld);
				DeleteDC(hdc_src);

				DeleteObject(hBitmap);
			}
		}
		::ReleaseDC(m_hWnd, hdc_dst);
	}

	// 清空绘制区域
	m_need_draw_rc = Gdiplus::Rect(0,0,0,0);
	if(view == NULL)
		m_bDrawAllView = false;
}

void UIFrame::DrawView(UIDC* dc, UIView* view)
{
	if (!view->IsVisible())
	{
		return;
	}

	Gdiplus::Rect rc = view->FrameRect();
	if (rc.Width <= 0 || rc.Height <= 0)
	{
		return;
	}

	if (view->m_is_clip)
	{
		dc->SetClipRaw(view->WindowRect());
	}

	view->OnClip(dc); // 自定义剪切区域

	dc->Save();

	// 绘制控件
	view->OnPaint(dc);

	std::vector< AutoRef<UIView> >& childs = view->m_childs;
	std::vector< AutoRef<UIView> >::iterator iter = childs.begin();
	for (; iter != childs.end(); ++iter)
	{
		AutoRef<UIView>& v = *iter;

		dc->Save();

		dc->m_off_x += v->FrameRect().X;
		dc->m_off_y += v->FrameRect().Y;

		DrawView(dc,v);

		dc->Restore();
	}

	view->OnPostPaint(dc);

	dc->Restore();
}
void UIFrame::DrawView(UIDC* dc,UIView* viewNeed, UIView* view)
{
	if (!view->IsVisible())
	{
		return;
	}

	Gdiplus::Rect rc = view->FrameRect();
	if (rc.Width <= 0 || rc.Height <= 0)
	{
		return;
	}

	if (view->m_is_clip)
	{
		dc->SetClipRaw(view->WindowRect());
	}

	view->OnClip(dc); // 自定义剪切区域

	dc->Save();

	// 绘制控件
	if(view == viewNeed)
		view->OnPaint(dc);

	std::vector< AutoRef<UIView> >& childs = view->m_childs;
	std::vector< AutoRef<UIView> >::iterator iter = childs.begin();
	for (; iter != childs.end(); ++iter)
	{
		AutoRef<UIView>& v = *iter;

		dc->Save();

		dc->m_off_x += v->FrameRect().X;
		dc->m_off_y += v->FrameRect().Y;

		if(view == viewNeed)
			DrawView(dc, v);
		else
			DrawView(dc,viewNeed, v);
		dc->Restore();
	}

	view->OnPostPaint(dc);

	dc->Restore();
}

bool UIFrame::IsCanDrag(int x, int y)
{
	return true;
}

Gdiplus::Image* UIFrame::GetThemeBackgroud()
{
	return m_theme_bg_image;
}

void UIFrame::SetThemeBackgroud(Gdiplus::Image* image)
{
	if (m_theme_bg_image)
	{
		delete m_theme_bg_image;
		m_theme_bg_image = NULL;
	}
	m_theme_bg_image = image;
}

void UIFrame::SetViewFocus(UIView* v)
{
	AutoRef<UIView> prev_focus = m_focus_view;

	if (prev_focus != v)
	{
		m_focus_view = v;

		if (prev_focus)
		{
			prev_focus->OnKillFocus();
			prev_focus->RedrawAll();
		}
		if (m_focus_view)
		{
			m_focus_view->OnSetFocus();
			m_focus_view->RedrawAll();
		}
	}

}

void UIFrame::OnSetFocus(CWnd* pOldWnd)
{
	UI_FRAME_BASE::OnSetFocus(pOldWnd);
	if ((::GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) != WS_EX_LAYERED)
	{
		::PostMessage(m_hWnd,WM_REDRAW_WINDOW,0,0);
	}
	if (m_prev_focus_view)
	{
		m_focus_view = m_prev_focus_view;
		if (m_focus_view)
		{
			m_focus_view->OnSetFocus();
		}
		m_prev_focus_view = NULL;
	}
}

void UIFrame::OnKillFocus(CWnd* pNewWnd)
{
	UI_FRAME_BASE::OnKillFocus(pNewWnd);

	if ((::GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) != WS_EX_LAYERED)
	{
		::PostMessage(m_hWnd,WM_REDRAW_WINDOW,0,0);
	}
	if (m_focus_view)
	{
		m_prev_focus_view = m_focus_view;
		if (m_focus_view)
		{
			m_focus_view->OnKillFocus();
		}
		m_focus_view = NULL;
	}
	if(GetCapture() != NULL && !m_is_user_drag_size)
	{
		ReleaseCapture();
		m_capture_view = NULL;
	}
}

LRESULT UIFrame::HittestDrag(CPoint point)
{
	CRect rc;
	GetWindowRect(rc);

	int x = point.x;
	int y = point.y;
	int height = rc.Height();
	int width = rc.Width();

	if (m_is_can_user_drag_size)
	{
		if (x < 12 && y < 12)
		{
			return HTTOPLEFT;
		}
		if (x < 12 && y > height - 12)
		{
			return HTBOTTOMLEFT;
		}
		if (x > width - 12 && y < 12)
		{
			return HTTOPRIGHT;
		}
		if (x > width - 12 && y > height - 12)
		{
			return HTBOTTOMRIGHT;
		}

		if (x < 6)
		{
			return HTLEFT;
		}
		if (x > width - 6)
		{
			return HTRIGHT;
		}
		if (y < 6)
		{
			return HTTOP;
		}
		if (y > height - 6)
		{
			return HTBOTTOM;
		}
	}
	return HTCLIENT;
}

LRESULT UIFrame::OnNcHitTest(CPoint point)
{
	//UI_FRAME_BASE::OnNcHitTest(point);

	CRect rc;
	GetWindowRect(rc);

	point.x -= rc.left;
	point.y -= rc.top;

	LRESULT nHitCode = HittestDrag(point);

	if (m_is_can_user_drag_size)
	{
		if (nHitCode == HTTOPLEFT)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
			return HTCLIENT;
		}
		if (nHitCode == HTBOTTOMLEFT)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));
			return HTCLIENT;
		}
		if (nHitCode == HTTOPRIGHT)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENESW));
			return HTCLIENT;
		}
		if (nHitCode == HTBOTTOMRIGHT)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
			return HTCLIENT;
		}

		if (nHitCode == HTLEFT)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			return HTCLIENT;
		}
		if (nHitCode == HTRIGHT)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			return HTCLIENT;
		}
		if (nHitCode == HTTOP)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
			return HTCLIENT;
		}
		if (nHitCode == HTBOTTOM)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
			return HTCLIENT;
		}
	}

	AutoRef<UIView> hit = FindView(point.x, point.y);
	if (hit == NULL)
	{
		if (m_hover_view)
		{
			m_hover_view->OnMouseLeave();
			m_hover_view = NULL;
		}

		//OutputDebugString(_T("HTCAPTION\r\n"));
		return HTCAPTION;
	}

	if (hit)
	{
		Gdiplus::Rect rc_win = WindowRect();

		if (hit->IsCanDrag(point.x - rc_win.X, point.y - rc_win.Y))
		{
			if (m_hover_view)
			{
				m_hover_view->OnMouseLeave();
				m_hover_view = NULL;
			}

			//OutputDebugString(_T("HTCAPTION\r\n"));
			return HTCAPTION;
		}
	}

	//OutputDebugString(_T("HTCLIENT\r\n"));
	return HTCLIENT;
}

void UIFrame::SetWindowBottom()
{
	bool old_value = m_is_no_zorder;
	m_is_no_zorder = false;
	SetWindowPos(CWnd::FromHandle(HWND_BOTTOM), 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	m_is_no_zorder = old_value;
}

void UIFrame::MoveShadow()
{
	CRect rc;
	GetWindowRect(rc);
	MoveShadow(rc);
}

void UIFrame::MoveShadow(CRect win_rc)
{
	if (m_shadow_window)
	{
		win_rc.InflateRect(9,9,15,15);

		bool is_show = false;
		if (!IsIconic() && IsWindowVisible())
		{
			is_show = true;
		}

		CRect rc;
		m_shadow_window->GetWindowRect(rc);

		if (rc == win_rc)
		{
			return;
		}

		if (is_show)
		{
			m_shadow_window->SetWindowPos(CWnd::FromHandle(HWND_TOP), win_rc.left, win_rc.top, win_rc.Width(), win_rc.Height(), SWP_NOACTIVATE | SWP_NOCOPYBITS);
		}
		else
		{
			m_shadow_window->SetWindowPos(CWnd::FromHandle(HWND_TOP), win_rc.left, win_rc.top, win_rc.Width(), win_rc.Height(), SWP_NOACTIVATE | SWP_NOCOPYBITS);
		}

		//if (!IsIconic() && IsWindowVisible())
		//{
		//	m_shadow_window->ShowWindow(SW_SHOW);
		//}
		//m_shadow_window->MoveWindow(win_rc, TRUE);
	}
}

void UIFrame::OnDestroy()
{

}

void UIFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{

}

void UIFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{

}

void UIFrame::OnGetMinMaxInfo(MINMAXINFO* pMMI)
{
	//UI_FRAME_BASE::OnGetMinMaxInfo(pMMI);
	if (m_is_can_user_drag_size)
	{
		pMMI->ptMinTrackSize.x = m_min_width;
		pMMI->ptMinTrackSize.y = m_min_height;
	}
}

LRESULT UIFrame::OnTimer2(WPARAM wParam, LPARAM lParam)
{
	DWORD timer_id = (DWORD)wParam;
	std::map<DWORD, AutoRef<UIView>>::iterator iter = m_timer_map.find(timer_id);
	if (iter != m_timer_map.end())
	{
		if (!iter->second->OnTimer(timer_id))
		{
			KillTimer(iter->first);
		}
	}
	return 0;
}

void UIFrame::OnNcPaint()
{

}

void UIFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	if (bCalcValidRects)
	{
		m_client_off_x = lpncsp->rgrc[1].left - lpncsp->rgrc[2].left;
		m_client_off_y = lpncsp->rgrc[1].top - lpncsp->rgrc[2].top;

		lpncsp->rgrc[0] = CRect(lpncsp->lppos->x,lpncsp->lppos->y,lpncsp->lppos->x+lpncsp->lppos->cx,lpncsp->lppos->y+lpncsp->lppos->cy);

		lpncsp->rgrc[2] = lpncsp->rgrc[0];
		lpncsp->rgrc[1] = lpncsp->rgrc[0];
	}
	else
	{
		UI_FRAME_BASE::OnNcCalcSize(bCalcValidRects, lpncsp);
	}
}

void UIFrame::OnWindowPosChanged(WINDOWPOS* pos)
{
	UI_FRAME_BASE::OnWindowPosChanged(pos);

	//if (!m_is_max_fullscreen && IsZoomed())
	//{
	//	CRect rt;  
	//	SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)(RECT*)&rt,0);

	//	CRect rcWin;
	//	GetWindowRect(rcWin);

	//	if (rcWin.top == 0 && rcWin.left == 0 && (rcWin.Width() != rt.Width() || rcWin.Height() != rt.Height()))
	//	{
	//		MoveWindow(rt);
	//		return;
	//	}
	//}

	if ((::GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) == WS_EX_LAYERED)
	{
		if (!IsIconic())
		{
			OnSize(0, pos->cx, pos->cy);
			::PostMessage(m_hWnd,WM_REDRAW_WINDOW, 0,0);
		}
	}

	CRect rc(pos->x, pos->y, pos->x + pos->cx, pos->y + pos->cy);

	MoveShadow(rc);

	this->OnPosChanged();
}

void UIFrame::OnWindowPosChanging(WINDOWPOS* pos)
{
	UI_FRAME_BASE::OnWindowPosChanging(pos);

	if (m_is_no_zorder)
	{
		pos->flags |= (SWP_NOZORDER);
	}
	if(m_is_no_zorder && IsIconic())
	{
		ShowWindow(SW_SHOWNORMAL);
	}

	if (!m_is_max_fullscreen && IsZoomed())
	{
		CRect rt;
		SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)(RECT*)&rt,0);

		pos->cx = rt.Width();
		pos->cy = rt.Height();
		pos->x = 0;
		pos->y = 0;
	}
}

LRESULT UIFrame::OnCloseWindow(WPARAM wParam, LPARAM lParam)
{
	Destroy();
	return 0;
}

void UIFrame::OnShowWindow(BOOL bShow,UINT nStatus)
{
	UI_FRAME_BASE::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		if (m_shadow_window) m_shadow_window->ShowWindow(SW_SHOW);
	}
	else
	{
		if (m_shadow_window) m_shadow_window->ShowWindow(SW_HIDE);
	}
}

LRESULT UIFrame::OnRedrawWindow(WPARAM wParam, LPARAM lParam)
{
	DoPaint((UIView*) lParam);
	return 0;
}

void UIFrame::PostNcDestroy()
{
	Destroy();
	Release();
}

void UIFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	UI_FRAME_BASE::SetFocus();
	AutoRef<UIView> v = this->FindView(point.x, point.y);
	if (v)
	{
		while (!v->IsActive())
		{
			// 子View不接受输入，就向上级View输入
			v = v->Parent();
			if (!v)
			{
				return;
			}
		}

		if (v->IsActive())
		{
			SetViewFocus(v);

			ClientToView(point, v);
			v->OnLButtonDblClk(point.x, point.y);
		}
	}
}

void UIFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	::SetFocus(m_hWnd);
	SetCapture();

	GetCursorPos(&m_down_pos);
	{
		CRect rc;
		GetWindowRect(rc);
		m_down_rc = Gdiplus::Rect(rc.left,rc.top,rc.Width(),rc.Height());
	}

	if (m_is_can_user_drag_size)
	{
		LRESULT nHitCode = HittestDrag(point);
		if (nHitCode != HTCLIENT)
		{
			m_drag_code = (int)nHitCode;
			m_is_user_drag_size = true;
			return;
		}
	}

	AutoRef<UIView> v = this->FindView(point.x, point.y);
	if (v)
	{
		while (!v->IsActive())
		{
			// 子View不接受输入，就向上级View输入
			v = v->Parent();
			if (!v)
			{
				return;
			}
		}

		if (v->IsActive())
		{
			m_capture_view = v;
			SetViewFocus(v);

			ClientToView(point, v);
			v->OnLButtonDown(point.x, point.y);
		}
	}
}

void UIFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	m_is_user_drag_size = false;
	if (m_capture_view)
	{
		ClientToView(point, m_capture_view);
		AutoRef<UIView>	capture_view = m_capture_view;
		m_capture_view = NULL;
		capture_view->OnLButtonUp(point.x, point.y);
	}
	else if (m_focus_view)
	{
		ClientToView(point, m_focus_view);
		m_focus_view->OnLButtonUp(point.x, point.y);
	}
}

void UIFrame::OnRButtonDown(UINT nFlags, CPoint point)
{
	UI_FRAME_BASE::SetFocus();
	AutoRef<UIView> v = this->FindView(point.x, point.y);
	if (v)
	{
		while (!v->IsActive())
		{
			// 子View不接受输入，就向上级View输入
			v = v->Parent();
			if (!v)
			{
				return;
			}
		}

		if (v->IsActive())
		{
			m_capture_view = v;
			SetViewFocus(v);

			ClientToView(point, v);
			v->OnRButtonDown(point.x, point.y);
		}
	}
}

void UIFrame::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_capture_view)
	{
		ClientToView(point, m_capture_view);
		AutoRef<UIView>	capture_view = m_capture_view;
		m_capture_view = NULL;
		capture_view->OnRButtonUp(point.x, point.y);
	}
	else if (m_focus_view)
	{
		ClientToView(point, m_focus_view);
		m_focus_view->OnRButtonUp(point.x, point.y);
	}
}

void UIFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	{
		TRACKMOUSEEVENT event = {0};
		event.cbSize = sizeof(event);
		event.dwFlags = TME_LEAVE;
		event.hwndTrack = m_hWnd;
		TrackMouseEvent(&event);		
	}

	UI_FRAME_BASE::OnMouseMove(nFlags, point);

	if (m_is_user_drag_size)
	{
		GetCursorPos(&point);
		if (m_drag_code == HTTOPLEFT || m_drag_code == HTBOTTOMRIGHT)
		{
			if (m_drag_code == HTTOPLEFT)
			{
				int x = m_down_rc.X + (point.x - m_down_pos.x);
				int y = m_down_rc.Y + (point.y - m_down_pos.y);
				int w = m_down_rc.Width - (point.x - m_down_pos.x);
				int h = m_down_rc.GetBottom() - y;

				if (w < m_min_width)
				{
					x = m_down_rc.GetRight() - m_min_width;
					w = m_min_width;
				}
				if (h < m_min_height)
				{
					y = m_down_rc.GetBottom() - m_min_height;
					h = m_min_height;
				}

				if (m_shadow_window)
				{
					HDWP hDWP = BeginDeferWindowPos(2);
					CRect rc_shadow(x,y,x+w,y+h);
					rc_shadow.InflateRect(9,9,15,15);
					::DeferWindowPos(hDWP, m_shadow_window->m_hWnd, NULL, rc_shadow.left, rc_shadow.top, rc_shadow.Width(), rc_shadow.Height(), 0);
					::DeferWindowPos(hDWP, m_hWnd, m_shadow_window->m_hWnd, x, y, w, h, 0);
					EndDeferWindowPos(hDWP);
				}
				else
				{
					MoveWindow(x,y,w,h);
				}
			}
			else
			{
				int x = m_down_rc.X;
				int y = m_down_rc.Y;
				int w = m_down_rc.Width + (point.x - m_down_pos.x);
				int h = m_down_rc.Height + (point.y - m_down_pos.y);

				if (w < m_min_width)
				{
					w = m_min_width;
				}
				if (h < m_min_height)
				{
					h = m_min_height;
				}

				if (m_shadow_window)
				{
					HDWP hDWP = BeginDeferWindowPos(2);
					CRect rc_shadow(x,y,x+w,y+h);
					rc_shadow.InflateRect(9,9,15,15);
					::DeferWindowPos(hDWP, m_shadow_window->m_hWnd, NULL, rc_shadow.left, rc_shadow.top, rc_shadow.Width(), rc_shadow.Height(), 0);
					::DeferWindowPos(hDWP, m_hWnd, m_shadow_window->m_hWnd, x, y, w, h, 0);
					EndDeferWindowPos(hDWP);
				}
				else
				{
					MoveWindow(x,y,w,h);
				}
			}
		}
		else if (m_drag_code == HTTOPRIGHT || m_drag_code == HTBOTTOMLEFT)
		{
			if (m_drag_code == HTTOPRIGHT)
			{
				int x = m_down_rc.X;
				int y = m_down_rc.Y + (point.y - m_down_pos.y);
				int w = m_down_rc.Width + (point.x - m_down_pos.x);
				int h = m_down_rc.GetBottom() - y;

				if (w < m_min_width)
				{
					x = m_down_rc.X;
					w = m_min_width;
				}
				if (h < m_min_height)
				{
					y = m_down_rc.GetBottom() - m_min_height;
					h = m_min_height;
				}

				if (m_shadow_window)
				{
					HDWP hDWP = BeginDeferWindowPos(2);
					CRect rc_shadow(x,y,x+w,y+h);
					rc_shadow.InflateRect(9,9,15,15);
					::DeferWindowPos(hDWP, m_shadow_window->m_hWnd, NULL, rc_shadow.left, rc_shadow.top, rc_shadow.Width(), rc_shadow.Height(), 0);
					::DeferWindowPos(hDWP, m_hWnd, m_shadow_window->m_hWnd, x, y, w, h, 0);
					EndDeferWindowPos(hDWP);
				}
				else
				{
					MoveWindow(x,y,w,h);
				}
			}
			else
			{
				int x = m_down_rc.X + (point.x - m_down_pos.x);
				int y = m_down_rc.Y;
				int w = m_down_rc.GetRight() - x;
				int h = m_down_rc.Height + (point.y - m_down_pos.y);

				if (w < m_min_width)
				{
					x = m_down_rc.GetRight() - m_min_width;
					w = m_min_width;
				}
				if (h < m_min_height)
				{
					y = m_down_rc.Y;
					h = m_min_height;
				}

				if (m_shadow_window)
				{
					HDWP hDWP = BeginDeferWindowPos(2);
					CRect rc_shadow(x,y,x+w,y+h);
					rc_shadow.InflateRect(9,9,15,15);
					::DeferWindowPos(hDWP, m_shadow_window->m_hWnd, NULL, rc_shadow.left, rc_shadow.top, rc_shadow.Width(), rc_shadow.Height(), 0);
					::DeferWindowPos(hDWP, m_hWnd, m_shadow_window->m_hWnd, x, y, w, h, 0);
					EndDeferWindowPos(hDWP);
				}
				else
				{
					MoveWindow(x,y,w,h);
				}
			}
		}
		else if (m_drag_code == HTLEFT || m_drag_code == HTRIGHT)
		{
			if (m_drag_code == HTLEFT)
			{
				int x = m_down_rc.X + (point.x - m_down_pos.x);
				int y = m_down_rc.Y;
				int w = m_down_rc.GetRight() - x;
				int h = m_down_rc.Height;

				if (w < m_min_width)
				{
					x = m_down_rc.GetRight() - m_min_width;
					w = m_min_width;
				}

				if (m_shadow_window)
				{
					HDWP hDWP = BeginDeferWindowPos(2);
					CRect rc_shadow(x,y,x+w,y+h);
					rc_shadow.InflateRect(9,9,15,15);
					::DeferWindowPos(hDWP, m_shadow_window->m_hWnd, NULL, rc_shadow.left, rc_shadow.top, rc_shadow.Width(), rc_shadow.Height(), 0);
					::DeferWindowPos(hDWP, m_hWnd, m_shadow_window->m_hWnd, x, y, w, h, 0);
					EndDeferWindowPos(hDWP);
				}
				else
				{
					MoveWindow(x,y,w,h);
				}
			}
			else
			{
				int x = m_down_rc.X;
				int y = m_down_rc.Y;
				int w = m_down_rc.Width + (point.x - m_down_pos.x);
				int h = m_down_rc.Height;

				if (w < m_min_width)
				{
					x = m_down_rc.X;
					w = m_min_width;
				}

				if (m_shadow_window)
				{
					HDWP hDWP = BeginDeferWindowPos(2);
					CRect rc_shadow(x,y,x+w,y+h);
					rc_shadow.InflateRect(9,9,15,15);
					::DeferWindowPos(hDWP, m_shadow_window->m_hWnd, NULL, rc_shadow.left, rc_shadow.top, rc_shadow.Width(), rc_shadow.Height(), 0);
					::DeferWindowPos(hDWP, m_hWnd, m_shadow_window->m_hWnd, x, y, w, h, 0);
					EndDeferWindowPos(hDWP);
				}
				else
				{
					MoveWindow(x,y,w,h);
				}
			}
		}
		else if (m_drag_code == HTTOP || m_drag_code == HTBOTTOM)
		{
			if (m_drag_code == HTTOP)
			{
				int x = m_down_rc.X;
				int y = m_down_rc.Y + (point.y - m_down_pos.y);
				int w = m_down_rc.Width;
				int h = m_down_rc.GetBottom() - y;

				if (h < m_min_height)
				{
					y = m_down_rc.GetBottom() - m_min_height;
					h = m_min_height;
				}

				if (m_shadow_window)
				{
					HDWP hDWP = BeginDeferWindowPos(2);
					CRect rc_shadow(x,y,x+w,y+h);
					rc_shadow.InflateRect(9,9,15,15);
					::DeferWindowPos(hDWP, m_shadow_window->m_hWnd, NULL, rc_shadow.left, rc_shadow.top, rc_shadow.Width(), rc_shadow.Height(), 0);
					::DeferWindowPos(hDWP, m_hWnd, m_shadow_window->m_hWnd, x, y, w, h, 0);
					EndDeferWindowPos(hDWP);
				}
				else
				{
					MoveWindow(x,y,w,h);
				}
			}
			else
			{
				int x = m_down_rc.X;
				int y = m_down_rc.Y;
				int w = m_down_rc.Width;
				int h = m_down_rc.Height + (point.y - m_down_pos.y);

				if (h < m_min_height)
				{
					y = m_down_rc.Y;
					h = m_min_height;
				}

				if (m_shadow_window)
				{
					HDWP hDWP = BeginDeferWindowPos(2);
					CRect rc_shadow(x,y,x+w,y+h);
					rc_shadow.InflateRect(9,9,15,15);
					::DeferWindowPos(hDWP, m_shadow_window->m_hWnd, NULL, rc_shadow.left, rc_shadow.top, rc_shadow.Width(), rc_shadow.Height(), 0);
					::DeferWindowPos(hDWP, m_hWnd, m_shadow_window->m_hWnd, x, y, w, h, 0);
					EndDeferWindowPos(hDWP);
				}
				else
				{
					MoveWindow(x,y,w,h);
				}
			}
		}
		return;
	}


	if (m_capture_view)
	{
		ClientToView(point, m_capture_view);
		m_capture_view->OnMouseMove(point.x, point.y);
	}
	else
	{
		if ((GetKeyState(VK_SHIFT) & 0x8000) == 0x8000)
		{
			int j = 0;
		}

		AutoRef<UIView> v = this->FindView(point.x,point.y);

		if (!v)
		{
			return;
		}

		while (!v->IsActive())
		{
			// 子View不接受输入，就向上级View输入
			v = v->Parent();
			if (!v)
			{
				return;
			}
		}

		if (m_hover_view != v)
		{
			if (m_hover_view)
			{
				m_hover_view->OnMouseLeave();
			}
			m_hover_view = v;
			if (m_hover_view)
			{
				m_hover_view->OnMouseEnter();
			}
		}

		if (v && v->IsEnbale())
		{
			ClientToView(point, v);
			v->OnMouseMove(point.x, point.y);
		}
	}
}

LRESULT UIFrame::OnMouseLeave2(WPARAM wParam, LPARAM lParam)
{
	UI_FRAME_BASE::OnMouseLeave();
	if (m_hover_view)
	{
		m_hover_view->OnMouseLeave();
		m_hover_view = NULL;
	}
	return 0;
}

BOOL UIFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	CRect rc;
	GetWindowRect(rc);

	point.x -= rc.left;
	point.y -= rc.top;

	AutoRef<UIView> v = this->FindView(point.x, point.y);
	if (v)
	{
		float scroll_value = (float)zDelta / (float)(-WHEEL_DELTA);
		
		Gdiplus::Rect rc2 = v->WindowRect();

		point.x -= rc2.X;
		point.y -= rc2.Y;

		v->OnMouseWheel(point.x, point.y, scroll_value);
	}
	return TRUE;
}

BOOL UIFrame::OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message)
{
	if (nHitTest == HTCAPTION || nHitTest == HTLEFT || nHitTest == HTTOPLEFT || nHitTest == HTBOTTOM || nHitTest == HTBOTTOMLEFT || nHitTest == HTRIGHT || nHitTest == HTBOTTOMRIGHT || nHitTest == HTTOPRIGHT)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return TRUE;
	}

	CRect rc;
	GetWindowRect(rc);

	CPoint pt;
	GetCursorPos(&pt);

	pt.x -= rc.left;
	pt.y -= rc.top;

	AutoRef<UIView> hit = FindView(pt.x, pt.y);

	if (hit)
	{
		Gdiplus::Rect rc_view = hit->WindowRect();
		int x = pt.x - rc_view.X;
		int y = pt.y - rc_view.Y;

		hit->OnSetCursor(x, y);
	}

	//if (m_is_can_user_drag_size)
	//{
	//	if (nHitTest == HTTOPLEFT || nHitTest == HTBOTTOMRIGHT)
	//	{
	//		SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
	//		return TRUE;
	//	}
	//	else if (nHitTest == HTTOPRIGHT || nHitTest == HTBOTTOMLEFT)
	//	{
	//		SetCursor(LoadCursor(NULL, IDC_SIZENESW));
	//		return TRUE;
	//	}
	//	else if (nHitTest == HTTOP || nHitTest == HTBOTTOMLEFT)
	//	{
	//		SetCursor(LoadCursor(NULL, IDC_SIZENS));
	//		return TRUE;
	//	}
	//	else if (nHitTest == HTLEFT || nHitTest == HTRIGHT)
	//	{
	//		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
	//		return TRUE;
	//	}
	//}

	//if (nHitTest == HTCLIENT)
	//{
	//	CPoint pt;
	//	GetCursorPos(&pt);

	//	CRect rc;
	//	GetWindowRect(rc);

	//	pt.x -= rc.left;
	//	pt.y -= rc.top;

	//	AutoRef<UIView> v = this->FindView(pt.x, pt.y);
	//	if (v)
	//	{
	//		v->OnSetCursor(pt.x, pt.y);
	//	}
	//}
	//else
	//{
	//	UI_FRAME_BASE::OnSetCursor(pWnd, nHitTest, message);
	//}
	//SetCursor(LoadCursor(NULL, IDC_ARROW));
	return TRUE;
}

void UIFrame::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	if (m_focus_view)
	{
		if (nChar == VK_BACK || nChar == VK_RETURN)
		{
			return;
		}

		if (nChar < 32)
		{
			return;
		}
		CString s((wchar_t)nChar);
		m_focus_view->OnChar(s);
	}
}

LRESULT UIFrame::OnImeChar2(WPARAM wParam, LPARAM lParam)
{
	if (m_focus_view)
	{
		CString s((wchar_t)wParam);
		m_focus_view->OnImeChar(s);
	}
	return 0;
}

LRESULT UIFrame::OnImeComposition(WPARAM wParam, LPARAM lParam)
{
	if (m_focus_view)
	{
		m_focus_view->OnImeComposition();
	}
	return 0;
}

void UIFrame::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	if (m_focus_view)
	{
		m_focus_view->OnKeyDown(nChar);
	}
}

void UIFrame::OnKeyUp(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	if (m_focus_view)
	{
		m_focus_view->OnKeyUp(nChar);
	}
}

AutoRef<UIView> UIFrame::GetFocusView()
{
	return m_focus_view;
}

void UIFrame::SetMinWidthHeight( int nWith,int nHeight )
{
	m_min_width = nWith;
	m_min_height = nHeight;
}

HWND UIFrame::GetHWND()
{
	return m_hWnd;
}

Gdiplus::Rect UIFrame::Rect()
{
	return UIView::Rect();
}

UIFrameInterface* UIFrame::ToFrameInterface()
{
	return this;
}

CWnd* UIFrame::ToWnd()
{
	return this;
}

void UIFrame::SetNoZorder(bool is_no_zorder)
{
	m_is_no_zorder = is_no_zorder;
}
