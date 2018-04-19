#include "StdAfx.h"
#include "UIDC.h"
#include "UIView.h"
#include "ZipSkin.h"

UIDC::UIDC(int width, int height)
{
	m_image = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
	m_dc = new Gdiplus::Graphics(m_image);
	m_dc->SetTextRenderingHint(Gdiplus::TextRenderingHintSystemDefault);
	//m_dc->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	m_width = width;
	m_height = height;
	m_off_x = m_off_y = 0;
}

UIDC::~UIDC(void)
{
	Reset();
	if (m_dc)
	{
		delete m_dc;
		m_dc = NULL;
	}
	if (m_image)
	{
		delete m_image;
		m_image = NULL;
	}
}

struct _LineInfo
{
	int begin_i;
	int end_i;
	float width;
	float start_x;
};

static void _GetLineInfo(CString& txt, std::vector<_LineInfo>& vLines, int style, 
						 std::vector<float>& ww, int max_width, bool is_no_wrap)
{
	vLines.clear();

	int begin_i = 0;
	int i = 0;
	float width = 0;
	float xx = 0;

	int count = txt.GetLength();

	_LineInfo line;

	while (i < count)
	{
		wchar_t c = txt.GetAt(i);
		if (c == '\n')
		{
			++i;
			line.begin_i = begin_i;
			line.end_i = i;
			line.width = xx;
			vLines.push_back(line);
			if (xx > width) width = xx;
			xx = 0;
			begin_i = i;
			if (i == count)
			{
				// add a empty line
				line.begin_i = begin_i;
				line.end_i = i;
				line.width = 0;
				vLines.push_back(line);
			}
			continue;
		}

		float char_width = ww[i];

		if (char_width + xx > max_width && i - begin_i >= 1)
		{
			if (is_no_wrap)
			{
				xx = (float)max_width;
				// search next line
				while (i < count)
				{
					c = txt.GetAt(i);
					if (c == '\n')
					{
						++i;
						line.begin_i = begin_i;
						line.end_i = i;
						line.width = xx;
						vLines.push_back(line);
						if (xx > width) width = xx;
						xx = 0;
						begin_i = i;
						break;
					}
					else
					{
						++i;
					}
				} // while
			}
			else
			{
				line.begin_i = begin_i;
				line.end_i = i;
				line.width = xx;
				vLines.push_back(line);
				if (xx > width) width = xx;
				xx = 0;
				begin_i = i;
				continue;
			}
		}
		else
		{
			xx += char_width;
			++i;
		}
	} // while

	if (begin_i < i)
	{
		line.begin_i = begin_i;
		line.end_i = i;
		line.width = xx;
		vLines.push_back(line);
		if (xx > width) width = xx;
	}

	if (vLines.size() == 0)
	{
		line.begin_i = 0;
		line.end_i = 0;
		line.width = 0;
		vLines.push_back(line);
	}

	// calc line start_x
	// center align
	if ((style & DRAW_TEXT_STYLE_CENTER) == DRAW_TEXT_STYLE_CENTER)
	{
		for (i = 0; i < (int)vLines.size(); ++i)
		{
			_LineInfo& line = vLines[i];
			line.start_x = (max_width - line.width) / 2;
		}
	}
	// right align
	else if ((style & DRAW_TEXT_STYLE_RIGHT) == DRAW_TEXT_STYLE_RIGHT)
	{
		for (i = 0; i < (int)vLines.size(); ++i)
		{
			_LineInfo& line = vLines[i];
			line.start_x = max_width - line.width;
		}
	}
	// left align
	else
	{
		for (i = 0; i < (int)vLines.size(); ++i)
		{
			_LineInfo& line = vLines[i];
			line.start_x = 0;
		}
	}
}


void UIDC::Save()
{
	SaveInfo info;
	info.off_x = m_off_x;
	info.off_y = m_off_y;
	info.dc_state = m_dc->Save();
	m_save_infos.push_back(info);
}

void UIDC::Restore()
{
	if (m_save_infos.size() > 0)
	{
		std::vector<SaveInfo>::reverse_iterator iter = m_save_infos.rbegin();

		m_off_x = iter->off_x;
		m_off_y = iter->off_y;
		
		m_dc->Restore(iter->dc_state);

		m_save_infos.resize(m_save_infos.size() - 1);
	}
}

void UIDC::SetClip(Gdiplus::Rect rc)
{
	rc.Offset(m_off_x, m_off_y);
	m_dc->SetClip(rc, Gdiplus::CombineModeIntersect);

	Gdiplus:: Rect rc2;
	m_dc->GetClipBounds(&rc2);

	int ii = 0;
}

void UIDC::SetClip(Gdiplus::GraphicsPath* path)
{
	Gdiplus::Matrix matrix;
	matrix.Translate((float)m_off_x, (float)m_off_y, Gdiplus::MatrixOrderAppend);

	Gdiplus::GraphicsPath* path2 = path->Clone();
	path2->Transform(&matrix);

	m_dc->SetClip(path2, Gdiplus::CombineModeIntersect);
	delete path2;
}

void UIDC::SetClipRoundRect(Gdiplus::Rect rc, float round)
{
	Gdiplus::GraphicsPath path;

	float x = (float)rc.X + m_off_x;
	float y = (float)rc.Y + m_off_y;
	float width = (float)rc.Width;
	float height = (float)rc.Height;

	if (round > 0)
	{
		path.AddArc(x, y, round, round, 180, 90); // ×óÉÏ½ÇÔ²»¡
		path.AddArc(x + width - round, y, round, round, 270, 90); // ÓÒÉÏ½ÇÔ²»¡
		path.AddArc(x + width - round, y + height - round, round, round, 0, 90); // ÓÒÏÂ½ÇÔ²»¡
		path.AddArc(x, y + height - round, round, round, 90, 90);
		path.CloseFigure();
	}
	else
	{
		path.AddRectangle(Gdiplus::Rect((int)x,(int)y,(int)width,(int)height));
	}

	m_dc->SetClip(&path, Gdiplus::CombineModeIntersect);
}

void UIDC::SetClipRaw(Gdiplus::Rect rc)
{
	m_dc->SetClip(rc, Gdiplus::CombineModeIntersect);
}

void UIDC::ResetClip()
{
	m_dc->ResetClip();
}

void UIDC::Reset()
{
	for (int i = 0; i < (int)m_save_infos.size(); ++i)
	{
		SaveInfo& info = m_save_infos[i];

		m_dc->Restore(info.dc_state);
	}
	m_save_infos.clear();
	m_dc->ResetClip();

	m_off_x = m_off_y = 0;

}

void UIDC::Clear(Gdiplus::Color color)
{
	m_dc->Clear(color);
}

void UIDC::EnableSmooth(bool is_enable)
{
	if (is_enable)
	{
		m_dc->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	}
	else
	{
		m_dc->SetSmoothingMode(Gdiplus::SmoothingModeDefault);
	}
}

void UIDC::EnableZoomImageSmooth(bool is_enable)
{
	if (is_enable)
	{
		m_dc->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		//m_dc->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBilinear);
	}
	else
	{
		m_dc->SetInterpolationMode(Gdiplus::InterpolationModeDefault);
	}
}

