#pragma once

#include "UIView.h"
#include "FastDelegate.h"


class UIProgress : public UIView
{
public:
	UIProgress(AutoRef<UIView> parent, int id, Gdiplus::Rect rc)
		:UIView(parent, id, rc)
	{
		m_progress = 0;

		m_color = Gdiplus::Color(118,195,41);
		m_bg_color = Gdiplus::Color(26,26,26);
		m_frame_color = Gdiplus::Color(14,14,14);
	}

	~UIProgress()
	{

	}

	void Destroy()
	{
		UIView::Destroy();
	}

	void SetProgress(float progress)
	{
		if (progress < 0.0f) progress = 0.0f;
		if (progress > 1.0f) progress = 1.0f;
		m_progress = progress;
		Redraw();
	}

	void OnPaint(UIDC* dc)
	{
		Gdiplus::Rect rc = Rect();
		dc->FillRect(m_bg_color, rc);

		Gdiplus::Rect rc2 = rc;
		rc2.X += 3;
		rc2.Y += 3;
		rc2.Width -= 3 + 3;
		rc2.Height -= 3 + 3;

		if (m_progress > 0.0f)
		{
			rc2.Width = (int)(rc2.Width * (m_progress));

			dc->FillRect(m_color, rc2);
		}
		dc->DrawRect(m_frame_color, rc);
	}

	float				m_progress;
	Gdiplus::Color		m_color;
	Gdiplus::Color		m_bg_color;
	Gdiplus::Color		m_frame_color;
};


