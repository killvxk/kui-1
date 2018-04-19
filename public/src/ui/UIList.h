

#ifndef __UI_LIST_H__
#define __UI_LIST_H__

#include "UIView.h"
#include "FastDelegate.h"
#include "UIVScroll.h"

class UIList : public UIView
{
public:

	FastDelegate<void()>	m_click_delegate;

protected:


	int						HEADER_HEIGHT;
	int						ITEM_HEIGHT;

	AutoRef<UIVScroll>		m_vscroll;

	struct Item
	{
		std::vector<CString>	values;
		bool					is_check;
		int						id;
	};

	struct Column
	{
		CString				name;
		int					width;
	};

	std::vector<Column>			m_columns;
	std::vector<Item>			m_items;

	int							m_select_index;

	Gdiplus::Image*				m_check_image;
	Gdiplus::Image*				m_uncheck_image;

	bool						m_is_enable_check;

public:

	UIList(AutoRef<UIView> parent, int id)
		:UIView(parent, id)
	{
		m_is_enable_check = false;

		m_uncheck_image = AfxUIGetImageEx(_T("uncheck.png"));
		m_check_image = AfxUIGetImageEx(_T("check.png"));

		HEADER_HEIGHT = 24;
		ITEM_HEIGHT = 24;

		m_select_index = -1;

		m_vscroll = new UIVScroll(this, 0, 5);
		m_vscroll->SetSizer(-1,HEADER_HEIGHT,1,1,-1,-1);
	}

	virtual ~UIList()
	{

	}

	void Destroy()
	{
		UIView::Destroy();
		m_vscroll = NULL;
		if (m_check_image)
		{
			delete m_check_image;
			m_check_image = NULL;
		}
		if (m_uncheck_image)
		{
			delete m_uncheck_image;
			m_uncheck_image = NULL;
		}
	}

	void Clear()
	{
		m_items.clear();
		m_select_index = -1;
	}

	void AddColumn(CString name, int width)
	{
		Column column;
		column.name = name;
		column.width = width;
		m_columns.push_back(column);
	}

	void AddItem(int id, bool is_check, const TCHAR* value, ...)
	{
		va_list vargs;
		va_start(vargs, value);

		Item item;
		item.id = id;
		item.is_check = is_check;

		const TCHAR* next_value = value;
		while (next_value)
		{
			item.values.push_back(next_value);
			next_value = va_arg( vargs, const TCHAR*);
		}

		m_items.push_back(item);

		va_end(vargs);

		Layout();

		Redraw();
	}

	void EnableCheck(bool is_enable)
	{
		m_is_enable_check = is_enable;
	}

	UIList::Item* GetItem(int index)
	{
		if (index >= 0 && index < m_items.size())
		{
			return &m_items[index];
		}
		return NULL;
	}

	UIList::Item* GetItemByID(int id)
	{
		for (int i = 0; i < m_items.size(); ++i)
		{
			if (m_items[i].id == id)
			{
				return &m_items[i];
			}
		}
		return NULL;
	}

	CString GetItemText(int index, int column)
	{
		if (index >= 0 && index < m_items.size())
		{
			Item& item = m_items[index];
			if (column >= 0 && column < item.values.size())
			{
				return item.values[column];
			}
		}
		return _T("");
	}

	int GetSelectItemIndex()
	{
		return m_select_index;
	}

	void SelectItem(int index)
	{
		if (index >= 0 && index < m_items.size())
		{
			m_select_index = index;
		}
		else if (index < 0)
		{
			m_select_index = -1;
		}
		Redraw();
	}

	bool IsItemCheck(int index)
	{
		if (index >= 0 && index < m_items.size())
		{
			return m_items[index].is_check;
		}
		return false;
	}

	int GetItemCount()
	{
		return m_items.size();
	}
	
	int GetCheckItemCount()
	{
		int count = 0;
		for (int i = 0; i < m_items.size(); ++i)
		{
			if (m_items[i].is_check)
			{
				count++;
			}
		}
		return count;
	}

	void CheckAllItem(bool is_check)
	{
		for (int i = 0; i < m_items.size(); ++i)
		{
			m_items[i].is_check = is_check;
		}
		Redraw();
	}

	void CheckAllItemInvert()
	{
		for (int i = 0; i < m_items.size(); ++i)
		{
			m_items[i].is_check = !m_items[i].is_check;
		}
		Redraw();
	}

	void CheckItem(int index, bool is_check)
	{
		if (index >= 0 && index < m_items.size())
		{
			m_items[index].is_check = is_check;
			Redraw();
		}
	}

	void CheckItemByID(int id, bool is_check)
	{
		for (int i = 0; i < m_items.size(); ++i)
		{
			if (m_items[i].id == id)
			{
				m_items[i].is_check = is_check;
				Redraw();
				return;
			}
		}
	}

