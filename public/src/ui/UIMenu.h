

#ifndef __UI_MENU_H__
#define __UI_MENU_H__

#include "UIView.h"
#include "FastDelegate.h"
#include "UIFrame.h"

#include <vector>

class UIMenu : public UIFrame
{
public:

	UIMenu()
	{
		m_hover_index = -1;

		X_START_PADDING = 30;
		ITEM_HEIGHT = 26;
	}

	virtual ~UIMenu()
	{
		Destroy();
	}

	void Destroy()
	{
		UIFrame::Destroy();
	}

	void Clear()
	{
		m_items.clear();
	}

	void AddMenu(const TCHAR* name, int id)
	{
		Item item;
		item.id = id;
		item.name = name;
		item.is_split = false;

		m_items.push_back(item);
	}

	void AddSplit()
	{
		Item item;
		item.is_split = true;

		m_items.push_back(item);
	}

	int ShowMenu(int x, int y)
	{
		Gdiplus::Rect rc = CalcRect(x, y);
		CRect rc2(rc.X, rc.Y, rc.GetRight(), rc.GetBottom());
		if (m_hWnd == NULL)
		{
			CreateEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST, NULL, _T(""), WS_POPUP, rc2, NULL, NULL, NULL);
		}
		MoveWindow(rc2, TRUE);
		ShowWindow(SW_SHOW);
		HWND hOldFocus = ::SetFocus(m_hWnd);
		SetForegroundWindow();

		SetTimer(this, 1000, 100);

		m_is_show = true;
		m_select_id = -1;

		int uMsg = 0;
		MSG msg;
		// 主消息循环:
		while (m_is_show)
		{
			int ret = 0;
			//ret = PeekMessageW(&msg, NULL, 0, 0);
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

		return m_select_id;
	}

	bool OnTimer(DWORD timer_id)
	{
		if (::GetFocus() != m_hWnd || ::GetForegroundWindow() != m_hWnd)
		{
			m_is_show = false;
			ShowWindow(SW_HIDE);
			return false;
		}
		return true;
	}

	virtual Gdiplus::Rect CalcRect(int x, int y)
	{
		UIDC dc(1,1);
		Gdiplus::Font* font = GetDefaultFontEx(12);
		int w = X_START_PADDING + 90;
		int h = 0;
		for (int i = 0; i < (int)m_items.size(); ++i)
		{
			Item& item = m_items[i];
			if (item.is_split)
			{
				h += 3;
			}
			else
			{
				Gdiplus::Rect rc_text = dc.GetTextSizeEx(item.name, font);
				int ww = X_START_PADDING + max(rc_text.Width + 30, 90);

				if (w < ww) w = ww;
				h += ITEM_HEIGHT;
			}
		}

		delete font;

		CRect rt;  
		SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rt,0);

		Gdiplus::Rect rc(x,y,w,h);
		if (rc.GetRight() > rt.Width())
		{
			rc.X = rc.X - w;
		}
		if (rc.GetBottom() > rt.Height())
		{
			rc.Y = rc.Y - h;
		}
		return rc;
	}

	int Hittest(int x, int y)
	{
		int yy = 0;
		int i = 0;
		for (; i < (int)m_items.size(); ++i)
		{
			Item& item = m_items[i];
			if (item.is_split)
			{
				yy += 3;
			}
			else
			{
				if (y > yy && y < yy + ITEM_HEIGHT)
				{
					return i;
				}
				yy += ITEM_HEIGHT;
			}
		}
		return -1;
	}

	void OnLButtonDown(int x, int y)
	{

	}

	void OnLButtonUp(int x, int y)
	{
		int index = Hittest(x, y);
		if (index >= 0)
		{
			m_select_id = m_items[index].id;
			m_is_show = false;
			ShowWindow(SW_HIDE);
		}
	}

	void OnMouseMove(int x, int y)
	{
		Gdiplus::Rect rc = Rect();
		int index = Hittest(x, y);
		if (index != m_hover_index)
		{
			m_hover_index = index;
			Redraw();
		}
	}

	LRESULT OnNcHitTest(CPoint point)
	{
		return HTCLIENT;
	}

	void OnMouseLeave()
	{
		m_hover_index = -1;
		Redraw();
	}

	void OnKillFocus()
	{
		ShowWindow(SW_HIDE);
		m_is_show = false;
	}

	void OnPaint(UIDC* dc)
	{
		Gdiplus::Rect rc = Rect();
		dc->Clear(Gdiplus::Color(0,0,0,0));
		//dc->FillRect(Gdiplus::Color(255,255,255,255), rc);

		dc->EnableSmooth(true);

		float ROUND = 8;
		dc->FillRoundRect(Gdiplus::Color(255,255,255,255), ROUND, rc);

		dc->Save();
		dc->SetClipRoundRect(rc, ROUND);

		Gdiplus::Font* font = GetDefaultFontEx(12);

		int y = 0;
		for (int i = 0; i < (int)m_items.size(); ++i)
		{
			Item& item = m_items[i];

			if (item.is_split)
			{
				dc->DrawLine(1, Gdiplus::Color(255,211,211,211), 0, y + 2, rc.Width, y + 2);
				y += 3;
			}
			else
			{
				Gdiplus::Color color(255,0,0,0);
				if (m_hover_index == i)
				{
					color = Gdiplus::Color(255,255,255,255);
					dc->FillRect(Gdiplus::Color(255,42,137,224), Gdiplus::Rect(0,y,rc.Width,ITEM_HEIGHT));
				}
				dc->DrawTextSingleLineEx(item.name, color, Gdiplus::Rect(X_START_PADDING,y,rc.Width,ITEM_HEIGHT), DRAW_TEXT_STYLE_VCENTER, font);
				y += ITEM_HEIGHT;
			}
		}

		delete font;

		dc->Restore();

		dc->EnableSmooth(true);

		dc->DrawRoundRect(1, Gdiplus::Color(255,42,137,204), ROUND, rc);
	}

public:

	FastDelegate1<int, void>	m_click_delegate;

protected:

	struct Item
	{
		CString		name;
		int			id;
		bool		is_split;
	};

	int						X_START_PADDING;		// 菜单项的空白区域
	int						ITEM_HEIGHT;

	bool					m_is_show;
	int						m_select_id;

	std::vector<Item>		m_items;
	int						m_hover_index;
};

#endif



