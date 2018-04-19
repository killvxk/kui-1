#include "StdAfx.h"
#include "UIView.h"

#include "UIFrame.h"
#include <Imm.h>
#include "kcore.h"

#pragma comment(lib, "Imm32.lib")

UIView::UIView(AutoRef<UIView> parent, int id, Gdiplus::Rect rc)
{
	if (parent)
	{
		m_parent = parent;
		m_parent->m_childs.push_back(this);
	}
	else
	{
		m_parent = NULL;
	}
	m_id = id;
	m_rc = rc;

	m_is_enable = true;
	m_is_visible = true;
	m_is_clip = false;

	m_sizer_left = -1;
	m_sizer_top = -1;
	m_sizer_right = -1;
	m_sizer_bottom = -1;
	m_sizer_width = -1;
	m_sizer_height = -1;
}

UIView::UIView(AutoRef<UIView> parent, int id)
{
	if (parent)
	{
		m_parent = parent;
		m_parent->m_childs.push_back(this);
	}
	else
	{
		m_parent = NULL;
	}
	m_id = id;
	m_rc = Gdiplus::Rect(0,0,0,0);

	m_is_enable = true;
	m_is_visible = true;
	m_is_clip = false;

	m_sizer_left = -1;
	m_sizer_top = -1;
	m_sizer_right = -1;
	m_sizer_bottom = -1;
	m_sizer_width = -1;
	m_sizer_height = -1;
}

UIView::~UIView(void)
{
	
}

void UIView::Destroy()
{
	UIFrameInterface* win = GetFrameInterface();
	if (win)
	{
		win->KillTimer(this);
	}

	ReferenceObj::Destroy();

	if (!m_childs.empty())
	{
		std::vector< AutoRef<UIView> > childs = m_childs;
		m_childs.clear();
		std::vector< AutoRef<UIView> >::iterator iter = childs.begin();
		for (; iter != childs.end(); ++iter)
		{
			(*iter)->Destroy();
		}
		childs.clear();
	}

	RemoveFromParentView();
}

bool UIView::IsEnbale()
{
	if (!m_is_enable) return false;
	UIView* v = m_parent;
	while (v)
	{
		if (!v->m_is_enable)
		{
			return false;
		}
		v = v->m_parent;
	}
	return true;
}

bool UIView::IsVisible()
{
	if (!m_is_visible) return false;
	UIView* v = m_parent;
	while (v)
	{
		if (!v->m_is_visible)
		{
			return false;
		}
		v = v->m_parent;
	}
	return true;
}

bool UIView::IsActive()
{
	return IsEnbale() && IsVisible();
}

bool UIView::IsWindowView()
{
	return false;
}

void UIView::DoInit()
{

}

Gdiplus::Rect UIView::Rect()
{
	return Gdiplus::Rect(0,0,m_rc.Width,m_rc.Height);
}

const Gdiplus::Rect& UIView::FrameRect()
{
	return m_rc;
}

Gdiplus::Rect UIView::WindowRect()
{
	UIView* v = this;
	int off_x = 0;
	int off_y = 0;
	while (v)
	{
		off_x = off_x + v->m_rc.X;
		off_y = off_y + v->m_rc.Y;
		v = v->m_parent;
	}
	return Gdiplus::Rect(off_x, off_y, m_rc.Width, m_rc.Height);
}

Gdiplus::Rect UIView::ClientRect()
{
	CPoint pt(0,0);

	HWND hWnd = GetHWND();

	ClientToScreen(hWnd, &pt);
	//GetFrameWindow()->ClientToScreen(&pt);
	CRect rc;
	//GetFrameWindow()->GetWindowRect(rc);
	GetWindowRect(hWnd, &rc);

	UIView* v = this;
	int off_x = rc.left - pt.x;
	int off_y = rc.top - pt.y;
	while (v)
	{
		off_x = off_x + v->m_rc.X;
		off_y = off_y + v->m_rc.Y;
		v = v->m_parent;
	}
	return Gdiplus::Rect(off_x, off_y, m_rc.Width, m_rc.Height);
}

Gdiplus::Rect UIView::GlobalRect()
{
	Gdiplus::Rect rc = WindowRect();

	UIFrameInterface* window = GetFrameInterface();
	if (window)
	{
		CRect rc2;
		GetWindowRect(window->GetHWND(), rc2);

		rc.X += rc2.left;
		rc.Y += rc2.top;
	}
	return rc;
}

