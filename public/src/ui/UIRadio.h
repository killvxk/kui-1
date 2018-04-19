

#ifndef __RADIO_H__
#define __RADIO_H__

#include "UIView.h"
#include "FastDelegate.h"
#include "ZipSkin.h"

// 1态button
class UIRadio : public UIView
{
public:

	UIRadio(AutoRef<UIView> parent, int id, Gdiplus::Rect rc)
		:UIView(parent, id)
	{
		m_uncheck_image = AfxUIGetImageEx(_T("radio_uncheck.png"));
		m_check_image = AfxUIGetImageEx(_T("radio_check.png"));

		SetFrameRect(rc);

		m_is_check = false;

		m_font_size = 12;
		m_color = Gdiplus::Color(255,0,0,0);
	}

	virtual ~UIRadio()
	{
		if (m_uncheck_image)
		{
			delete m_uncheck_image;
			m_uncheck_image = NULL;
		}
		if (m_check_image)
		{
			delete m_check_image;
			m_check_image = NULL;
		}
	}

	void Destroy()
	{
		UIView::Destroy();
		if (m_uncheck_image)
		{
			delete m_uncheck_image;
			m_uncheck_image = NULL;
		}
		if (m_check_image)
		{
			delete m_check_image;
			m_check_image = NULL;
		}
	}

	void SetCheck(bool is_check)
	{
		m_is_check = is_check;
		Redraw();
	}

	bool IsCheck()
	{
		return m_is_check;
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
		Redraw();
	}

	void OnLButtonUp(int x, int y)
	{
		Gdiplus::Rect rc = Rect(); 

		if (rc.Contains(x, y))
		{
			if (!m_is_check)
			{
				m_is_check = true;
				Redraw();
			}
			if (m_click_delegate)
			{
				m_click_delegate();
			}
		}
	}

	void OnMouseMove(int x, int y)
	{
		Gdiplus::Rect rc = Rect(); 

		if (rc.Contains(x, y))
		{

		}
	}

	void OnMouseLeave()
	{

	}

	void OnPaint(UIDC* dc)
	{
		Gdiplus::Rect rc = Rect();

		if (m_is_check)
		{
			if (m_check_image)
			{
				dc->DrawImageCenter(m_check_image, Gdiplus::Rect(0,0,24,rc.Height));
			}
		}
		else
		{
			if (m_uncheck_image)
			{
				dc->DrawImageCenter(m_uncheck_image, Gdiplus::Rect(0,0,24,rc.Height));
			}
		}

		if (m_txt.GetLength() > 0)
		{
			Gdiplus::Color color = m_color;
			if (!IsEnbale())
			{
				color = Gdiplus::Color(255,133,133,133);
			}
			Gdiplus::Font* font = GetDefaultFontEx(m_font_size);
			dc->DrawTextSingleLineEx(m_txt, color, Gdiplus::Rect(24,0,rc.Width-24,rc.Height), DRAW_TEXT_STYLE_VCENTER, font);
			delete font;
		}
	}

public:

	FastDelegate<void()>	m_click_delegate;

protected:

	Gdiplus::Image*			m_uncheck_image;
	Gdiplus::Image*			m_check_image;

	bool					m_is_check;

	CString					m_txt;
	Gdiplus::Color			m_color;
	int						m_font_size;
};

#endif