void UIDC::EnableFontAntiAlias(bool is_enable)
{
	if (is_enable)
	{
		m_dc->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	}
	else
	{
		m_dc->SetTextRenderingHint(Gdiplus::TextRenderingHintSystemDefault);
	}
}

void UIDC::DrawImage(Gdiplus::Image* image, Gdiplus::Rect dst_rc, Gdiplus::Rect src_rc, int alpha)
{
	if (image == NULL)
	{
		return;
	}
	dst_rc.Offset(m_off_x, m_off_y);

	if (alpha > 255)
	{
		alpha = 255;
	}

	Gdiplus::ImageAttributes a;
	Gdiplus::ImageAttributes* attri = NULL;
	if (alpha < 255)
	{
		Gdiplus::ColorMatrix ClrMatrix =         { 
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, (float)alpha / 255.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		a.SetColorMatrix(&ClrMatrix, Gdiplus::ColorMatrixFlagsDefault, 
			Gdiplus::ColorAdjustTypeBitmap);

		attri = &a;
	}
	m_dc->DrawImage(image, dst_rc, src_rc.X, src_rc.Y, src_rc.Width, src_rc.Height, Gdiplus::UnitPixel, attri);
}

void UIDC::DrawImage(Gdiplus::Image* image, int x, int y, int alpha)
{
	if (image == NULL)
	{
		return;
	}

	Gdiplus::Rect dst_rc(x,y,image->GetWidth(),image->GetHeight());

	dst_rc.Offset(m_off_x, m_off_y);

	if (alpha > 255)
	{
		alpha = 255;
	}

	Gdiplus::ImageAttributes a;
	Gdiplus::ImageAttributes* attri = NULL;
	if (alpha < 255)
	{
		Gdiplus::ColorMatrix ClrMatrix =         { 
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, (float)alpha / 255.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		a.SetColorMatrix(&ClrMatrix, Gdiplus::ColorMatrixFlagsDefault, 
			Gdiplus::ColorAdjustTypeBitmap);

		attri = &a;
	}

	Gdiplus::Rect src_rc = Gdiplus::Rect(0,0,image->GetWidth(),image->GetHeight());

	m_dc->DrawImage(image, dst_rc, src_rc.X, src_rc.Y, src_rc.Width, src_rc.Height, Gdiplus::UnitPixel, attri);
}

void UIDC::DrawImage(Gdiplus::Image* image, Gdiplus::Rect dst_rc, int alpha)
{
	if (image == NULL)
	{
		return;
	}

	dst_rc.Offset(m_off_x, m_off_y);

	if (alpha > 255)
	{
		alpha = 255;
	}

	Gdiplus::Rect src_rc = Gdiplus::Rect(0,0,image->GetWidth(),image->GetHeight());

	Gdiplus::ImageAttributes a;
	Gdiplus::ImageAttributes* attri = NULL;
	if (alpha < 255)
	{
		Gdiplus::ColorMatrix ClrMatrix =         { 
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, (float)alpha / 255.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		a.SetColorMatrix(&ClrMatrix, Gdiplus::ColorMatrixFlagsDefault, 
			Gdiplus::ColorAdjustTypeBitmap);

		attri = &a;
	}
	Gdiplus::Status s = m_dc->DrawImage(image, dst_rc, src_rc.X, src_rc.Y, src_rc.Width, src_rc.Height, Gdiplus::UnitPixel, attri);
	int kk = 0;
}

void UIDC::DrawImageCenter(Gdiplus::Image* image, Gdiplus::Rect dst_rc, int alpha)
{
	if (image == NULL)
	{
		return;
	}

	dst_rc.Offset(m_off_x, m_off_y);

	if (alpha > 255)
	{
		alpha = 255;
	}

	Gdiplus::Rect src_rc = Gdiplus::Rect(0,0,image->GetWidth(),image->GetHeight());

	Gdiplus::ImageAttributes a;
	Gdiplus::ImageAttributes* attri = NULL;
	if (alpha < 255)
	{
		Gdiplus::ColorMatrix ClrMatrix =         { 
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, (float)alpha / 255.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		a.SetColorMatrix(&ClrMatrix, Gdiplus::ColorMatrixFlagsDefault, 
			Gdiplus::ColorAdjustTypeBitmap);

		attri = &a;
	}

	int x = dst_rc.X + ((dst_rc.Width - src_rc.Width) / 2);
	int y = dst_rc.Y + ((dst_rc.Height - src_rc.Height) / 2);
	m_dc->DrawImage(image, Gdiplus::Rect(x,y,src_rc.Width,src_rc.Height), src_rc.X, src_rc.Y, src_rc.Width, src_rc.Height, Gdiplus::UnitPixel, attri);
}

void UIDC::DrawLine(Gdiplus::Pen* pen, int x1, int y1, int x2, int y2)
{
	m_dc->DrawLine(pen, x1+m_off_x, y1+m_off_y, x2+m_off_x, y2+m_off_y);
}

void UIDC::DrawLine(float pen_width, Gdiplus::Color color, int x1, int y1, int x2, int y2)
{
	Gdiplus::Pen pen(color, pen_width);
	m_dc->DrawLine(&pen, x1+m_off_x, y1+m_off_y, x2+m_off_x, y2+m_off_y);
}

void UIDC::DrawRect(Gdiplus::Pen* pen, Gdiplus::Rect rc)
{
	rc.Offset(m_off_x, m_off_y);
	float pen_width = pen->GetWidth();
	float half = pen_width * 0.5f;

	m_dc->DrawRectangle(pen, Gdiplus::RectF(rc.X+half-0.5f,rc.Y+half-0.5f,rc.Width-pen_width,rc.Height-pen_width));
}

void UIDC::FillRoundRect(Gdiplus::Color color, float round, Gdiplus::Rect rc)
{
	Gdiplus::GraphicsPath path;

	float x = (float)rc.X + m_off_x + 0.5f;
	float y = (float)rc.Y + m_off_y + 0.5f;
	float width = (float)rc.Width - 1.0f;
	float height = (float)rc.Height - 1.0f;

	if (m_dc->GetSmoothingMode() == Gdiplus::SmoothingModeAntiAlias)
	{
		width -= 0.5f;
		height -= 0.5f;
	}

	if (round == 0)
	{
		path.AddLine(x,y,x+width,y);
		path.AddLine(x+width,y+height,x,y+height);
		path.CloseFigure();
	}
	else
	{
		path.AddArc(x, y, round, round, 180, 90); // ×óÉÏ½ÇÔ²»¡
		path.AddArc(x + width - round, y, round, round, 270, 90); // ÓÒÉÏ½ÇÔ²»¡
		path.AddArc(x + width - round, y + height - round, round, round, 0, 90); // ÓÒÏÂ½ÇÔ²»¡
		path.AddArc(x, y + height - round, round, round, 90, 90);
		path.CloseFigure();
	}

	//Gdiplus::Matrix matrix;
	//matrix.Translate((float)m_off_x, (float)m_off_y, Gdiplus::MatrixOrderAppend);

	//path.Transform(&matrix);

	Gdiplus::SolidBrush brush(color);
	Gdiplus::Status status = m_dc->FillPath(&brush, &path);
	int ii = 0;
}

void UIDC::FillRoundRectGradientVertical(Gdiplus::Color color1, Gdiplus::Color color2, float round, Gdiplus::Rect rc)
{
	Gdiplus::GraphicsPath path;

	float x = (float)rc.X + m_off_x;
	float y = (float)rc.Y + m_off_y;
	float width = (float)rc.Width;
	float height = (float)rc.Height;

	if (m_dc->GetSmoothingMode() == Gdiplus::SmoothingModeAntiAlias)
	{
		width -= 0.5f;
		height -= 0.5f;
	}

	if (round == 0)
	{
		path.AddLine(x,y,x+width,y);
		path.AddLine(x+width,y+height,x,y+height);
		path.CloseFigure();
	}
	else
	{
		path.AddArc(x, y, round, round, 180, 90); // ×óÉÏ½ÇÔ²»¡
		path.AddArc(x + width - round, y, round, round, 270, 90); // ÓÒÉÏ½ÇÔ²»¡
		path.AddArc(x + width - round, y + height - round, round, round, 0, 90); // ÓÒÏÂ½ÇÔ²»¡
		path.AddArc(x, y + height - round, round, round, 90, 90);
		path.CloseFigure();
	}

	Gdiplus::Rect rc2 = rc;
	rc.Offset(m_off_x, m_off_y);
	Gdiplus::LinearGradientBrush brush(rc, color1,color2, Gdiplus::LinearGradientModeVertical);

	Gdiplus::Status status = m_dc->FillPath(&brush, &path);
}
void UIDC::FillRoundRect(Gdiplus::Color color1, Gdiplus::Color color2,float round, Gdiplus::Rect rc,int nHeight)
{
	float x = (float)rc.X + m_off_x + 0.5f;
	float y = (float)rc.Y + m_off_y + 0.5f;
	float width = (float)rc.Width - 1.0f;
	float height = (float)rc.Height - 1.0f;

	if (m_dc->GetSmoothingMode() == Gdiplus::SmoothingModeAntiAlias)
	{
		width -= 0.5f;
		height -= 0.5f;
	}

	Gdiplus::GraphicsPath path;
	if (round == 0)
	{
		path.AddLine(x,y,x+width,y);
		path.AddLine(x+width,y+nHeight,x,y+nHeight);
		path.CloseFigure();
	}
	else
	{
		path.AddArc(x, y, round, round, 180, 90); // ×óÉÏ½ÇÔ²»¡
		path.AddArc(x+width-round, y, round, round, 270, 90); // ÓÒÉÏ½ÇÔ²»¡
		path.AddLine(x+width,y+nHeight,x,y+nHeight);
		path.CloseFigure();
	}
	Gdiplus::SolidBrush br(color1);
	m_dc->FillPath(&br, &path);

	Gdiplus::GraphicsPath path1;
	if (round == 0)
	{
		path1.AddLine(x,y+nHeight,x+width,y+nHeight);
		path1.AddLine(x+width,y+height,x,y+height);
		path1.CloseFigure();
	}
	else
	{
		path1.AddLine(x,y+nHeight,x+width,y+nHeight);
		path1.AddArc(x+width- round, y+height-round, round, round, 0, 90); // ÓÒÏÂ½ÇÔ²»¡
		path1.AddArc(x, y+height-round, round, round, 90, 90);
		path1.CloseFigure();
	}

	Gdiplus::SolidBrush br1(color2);
	m_dc->FillPath(&br1, &path1);
}
void UIDC::DrawRoundRect(float pen_width, Gdiplus::Color pen_color, float round, Gdiplus::Rect rc)
{
	float half = pen_width * 0.5f;

	Gdiplus::GraphicsPath path;

	float x = (float)rc.X + m_off_x;
	float y = (float)rc.Y + m_off_y;
	float width = (float)rc.Width - pen_width;
	float height = (float)rc.Height - pen_width;

	if (round == 0)
	{
		path.AddLine(x,y,x+width,y);
		path.AddLine(x+width,y+height,x,y+height);
		path.CloseFigure();
	}
	else
	{
		path.AddArc(x, y, round, round, 180, 90); // ×óÉÏ½ÇÔ²»¡
		path.AddArc(x + width - round, y, round, round, 270, 90); // ÓÒÉÏ½ÇÔ²»¡
		path.AddArc(x + width - round, y + height - round, round, round, 0, 90); // ÓÒÏÂ½ÇÔ²»¡
		path.AddArc(x, y + height - round, round, round, 90, 90);
		path.CloseFigure();
	}

	//Gdiplus::Matrix matrix;
	//matrix.Translate((float)m_off_x, (float)m_off_y, Gdiplus::MatrixOrderAppend);

	//path.Transform(&matrix);

	Gdiplus::Pen pen(pen_color, pen_width);
	m_dc->DrawPath(&pen, &path);
}

void UIDC::DrawRect(Gdiplus::Color pen_color, Gdiplus::Rect rc)
{
	rc.Offset(m_off_x, m_off_y);

	Gdiplus::Pen pen(pen_color, 1.0f);
	m_dc->DrawRectangle(&pen, Gdiplus::Rect(rc.X,rc.Y,rc.Width-1,rc.Height-1));
}

void UIDC::DrawRect(float pen_width, Gdiplus::Color pen_color, Gdiplus::Rect rc)
{
	rc.Offset(m_off_x, m_off_y);

	float half = pen_width * 0.5f;
	Gdiplus::Pen pen(pen_color, pen_width);
	m_dc->DrawRectangle(&pen, Gdiplus::RectF(rc.X+half-0.5f,rc.Y+half-0.5f,rc.Width-pen_width,rc.Height-pen_width));
}

void UIDC::FillRect(Gdiplus::Color color, Gdiplus::Rect rc)
{
	rc.Offset(m_off_x, m_off_y);

	Gdiplus::SolidBrush brush(color);
	if (m_dc->GetSmoothingMode() == Gdiplus::SmoothingModeAntiAlias)
	{
		m_dc->FillRectangle(&brush, Gdiplus::RectF(rc.X+0.5f,rc.Y+0.5f,rc.Width-1.0f,rc.Height-1.0f));
	}
	else
	{
		m_dc->FillRectangle(&brush, rc);
	}
}

void UIDC::DrawPath(Gdiplus::GraphicsPath* path, float pen_width, Gdiplus::Color color)
{
	Gdiplus::Matrix matrix;
	matrix.Translate((float)m_off_x, (float)m_off_y, Gdiplus::MatrixOrderAppend);

	Gdiplus::GraphicsPath* path2 = path->Clone();
	path2->Transform(&matrix);

	Gdiplus::Pen pen(color, pen_width);
	m_dc->DrawPath(&pen, path2);
	delete path2;
}

void UIDC::FillPath(Gdiplus::GraphicsPath* path, Gdiplus::Color color)
{
	Gdiplus::Matrix matrix;
	matrix.Translate((float)m_off_x, (float)m_off_y, Gdiplus::MatrixOrderAppend);

	Gdiplus::GraphicsPath* path2 = path->Clone();
	path2->Transform(&matrix);

	Gdiplus::SolidBrush brush(color);
	m_dc->FillPath(&brush, path2);
	delete path2;
}

void UIDC::FillRectGradientVertical(Gdiplus::Rect rc, Gdiplus::Color color1, Gdiplus::Color color2)
{
	rc.Offset(m_off_x, m_off_y);

	Gdiplus::RectF rc2;

	rc2 = Gdiplus::RectF(rc.X-0.5f, rc.Y-0.5f, (float)rc.Width, (float)rc.Height);

	Gdiplus::LinearGradientBrush brush(rc2, color1, color2, Gdiplus::LinearGradientModeVertical);
	m_dc->FillRectangle(&brush, rc2);
}

void UIDC::DrawTextSingleLine(CString& txt, Gdiplus::Color color, Gdiplus::Rect rc, int style, HFONT hFont)
{
	Gdiplus::Rect rc_clip; 
	Gdiplus::Status status = m_dc->GetClipBounds(&rc_clip);

	HDC hDC = m_dc->GetHDC();

	HFONT hNewFont = NULL;

	if (hFont == NULL)
	{
		hNewFont = hFont = GetDefaultFont();
	}

	int save_id = SaveDC(hDC);
	::IntersectClipRect(hDC, rc_clip.X, rc_clip.Y, rc_clip.GetRight(), rc_clip.GetBottom());

	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	UINT s = DT_SINGLELINE;

	if ((style & DRAW_TEXT_STYLE_VCENTER) == DRAW_TEXT_STYLE_VCENTER)
	{
		s |= DT_VCENTER;
	}
	if ((style & DRAW_TEXT_STYLE_CENTER) == DRAW_TEXT_STYLE_CENTER)
	{
		s |= DT_CENTER;
	}
	if ((style & DRAW_TEXT_STYLE_RIGHT) == DRAW_TEXT_STYLE_RIGHT)
	{
		s |= DT_RIGHT;
	}
	if ((style & DRAW_TEXT_STYLE_END_ELLIPSIS) == DRAW_TEXT_STYLE_END_ELLIPSIS)
	{
		s |= DT_END_ELLIPSIS;
	}

	CRect rc2(rc.X + m_off_x, rc.Y + m_off_y, m_off_x + rc.X + rc.Width, m_off_y + rc.Y + rc.Height);

	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, color.ToCOLORREF());

	::DrawText(hDC, txt, txt.GetLength(), rc2, s);

	SelectObject(hDC, hOldFont);

	RestoreDC(hDC,save_id);
	m_dc->ReleaseHDC(hDC);

	if (hNewFont)
	{
		DeleteObject(hNewFont);
	}
}

void UIDC::DrawTextSingleLine(CString& txt, Gdiplus::Color color, Gdiplus::Color bgColor, Gdiplus::Rect rc, int style, HFONT hFont)
{
	Gdiplus::Rect rc_clip; 
	Gdiplus::Status status = m_dc->GetClipBounds(&rc_clip);

	HDC hDC = m_dc->GetHDC();

	HFONT hNewFont = NULL;

	if (hFont == NULL)
	{
		hNewFont = hFont = GetDefaultFont();
	}

	int save_id = SaveDC(hDC);
	::IntersectClipRect(hDC, rc_clip.X, rc_clip.Y, rc_clip.GetRight(), rc_clip.GetBottom());

	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	UINT s = DT_SINGLELINE;

	if ((style & DRAW_TEXT_STYLE_VCENTER) == DRAW_TEXT_STYLE_VCENTER)
	{
		s |= DT_VCENTER;
	}
	if ((style & DRAW_TEXT_STYLE_CENTER) == DRAW_TEXT_STYLE_CENTER)
	{
		s |= DT_CENTER;
	}
	if ((style & DRAW_TEXT_STYLE_RIGHT) == DRAW_TEXT_STYLE_RIGHT)
	{
		s |= DT_RIGHT;
	}
	if ((style & DRAW_TEXT_STYLE_END_ELLIPSIS) == DRAW_TEXT_STYLE_END_ELLIPSIS)
	{
		s |= DT_END_ELLIPSIS;
	}

	CRect rc2(rc.X + m_off_x, rc.Y + m_off_y, m_off_x + rc.X + rc.Width, m_off_y + rc.Y + rc.Height);

	//SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, color.ToCOLORREF());
	SetBkColor(hDC, bgColor.ToCOLORREF());

	int ret = ::DrawText(hDC, txt, txt.GetLength(), rc2, s);

	SelectObject(hDC, hOldFont);

	RestoreDC(hDC,save_id);
	m_dc->ReleaseHDC(hDC);

	if (hNewFont)
	{
		DeleteObject(hNewFont);
	}
}
typedef struct
{
	TCHAR gStrFontName[MAX_PATH];
	TCHAR strFontName[MAX_PATH];
}FONTNAME,*PFONTNAME;
static CString strFontName;

static INT_PTR CALLBACK NoticeNEnumFontNameProc(LOGFONT *plf, TEXTMETRIC* /*ptm*/, INT /*nFontType*/, LPARAM lParam)
{
	PFONTNAME pStructData = (PFONTNAME) lParam;
	if (_tcscmp(plf->lfFaceName, pStructData->gStrFontName) == 0)
	{
		_tcscpy_s(pStructData->strFontName,MAX_PATH, plf->lfFaceName);
		return FALSE;
	}
	return TRUE;
}

void UIDC::DrawTextSingleLineShadow(CString& txt, Gdiplus::Color Textcolor,Gdiplus::Color Shadowcolor, Gdiplus::Rect rc, int style, int fontSize)
{
	rc.Offset(m_off_x, m_off_y);

	Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());

	if ((style & DRAW_TEXT_STYLE_VCENTER) == DRAW_TEXT_STYLE_VCENTER)
	{
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	}
	if ((style & DRAW_TEXT_STYLE_CENTER) == DRAW_TEXT_STYLE_CENTER)
	{
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
	}
	if ((style & DRAW_TEXT_STYLE_RIGHT) == DRAW_TEXT_STYLE_RIGHT)
	{
		format.SetAlignment(Gdiplus::StringAlignmentFar);
	}
	if ((style & DRAW_TEXT_STYLE_END_ELLIPSIS) == DRAW_TEXT_STYLE_END_ELLIPSIS)
	{
		format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
	}
	format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

	Gdiplus::GraphicsPath path;
	if(strFontName.IsEmpty())
	{
		strFontName = _T("Î¢ÈíÑÅºÚ");
		CDC dcScreen;
		dcScreen.CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
		FONTNAME fontName = {0};
		_tcscpy_s(fontName.gStrFontName ,MAX_PATH, strFontName);
		EnumFontFamilies(dcScreen, (LPTSTR) NULL, (FONTENUMPROC)NoticeNEnumFontNameProc, (LPARAM)&fontName);
		LOGFONT lf = {0};
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);
		if (wcslen(fontName.strFontName) == 0)
			strFontName = lf.lfFaceName;
	}

	Gdiplus::FontFamily fontFamily(strFontName);
	path.AddString(txt,txt.GetLength(),&fontFamily,Gdiplus::FontStyleRegular,(float)fontSize,rc,&format);
	for(int i=1; i<=1; ++i)
	{
		Gdiplus::Pen pen(Shadowcolor,(float)i);
		pen.SetLineJoin(Gdiplus::LineJoinRound);
		m_dc->DrawPath(&pen,&path);
	}
	Gdiplus::SolidBrush brush(Textcolor);
	m_dc->FillPath(&brush,&path);

}
void UIDC::DrawTextSingleLineEx(CString& txt, Gdiplus::Color color, Gdiplus::Rect rc, int style, Gdiplus::Font* font)
{
	Gdiplus::Font* pNewFont = NULL;
	if (font == NULL)
	{
		font = pNewFont = GetDefaultFontEx();
	}

	Gdiplus::SolidBrush brush(color);

	Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());

	if ((style & DRAW_TEXT_STYLE_VCENTER) == DRAW_TEXT_STYLE_VCENTER)
	{
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	}
	if ((style & DRAW_TEXT_STYLE_CENTER) == DRAW_TEXT_STYLE_CENTER)
	{
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
	}
	if ((style & DRAW_TEXT_STYLE_RIGHT) == DRAW_TEXT_STYLE_RIGHT)
	{
		format.SetAlignment(Gdiplus::StringAlignmentFar);
	}
	if ((style & DRAW_TEXT_STYLE_END_ELLIPSIS) == DRAW_TEXT_STYLE_END_ELLIPSIS)
	{
		format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
	}
	format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

	Gdiplus::Status status = m_dc->DrawString(txt, txt.GetLength(), font, Gdiplus::RectF((float)rc.X+m_off_x,(float)rc.Y+m_off_y,(float)rc.Width,(float)rc.Height), &format, &brush);

	if (pNewFont)
	{
		delete pNewFont;
	}
}

