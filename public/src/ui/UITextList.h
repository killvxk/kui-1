

#ifndef __UI_TEXT_LIST_H__
#define __UI_TEXT_LIST_H__

#include "UIView.h"
#include "FastDelegate.h"
#include "UIVScroll.h"
#include <vector>

// 1态button
class UITextList : public UIView
{
public:

	struct Item
	{
		CString		name;
	};

	std::vector<Item>		m_items;

	Gdiplus::Color			m_color;
	Gdiplus::Color			m_frame_color;
	int						FONT_SIZE;;
	int						ITEM_HEIGHT;
	int						m_select_index;

	AutoRef<UIVScroll>		m_vscroll;

	UITextList(AutoRef<UIView> parent, int id, Gdiplus::Rect rc)
		:UIView(parent, id)
	{
		SetFrameRect(rc);

		FONT_SIZE = 12;
		ITEM_HEIGHT = 20;
		m_color = Gdiplus::Color(255,0,0,0);

		m_select_index = -1;

		m_vscroll = new UIVScroll(this, 0, 10);
		m_frame_color = Gdiplus::Color(255,163,206,233);
	}

	virtual ~UITextList()
	{
	}

	void Destroy()
	{
		UIView::Destroy();
		m_vscroll = NULL;
	}

	void AddItem(CString name)
	{
		Item item;
		item.name = name;

		m_items.push_back(item);

		Layout();

		Redraw();
	}

	void Clear()
	{
		m_select_index = -1;
		m_items.clear();
		Layout();

		Redraw();
	}

	int GetItemCount()
	{
		return (int)m_items.size();
	}

	int GetSelectIndex()
	{
		return m_select_index;
	}

	CString GetItemText(int index)
	{
		if (index >= 0 && index < (int)m_items.size())
		{
			return m_items[index].name;
		}
		return _T("");
	}

	void RemoveByName(CString name)
	{
		std::vector<Item>::iterator iter = m_items.begin();
		for (; iter != m_items.end(); ++iter)
		{
			Item& item = *iter;
			if (item.name == name)
			{
				m_items.erase(iter);
				break;
			}
		}

		if (m_items.size() > 0)
		{
			if (m_select_index >= (int)m_items.size())
			{
				m_select_index = (int)m_items.size() - 1;
			}
		}
		else
		{
			m_select_index = -1;
		}
		Redraw();
	}

	void Layout()
	{
		Gdiplus::Rect rc = Rect();

		int y = ITEM_HEIGHT * (int)m_items.size();
		m_vscroll->SetScroll(y, rc.Height);
	}

	void SetColor(Gdiplus::Color color)
	{
		m_color = color;
		Redraw();
	}

	void SetFontSize(int font_size)
	{
		FONT_SIZE = font_size;
		Redraw();
	}

	void SetItemHeight(int height)
	{
		ITEM_HEIGHT = height;
		Layout();
	}

	void OnLButtonDown(int x, int y)
	{
		int index = Hittest(x, y);
		if (index >= 0)
		{
			m_select_index = index;
			Redraw();

			if (m_click_delegate)
			{
				m_click_delegate(m_items[index].name);
			}
		}
	}

	void OnLButtonDblClk(int x, int y)
	{
		int index = Hittest(x, y);
		if (index >= 0)
		{
			m_select_index = index;
			Redraw();

			if (m_dbclick_delegate)
			{
				m_dbclick_delegate(m_items[index].name);
			}
		}
	}

	int Hittest(int x, int y)
	{
		int index = (m_vscroll->GetScrollPos() + y) / ITEM_HEIGHT;
		if (index >= 0 && index < (int)m_items.size())
		{
			return index;
		}
		return -1;
	}

	void OnPaint(UIDC* dc)
	{
		Gdiplus::Rect rc = Rect();

		dc->FillRect(Gdiplus::Color(255,255,255,255), rc);
		
		Gdiplus::Font* font = GetDefaultFontEx(FONT_SIZE);

		Gdiplus::Color font_color(255,0,0,0);

		if (!this->IsEnbale())
		{
			font_color = Gdiplus::Color(255,169,169,169);
		}

		int y = -m_vscroll->GetScrollPos();
		for (int i = 0; i < (int)m_items.size(); ++i)
		{
			Item& item = m_items[i];

			if (y + ITEM_HEIGHT < 0)
			{
				y += ITEM_HEIGHT;
				continue;
			}
			if (y > rc.Height)
			{
				break;
			}

			if (m_select_index == i)
			{
				dc->FillRect(Gdiplus::Color(255,51,153,255), Gdiplus::Rect(0,y,rc.Width,ITEM_HEIGHT));
				dc->DrawTextSingleLineEx(item.name, Gdiplus::Color(255,255,255,255), Gdiplus::Rect(5,y,rc.Width-6,ITEM_HEIGHT), DRAW_TEXT_STYLE_VCENTER, font);
			}
			else
			{
				dc->DrawTextSingleLineEx(item.name, font_color, Gdiplus::Rect(5,y,rc.Width-6,ITEM_HEIGHT), DRAW_TEXT_STYLE_VCENTER, font);
			}

			y += ITEM_HEIGHT;
		}

		delete font;

		dc->DrawRect(1, m_frame_color, rc);
	}

public:

	FastDelegate1<CString, void>		m_click_delegate;
	FastDelegate1<CString, void>		m_dbclick_delegate;

protected:

};

#endif

