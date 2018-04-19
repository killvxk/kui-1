

#ifndef __LABLE_H__
#define __LABLE_H__

#include "UIView.h"
#include "FastDelegate.h"

class UILable : public UIView
{
public:

	UILable(AutoRef<UIView> parent, int id, Gdiplus::Rect rc, CString txt)
		:UIView(parent, id)
	{
		SetFrameRect(rc);

		m_is_down = false;

		m_font_size = 14;
		m_color = Gdiplus::Color(255,0,0,0);
	}

	virtual ~UILable()
	{
	}

	void Destroy()
	{
		UIView::Destroy();
	}

	void SetText(CString txt)
	{
		m_txt = txt;
		Redraw();
	}

	void SetColor(Gdiplus::Color color)
	{
		m_color = color;
		Redraw();
	}

	void SetFontSize(int font_size)
	{
		m_font_size = font_size;
		Redraw();
	}

	void OnLButtonDown(int x, int y)
	{
		m_is_down = true;
		Redraw();
	}

	void OnLButtonUp(int x, int y)
	{
		Gdiplus::Rect rc = Rect(); 

		if (m_is_down)
		{
			if (rc.Contains(x, y))
			{
				if (m_click_delegate)
				{
					m_click_delegate();
				}
			}
		}

		m_is_down = false;
	}

	void OnPaint(UIDC* dc)
	{
		Gdiplus::Rect rc = Rect(); 

		if (m_txt.GetLength() > 0)
		{
			Gdiplus::Color color = m_color;
			if (!IsEnbale())
			{
				color = Gdiplus::Color(255,133,133,133);
			}
			Gdiplus::Font* font = GetDefaultFontEx(m_font_size);
			dc->DrawTextSingleLineEx(m_txt, color, rc, DRAW_TEXT_STYLE_VCENTER, font);
			delete font;
		}
	}

public:

	FastDelegate<void()>	m_click_delegate;

protected:

	bool					m_is_down;

	CString					m_txt;
	Gdiplus::Color			m_color;
	int						m_font_size;
};

#endif