void UIView::SetFrameRect(Gdiplus::Rect rc)
{
	if (!m_rc.Equals(rc))
	{
		m_rc = rc;
		std::vector< AutoRef<UIView> >::iterator iter = m_childs.begin();
		for (; iter != m_childs.end(); ++iter)
		{
			(*iter)->OnSizer();
		}
	}
}

AutoRef<UIView> UIView::Parent()
{
	return m_parent;
}

void UIView::Show(bool is_visible)
{
	if (m_is_visible != is_visible)
	{
		m_is_visible = is_visible;
		if (m_parent)
		{
			m_parent->Redraw();
		}
		else
		{
			Redraw();
		}
		//AutoRef<Window> win = window();
		//if (win)
		//{
		//	win->show_view(this, is_visible);
		//}
	}
}

void UIView::Hide()
{
	if (m_is_visible != false)
	{
		m_is_visible = false;
		if (m_parent)
		{
			m_parent->Redraw();
		}
		else
		{
			Redraw();
		}
	}
}

void UIView::SetEnable(bool is_enable)
{
	if (m_is_enable != is_enable)
	{
		m_is_enable = is_enable;
		Redraw();
	}
}

void UIView::EnableClip(bool is_enable)
{
	m_is_clip = is_enable;
}

bool UIView::SetTimer(DWORD timer_id, DWORD elapse_millsec)
{
	UIFrameInterface* win = GetFrameInterface();
	if (win)
	{
		return win->SetTimer(this, timer_id, elapse_millsec);
	}
	return false;
}

bool UIView::KillTimer(DWORD timer_id)
{
	UIFrameInterface* win = GetFrameInterface();
	if (win)
	{
		return win->KillTimer(timer_id);
	}
	return false;
}

void UIView::SetFocus()
{
	GetFrameInterface()->SetViewFocus(this);
}

AutoRef<UIView> UIView::GetRootView()
{
	UIView* v = this;
	while (v != NULL)
	{
		if (v->m_parent == NULL)
		{
			if (v->IsWindowView())
			{
				return v;
			}
		}
		v = v->m_parent;
	}
	return NULL;
}

CWnd* UIView::GetFrameWindow()
{
	UIView* v = this;
	while (v != NULL)
	{
		if (v->m_parent == NULL)
		{
			if (v->IsWindowView())
			{
				return v->ToWnd();
			}
		}
		v = v->m_parent;
	}
	return NULL;
}

UIFrameInterface* UIView::GetFrameInterface()
{
	UIView* v = this;
	while (v != NULL)
	{
		if (v->m_parent == NULL)
		{
			if (v->IsWindowView())
			{
				return v->ToFrameInterface();
			}
		}
		v = v->m_parent;
	}
	return NULL;
}

HWND UIView::GetHWND()
{
	UIView* v = this;
	while (v != NULL)
	{
		if (v->m_parent == NULL)
		{
			if (v->IsWindowView())
			{
				return v->GetHWND();
			}
		}
		v = v->m_parent;
	}
	return NULL;
}

AutoRef<UIView> UIView::FindView(int x, int y)
{
	if (!m_is_visible)
	{
		return NULL;
	}

	if (!Gdiplus::Rect(0,0,m_rc.Width,m_rc.Height).Contains(x,y))
	{
		return NULL;
	}

	std::vector< AutoRef<UIView> >::reverse_iterator iter = m_childs.rbegin();
	for (; iter != m_childs.rend(); ++iter)
	{
		Gdiplus::Rect rc = (*iter)->FrameRect();
		int xx = x - rc.X;
		int yy = y - rc.Y;
		AutoRef<UIView> v = (*iter)->FindView(xx,yy);
		if (v)
		{
			return v;
		}
	}

	if (!m_is_enable)
	{
		return NULL;
	}
	return this;
}

void UIView::AddChildView(AutoRef<UIView> child)
{
	if (child->m_parent)
	{
		if (child->m_parent == this)
		{
			return;
		}
		child->RemoveFromParentView();
	}
	child->m_parent = this;
	m_childs.push_back(child);
}

void UIView::RemoveFromParentView()
{
	if (m_parent == NULL)
	{
		return;
	}

	std::vector< AutoRef<UIView> >::iterator iter = m_parent->m_childs.begin();
	for (; iter != m_parent->m_childs.end(); ++iter)
	{
		if (*iter == this)
		{
			m_parent->m_childs.erase(iter);
			break;
		}
	}
	m_parent = NULL;
}

