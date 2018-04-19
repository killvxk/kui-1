#pragma once

#include "UIView.h"
#include "ZipSkin.h"
#include <GdiPlus.h>


class UIVScroll : public UIView
{
public:

	int					m_step;
	Gdiplus::Color		m_color;
	int					m_pos;
	int					m_max;
	int					m_page;
	Gdiplus::Rect		m_thumb_rc;
	int					m_down_y;
	int					m_last_y;
	bool				m_is_down;

	UIVScroll(AutoRef<UIView> parent, int id, int scroll_step)
		:UIView(parent, id)
	{
		m_color = Gdiplus::Color(157,167,176);
		m_pos = 0;
		m_max = 1;
		m_page = 1;
		m_is_down = false;

		Gdiplus::Rect rc = m_parent->Rect();
		m_step = scroll_step;
		this->SetSizer(-1,1,1,1,8,-1);
	}

	~UIVScroll()
	{

	}

	void SetColor(Gdiplus::Color color)
	{
		m_color = color;
	}

	void SetStep(int step)
	{
		m_step = step;
	}

	void SetScroll(int max, int page)
	{
		if (max != m_max || page != m_page)
		{
			m_max = max;
			if (m_max <= 0)
				m_max = 1;
			m_page = page;
			if (m_pos > max - page)
			{
				m_pos = max - page;
				if (m_pos < 0)
				{
					m_pos = 0;
					Redraw();
					//emit scroll_change();
				}
			}
			Layout();
		}
	}

	void SetScrollPos(int pos)
	{
		if (pos != m_pos)
		{
			if (pos > m_max - m_page)
				pos = m_max - m_page;
			if (pos < 0)
				pos = 0;
			if (m_pos != pos)
			{
				m_pos = pos;
				Layout();
				Redraw();
				//emit scroll_change();
			}
			else
			{
				Layout();
			}
		}
	}

	void ScrollToEnd()
	{
		SetScrollPos(m_max);
	}

	int GetScrollPos()
	{
		return m_pos;
	}

	int GetMax()
	{
		return m_max;
	}

	int GetPage()
	{
		return m_page;
	}

	void Layout()
	{
		Gdiplus::Rect rc = Rect();
		float unit_pixel = (float)(rc.Height) / m_max;
		int thum_height = int(m_page * rc.Height / m_max);
		int yy = (int)(m_pos * unit_pixel);

		if (thum_height < 20)
		{
			thum_height = 20;
		}

		int thum_width = rc.Width;
		int xx = rc.GetRight() - thum_width;

		if (thum_height + yy > rc.Height) yy = rc.Height - thum_height;

		m_thumb_rc = Gdiplus::Rect(xx,yy,thum_width,thum_height);
	}

	void OnMouseWheel(int x, int y, float scroll)
	{
		SetScrollPos(m_pos + (int)(scroll * m_step));
	}

	void Redraw()
	{
		if (m_parent)
		{
			m_parent->Redraw();
		}
		else
		{
			UIView::Redraw();
		}
	}

protected:

	void OnSizer()
	{
		UIView::OnSizer();
		Layout();
	}

	void OnPaint(UIDC* dc)
	{
		Gdiplus::Rect rc = Rect();

		dc->EnableSmooth(true);

		if (m_max > m_page)
		{
			rc = m_thumb_rc;

			rc.Offset(dc->m_off_x,dc->m_off_y);

			Gdiplus::SolidBrush brush(m_color);

			Gdiplus::GraphicsPath path;

			INT elWid = rc.Width;
			INT elHei = rc.Width;
			INT x = rc.X;
			INT y = rc.Y;
			INT width = rc.Width;
			INT height = rc.Height;

			path.AddArc(x,y,elWid,elHei,180,90); // ×óÉÏ½ÇÔ²»¡
			path.AddArc(x+width-elWid,y, elWid,elHei,270,90); // ÓÒÉÏ½ÇÔ²»¡
			path.AddArc(x+width-elWid,y+height-elHei, elWid,elHei,0,90); // ÓÒÏÂ½ÇÔ²»¡
			path.AddArc(x,y+height-elHei, elWid,elHei,90,90);
			path.CloseFigure();

			dc->m_dc->FillPath(&brush, &path);
			Gdiplus::Pen pen(Gdiplus::Color(70,255,255,255), 1.0);
			dc->m_dc->DrawPath(&pen, &path);
		}
	}

	void OnLButtonDown(int x, int y)
	{
		m_is_down = true;
		m_down_y = y;
		m_last_y = m_down_y;
	}

	void OnMouseMove(int x, int y)
	{
		if (m_page > m_max)
		{
			return;
		}

		Gdiplus::Rect rc = Rect();

		if (y < 0)
		{
			y = 0;
		}
		else if (y > rc.Height)
		{
			y = rc.Height;
		}

		if (m_is_down)
		{
			int off = y - m_last_y;
			float unit_pixel = (float)(rc.Height) / m_max;
			m_thumb_rc.Offset(0,off);
			if (m_thumb_rc.Y < 0)
			{
				m_thumb_rc.Offset(0, -m_thumb_rc.Y);
			}
			else if (m_thumb_rc.GetBottom() > rc.Height)
			{
				m_thumb_rc.Offset(0, rc.Height - m_thumb_rc.GetBottom());
			}
			int pos = (int)(m_thumb_rc.Y / unit_pixel);
			if (m_pos != pos)
			{
				m_pos = pos;
				//emit scroll_change();
			}
			m_last_y = y;
			Redraw();
		}
	}

	void OnLButtonUp(int x, int y)
	{
		m_is_down = false;
	}

private:


};