void UIDC::DrawText(CString& txt, Gdiplus::Color color, Gdiplus::Color bgColor, Gdiplus::Rect rc, int style, HFONT hFont)
{
	{
		Gdiplus::Rect rc_clip; 
		Gdiplus::Status status = m_dc->GetClipBounds(&rc_clip);

		HFONT hNewFont = NULL;

		if (hFont == NULL)
		{
			hNewFont = hFont = GetDefaultFont();
		}

		int font_height = (int)GetFontHeight(hFont);

		std::vector<float> ww;
		GetTextExtend(ww, txt, hFont);
		std::vector<_LineInfo> vLines;
		_GetLineInfo(txt, vLines, style, ww, rc.Width, false);

		HDC hDC = m_dc->GetHDC();

		int save_id = SaveDC(hDC);
		::IntersectClipRect(hDC, rc_clip.X, rc_clip.Y, rc_clip.GetRight(), rc_clip.GetBottom());

		HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

		UINT s = 0;

		if ((style & DRAW_TEXT_STYLE_VCENTER) == DRAW_TEXT_STYLE_VCENTER)
		{
			s |= DT_VCENTER | DT_SINGLELINE;
		}
		if ((style & DRAW_TEXT_STYLE_CENTER) == DRAW_TEXT_STYLE_CENTER)
		{
			s |= DT_CENTER;
		}
		if ((style & DRAW_TEXT_STYLE_RIGHT) == DRAW_TEXT_STYLE_RIGHT)
		{
			s |= DT_RIGHT;
		}
		if ((style & DRAW_TEXT_STYLE_END_ELLIPSIS) == DRAW_TEXT_STYLE_END_ELLIPSIS)
		{
			s |= DT_END_ELLIPSIS | DT_SINGLELINE;
		}

		//SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, color.ToCOLORREF());
		SetBkColor(hDC, bgColor.ToCOLORREF());

		if (vLines.size() <= 1)
		{
			CRect rc2(rc.X + m_off_x, rc.Y + m_off_y, m_off_x + rc.X + rc.Width, m_off_y + rc.Y + rc.Height);

			::DrawText(hDC, txt, txt.GetLength(), rc2, s);
		}
		else
		{
			int y = rc.Y + m_off_y + ((rc.Height - font_height*(int)vLines.size()) / 2);
			int x = rc.X + m_off_x;
			for (int i = 0 ; i < (int)vLines.size(); ++i)
			{
				_LineInfo& line = vLines[i];
				CString str = txt.Mid(line.begin_i, line.end_i - line.begin_i);
				::DrawText(hDC, str, str.GetLength(), CRect(x + (int)line.start_x, y, x + (int)line.start_x + (int)line.width, y + font_height), DT_SINGLELINE | DT_LEFT);
				y += font_height;
			}
		}

		SelectObject(hDC, hOldFont);

		RestoreDC(hDC,save_id);
		m_dc->ReleaseHDC(hDC);

		if (hNewFont)
		{
			DeleteObject(hNewFont);
		}
	}
}

