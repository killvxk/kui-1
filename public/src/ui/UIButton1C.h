

#ifndef __BUTTON1C_H__
#define __BUTTON1C_H__

#include "UIView.h"
#include "FastDelegate.h"

// 图片居中1态button
class UIButton1C : public UIView
{
public:

	enum ButtonStatus
	{
		ButtonStatus_Normal = 0,
		ButtonStatus_Hover,
		ButtonStatus_Down,
		ButtonStatus_Disable,
	};

	UIButton1C(AutoRef<UIView> parent, int id, Gdiplus::Rect rc, Gdiplus::Image* image)
		:UIView(parent, id)
	{
		m_image = image;

		SetFrameRect(rc);

		m_is_down = false;
		m_status = ButtonStatus_Normal;

		m_font_size = 20;
		m_color = Gdiplus::Color(255,0,0,0);
	}

	virtual ~UIButton1C()
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

	void SetImage(Gdiplus::Image* image)
	{
		if (m_image)
		{
			delete m_image;
			m_image = NULL;
		}
		m_image = m_image;

		Gdiplus::Rect rc = FrameRect();

		rc.Width = m_image->GetWidth();
		rc.Height = m_image->GetHeight();

		SetFrameRect(rc);
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
		m_status = ButtonStatus_Down;
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
				if (m_status != ButtonStatus_Normal)
				{
					m_status = ButtonStatus_Normal;
					Redraw();
				}
			}
			else
			{
				if (m_status != ButtonStatus_Normal)
				{
					m_status = ButtonStatus_Normal;
					Redraw();
				}
			}
		}

		m_is_down = false;
	}

	void OnMouseMove(int x, int y)
	{
		Gdiplus::Rect rc = Rect(); 

		if (rc.Contains(x, y))
		{
			if (!m_is_down)
			{
				if (m_status != ButtonStatus_Hover)
				{
					m_status = ButtonStatus_Hover;
					Redraw();
				}
			}
		}
	}

	void OnMouseLeave()
	{
		if (m_status != ButtonStatus_Normal)
		{
			m_status = ButtonStatus_Normal;
			Redraw();
		}
	}

	void OnPaint(UIDC* dc)
	{
		Gdiplus::Rect rc = Rect(); 

		if (m_image)
		{
			dc->DrawImageCenter(m_image, Gdiplus::Rect(0,0,rc.Width,rc.Height));
		}

		if (m_txt.GetLength() > 0)
		{
			Gdiplus::Color color = m_color;
			if (!IsEnbale())
			{
				color = Gdiplus::Color(255,133,133,133);
			}
			Gdiplus::Font* font = GetDefaultFontEx(m_font_size);
			dc->DrawTextSingleLineEx(m_txt, color, rc, DRAW_TEXT_STYLE_VCENTER | DRAW_TEXT_STYLE_CENTER, font);
			delete font;
		}
	}

public:

	FastDelegate<void()>	m_click_delegate;

protected:

	Gdiplus::Image*			m_image;
	bool					m_is_down;
	ButtonStatus			m_status;

	CString					m_txt;
	Gdiplus::Color			m_color;
	int						m_font_size;
};

#endif