UIFrameInterface* UIView::ToFrameInterface()
{
	return NULL;
}

CWnd* UIView::ToWnd()
{
	return NULL;
}

void UIView::SetSizer(int sizer_left, int sizer_top, int sizer_right, int sizer_bottom, int width, int height)
{
	m_sizer_left = sizer_left;
	m_sizer_top = sizer_top;
	m_sizer_right = sizer_right;
	m_sizer_bottom = sizer_bottom;
	m_sizer_width = width;
	m_sizer_height = height;

	OnSizer();
}

void UIView::OnSizer()
{
	if (m_parent == NULL)
	{
		try
		{
			std::vector< AutoRef<UIView> >::iterator iter = m_childs.begin();
			for (; iter != m_childs.end(); ++iter)
			{
				(*iter)->OnSizer();
			}
		}
		catch(...)
		{

		}
		return;
	}
	else
	{
		int left = 0;
		int right = 0;
		int top = 0;
		int bottom = 0;
		if (m_sizer_left >= 0)
		{
			if (m_sizer_right >= 0)
			{
				left = m_sizer_left;
				right = m_parent->m_rc.Width - m_sizer_right;
			}
			else
			{
				left = m_sizer_left;
				right = left + m_rc.Width;
			}
		}
		else
		{
			if (m_sizer_right >= 0)
			{
				right = m_parent->m_rc.Width - m_sizer_right;
				left = right - m_rc.Width;
			}
			else
			{
				left = m_rc.X;
				right = m_rc.X + m_rc.Width;
			}
		}

		if (m_sizer_top >= 0)
		{
			if (m_sizer_bottom >= 0)
			{
				top = m_sizer_top;
				bottom = m_parent->m_rc.Height - m_sizer_bottom;
			}
			else
			{
				top = m_sizer_top;
				bottom = top + m_rc.Height;
			}
		}
		else
		{
			if (m_sizer_bottom >= 0)
			{
				bottom = m_parent->m_rc.Height - m_sizer_bottom;
				top = bottom - m_rc.Height;
			}
			else
			{
				top = m_rc.Y;
				bottom = m_rc.Y + m_rc.Height;
			}
		}

		if (m_sizer_width >= 0)
		{
			right = left + m_sizer_width;
		}
		if (m_sizer_height >= 0)
		{
			bottom = top + m_sizer_height;
		}

		int width = right - left;
		int height = bottom - top;

		m_rc.X = left;
		m_rc.Y = top;
		m_rc.Width = width;
		m_rc.Height = height;

		try
		{
			std::vector< AutoRef<UIView> >::iterator iter = m_childs.begin();
			for (; iter != m_childs.end(); ++iter)
			{
				(*iter)->OnSizer();
			}
		}
		catch (...)
		{
			
		}
	}
}

void UIView::OnClip(UIDC* dc)
{
	
}

void UIView::OnPaint(UIDC* dc)
{

}

void UIView::OnPostPaint(UIDC* dc)
{

}

void UIView::OnLButtonDown(int x, int y)
{

}

void UIView::OnLButtonUp(int x, int y)
{

}

void UIView::OnLButtonDblClk(int x, int y)
{

}

void UIView::OnRButtonDown(int x, int y)
{

}

void UIView::OnRButtonUp(int x, int y)
{

}

void UIView::OnMouseMove(int x, int y)
{

}

void UIView::OnMouseEnter()
{

}

void UIView::OnMouseLeave()
{

}

void UIView::OnMouseWheel(int x, int y, float scroll)
{

}