void UIDC::DrawTextEx(CString& txt, Gdiplus::Color color, Gdiplus::Rect rc, int style, Gdiplus::Font* font)
{
	Gdiplus::Font* pNewFont = NULL;
	if (font == NULL)
	{
		font = pNewFont = GetDefaultFontEx();
	}

	Gdiplus::SolidBrush brush(color);

	Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());

	if ((style & DRAW_TEXT_STYLE_VCENTER) == DRAW_TEXT_STYLE_VCENTER)
	{
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	}
	if ((style & DRAW_TEXT_STYLE_CENTER) == DRAW_TEXT_STYLE_CENTER)
	{
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
	}
	if ((style & DRAW_TEXT_STYLE_RIGHT) == DRAW_TEXT_STYLE_RIGHT)
	{
		format.SetAlignment(Gdiplus::StringAlignmentFar);
	}
	if ((style & DRAW_TEXT_STYLE_END_ELLIPSIS) == DRAW_TEXT_STYLE_END_ELLIPSIS)
	{
		format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
	}

	format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

	Gdiplus::Status status = m_dc->DrawString(txt, txt.GetLength(), font, Gdiplus::RectF((float)rc.X+m_off_x,(float)rc.Y+m_off_y,(float)rc.Width,(float)rc.Height), &format, &brush);

	if (pNewFont)
	{
		delete pNewFont;
	}
}

