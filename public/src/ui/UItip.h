#ifndef __UI_TIP_H__
#define __UI_TIP_H__

#include "UIView.h"
#include "UIFrame.h"

struct TextLine
{
	int begin_index;
	int end_index;
};


class UItip : public UIFrame
{
public:
	UItip()
	{
		m_itemWidth = 0;
		m_itemHeight = 0;
		ITEM_WIDTH = 120;
	}

	virtual ~UItip()
	{
		Destroy();
	}

	void Destroy()
	{
		UIFrame::Destroy();
	}

	LRESULT OnNcHitTest(CPoint point)
	{
		return HTTRANSPARENT;
	}
	
	void ShowTip(int x, int y,CWnd* pParete = NULL)
	{
		Gdiplus::Rect rc = CalcRect(x, y);
		CRect rc2(rc.X, rc.Y-10, rc.GetRight() + 10, rc.GetBottom());

		if(m_hWnd == NULL)
			CreateEx(WS_EX_TOOLWINDOW,NULL,NULL,WS_POPUP|WS_BORDER,CRect(CPoint(0,0),CSize(0,0)),pParete,0);

		SetWindowPos(NULL, rc2.left, rc2.top, rc2.Width(),rc2.Height(), SWP_NOZORDER|SWP_NOACTIVATE);
		ShowWindow(SW_SHOWNA);
		Invalidate();
		UpdateWindow();
	}
	virtual Gdiplus::Rect CalcRect(int x, int y)
	{
		UIDC dc(1,1);
		Gdiplus::Font* font = GetDefaultFontEx(12);

		std::vector<float> ww;
		dc.GetTextExtendEx(ww, m_strTipText, font);

		m_lines.clear();
		CalcTextLayout(ww, m_lines);

		int h = int(m_lines.size() * dc.GetFontHeight(font));
		int w = m_itemWidth;
		delete font;

		CRect rt;  
		SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rt,0);

		Gdiplus::Rect rc(x,y-h,w,h);
		if (rc.GetRight() > rt.Width())
		{
			rc.X = rc.X - w;
		}
		if (rc.GetTop() < rt.top)
		{
			rc.Y = rc.Y + h;
		}
		return rc;
	}
	
	void CalcTextLayout(std::vector<float>& ww, std::vector<TextLine>& lines)
	{
		float MAX_TEXT_WIDTH = (float)ITEM_WIDTH;
		m_itemWidth = ITEM_WIDTH;
		float x = 0;
		lines.clear();
		int i = 0;
		int begin_index = 0;
		for (; i < (int)ww.size();)
		{
			float w = ww[i];
			if (x + w > MAX_TEXT_WIDTH)
			{
				TextLine line;
				line.begin_index = begin_index;
				line.end_index = i;
				begin_index = i;
				lines.push_back(line);
				x = 0;
				continue;
			}
			if(i == ww.size()-1)
			{
				TextLine line;
				line.begin_index = begin_index;
				line.end_index = (int)ww.size();
				lines.push_back(line);
			}
			x += w;
			++i;
		}
		if (lines.size() == 0)
		{
			m_itemWidth = (int)x;
			TextLine line;
			line.begin_index = 0;
			line.end_index = (int)ww.size();
			lines.push_back(line);
			return;
		}
		if (lines.size() == 1)
		{
			m_itemWidth = (int)x;
		}
	}

	void SetTipText(CString& strTip)
	{
		m_strTipText = strTip;
	}
	void OnPaint(UIDC* dc)
	{
		Gdiplus::Rect rc = Rect();
		dc->Clear(Gdiplus::Color(255,255,255,255));
		dc->EnableSmooth(true);
		dc->DrawRect(1,Gdiplus::Color(255,0,0,0),rc);
		Gdiplus::Font* font = GetDefaultFontEx(12);
		for (size_t i=0;i<m_lines.size();i++)
		{
			CString str = m_strTipText.Mid(m_lines[i].begin_index, m_lines[i].end_index - m_lines[i].begin_index);
			DWORD nColor = GetSysColor(COLOR_INFOTEXT);
			dc->DrawTextSingleLineEx(str, Gdiplus::Color(255,GetRValue(nColor),GetGValue(nColor),GetBValue(nColor)), 
				Gdiplus::Rect(rc.X + 5,rc.Y + 5 +int(dc->GetFontHeight(font)*i),ITEM_WIDTH, (int)dc->GetFontHeight(font)), DRAW_TEXT_STYLE_VCENTER, font);
		}
	}
private:
	int								m_itemWidth;
	int								m_itemHeight;
	CString							m_strTipText;
	std::vector<TextLine>			m_lines;
	int								ITEM_WIDTH;
};



#endif