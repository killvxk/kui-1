#pragma once

#include <Windows.h>
#include <Gdiplus.h>
#include <vector>

// DrawText 的 style字段的值
#define DRAW_TEXT_STYLE_CENTER			1
#define DRAW_TEXT_STYLE_VCENTER			2
#define DRAW_TEXT_STYLE_RIGHT			4
#define DRAW_TEXT_STYLE_END_ELLIPSIS	8
#define DRAW_TEXT_STYLE_SINGLE_LINE		16

class UIDC
{
public:
	UIDC(int width, int height);
	~UIDC(void);

	void Save();
	void Restore();

	void SetClip(Gdiplus::Rect rc);
	void SetClip(Gdiplus::GraphicsPath* path);
	void SetClipRoundRect(Gdiplus::Rect rc, float round);
	void SetClipRaw(Gdiplus::Rect rc);
	void ResetClip();

	void Reset();

	void Clear(Gdiplus::Color color);

	void EnableSmooth(bool is_enable);
	void EnableZoomImageSmooth(bool is_enable);
	void EnableFontAntiAlias(bool is_enable);

	void DrawImage(Gdiplus::Image* image, Gdiplus::Rect dst_rc, Gdiplus::Rect src_rc, int alpha = 255);
	void DrawImage(Gdiplus::Image* image, Gdiplus::Rect dst_rc, int alpha = 255);
	void DrawImage(Gdiplus::Image* image, int x, int y, int alpha = 255);
	void DrawImageCenter(Gdiplus::Image* image, Gdiplus::Rect dst_rc, int alpha = 255);

	void DrawLine(Gdiplus::Pen* pen, int x1, int y1, int x2, int y2);
	void DrawLine(float pen_width, Gdiplus::Color color, int x1, int y1, int x2, int y2);
	void DrawRect(Gdiplus::Pen* pen, Gdiplus::Rect rc);
	void DrawRect(Gdiplus::Color pen_color, Gdiplus::Rect rc);
	void DrawRect(float pen_width, Gdiplus::Color pen_color, Gdiplus::Rect rc);

	void FillRoundRect(Gdiplus::Color color, float round, Gdiplus::Rect rc);
	void FillRoundRect(Gdiplus::Color color1, Gdiplus::Color color2,float round, Gdiplus::Rect rc,int nHeight);
	void FillRoundRectGradientVertical(Gdiplus::Color color1, Gdiplus::Color color2, float round, Gdiplus::Rect rc);
	void DrawRoundRect(float pen_width, Gdiplus::Color pen_color, float round, Gdiplus::Rect rc);

	void FillRect(Gdiplus::Color color, Gdiplus::Rect rc);

	void DrawPath(Gdiplus::GraphicsPath* path, float pen_width, Gdiplus::Color color);
	void FillPath(Gdiplus::GraphicsPath* path, Gdiplus::Color color);

	void FillRectGradientVertical(Gdiplus::Rect rc, Gdiplus::Color color1, Gdiplus::Color color2);

	void DrawTextSingleLine(CString& txt, Gdiplus::Color color, Gdiplus::Rect rc, int style = 0, HFONT hfont = NULL);
	void DrawTextSingleLine(CString& txt, Gdiplus::Color color, Gdiplus::Color bgColor, Gdiplus::Rect rc, int style = 0, HFONT hfont = NULL);
	void DrawTextSingleLineEx(CString& txt, Gdiplus::Color color, Gdiplus::Rect rc, int style = 0, Gdiplus::Font* font = NULL);
	void DrawTextSingleLineShadow(CString& txt, Gdiplus::Color Textcolor,Gdiplus::Color Shadowcolor, Gdiplus::Rect rc, int style, int fontSize);

	void DrawText(CString& txt, Gdiplus::Color color, Gdiplus::Color bgColor, Gdiplus::Rect rc, int style = 0, HFONT hfont = NULL);
	void DrawTextEx(CString& txt, Gdiplus::Color color, Gdiplus::Rect rc, int style = 0, Gdiplus::Font* font = NULL);

	void GetTextSize(CString& txt, SIZE& size, HFONT hfont = NULL);

	Gdiplus::Rect GetTextSizeEx(CString& txt, Gdiplus::Font* font = NULL);
	void GetTextSizeEx(CString& txt, Gdiplus::RectF& in_rc, Gdiplus::RectF& rc, int style, Gdiplus::Font* font = NULL);

	void GetTextExtend(std::vector<float>& ww, CString& txt, HFONT hfont);
	void GetTextExtendEx(std::vector<float>& ww, CString& txt, Gdiplus::Font* font);
	Gdiplus::Rect GetTextBounding(CString& txt, Gdiplus::Rect rc, int style, HFONT hfont);
	Gdiplus::Rect GetTextBoundingEx(CString& txt, Gdiplus::Rect rc, int style, Gdiplus::Font* font);

	float GetFontHeight(Gdiplus::Font* font);
	float GetFontHeight(HFONT hfont);

	float GetCharWidth(HFONT hfont, wchar_t c);
	float GetCharWidthEx(Gdiplus::Font* font, wchar_t c);

	Gdiplus::RectF UIDC::GetCharRectEx(Gdiplus::Font* font, wchar_t c);

	int HittestText(CString& txt, Gdiplus::Rect rc, int x, int y, int style, HFONT hfont);
	int HittestTextEx(CString& txt, Gdiplus::Rect rc, int x, int y, int style, Gdiplus::Font* font);

	Gdiplus::Point GetTextPosByIndex(CString& txt, Gdiplus::Rect rc, int index, int style, HFONT hfont);
	Gdiplus::Point GetTextPosByIndexEx(CString& txt, Gdiplus::Rect rc, int index, int style, Gdiplus::Font* font);

	static Gdiplus::Color AddColorBrightness(Gdiplus::Color color, float brightness); // 添加颜色的亮度，brightness取值范围 1.0f ~ -1.0f

	bool SaveImage(CString& filename, int quality = 90);


	Gdiplus::Graphics*		m_dc;
	int						m_off_x;
	int						m_off_y;
	int						m_width;
	int						m_height;

	Gdiplus::Bitmap*		m_image;

	struct SaveInfo
	{
		int						off_x;
		int						off_y;
		Gdiplus::GraphicsState	dc_state;
	};

	std::vector<SaveInfo>	m_save_infos;

};