void UIDC::GetTextSize(CString& txt, SIZE& size, HFONT hFont)
{
	HDC hDC = CreateCompatibleDC(NULL);

	HFONT hNewFont = NULL;

	if (hFont == NULL)
	{
		hNewFont = hFont = GetDefaultFont();
	}

	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);

	::GetTextExtentPoint32(hDC, txt, txt.GetLength(), &size);

	SelectObject(hDC, hOldFont);

	DeleteDC(hDC);
}

Gdiplus::Rect UIDC::GetTextSizeEx(CString& txt, Gdiplus::Font* font)
{
	Gdiplus::Font* pNewFont = NULL;
	if (font == NULL)
	{
		font = pNewFont = GetDefaultFontEx();
	}

	Gdiplus::PointF pt(0,0);
	Gdiplus::RectF rc;
	Gdiplus::Status status = m_dc->MeasureString(txt, txt.GetLength(), font, pt, Gdiplus::StringFormat::GenericTypographic(), &rc);

	if (pNewFont)
	{
		delete pNewFont;
	}
	return Gdiplus::Rect((int)rc.X,(int)rc.Y,(int)rc.Width,(int)rc.Height);
}

void UIDC::GetTextSizeEx(CString& txt, Gdiplus::RectF& in_rc, Gdiplus::RectF& rc, int style, Gdiplus::Font* font)
{
	Gdiplus::Font* pNewFont = NULL;
	if (font == NULL)
	{
		font = pNewFont = GetDefaultFontEx();
	}

	Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());

	if ((style & DRAW_TEXT_STYLE_VCENTER) == DRAW_TEXT_STYLE_VCENTER)
	{
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	}
	if ((style & DRAW_TEXT_STYLE_CENTER) == DRAW_TEXT_STYLE_CENTER)
	{
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
	}
	if ((style & DRAW_TEXT_STYLE_RIGHT) == DRAW_TEXT_STYLE_RIGHT)
	{
		format.SetAlignment(Gdiplus::StringAlignmentFar);
	}
	if ((style & DRAW_TEXT_STYLE_END_ELLIPSIS) == DRAW_TEXT_STYLE_END_ELLIPSIS)
	{
		format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
	}
	if ((style & DRAW_TEXT_STYLE_SINGLE_LINE) == DRAW_TEXT_STYLE_SINGLE_LINE)
	{
		format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
	}

	Gdiplus::Status status = m_dc->MeasureString(txt, txt.GetLength(), font, in_rc, &format, &rc);

	if (pNewFont)
	{
		delete pNewFont;
	}
}

