

#ifndef __CHECK2_H__
#define __CHECK2_H__

#include "UIView.h"
#include "FastDelegate.h"

// 2态Check
class UICheck2 : public UIView
{
public:

	UICheck2(AutoRef<UIView> parent, int id, Gdiplus::Rect rc, Gdiplus::Image* image)
		:UIView(parent, id)
	{
		m_image = image;

		SetFrameRect(rc);

		m_is_down = false;
		m_is_check = false;

		m_font_size = 14;
		m_color = Gdiplus::Color(255,0,0,0);
	}

	virtual ~UICheck2()
	{
		if (m_image)
		{
			delete m_image;
			m_image = NULL;
		}
	}

	void Destroy()
	{
		UIView::Destroy();
		if (m_image)
		{
			delete m_image;
			m_image = NULL;
		}
	}

	void SetCheck(bool is_check)
	{
		if (m_is_check != is_check)
		{
			m_is_check = is_check;
			Redraw();
		}
	}

	bool GetCheck()
	{
		return m_is_check;
	}

	void SetImage(Gdiplus::Image* image)
	{
		if (m_image)
		{
			delete m_image;
			m_image = NULL;
		}
		m_image = m_image;
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
				m_is_check = !m_is_check;
				Redraw();
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

		int x = 0;

		if (m_image)
		{
			int w = m_image->GetWidth() / 2;
			int h = m_image->GetHeight();
			Gdiplus::Rect src_rc(w * (m_is_check ? 1 : 0), 0, w, h);

			dc->DrawImage(m_image, Gdiplus::Rect(0,(rc.Height - h) / 2,w,h),
				src_rc);

			x += w + 4;
		}

		if (m_txt.GetLength() > 0)
		{
			Gdiplus::Color color = m_color;
			Gdiplus::Font* font = GetDefaultFontEx(m_font_size);
			dc->DrawTextSingleLineEx(m_txt, color, Gdiplus::Rect(x, -1, rc.Width - x, rc.Height), DRAW_TEXT_STYLE_VCENTER | DRAW_TEXT_STYLE_END_ELLIPSIS, font);
			delete font;
		}
	}

public:

	FastDelegate<void()>	m_click_delegate;

protected:

	Gdiplus::Image*			m_image;
	bool					m_is_down;
	bool					m_is_check;

	CString					m_txt;
	Gdiplus::Color			m_color;
	int						m_font_size;
};

#endif