void UIView::OnSetCursor(int x, int y)
{
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void UIView::OnKeyDown(unsigned int key)
{

}

void UIView::OnKeyUp(unsigned int key)
{

}

void UIView::OnSetFocus()
{

}

void UIView::OnKillFocus()
{

}

void UIView::OnChar(CString& str)
{

}

void UIView::OnImeChar(CString& str)
{

}

void UIView::OnImeComposition()
{

}

bool UIView::OnClose()
{
	return true;
}

bool UIView::IsCanDrag(int x, int y)
{
	return false;
}

bool UIView::OnTimer(DWORD timer_id)
{
	return true;
}

void UIView::Redraw()
{
	UIFrameInterface* window = GetFrameInterface();
	if (window)
	{
		window->RedrawView(this);
	}
}
void UIView::RedrawAll()
{
	UIFrameInterface* window = GetFrameInterface();
	if (window)
	{
		window->RedrawAllView();
	}
}

void UIView::SetImePos(int x, int y)
{
	UIFrameInterface* win = GetFrameInterface();

	HWND hWnd = win->GetHWND();

	if (win && ::IsWindow(hWnd))
	{
		HIMC himc = ImmGetContext(hWnd);

		COMPOSITIONFORM d;

		Gdiplus::Rect rc = WindowRect();

		d.dwStyle = CFS_POINT;
		d.ptCurrentPos.x = x + rc.X;
		d.ptCurrentPos.y = y + rc.Y;

		ImmSetCompositionWindow(himc, &d);
		ImmReleaseContext(hWnd, himc);
	}
}

void UIView::OnDrawThemeBackgroud(UIDC* dc)
{
	Gdiplus::Rect rc = WindowRect();
	UIFrameInterface* win = GetFrameInterface();
	Gdiplus::Image* image = win->GetThemeBackgroud();
	Gdiplus::Rect win_rc = win->Rect();
	if (image)
	{
		float ratio_x = (float)rc.X / (float)win_rc.Width;
		float ratio_y = (float)rc.Y / (float)win_rc.Height;

		float ratio_width = (float)rc.Width / (float)win_rc.Width;
		float ratio_height = (float)rc.Height / (float)win_rc.Height;

		int src_x = (int)(ratio_x * image->GetWidth());
		int src_y = (int)(ratio_y * image->GetHeight());
		int src_width = (int)(ratio_width * image->GetWidth());
		int src_height = (int)(ratio_height * image->GetHeight());

		dc->Save();
		dc->EnableSmooth(true);
		dc->DrawImage(image, Rect(), Gdiplus::Rect(src_x,src_y,src_width,src_height));
		dc->Restore();
	}
}
void UIView::OnDrawThemeBackgroudDst(UIDC* dc,Gdiplus::Rect dstRc)
{
	Gdiplus::Rect rc = WindowRect();
	UIFrameInterface* win = GetFrameInterface();
	Gdiplus::Image* image = win->GetThemeBackgroud();
	Gdiplus::Rect win_rc = win->Rect();
	if (image)
	{
		float ratio_x = (float)rc.X / (float)win_rc.Width;
		float ratio_y = (float)rc.Y / (float)win_rc.Height;
		float ratio_width = (float)rc.Width / (float)win_rc.Width;
		float ratio_height = (float)rc.Height / (float)win_rc.Height;
		int src_x = (int)(ratio_x * image->GetWidth());
		int src_y = (int)(ratio_y * image->GetHeight());
		int src_width = (int)(ratio_width * image->GetWidth());
		int src_height = (int)(ratio_height * image->GetHeight());

		Gdiplus::Image* pImage = new Gdiplus::Bitmap(Rect().Width, Rect().Height, PixelFormat32bppARGB);
		Gdiplus::Graphics* pdc = new Gdiplus::Graphics(pImage);
		pdc->DrawImage(image, Rect(), src_x,src_y,src_width,src_height, Gdiplus::UnitPixel);

		dc->Save();
		dc->EnableSmooth(true);
		dc->DrawImage(pImage, dstRc, dstRc);
		dc->Restore();

		delete pImage;
		delete pdc;
	}
}


CString	g_default_font_family;
CString	g_lang;

void SetDefaultFontFamily(CString font_family)
{
	g_default_font_family = font_family;
}

HFONT GetDefaultFont(int fontsize, bool is_bold,bool is_italic,bool is_underline)
{
	LOGFONT lf = {0};
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);

	if (g_lang.IsEmpty())
	{
		TCHAR szLanguage[MAX_PATH] = {0};
		kcore::AfxGetSysString(TEXT("config"), TEXT("language"), szLanguage, _countof(szLanguage));
		if (lstrlen(szLanguage) == 0)
		{
			g_lang = _T("chs");
		}
		else
		{
			g_lang = szLanguage;
		}
	}

	if (g_lang == _T("chs"))
	{
		// 中文
		if (!g_default_font_family.IsEmpty())
		{
			lstrcpy(lf.lfFaceName, g_default_font_family);
		}
		else
		{
			//lstrcpy(lf.lfFaceName, _T("Microsoft YaHei"));
			lstrcpy(lf.lfFaceName, _T("SimSun"));
		}
	}
	else if (g_lang == _T("trk"))
	{
		// 土耳其
		lstrcpy(lf.lfFaceName, _T("Segoe UI"));
	}

	//LANGID lang = GetUserDefaultUILanguage();
	//if (lang == 2052)
	//{
	//	// 中文
	//	if (g_default_font_family)
	//	{
	//		lstrcpy(lf.lfFaceName, *g_default_font_family);
	//	}
	//	else
	//	{
	//		//lstrcpy(lf.lfFaceName, _T("Microsoft YaHei"));
	//		lstrcpy(lf.lfFaceName, _T("SimSun"));
	//	}
	//}
	//else if (lang == 1055)
	//{
	//	// 土耳其
	//	lstrcpy(lf.lfFaceName, _T("Segoe UI"));
	//}

	lf.lfHeight = -fontsize;
	lf.lfWeight = is_bold ? FW_BOLD : FW_NORMAL;
	if (is_underline)
		lf.lfUnderline = TRUE;
	if (is_italic)
		lf.lfItalic = TRUE;
	HFONT hFont = ::CreateFontIndirect(&lf);
	return hFont;
}