void UIDC::GetTextExtend(std::vector<float>& ww, CString& txt, HFONT hfont)
{
	HDC hdc = m_dc->GetHDC();
	HFONT hOldFont = (HFONT)SelectObject(hdc, hfont);

	ww.clear();

	for (int i = 0; i < txt.GetLength(); ++i)
	{
		SIZE size = {0};
		GetTextExtentPoint32(hdc, (LPCTSTR)txt+i, 1, &size);

		ww.push_back((float)size.cx);
	}

	SelectObject(hdc, hOldFont);
	m_dc->ReleaseHDC(hdc);
}

void UIDC::GetTextExtendEx(std::vector<float>& ww, CString& txt, Gdiplus::Font* font)
{
	Gdiplus::Font* pNewFont = NULL;
	if (font == NULL)
	{
		font = pNewFont = GetDefaultFontEx();
	}

	//Gdiplus::TextRenderingHint hint = m_dc->GetTextRenderingHint();
	//m_dc->SetTextRenderingHint(Gdiplus::TextRenderingHintSystemDefault);
	ww.clear();

	Gdiplus::PointF pt(0,0);
	Gdiplus::RectF rc;

	Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic()->Clone();
	format->SetFormatFlags(format->GetFormatFlags() | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

	for (int i = 0; i < txt.GetLength(); ++i)
	{
		Gdiplus::Status status = m_dc->MeasureString(((LPCTSTR)txt)+i, 1, font, pt, format, &rc);
		ww.push_back(rc.Width);
	}

	if (pNewFont)
	{
		delete pNewFont;
	}

	delete format;

	//m_dc->SetTextRenderingHint(hint);
}

float UIDC::GetFontHeight(Gdiplus::Font* font)
{
	Gdiplus::PointF pt(0,0);
	Gdiplus::RectF rc;
	Gdiplus::Status status = m_dc->MeasureString(L"A", 1, font, pt, Gdiplus::StringFormat::GenericTypographic(), &rc);
	return rc.Height;
}

float UIDC::GetFontHeight(HFONT hfont)
{
	HDC hdc = m_dc->GetHDC();
	HFONT hOldFont = (HFONT)SelectObject(hdc, hfont);

	SIZE size = {0};
	GetTextExtentPoint32(hdc, _T("A"), 1, &size);
	SelectObject(hdc, hOldFont);
	m_dc->ReleaseHDC(hdc);
	return (float)size.cy;
}

float UIDC::GetCharWidth(HFONT hfont, wchar_t c)
{
	wchar_t str[] = {c,0};
	HDC hdc = m_dc->GetHDC();
	HFONT hOldFont = (HFONT)SelectObject(hdc, hfont);

	SIZE size = {0};
	GetTextExtentPoint32(hdc, str, 1, &size);
	SelectObject(hdc, hOldFont);
	m_dc->ReleaseHDC(hdc);
	return (float)size.cx;
}

float UIDC::GetCharWidthEx(Gdiplus::Font* font, wchar_t c)
{
	Gdiplus::PointF pt(0,0);
	Gdiplus::RectF rc;
	wchar_t str[] = {c,0};

	Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic()->Clone();
	format->SetFormatFlags(format->GetFormatFlags() | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

	Gdiplus::Status status = m_dc->MeasureString(str, 1, font, pt, format, &rc);

	delete format;

	return rc.Width;
}

Gdiplus::RectF UIDC::GetCharRectEx(Gdiplus::Font* font, wchar_t c)
{
	Gdiplus::PointF pt(0,0);
	Gdiplus::RectF rc;
	wchar_t str[] = {c,0};

	Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic()->Clone();
	format->SetFormatFlags(format->GetFormatFlags() | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

	Gdiplus::Status status = m_dc->MeasureString(str, 1, font, pt, format, &rc);

	delete format;
	return rc;
}

int UIDC::HittestText(CString& txt, Gdiplus::Rect rc, int x, int y, int style, HFONT hfont)
{
	std::vector<float> ww;
	GetTextExtend(ww, txt, hfont);
	std::vector<_LineInfo> vLines;

	_GetLineInfo(txt, vLines, style, ww, rc.Width, false);

	int i = 0;
	float xx = 0;
	int font_size = (int)GetFontHeight(hfont);
	x -= rc.X;
	y -= rc.Y;

	while (true)
	{
		// hittest line
		if (vLines.size() == 0)
		{
			break;
		}

		_LineInfo* line = NULL;
		int line_y = 0;
		if (y <= 0)
		{
			line = &vLines[0];
			line_y = font_size;
		}
		else
		{
			int line_index = y / font_size;
			if (line_index >= (int)vLines.size())
			{
				line = &vLines[vLines.size()-1];
				line_y = (int)vLines.size() * font_size;
			}
			else
			{
				line = &vLines[line_index];
				line_y = (line_index + 1) * font_size;
			}
		}

		// hittest text in line
		if (x < line->start_x)
		{
			return line->begin_i;
		}
		i = line->begin_i;
		xx = line->start_x;
		if (line->begin_i == line->end_i)
		{
			xx = line->start_x;
		}
		else
		{
			while (i < line->end_i)
			{
				float char_width = ww[i];
				if ((char_width / 2) + xx > x)
				{
					break;
				}
				wchar_t c = txt.GetAt(i);
				if (c == '\n')
				{
					break;
				}
				xx += char_width;
				++i;
			}
		}
		return i;
	}

	return 0;
}

int UIDC::HittestTextEx(CString& txt, Gdiplus::Rect rc, int x, int y, int style, Gdiplus::Font* font)
{
	std::vector<float> ww;
	GetTextExtendEx(ww, txt, font);
	std::vector<_LineInfo> vLines;

	_GetLineInfo(txt, vLines, style, ww, rc.Width, false);

	int i = 0;
	float xx = 0;
	int font_size = (int)GetFontHeight(font);
	x -= rc.X;
	y -= rc.Y;

	while (true)
	{
		// hittest line
		if (vLines.size() == 0)
		{
			break;
		}

		_LineInfo* line = NULL;
		int line_y = 0;
		if (y <= 0)
		{
			line = &vLines[0];
			line_y = font_size;
		}
		else
		{
			int line_index = y / font_size;
			if (line_index >= (int)vLines.size())
			{
				line = &vLines[vLines.size()-1];
				line_y = (int)vLines.size() * font_size;
			}
			else
			{
				line = &vLines[line_index];
				line_y = (line_index + 1) * font_size;
			}
		}

		// hittest text in line
		if (x < line->start_x)
		{
			return line->begin_i;
		}
		i = line->begin_i;
		xx = line->start_x;
		if (line->begin_i == line->end_i)
		{
			xx = line->start_x;
		}
		else
		{
			while (i < line->end_i)
			{
				float char_width = ww[i];
				if ((char_width / 2) + xx > x)
				{
					break;
				}
				wchar_t c = txt.GetAt(i);
				if (c == '\n')
				{
					break;
				}
				xx += char_width;
				++i;
			}
		}
		return i;
	}

	return 0;
}

Gdiplus::Point UIDC::GetTextPosByIndex(CString& txt, Gdiplus::Rect rc, int index, int style, HFONT hfont)
{
	// calc line info
	int i = 0;
	float xx = 0;
	int font_size = (int)GetFontHeight(hfont);

	std::vector<float> ww;
	GetTextExtend(ww, txt, hfont);
	std::vector<_LineInfo> vLines;
	_GetLineInfo(txt, vLines, style, ww, rc.Width, false);

	if (vLines.size() == 0)
	{
		return Gdiplus::Point(rc.X, rc.Y+font_size);
	}
	// find line
	_LineInfo* line = NULL;
	int line_y = 0;
	for (i = 0; i < (int)vLines.size(); ++i)
	{
		_LineInfo* l = &vLines[i];
		if (index >= l->begin_i && index < l->end_i)
		{
			line = l;
			line_y = ((int)i + 1) * font_size;
			break;
		}
	}
	if (line == NULL)
	{
		line = &vLines[vLines.size()-1];
		line_y = (int)(vLines.size() * font_size); 
	}
	// find pos in line
	i = line->begin_i;
	xx = line->start_x;
	while (i < line->end_i)
	{
		if (i == index)
		{
			break;
		}
		xx += ww[i];
		//wchar_t c = txt.charAt(i);
		++i;
	}
	return Gdiplus::Point(rc.X+(int)xx, rc.Y+line_y);
}

Gdiplus::Point UIDC::GetTextPosByIndexEx(CString& txt, Gdiplus::Rect rc, int index, int style, Gdiplus::Font* font)
{
	// calc line info
	int i = 0;
	float xx = 0;
	int font_size = (int)GetFontHeight(font);

	std::vector<float> ww;
	GetTextExtendEx(ww, txt, font);
	std::vector<_LineInfo> vLines;
	_GetLineInfo(txt, vLines, style, ww, rc.Width, false);

	if (vLines.size() == 0)
	{
		return Gdiplus::Point(rc.X, rc.Y+font_size);
	}
	// find line
	_LineInfo* line = NULL;
	int line_y = 0;
	for (i = 0; i < (int)vLines.size(); ++i)
	{
		_LineInfo* l = &vLines[i];
		if (index >= l->begin_i && index < l->end_i)
		{
			line = l;
			line_y = ((int)i + 1) * font_size;
			break;
		}
	}
	if (line == NULL)
	{
		line = &vLines[vLines.size()-1];
		line_y = (int)(vLines.size() * font_size); 
	}
	// find pos in line
	i = line->begin_i;
	xx = line->start_x;
	while (i < line->end_i)
	{
		if (i == index)
		{
			break;
		}
		xx += ww[i];
		//wchar_t c = txt.charAt(i);
		++i;
	}
	return Gdiplus::Point(rc.X+(int)xx, rc.Y+line_y);
}

Gdiplus::Rect UIDC::GetTextBounding(CString& txt, Gdiplus::Rect rc, int style, HFONT hfont)
{
	int font_size = (int)GetFontHeight(hfont);
	if (txt.GetLength() == 0)
	{
		return Gdiplus::Rect(rc.X, rc.Y, 0, font_size);
	}

	std::vector<float> ww;
	GetTextExtend(ww, txt, hfont);
	std::vector<_LineInfo> vLines;
	_GetLineInfo(txt, vLines, style, ww, rc.Width, false);

	Gdiplus::Rect rc_txt;
	int w = 0;
	int h = 0;
	int x = 0;
	for (int i = 0; i < (int)vLines.size(); ++i)
	{
		_LineInfo& line = vLines[i];
		if (w < line.width) w = (int)line.width;
		if (x > line.start_x) x = (int)line.start_x;
		h += font_size;
	}

	int y = 0;
	if (style & DRAW_TEXT_STYLE_VCENTER)
	{
		y = (rc.Height - h) / 2;
	}

	return Gdiplus::Rect(x+rc.X,y+rc.Y,w,h);


	//std::vector<float> ww;
	//GetTextExtend(ww, txt, hfont);

	//boolean is_no_warp = false;

	//float x = 0;
	//float y = 0;
	//float max_width = 0;
	//wchar_t c = 0;
	//int count = txt.GetLength();
	//for (int i = 0; i < count; ++i)
	//{
	//	c = txt.GetAt(i);
	//	if (c == '\n')
	//	{
	//		if (x > max_width) max_width = x;
	//		y += font_size;
	//		x = 0;
	//	}
	//	else
	//	{
	//		if (x+(int)ww[i] > rc.Width)
	//		{
	//			if (is_no_warp)
	//			{
	//				if (x > max_width) max_width = x;

	//				for (; i < count; ++i)
	//				{
	//					c = txt.GetAt(i);
	//					if (c == '\n')
	//					{
	//						y += font_size;
	//						break;
	//					}
	//				}
	//				continue;
	//			}
	//			else
	//			{
	//				y += font_size;
	//				if (x > max_width) max_width = x;
	//				x = 0;
	//				i -= 1;
	//				continue;
	//			}
	//		}

	//		x += ww[i];
	//	}
	//}
	//if (x > max_width) max_width = x;
	//if (c != '\n')
	//{
	//	y += font_size;
	//}

	//if ((style & DRAW_TEXT_STYLE_RIGHT) == DRAW_TEXT_STYLE_RIGHT)
	//{
	//	int off_x = 0;
	//	if (max_width < rc.Width)
	//	{
	//		off_x = rc.Width - (int)max_width;
	//	}
	//	return Gdiplus::Rect(rc.X+off_x, rc.Y, (int)max_width, (int)y);
	//}
	//else if ((style & DRAW_TEXT_STYLE_CENTER) == DRAW_TEXT_STYLE_CENTER)
	//{
	//	int off_x = 0;
	//	if (max_width + font_size < rc.Width)
	//	{
	//		off_x = (rc.Width - (int)max_width) / 2;
	//	}
	//	return Gdiplus::Rect(rc.X+off_x, rc.Y, (int)max_width, (int)y);
	//}
	//else
	//{
	//	return Gdiplus::Rect(rc.X, rc.Y, (int)max_width, (int)y);
	//}
}



Gdiplus::Rect UIDC::GetTextBoundingEx(CString& txt, Gdiplus::Rect rc, int style, Gdiplus::Font* font)
{
	int font_size = (int)GetFontHeight(font);
	if (txt.GetLength() == 0)
	{
		return Gdiplus::Rect(rc.X, rc.Y, 0, font_size);
	}

	std::vector<float> ww;
	GetTextExtendEx(ww, txt, font);

	boolean is_no_warp = false;

	float x = 0;
	float y = 0;
	float max_width = 0;
	wchar_t c = 0;
	int count = txt.GetLength();
	for (int i = 0; i < count; ++i)
	{
		c = txt.GetAt(i);
		if (c == '\n')
		{
			if (x > max_width) max_width = x;
			y += font_size;
			x = 0;
		}
		else
		{
			if (x+(int)ww[i] > rc.Width)
			{
				if (is_no_warp)
				{
					if (x > max_width) max_width = x;

					for (; i < count; ++i)
					{
						c = txt.GetAt(i);
						if (c == '\n')
						{
							y += font_size;
							break;
						}
					}
					continue;
				}
				else
				{
					y += font_size;
					if (x > max_width) max_width = x;
					x = 0;
					i -= 1;
					continue;
				}
			}

			x += ww[i];
		}
	}
	if (x > max_width) max_width = x;
	if (c != '\n')
	{
		y += font_size;
	}

	if ((style & DRAW_TEXT_STYLE_RIGHT) == DRAW_TEXT_STYLE_RIGHT)
	{
		int off_x = 0;
		if (max_width < rc.Width)
		{
			off_x = rc.Width - (int)max_width;
		}
		return Gdiplus::Rect(rc.X+off_x, rc.Y, (int)max_width, (int)y);
	}
	else if ((style & DRAW_TEXT_STYLE_CENTER) == DRAW_TEXT_STYLE_CENTER)
	{
		int off_x = 0;
		if (max_width + font_size < rc.Width)
		{
			off_x = (rc.Width - (int)max_width) / 2;
		}
		return Gdiplus::Rect(rc.X+off_x, rc.Y, (int)max_width, (int)y);
	}
	else
	{
		return Gdiplus::Rect(rc.X, rc.Y, (int)max_width, (int)y);
	}
}

void rgb_to_hsl(float R, float G, float B, float& H, float& S, float& L)
{
	float Max,Min,del_R,del_G,del_B,del_Max;
	R = R / 255.0f;       //Where RGB values = 0 ¡Â 255
	G = G / 255.0f;
	B = B / 255.0f;

	Min = min(R, min(G, B));    //Min. value of RGB
	Max = max(R, max(G, B));    //Max. value of RGB
	del_Max = Max - Min;        //Delta RGB value

	L = (Max + Min) / 2.0f;

	if (del_Max == 0)           //This is a gray, no chroma...
	{
		//H = 2.0/3.0;          //WindowsÏÂSÖµÎª0Ê±£¬HÖµÊ¼ÖÕÎª160£¨2/3*240£©
		H = 0.0f;                  //HSL results = 0 ¡Â 1
		S = 0.0f;
	}
	else                        //Chromatic data...
	{
		if (L < 0.5f) S = del_Max / (Max + Min);
		else         S = del_Max / (2 - Max - Min);

		del_R = (((Max - R) / 6.0f) + (del_Max / 2.0f)) / del_Max;
		del_G = (((Max - G) / 6.0f) + (del_Max / 2.0f)) / del_Max;
		del_B = (((Max - B) / 6.0f) + (del_Max / 2.0f)) / del_Max;

		if      (R == Max) H = del_B - del_G;
		else if (G == Max) H = (1.0f / 3.0f) + del_R - del_B;
		else if (B == Max) H = (2.0f / 3.0f) + del_G - del_R;

		if (H < 0)  H += 1.0f;
		if (H > 1)  H -= 1.0f;
	}
}



static float Hue2RGB(float v1, float v2, float vH)
{
	if (vH < 0.0f) vH += 1;
	if (vH > 1.0f) vH -= 1;
	if (6.0f * vH < 1.0f) return v1 + (v2 - v1) * 6.0f * vH;
	if (2.0f * vH < 1.0f) return v2;
	if (3.0f * vH < 2.0f) return v1 + (v2 - v1) * ((2.0f / 3.0f) - vH) * 6.0f;
	return (v1);
}


void hsl_to_rgb(float H, float S, float L, float& R, float& G, float& B)
{
	float var_1, var_2;
	if (S == 0)                       //HSL values = 0 ¡Â 1
	{
		R = L * 255.0f;                   //RGB results = 0 ¡Â 255
		G = L * 255.0f;
		B = L * 255.0f;
	}
	else
	{
		if (L < 0.5f) var_2 = L * (1 + S);
		else         var_2 = (L + S) - (S * L);

		var_1 = 2.0f * L - var_2;

		R = 255.0f * Hue2RGB(var_1, var_2, H + (1.0f / 3.0f));
		G = 255.0f * Hue2RGB(var_1, var_2, H);
		B = 255.0f * Hue2RGB(var_1, var_2, H - (1.0f / 3.0f));
	}
}

// Ìí¼ÓÑÕÉ«µÄÁÁ¶È£¬brightnessÈ¡Öµ·¶Î§ 1.0f ~ -1.0f
Gdiplus::Color UIDC::AddColorBrightness(Gdiplus::Color color, float brightness)
{
	float R,G,B,A;
	R = (float)color.GetR();
	G = (float)color.GetG();
	B = (float)color.GetB();
	A = (float)color.GetA();

	float H,S,L = 0.0f;
	rgb_to_hsl(R,G,B, H,S,L);

	L += brightness;
	if (L < 0.0f)
	{
		L = 0.0f;
	}
	else if (L > 1.0f)
	{
		L = 1.0f;
	}
	hsl_to_rgb(H,S,L,R,G,B);

	Gdiplus::Color color2((unsigned char)A,(unsigned char)R,(unsigned char)G,(unsigned char)B);
	return color2;
}


bool UIDC::SaveImage(CString& filename, int quality)
{
	if (m_image)
	{
		return ::SaveImage(filename, m_image, quality);
	}
	return false;
}