	int Hittest(int x, int y)
	{
		y += m_vscroll->GetScrollPos() - HEADER_HEIGHT;
		int index = y / ITEM_HEIGHT;
		if (index >= 0 && index < m_items.size())
		{
			return index;
		}
		return -1;
	}

	void OnSizer()
	{
		UIView::OnSizer();

		Layout();
	}

	void Layout()
	{
		Gdiplus::Rect rc = Rect();
		int page = rc.Height - HEADER_HEIGHT;
		int max = m_items.size() * ITEM_HEIGHT;

		m_vscroll->SetScroll(max, page);
	}

	void OnLButtonDown(int x, int y)
	{
		int index = Hittest(x, y);
		if (index >= 0)
		{
			if (m_is_enable_check)
			{
				m_items[index].is_check = !m_items[index].is_check;
			}

			m_select_index = index;
		}
		Redraw();
	}

	void OnLButtonUp(int x, int y)
	{
		Gdiplus::Rect rc = Rect(); 
	}

	void OnMouseMove(int x, int y)
	{
		Gdiplus::Rect rc = Rect(); 

	}

	void OnMouseLeave()
	{

	}

	void OnMouseWheel(int x, int y, float scroll)
	{
		m_vscroll->OnMouseWheel(x, y, scroll);
	}

	void OnPaint(UIDC* dc)
	{
		Gdiplus::Rect rc = Rect();

		dc->SetClip(rc);
		
		Gdiplus::Font* font = GetDefaultFontEx(12);

		dc->FillRect(Gdiplus::Color(255,255,255,255), rc);

		// 绘制header
		dc->FillRectGradientVertical(Gdiplus::Rect(0,0,rc.Width,HEADER_HEIGHT), Gdiplus::Color(255,255,255,255), Gdiplus::Color(255,241,242,243));
		int x = 0;
		for (int i = 0 ; i < m_columns.size(); ++i)
		{
			Column& column = m_columns[i];

			dc->DrawTextSingleLineEx(column.name, Gdiplus::Color(255,0,0,0), Gdiplus::Rect(x+3,0,column.width - 6, HEADER_HEIGHT), DRAW_TEXT_STYLE_VCENTER | DRAW_TEXT_STYLE_END_ELLIPSIS, font);

			x += column.width;

			dc->DrawLine(1, Gdiplus::Color(255,211,211,211), x, 0, x, HEADER_HEIGHT);
		}

		dc->DrawLine(1, Gdiplus::Color(255,211,211,211), 0, HEADER_HEIGHT - 1, rc.Width, HEADER_HEIGHT - 1);

		dc->Save();
		dc->SetClip(Gdiplus::Rect(0,HEADER_HEIGHT,rc.Width,rc.Height-HEADER_HEIGHT));
		dc->m_off_y += HEADER_HEIGHT;

		int y = -m_vscroll->GetScrollPos();
		// 绘制Item
		for (int i = 0; i < m_items.size(); ++i)
		{
			if (y + ITEM_HEIGHT < 0)
			{
				y += ITEM_HEIGHT;
				continue;
			}

			if (y > rc.Height)
			{
				break;
			}

			Gdiplus::Color font_color(255,0,0,0);
			if (m_select_index == i)
			{
				font_color = Gdiplus::Color(255,255,255,255);
				dc->FillRect(Gdiplus::Color(255,49,106,197), Gdiplus::Rect(0,y,rc.Width,ITEM_HEIGHT));
			}

			if (!this->IsEnbale())
			{
				font_color = Gdiplus::Color(255,169,169,169);
			}

			Item& item = m_items[i];
			int x = 0;
			for (int j = 0 ; j < m_columns.size(); ++j)
			{
				Column& column = m_columns[j];
				if (m_is_enable_check && j == 0)
				{
					if (item.is_check)
					{
						dc->DrawImageCenter(m_check_image, Gdiplus::Rect(0,y,24,24));
					}
					else
					{
						dc->DrawImageCenter(m_uncheck_image, Gdiplus::Rect(0,y,24,24));
					}

					if (item.values.size() > j)
					{
						dc->DrawTextSingleLineEx(item.values[j], font_color, Gdiplus::Rect(x+24,y,column.width-26,ITEM_HEIGHT), DRAW_TEXT_STYLE_VCENTER | DRAW_TEXT_STYLE_END_ELLIPSIS, font);
					}
				}
				else
				{
					if (item.values.size() > j)
					{
						dc->DrawTextSingleLineEx(item.values[j], font_color, Gdiplus::Rect(x+5,y,column.width-8,ITEM_HEIGHT), DRAW_TEXT_STYLE_VCENTER | DRAW_TEXT_STYLE_END_ELLIPSIS, font);
					}
				}

				x += column.width;
			}

			y += ITEM_HEIGHT;
		}

		dc->Restore();

		dc->DrawRect(1, Gdiplus::Color(255,164,206,233), rc);

		delete font;
	}
};

#endif