Gdiplus::Font* GetDefaultFontEx(int fontsize, bool is_bold,bool is_italic,bool is_underline)
{
	LPCTSTR family = NULL;

	if (g_lang.IsEmpty())
	{
		TCHAR szLanguage[MAX_PATH] = {0};
		kcore::AfxGetSysString(TEXT("config"), TEXT("language"), szLanguage, _countof(szLanguage));
		if (lstrlen(szLanguage) == 0)
		{
			g_lang = _T("chs");
		}
		else
		{
			g_lang = szLanguage;
		}
	}

	if (g_lang == _T("chs"))
	{
		// 中文
		if (g_default_font_family)
		{
			family = g_default_font_family;
		}
		else
		{
			//family = _T("Microsoft YaHei");
			family = _T("SimSun");
		}
	}
	else if (g_lang == _T("trk"))
	{
		// 土耳其
		family =  _T("Segoe UI");
	}


	//LANGID lang = GetUserDefaultUILanguage();
	//if (lang == 2052)
	//{
	//	// 中文
	//	if (g_default_font_family)
	//	{
	//		family = *g_default_font_family;
	//	}
	//	else
	//	{
	//		//family = _T("Microsoft YaHei");
	//		family = _T("SimSun");
	//	}
	//}
	//else if (lang == 1055)
	//{
	//	// 土耳其
	//	family = _T("Segoe UI");
	//}

	int style = Gdiplus::FontStyleRegular;
	if (is_bold)
	{
		style |= Gdiplus::FontStyleBold;
	}
	if(is_italic)
	{
		style |= Gdiplus::FontStyleItalic;
	}
	if(is_underline)
	{
		style |= Gdiplus::FontStyleUnderline;
	}
	return new Gdiplus::Font(family, (float)fontsize, style, Gdiplus::UnitPixel, NULL);
}

Gdiplus::Font* GetFamilyFont( CString font_family /*= _T("微软雅黑")*/,int fontsize /*= 14*/, bool is_bold /*= false*/,bool is_italic /*= false*/,bool is_underline /*= false*/ )
{
	int style = Gdiplus::FontStyleRegular;
	if (is_bold)
	{
		style |= Gdiplus::FontStyleBold;
	}
	if(is_italic)
	{
		style |= Gdiplus::FontStyleItalic;
	}
	if(is_underline)
	{
		style |= Gdiplus::FontStyleUnderline;
	}
	return new Gdiplus::Font(font_family, (float)fontsize, style, Gdiplus::UnitPixel, NULL);
}

void UIView::ShowError(DWORD error)
{
	if (error == 0) error = GetLastError();

	wchar_t error_txt[512];
	error_txt[0] = '\0';
	DWORD ret = FormatMessageW( 
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		error_txt, 512-1, NULL);

	MessageBoxW(NULL,error_txt, NULL, MB_OK);
}

void UIView::OnPosChanged()
{
	try
	{
		std::vector< AutoRef<UIView> >::iterator iter = m_childs.begin();
		for (; iter != m_childs.end(); ++iter)
		{
			(*iter)->OnPosChanged();
		}
	}
	catch(...)
	{

	}
}


