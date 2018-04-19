#include "StdAfx.h"
#include "UIRichEdit.h"
#include "giflib/gif_lib.h"
#include <math.h>
#include "xml/tinyxml.h"
#include "UI/UIFrame.h"
#include <list>
#include <sys/stat.h>
#include <map>

#pragma warning( disable : 4996 )
#define ID_TIMER_ANIMATE 1020

std::wstring Utf82Unicode(const std::string& utf8string);
std::string Unicode2Utf8(const std::wstring& widestring);

struct rgb_color { unsigned char r,g,b; };

class PaletteImage
{
public:
	PaletteImage(int width, int height)
	{
		m_width = width;
		m_height = height;
		m_bits = (unsigned char*)malloc(width*height);
		m_delay = 0;
		m_trans_color = -1;
	}

	~PaletteImage()
	{
		if (m_bits)
		{
			free(m_bits);
		}
	}

	rgb_color	m_pal[256];
	unsigned char*	m_bits;
	int	m_width;
	int m_height;
	int m_trans_color;
	int m_delay;
};

struct FileData
{
	const unsigned char*	data;
	int size;
	int pos;
};

static int readFunc(GifFileType* GifFile, GifByteType* buf, int count)
{
	FileData* ptr = (FileData*)GifFile->UserData;
	int len = count;
	if (ptr->pos + len > ptr->size)
	{
		len = ptr->size - ptr->pos;
	}
	if (len > 0)
	{
		memcpy(buf, ptr->data + ptr->pos, len);
		ptr->pos += len;
	}
	return len;
}


static int InterlacedOffset[] = { 0, 4, 2, 1 }; /* The way Interlaced image should. */
static int InterlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */


#define GIF_CONTROL_EXT_CODE 0xF9
#define GIF_CONTROL_EXT_SIZE 4


bool LoadGif(UIRichEdit::Item* self, const unsigned char* data_buffer, unsigned long size)
{
	int	i, j, Row, Col, Width, Height, ExtCode, Count;
	GifRecordType RecordType;
	GifByteType *Extension;
	GifFileType *GifFile;
	ColorMapObject *ColorMap;

	int trans_color = -1;
	int delay = 0;

	FileData fData;
	fData.data = data_buffer;
	fData.size = size;
	fData.pos = 0;
	int error = 0;
	if ((GifFile = DGifOpen(&fData, readFunc, &error)) == NULL) {
		return false;
	}

	std::vector<PaletteImage*> vImage;
	PaletteImage* image = NULL;

	/* Scan the content of the GIF file and load the image(s) in: */
	do {
		if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) {
			goto failed;
		}
		switch (RecordType) {
case IMAGE_DESC_RECORD_TYPE:
	if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
		goto failed;
	}
	Row = GifFile->Image.Top; /* Image Position relative to Screen. */
	Col = GifFile->Image.Left;
	Width = GifFile->Image.Width;
	Height = GifFile->Image.Height;

	// Update Color map
	ColorMap = (GifFile->Image.ColorMap
		? GifFile->Image.ColorMap
		: GifFile->SColorMap);

	image = new PaletteImage(GifFile->SWidth, GifFile->SHeight);
	image->m_trans_color = trans_color;
	image->m_delay = delay;
	vImage.push_back(image);
	i = ColorMap->ColorCount;
	if (i > 256)
	{
		goto failed;
	}
	while (--i >= 0)
	{
		GifColorType* color = &ColorMap->Colors[i];
		rgb_color& c = image->m_pal[i];
		c.r = color->Red;
		c.g = color->Green;
		c.b = color->Blue;
	}

	// set background
	memset(image->m_bits, GifFile->SBackGroundColor, image->m_width * image->m_height);

	if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth ||
		GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight) {
			goto failed;
	}
	if (GifFile->Image.Interlace) {
		/* Need to perform 4 passes on the images: */
		for (Count = i = 0; i < 4; i++)
			for (j = Row + InterlacedOffset[i]; j < Row + Height; j += InterlacedJumps[i])
			{
				if (DGifGetLine(GifFile, image->m_bits + image->m_width * j + Col, //&ScreenBuff[j][Col],
					Width) == GIF_ERROR)
				{
					goto failed;
				}
			}
	}
	else {
		for (i = 0; i < Height; i++)
		{
			if (DGifGetLine(GifFile, image->m_bits + image->m_width * Row + Col,//&ScreenBuff[Row++][Col],
				Width) == GIF_ERROR) {
					goto failed;
			}
			++Row;
		}
	}
	break;
case EXTENSION_RECORD_TYPE:
	/* Skip any extension blocks in file: */
	if (DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR) {
		goto failed;
	}
	else
	{
		if (ExtCode == GIF_CONTROL_EXT_CODE && Extension)
		{
			if (Extension[0] == GIF_CONTROL_EXT_SIZE)
			{
				delay = (Extension[3] << 8 | Extension[2]) * 10;
			}
			if( (Extension[1] & 1) == 1 )
			{
				trans_color = Extension[4];
			}
			else
			{
				trans_color = -1;
			}
		}
	}

	while (Extension != NULL) {
		if (DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR) {
			goto failed;
		}
		else
		{
			if (ExtCode == GIF_CONTROL_EXT_CODE && Extension)
			{
				if (Extension[0] == GIF_CONTROL_EXT_SIZE)
				{
					delay = (Extension[3] << 8 | Extension[2]) * 10;
				}
				if( (Extension[1] & 1) == 1 )
				{
					trans_color = Extension[4];
				}
				else
				{
					trans_color = -1;
				}
			}
		}
	}
	break;
case TERMINATE_RECORD_TYPE:
	break;
default:		    /* Should be traps by DGifGetRecordType. */
	break;
		}
	}
	while (RecordType != TERMINATE_RECORD_TYPE);

	/* Close file when done */
	error = 0;
	if (DGifCloseFile(GifFile, &error) == GIF_ERROR) {
		goto failed;
	}

	if (vImage.size() == 0)
	{
		return false;
	}

	for (int i = 0; i < (int)self->animate_images.size(); ++i)
	{
		delete self->animate_images[i];
	}
	self->animate_images.clear();

	self->width = vImage[0]->m_width;
	self->height = vImage[0]->m_height;

	unsigned char* buffer = (unsigned char*)malloc(self->width * self->height * 4);

	for (size_t ii = 0; ii < vImage.size(); ++ii)
	{
		PaletteImage* image = vImage[ii];

		Gdiplus::Bitmap* image2 = new Gdiplus::Bitmap(self->width, self->height, PixelFormat32bppARGB);

		unsigned char* dst_bits = buffer;
		unsigned long dst_pitch = self->width * 4;

		for (int y = 0; y < image->m_height; ++y)
		{
			unsigned char* p1 = dst_bits + y * dst_pitch;
			unsigned char* p2 = image->m_bits + y * image->m_width;
			for (int x = 0; x < image->m_width; ++x, p1 += 4, ++p2)
			{
				if (*p2 == image->m_trans_color)
				{
					p1[0] = 255;
					p1[1] = 255;
					p1[2] = 255;
					p1[3] = 0;
				}
				else
				{
					rgb_color& c = image->m_pal[*p2];
					p1[0] = c.b;
					p1[1] = c.g;
					p1[2] = c.r;
					p1[3] = 255;
				}
			}
		}

		{
			Gdiplus::BitmapData data2;
			Gdiplus::Rect rc(0,0,self->width,self->height);
			data2.Scan0 = (void*)buffer;
			data2.Stride = 4 * self->width;
			if (Gdiplus::Ok == image2->LockBits(&rc, 
				Gdiplus::ImageLockModeWrite | Gdiplus::ImageLockModeUserInputBuf, 
				PixelFormat32bppARGB, &data2))
			{
				image2->UnlockBits(&data2);
			}
		}

		if (self->animate_delay < image->m_delay) self->animate_delay = image->m_delay;
		self->animate_images.push_back(image2);
		//self->m_surfaces->push_back(surface);
		delete image;
	}

	free(buffer);

	if (self->animate_delay == 0)
	{
		self->animate_delay = 60;
	}

	return true;

failed:

	for (size_t ii = 0; ii < vImage.size(); ++ii)
	{
		delete vImage[ii];
	}

	return false;
}

UIRichEdit::UIRichEdit(AutoRef<UIView> parent, int id, Gdiplus::Rect rc,
	UINT uiCopy, UINT uiPaste, UINT uiCut)
: UIView(parent, id, rc)
, m_uiCopy(uiCopy)
, m_uiPaste(uiPaste)
, m_uiCut(uiCut)
{
	m_max_text_length = -1;
	m_font_size = 14;
	m_color = Gdiplus::Color(255,0,0,0);
	m_is_down = false;

	m_link_color = Gdiplus::Color(255,0,0,238);

	m_cursor_index = 0;
	m_select_index[0] = -1;
	m_select_index[1] = -1;

	m_is_draw_bg = false;
	m_is_editable = false;

	m_is_draw_frame = false;

	m_bg_image = NULL;

	m_vscroll = new UIVScroll(this, 0, 5);

	//Gdiplus::Font* font = GetDefaultFontEx(12, false);
	//Gdiplus::FontFamily family;
	//font->GetFamily(&family);
	//wchar_t name[256] = {0};
	//family.GetFamilyName(name, LANG_NEUTRAL);
	//m_font_family = name;

	m_font_family = "Microsoft YaHei";
}

UIRichEdit::~UIRichEdit()
{
	Destroy();
}

void UIRichEdit::Destroy()
{
	UIView::Destroy();
	Clear();

	m_vscroll = NULL;

	if (m_bg_image)
	{
		delete m_bg_image;
		m_bg_image = NULL;
	}
}

void UIRichEdit::Clear()
{
	for (int i = 0; i < (int)m_items.size(); ++i)
	{
		delete m_items[i];
	}
	m_items.clear();
	m_lines.clear();

	Layout();
}

void UIRichEdit::AddImage(CString filepath)
{
	Item* item = LoadImageEx(filepath);
	if (item)
	{
		DeleteSelect();
		if (m_cursor_index >= 0 && m_cursor_index < (int)m_items.size())
		{
			std::vector<Item*>::iterator iter = m_items.begin() + m_cursor_index;
			m_items.insert(iter, item);
			m_cursor_index++;
		}
		else
		{
			m_items.push_back(item);
			m_cursor_index = (int)(m_items.size() - 1);
		}
		Layout();
		Redraw();
	}
}

UIRichEdit::Item* UIRichEdit::LoadImageEx(CString filepath)
{
	CString ext = GetFileExt(filepath);
	ext.MakeLower();
	if (ext == _T("gif"))
	{
		Item* item = new Item();
		item->type = Type_AnimateImage;
		item->txt = filepath;

		unsigned char* buffer = NULL;
		unsigned int size = 0;
		if (!ReadFileData(filepath, buffer, size))
		{
			delete item;
			return NULL;
		}
		if (!LoadGif(item, buffer, size))
		{
			free(buffer);
			delete item;
			return NULL;
		}

		free(buffer);

		return item;
	}
	else if (ext == _T("png") || ext == _T("jpg"))
	{
		Item* item = new Item();
		item->type = Type_Image;
		item->image = new Gdiplus::Bitmap(filepath);
		item->txt = filepath;

		item->width = item->image->GetWidth();
		item->height = item->image->GetHeight();

		return item;
	}
	else if (ext == _T("ico"))
	{
		Item* item = new Item();
		item->type = Type_Image;
		item->image = new Gdiplus::Bitmap(filepath);
		item->txt = filepath;

		item->width = item->image->GetWidth();
		item->height = item->image->GetHeight();

		return item;
	}
	return NULL;
}

void UIRichEdit::AddText(CString txt)
{
	DeleteSelect();

	if (m_cursor_index >= 1 && m_cursor_index <= (int)m_items.size())
	{
		Item* prev_item = m_items[m_cursor_index - 1];
		if (prev_item->type == Type_Text)
		{
			for (int i = 0; i < txt.GetLength(); ++i)
			{
				if((int)m_items.size() >= m_max_text_length && m_max_text_length != -1)
					return;

				Item* item = new Item();
				item->type = Type_Text;
				item->color = prev_item->color;
				item->font_size = prev_item->font_size;
				item->font_style = prev_item->font_style;
				item->font_family = prev_item->font_family;
				item->align = prev_item->align;
				item->txt = txt.Mid(i,1);
				if (item->txt == _T("\n"))
				{
					item->color = m_color;
					item->font_size = m_font_size;
				}

				std::vector<Item*>::iterator iter = m_items.begin() + m_cursor_index;
				m_items.insert(iter, item);
				m_cursor_index++;
			}
		}
		else if (prev_item->type == Type_Link)
		{
			for (int i = 0; i < txt.GetLength(); ++i)
			{
				if((int)m_items.size() >= m_max_text_length && m_max_text_length != -1)
					return;

				Item* item = new Item();
				item->type = Type_Link;
				item->color = prev_item->color;
				item->link_color = prev_item->link_color;
				item->link = prev_item->link;
				item->font_size = prev_item->font_size;
				item->font_style = prev_item->font_style;
				item->font_family = prev_item->font_family;
				item->align = prev_item->align;
				item->txt = txt.Mid(i,1);
				if (item->txt == _T("\n"))
				{
					item->type = Type_Text;
					item->color = m_color;
					item->font_size = m_font_size;
				}

				std::vector<Item*>::iterator iter = m_items.begin() + m_cursor_index;
				m_items.insert(iter, item);
				m_cursor_index++;
			}
		}
		else
		{
			for (int i = 0; i < txt.GetLength(); ++i)
			{
				if((int)m_items.size() >= m_max_text_length && m_max_text_length != -1)
					return;

				Item* item = new Item();
				item->type = Type_Text;
				item->color = m_color;
				item->font_size = m_font_size;
				item->font_family = m_font_family;
				item->txt = txt.Mid(i,1);

				std::vector<Item*>::iterator iter = m_items.begin() + m_cursor_index;
				m_items.insert(iter, item);
				m_cursor_index++;
			}
		}
	}
	else if (m_cursor_index == 0)
	{
		Item* next_item = NULL;
		if (m_items.size() > 0)
		{
			next_item = m_items[m_cursor_index];
			if (!(next_item->type == Type_Link || next_item->type == Type_Text))
			{
				next_item = NULL;
			}
		}
		
		for (int i = 0; i < txt.GetLength(); ++i)
		{
			if((int)m_items.size() >= m_max_text_length && m_max_text_length != -1)
				return;

			Item* item = new Item();
			item->type = Type_Text;
			item->color = m_color;
			item->font_size = m_font_size;
			item->font_family = m_font_family;
			if (next_item)
			{
				item->color = next_item->color;
				item->link_color = next_item->font_size;
				item->font_size = next_item->font_size;
				item->font_style = next_item->font_style;
				item->font_family = next_item->font_family;
				item->align = next_item->align;
			}
			item->txt = txt.Mid(i,1);

			std::vector<Item*>::iterator iter = m_items.begin() + m_cursor_index;
			m_items.insert(iter, item);
			m_cursor_index++;
		}
	}
	Layout();
	Redraw();
}

void UIRichEdit::Layout()
{
	m_lines.clear();

	if (m_vscroll == NULL)
	{
		return;
	}

	Gdiplus::Rect rc = Rect();
	Gdiplus::Rect rc2 = GetDrawRect();
	int max_width = rc2.Width - 8;
	int x = 0;
	int y = 0;
	int line_begin = 0;
	int line_height = 0;

	Gdiplus::Font* font = NULL;

	bool is_have_animate = false;

	UIDC dc(1,1);
	FontCache font_cache;

	AlignType align = Align_Left; // 当前对齐

	if (m_items.size() > 0)
	{
		align = m_items[0]->align;
	}

	int i = 0;
	for (; i < (int)m_items.size(); )
	{
		Item* item = m_items[i];

		if (item->align != align && line_begin < i && item->txt != _T("\n"))
		{
			// 对齐方式不一样，换行

			LineInfo line;
			line.begin_index = line_begin;
			line.end_index = i;
			line.height = line_height;

			m_lines.push_back(line);

			y += line_height;
			line_begin = i;
			x = 0;
			line_height = 0;

			align = item->align;

			continue;
		}

		if (item->type == Type_Text || item->type == Type_Link)
		{
			Gdiplus::Font* font = font_cache.GetFont(item->font_family, item->font_size, item->font_style);

			Gdiplus::RectF rc_txt = dc.GetCharRectEx(font, *(LPCTSTR)item->txt);

			item->width = (int)(rc_txt.Width);
			item->height = (int)(rc_txt.Height);

			if (item->txt == _T("\n"))
			{
				if (line_height < item->height)
				{
					line_height = item->height;
				}
				++i;
				LineInfo line;
				line.begin_index = line_begin;
				line.end_index = i;
				line.height = line_height;

				m_lines.push_back(line);

				y += line_height;
				line_begin = i;
				x = 0;
				line_height = 0;
				align = Align_Left;

				continue;
			}
			else if ((x + item->width > max_width && x > 0))
			{
				if (line_height < item->height)
				{
					line_height = item->height;
				}
				LineInfo line;
				line.begin_index = line_begin;
				line.end_index = i;
				line.height = line_height;

				m_lines.push_back(line);

				y += line_height;
				line_begin = i;
				x = 0;
				line_height = 0;

				continue;
			}

			if (item->height > line_height)
			{
				line_height = item->height;
			}

			align = item->align;
			x += item->width;
			++i;
		}
		else if (item->type == Type_Image || item->type == Type_AnimateImage)
		{
			if (item->type == Type_AnimateImage)
			{
				is_have_animate = true;
			}
			if (x + item->width > max_width && x > 0)
			{
				if (line_height < item->height)
				{
					line_height = item->height;
				}
				LineInfo line;
				line.begin_index = line_begin;
				line.end_index = i;
				line.height = line_height;

				m_lines.push_back(line);

				y += line_height;
				line_begin = i;
				x = 0;
				line_height = 0;

				continue;
			}

			if (item->height > line_height)
			{
				line_height = item->height;
			}

			x += item->width;
			++i;
		}
	}

	if (line_begin < i)
	{
		LineInfo line;
		line.begin_index = line_begin;
		line.end_index = i;
		line.height = line_height;

		m_lines.push_back(line);

		y += line_height;
	}

	int page = rc2.Height;
	int max = y;
	m_vscroll->SetScroll(max, page);

	if (is_have_animate)
	{
		SetTimer(ID_TIMER_ANIMATE, 10);
	}
	else
	{
		KillTimer(ID_TIMER_ANIMATE);
	}
}

void UIRichEdit::DeleteSelect()
{
	if (m_select_index[0] >= 0 && m_select_index[1] > m_select_index[0])
	{
		for (int i = m_select_index[0]; i < m_select_index[1]; ++i)
		{
			if (i < (int)m_items.size())
			{
				Item* item = m_items[i];
				delete item;
			}
		}

		m_items.erase(m_items.begin() + m_select_index[0], m_items.begin() + m_select_index[1]);

		m_cursor_index = m_select_index[0];

		m_select_index[0] = -1;
		m_select_index[1] = -1;
		Redraw();
	}
}

bool UIRichEdit::OnTimer(DWORD timer_id)
{
	bool is_need_redraw = false;
	DWORD now_time = GetTickCount();
	for (int i = 0 ; i < (int)m_items.size(); ++i)
	{
		Item* item = m_items[i];
		if (item->type == Type_AnimateImage)
		{
			if ((int)(now_time - item->time_ticket) >= item->animate_delay)
			{
				item->animate_index++;
				if (item->animate_index >= (int)item->animate_images.size())
				{
					item->animate_index = 0;
				}
				item->time_ticket = (now_time / item->animate_delay) * item->animate_delay;
				is_need_redraw = true;
			}
		}
	}
	if (is_need_redraw)
	{
		Redraw();
	}
	return true;
}

int UIRichEdit::Hittest(int _x, int _y)
{
	Gdiplus::Rect rc = GetDrawRect();
	int xx = _x - rc.X;
	int yy = _y - rc.Y;

	int x = 0;
	int y = -m_vscroll->GetScrollPos();
	int nEndIndex = -1;
	for (int i = 0; i < (int)m_lines.size(); ++i)
	{
		LineInfo line = m_lines[i];

		if (yy >= y && yy < y + line.height)
		{
			// 一行的对齐方式是一样的，只用看第一个就行了
			{
				x = 0;
				Item* item = m_items[line.begin_index];
				if (item->align == Align_Center)
				{
					int w = 0;
					for (int j = line.begin_index; j < line.end_index; ++j)
					{
						Item* item2 = m_items[j];
						w += item2->width;
					}
					x = (rc.Width - w) / 2;
				}
				if (item->align == Align_Right)
				{
					int w = 0;
					for (int j = line.begin_index; j < line.end_index; ++j)
					{
						Item* item2 = m_items[j];
						w += item2->width;
					}
					x = rc.Width - w - 1;
				}
			}

			for (int i = line.begin_index; i < line.end_index; ++i)
			{
				Item* item = m_items[i];

				if (item->type == Type_Text || item->type == Type_Link)
				{
					if (item->txt == _T("\n"))
					{
						// 换行符不选取在范围内
						return i;
					}
					if (xx < x + 3)
					{
						return i;
					}
					else if (xx < x + item->width && i + 1 < (int)m_items.size())
					{
						return i + 1;
					}
				}
				else if (item->type == Type_Image || item->type == Type_AnimateImage)
				{
					if (xx < x + 5)
					{
						return i;
					}
					else if (xx < x + item->width && i + 1 < (int)m_items.size())
					{
						return i + 1;
					}
				}
				x += item->width;
			}

			if (m_items[line.end_index - 1]->txt == _T("\n"))
			{
				return line.end_index - 1;
			}
			return line.end_index;
		}
		else if(yy < y)
		{
			return 0;
		}

		x = 0;
		y += line.height;
		nEndIndex = line.end_index;
	}
	return nEndIndex -= 1;
}

CPoint UIRichEdit::GetCursorPos()
{
	Gdiplus::Rect rc = GetDrawRect();

	int y = -m_vscroll->GetScrollPos() + rc.Y;
	for (int i = 0; i < (int)m_lines.size(); ++i)
	{
		LineInfo& line = m_lines[i];
		if (m_cursor_index >= line.begin_index && m_cursor_index < line.end_index)
		{
			int x = rc.X;
			for (int j = line.begin_index; j < line.end_index; ++j)
			{
				Item* item = m_items[j];
				if (j == m_cursor_index)
				{
					return CPoint(x,y+line.height);
				}
				x += item->width;
			}
		}
		y += line.height;
	}
	return CPoint(0,0);
}

int UIRichEdit::GetCursorIndex()
{
	return m_cursor_index;
}

void UIRichEdit::OnPaint(UIDC* dc)
{
	Gdiplus::Rect rc = Rect();
	if (m_is_draw_bg)
	{
		dc->FillRect(m_bg_color, rc);
	}

	if (m_is_draw_frame)
	{
		dc->DrawRect(1, m_frame_color, rc);
	}

	if (m_bg_image)
	{
		dc->DrawImage(m_bg_image, rc, 255);
	}

	dc->Save();

	dc->EnableFontAntiAlias(true);

	rc = GetDrawRect();
	{
		dc->SetClip(rc);
		dc->m_off_x += rc.X;
		dc->m_off_y += rc.Y;
		rc.X = 0;
		rc.Y = 0;
	}

	FontCache font_cache;

	int x = 0;
	int y = -m_vscroll->GetScrollPos();
	for (int i = 0; i < (int)m_lines.size(); ++i)
	{
		LineInfo line = m_lines[i];

		if (y + line.height < 0)
		{
			x = 0;
			y += line.height;
			continue;
		}

		if (y > rc.Height)
		{
			break;
		}

		// 一行的对齐方式是一样的，只用看第一个就行了
		{
			x = 0;
			Item* item = m_items[line.begin_index];
			if (item->align == Align_Center)
			{
				int w = 0;
				for (int j = line.begin_index; j < line.end_index; ++j)
				{
					Item* item2 = m_items[j];
					w += item2->width;
				}
				x = (rc.Width - w) / 2;
			}
			if (item->align == Align_Right)
			{
				int w = 0;
				for (int j = line.begin_index; j < line.end_index; ++j)
				{
					Item* item2 = m_items[j];
					w += item2->width;
				}
				x = rc.Width - w - 1;
			}
		}

		for (int i = line.begin_index; i < line.end_index; ++i)
		{
			Item* item = m_items[i];

			if (item->type == Type_Text)
			{
				Gdiplus::Font* font = font_cache.GetFont(item->font_family, item->font_size, item->font_style);

				if (m_is_editable && i >= m_select_index[0] && i < m_select_index[1])
				{
					dc->FillRect(Gdiplus::Color(255,51,153,255), Gdiplus::Rect(x,y+line.height-item->height,item->width,item->height));
					dc->DrawTextEx(item->txt, Gdiplus::Color(255,255,255,255), Gdiplus::Rect(x,y+line.height-item->height,item->width+1,item->height+1), 0, font);
				}
				else
				{
					dc->DrawTextEx(item->txt, item->color, Gdiplus::Rect(x,y+line.height-item->height,item->width+1,item->height+1), 0, font);
				}
			}
			else if (item->type == Type_Link)
			{
				Gdiplus::Font* font = font_cache.GetFont(item->font_family, item->font_size, item->font_style | Gdiplus::FontStyleUnderline);

				if (m_is_editable && i >= m_select_index[0] && i < m_select_index[1])
				{
					dc->FillRect(Gdiplus::Color(255,51,153,255), Gdiplus::Rect(x,y+line.height-item->height,item->width,item->height));
					dc->DrawTextEx(item->txt, Gdiplus::Color(255,255,255,255), Gdiplus::Rect(x,y+line.height-item->height,item->width+1,item->height+1), 0, font);
				}
				else
				{
					dc->DrawTextEx(item->txt, item->link_color, Gdiplus::Rect(x,y+line.height-item->height,item->width+1,item->height+1), 0, font);
				}
			}
			else if (item->type == Type_Image)
			{
				int xx = x;
				int yy = y+line.height-item->height;
				dc->DrawImage(item->image, xx, yy, 255);

				if (m_is_editable && i >= m_select_index[0] && i < m_select_index[1])
				{
					dc->FillRect(Gdiplus::Color(96,51,153,255), Gdiplus::Rect(xx,yy,item->width,item->height));
				}
			}
			else if (item->type == Type_AnimateImage)
			{
				Gdiplus::Bitmap* image = NULL;

				if (item->animate_index >= 0 && item->animate_index < (int)item->animate_images.size())
				{
					image = item->animate_images[item->animate_index];
				}
				else if (item->animate_images.size() > 0)
				{
					image = item->animate_images[0];
				}

				int xx = x;
				int yy = y+line.height-item->height;
				if (image)
				{
					dc->DrawImage(image, xx, yy, 255);
				}
				if (m_is_editable && i >= m_select_index[0] && i < m_select_index[1])
				{
					dc->FillRect(Gdiplus::Color(96,51,153,255), Gdiplus::Rect(xx,yy,item->width,item->height));
				}
			}

			if (i == m_cursor_index && m_is_editable)
			{
				if (item->type == Type_Text || item->type == Type_Link)
				{
					dc->DrawLine(1, Gdiplus::Color(255,0,0,0), x, y + line.height - item->height, x, y + line.height);
				}
				else if (item->type == Type_AnimateImage || item->type == Type_Image)
				{
					int h = m_font_size;
					if (h > line.height) h = line.height;
					dc->DrawLine(1, Gdiplus::Color(255,0,0,0), x, y + line.height - h, x, y + line.height);
				}
			}

			x += item->width;
		}

		x = 0;
		y += line.height;
	}

	dc->Restore();
}

void UIRichEdit::OnLButtonDown(int x, int y)
{
	m_is_down = true;
	int index = Hittest(x, y);
	if (index >= 0)
	{
		if (m_is_editable)
		{
			m_cursor_index = index;
			m_select_index[0] = -1;
			m_select_index[1] = -1;
		}
		else if (index < (int)m_items.size())
		{
			Item* item = m_items[index];
			if (item->type == Type_Link)
			{
				if (m_link_delegate)
				{
					m_link_delegate(item->link);
				}
			}
			else if (item->type == Type_Image || item->type == Type_AnimateImage)
			{
				if (item->link.GetLength() > 0)
				{
					if (m_link_delegate)
					{
						m_link_delegate(item->link);
					}
				}
			}
		}
	}
	Redraw();
}

void UIRichEdit::OnLButtonUp(int x, int y)
{
	Gdiplus::Rect rc = Rect(); 

	m_is_down = false;
}

void UIRichEdit::OnMouseMove(int x, int y)
{
	Gdiplus::Rect rc = Rect(); 

	if (m_is_down)
	{
		int index = Hittest(x, y);
		if (index >= 0)
		{
			if (m_is_editable)
			{
				if (m_cursor_index > index)
				{
					m_select_index[0] = index;
					m_select_index[1] = m_cursor_index;
				}
				else if (m_cursor_index < index)
				{
					m_select_index[0] = m_cursor_index;
					m_select_index[1] = index;
				}
				else
				{
					m_cursor_index = index;
					m_select_index[0] = -1;
					m_select_index[1] = -1;
				}
				Redraw();
				return;
			}
		}
	}
}

void UIRichEdit::OnRButtonDown(int x, int y)
{
	AutoRef<UIMenu> menu = new UIMenu();

	if (m_is_editable)
	{
		CString strTmp;
		strTmp.LoadString(m_uiCopy);
		menu->AddMenu(strTmp, IDC_COPY);
		strTmp.LoadString(m_uiPaste);
		menu->AddMenu(strTmp, IDC_PASTE);
		strTmp.LoadString(m_uiCut);
		menu->AddMenu(strTmp, IDC_CUT);
	}

	Gdiplus::Rect rc = GlobalRect();
	int index = menu->ShowMenu(rc.X+x, rc.Y+y);
	if (index >= 0)
	{
		if (index == IDC_COPY)
		{
			OnCopy();
		}
		else if (index == IDC_PASTE)
		{
			OnPaste();
		}
		else if (index == IDC_CUT)
		{
			OnCut();
		}
	}
	menu->Destroy();
	menu.Delete();
	menu = NULL;
	Redraw();
	return;
}

int UIRichEdit::_GetNextCharacterIndex(int index)
{
	if (index < (int)m_items.size())
	{
		if (index == (int)m_items.size() - 1 && m_items.size() > 0)
		{
			if (m_items[m_items.size() - 1]->txt == _T("\n"))
			{
				return index;
			}
		}
		index += 1;
	}
	return index;
}

int UIRichEdit::_GetPrevCharacterIndex(int index)
{
	if (index >= 1)
	{
		index -= 1;
	}
	return index;
}

void UIRichEdit::OnKeyDown(unsigned int key)
{
	if (key == VK_BACK)
	{
		if (!m_is_editable) return;
		if (m_select_index[0] >= 0)
		{
			DeleteSelect();
			Layout();
			Redraw();

			return;
		}
		else
		{
			if (m_cursor_index > 0)
			{
				Item* item = *(m_items.begin() + m_cursor_index - 1);
				m_items.erase(m_items.begin() + m_cursor_index - 1);
				delete item;
				m_cursor_index -= 1;
				Layout();
				Redraw();

				return;
			}
		}
	}
	else if (key == VK_DELETE)
	{
		if (!m_is_editable) return;
		if (m_select_index[0] >= 0)
		{
			DeleteSelect();
			Layout();
			Redraw();
			return;
		}
		else
		{
			if (m_cursor_index >= 0 && m_cursor_index < (int)m_items.size()-1)
			{
				Item* item = m_items[m_cursor_index];
				delete item;
				m_items.erase(m_items.begin() + m_cursor_index);
				Layout();
				Redraw();
				return;
			}
		}
	}
	else if (key == VK_RETURN)
	{
		OnChar(CString(_T("\n")));
		return;
	}
	else if (key == 'V') // ctrl + v
	{
		if ((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000)
		{
			OnPaste();
			return;
		}
	}

	else if (key == 'C') // ctrl + c
	{
		if ((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000)
		{
			OnCopy();
			return;
		}
	}
	else if (key == 'A') // ctrl + a
	{
		if ((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000)
		{
			m_select_index[0] = 0;
			m_select_index[1] = (int)(m_items.size() > 0 ? m_items.size() - 1:0);
			Redraw();
			return;
		}
	}
	else if (key == 'X') // ctrl + x
	{
		if ((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000)
		{
			OnCut();
			return;
		}
	}
	
	else if (key == VK_ESCAPE)
	{
		if (m_select_index[0] >= 0)
		{
			m_select_index[0] = -1;
			m_select_index[1] = -1;
			Redraw();
			return;
		}
	}

	else if (key == VK_LEFT)
	{
		if ((GetKeyState(VK_SHIFT) & 0x8000) == 0x8000)
		{
			if (m_select_index[0] >= 0)
			{
				int index = _GetPrevCharacterIndex(m_cursor_index);
				if (m_cursor_index == m_select_index[0])
				{
					m_select_index[0] = index;
				}
				else if (m_cursor_index == m_select_index[1])
				{
					m_select_index[1] = index;
				}
				m_cursor_index = index;
			}
			else if (m_cursor_index <= (int)m_items.size())
			{
				int index = _GetPrevCharacterIndex(m_cursor_index);
				m_select_index[0] = index;
				m_select_index[1] = m_cursor_index;
				m_cursor_index = index;
			}
			if (m_select_index[0] == m_select_index[1])
			{
				m_select_index[0] = -1;
				m_select_index[1] = -1;
			}
		}
		else
		{
			if (m_cursor_index > 0)
			{
				m_cursor_index -= 1;
			}
			m_select_index[0] = -1;
			m_select_index[1] = -1;
		}

		Redraw();
		return;
	}

	else if (key == VK_RIGHT)
	{
		if ((GetKeyState(VK_SHIFT) & 0x8000) == 0x8000)
		{
			if (m_select_index[1] > 0)
			{
				int index = _GetNextCharacterIndex(m_cursor_index);
				if (m_cursor_index == m_select_index[0])
				{
					m_select_index[0] = index;
				}
				else if (m_cursor_index == m_select_index[1])
				{
					m_select_index[1] = index;
				}
				m_cursor_index = index;
			}
			else if (m_cursor_index < (int)m_items.size() - 1)
			{
				int index = _GetNextCharacterIndex(m_cursor_index);
				m_select_index[0] = m_cursor_index;
				m_select_index[1] = index;
				m_cursor_index = index;
			}
			if (m_select_index[0] == m_select_index[1])
			{
				m_select_index[0] = -1;
				m_select_index[1] = -1;
			}
		}
		else
		{
			m_cursor_index = _GetNextCharacterIndex(m_cursor_index);
			m_select_index[0] = -1;
			m_select_index[1] = -1;
		}

		Redraw();
	}
	else if (key == VK_UP)
	{
		int line_index = 0;
		int line_pos = 0;
		for (int i = 0; i < (int)m_lines.size(); ++i)
		{
			LineInfo& line = m_lines[i];
			if (m_cursor_index >= line.begin_index && m_cursor_index < line.end_index)
			{
				line_index = i;
				line_pos = m_cursor_index - line.begin_index;
			}
		}

		if (line_index > 0)
		{
			LineInfo& line = m_lines[line_index - 1];
			int new_index = line.begin_index + line_pos;
			if (new_index >= line.end_index)
			{
				new_index = line.end_index-1;
			}

			if (new_index > (int)m_items.size())
			{
				new_index = (int)m_items.size();
			}

			if (new_index < 0) new_index = 0;
			/*if(new_index > 0)
			{
				Item* last_item = m_items[new_index - 1];
				if (last_item->txt == _T("\n"))
				{
					new_index -= 1;
					if (new_index < 0) new_index = 0;
				}
			}*/
			m_cursor_index = new_index;
			Redraw();
		}
	}
	else if (key == VK_DOWN)
	{
		int line_index = 0;
		int line_pos = 0;
		for (int i = 0; i < (int)m_lines.size(); ++i)
		{
			LineInfo& line = m_lines[i];
			if (m_cursor_index >= line.begin_index && m_cursor_index < line.end_index)
			{
				line_index = i;
				line_pos = m_cursor_index - line.begin_index;
			}
		}

		if ((int)m_lines.size() - 1 > line_index)
		{
			LineInfo& line = m_lines[line_index + 1];
			int new_index = line.begin_index + line_pos;
			if (new_index >= line.end_index)
			{
				new_index = line.end_index - 1;
			}

			if (new_index > (int)m_items.size())
			{
				new_index = (int)m_items.size();
			}

			if (new_index < 0) new_index = 0;
			/*Item* last_item = m_items[new_index - 1];
			if (last_item->txt == _T("\n"))
			{
				new_index -= 1;
				if (new_index < 0) new_index = 0;
			}*/

			m_cursor_index = new_index;
			Redraw();
		}
	}
	return;
}

void UIRichEdit::OnMouseLeave()
{

}

void UIRichEdit::OnSetCursor(int x, int y)
{
	if (!m_is_editable)
	{
		int index = Hittest(x, y);
		if (index >= 0 && index < (int)m_items.size())
		{
			Item* item = m_items[index];
			if (item->type == Type_Link)
			{
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
				return;
			}
			else if (item->type == Type_Image || item->type == Type_AnimateImage)
			{
				if (item->link.GetLength() > 0)
				{
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
					return;
				}
			}
		}
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
	}
	else
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
	}
}

CString UIRichEdit::GetCurrentLink()
{
	if (m_select_index[0] >= 0)
	{
		Item* item = m_items[m_select_index[0] + 1];
		if (item->type == Type_Link)
		{
			return item->link;
		}
	}

	if (m_cursor_index > 0)
	{
		if (m_cursor_index <= (int)m_items.size())
		{
			Item* item = m_items[m_cursor_index - 1];
			if (item->type == Type_Link)
			{
				return item->link;
			}
		}
	}
	else if (m_cursor_index == 0)
	{
		Item* item = m_items[m_cursor_index];
		if (item->type == Type_Link)
		{
			return item->link;
		}
	}
	return CString();
}

bool UIRichEdit::SetSelectLink(CString link)
{
	if (m_select_index[0] >= 0 && m_select_index[1] <= (int)m_items.size())
	{
		for (int i = m_select_index[0]; i < m_select_index[1]; ++i)
		{
			Item* item = m_items[i];
			if (item->type == Type_Link || item->type == Type_Text)
			{
				item->type = link.IsEmpty() ? Type_Text:Type_Link;
				item->link_color = m_link_color;
				item->link = link;
			}
		}
		Redraw();
		return true;
	}
	return false;
}

bool UIRichEdit::ClearSelectLink()
{
	if (m_select_index[0] >= 0 && m_select_index[1] <= (int)m_items.size())
	{
		for (int i = m_select_index[0]; i < m_select_index[1]; ++i)
		{
			Item* item = m_items[i];
			if (item->type == Type_Link || item->type == Type_Text)
			{
				item->type = Type_Text;
				item->color = m_color;
				item->link = CString();
			}
		}
		Redraw();
		return true;
	}
	return false;
}

bool UIRichEdit::SetSelectColor(Gdiplus::Color color)
{
	if (m_select_index[0] >= 0 && m_select_index[1] <= (int)m_items.size())
	{
		for (int i = m_select_index[0]; i < m_select_index[1]; ++i)
		{
			Item* item = m_items[i];
			if (item->type == Type_Link || item->type == Type_Text)
			{
				item->color = color;
			}
		}
		Layout();
		Redraw();
		return true;
	}
	return false;
}

bool UIRichEdit::SetSelectAlign(AlignType align)
{
	if (m_select_index[0] >= 0 && m_select_index[1] <= (int)m_items.size())
	{
		for (int i = m_select_index[0]; i < m_select_index[1]; ++i)
		{
			Item* item = m_items[i];
			item->align = align;
		}
		Layout();
		Redraw();
		return true;
	}
	return false;
}

bool UIRichEdit::SetSelectFontFamily(CString font_family)
{
	if (m_select_index[0] >= 0 && m_select_index[1] <= (int)m_items.size())
	{
		for (int i = m_select_index[0]; i < m_select_index[1]; ++i)
		{
			Item* item = m_items[i];
			if (item->type == Type_Link || item->type == Type_Text)
			{
				item->font_family = font_family;
			}
		}
		Layout();
		Redraw();
		return true;
	}
	return false;
}

bool UIRichEdit::SetSelectFontSize(int font_size)
{
	if (m_select_index[0] >= 0 && m_select_index[1] <= (int)m_items.size())
	{
		for (int i = m_select_index[0]; i < m_select_index[1]; ++i)
		{
			Item* item = m_items[i];
			if (item->type == Type_Link || item->type == Type_Text)
			{
				item->font_size = font_size;
			}
		}
		Layout();
		Redraw();
		return true;
	}
	return false;
}

bool UIRichEdit::AddSelectFontStyle(int style)
{
	if (m_select_index[0] >= 0 && m_select_index[1] <= (int)m_items.size())
	{
		for (int i = m_select_index[0]; i < m_select_index[1]; ++i)
		{
			Item* item = m_items[i];
			item->font_style |= style;
		}
		Layout();
		Redraw();
		return true;
	}
	return false;
}

bool UIRichEdit::RemoveSelectFontStyle(int style)
{
	if (m_select_index[0] >= 0 && m_select_index[1] <= (int)m_items.size())
	{
		for (int i = m_select_index[0]; i < m_select_index[1]; ++i)
		{
			Item* item = m_items[i];
			item->font_style &= (~style);
		}
		Layout();
		Redraw();
		return true;
	}
	return false;
}

CString UIRichEdit::GetSelectText()
{
	if (m_select_index[0] >= 0 && m_select_index[1] <= (int)m_items.size())
	{
		CString txt;
		for (int i = m_select_index[0]; i < m_select_index[1]; ++i)
		{
			Item* item = m_items[i];
			if (item->type == Type_Link || item->type == Type_Text)
			{
				txt += item->txt;
			}
		}
		return txt;
	}
	return CString();
}

UIRichEdit::Item* UIRichEdit::GetFirstSelectItem()
{
	if (m_select_index[0] >= 0 && m_select_index[1] > m_select_index[0] && m_select_index[1] < (int)m_items.size())
	{
		return m_items[m_select_index[0]];
	}
	return NULL;
}

UIRichEdit::Item* UIRichEdit::GetItem(int index)
{
	if (index >= 0 && index <(int)m_items.size())
	{
		return m_items[index];
	}
	return NULL;
}

int UIRichEdit::GetItemCount()
{
	if (m_is_editable)
	{
		Item* last_item = NULL;
		if (m_items.size() > 0)
		{
			last_item = m_items[m_items.size() - 1];
			if (last_item->txt == _T("\n"))
			{
				return (int)(m_items.size() - 1);
			}
		}
	}
	return (int)m_items.size();
}

bool UIRichEdit::SetSelect(int begin_index, int end_index)
{
	if (begin_index >= 0 && begin_index < (int)m_items.size() && end_index > begin_index && end_index <= (int)m_items.size())
	{
		m_select_index[0] = begin_index;
		m_select_index[1] = end_index;
		m_cursor_index = end_index;
		return true;
	}
	return false;
}

bool UIRichEdit::IsHaveSelectItem()
{
	return (m_select_index[0] >= 0 && m_select_index[1] > m_select_index[0] && m_select_index[1] < (int)m_items.size());
}

CString EscapeHTML(CString& txt)
{
	if (txt == _T(" "))
	{
		return _T("&nbsp;");
	}
	else if (txt == _T("\""))
	{
		return _T("&quot;");
	}
	else if (txt == _T("&"))
	{
		return _T("&amp;");
	}
	else if (txt == _T("<"))
	{
		return _T("&lt;");
	}
	else if (txt == _T(">"))
	{
		return _T("&gt;");
	}
	return txt;
}

CString UnEscapeHTML(CString& txt)
{
	if (txt.GetLength() >= 3)
	{
		wchar_t c = txt.GetAt(1);
		if (c == '#')
		{
			CString value = txt.Mid(2);
			if (isxdigit(value[0]))
			{
				int v = _wtoi((LPCTSTR)value);
				return CString((wchar_t)v);
			}
		}
	}

	if (txt == _T("&nbsp;"))
	{
		return _T(" ");
	}
	else if (txt == _T("&quot;"))
	{
		return _T("\"");
	}
	else if (txt == _T("&amp;"))
	{
		return _T("&");
	}
	else if (txt == _T("&lt;"))
	{
		return _T("<");
	}
	else if (txt == _T("&gt;"))
	{
		return _T(">");
	}
	return CString();
}

static bool IsFile(CString path)
{
#ifdef _UNICODE
	struct _stat s;
	int r = _wstat(path, &s);
	return (r == 0) && (s.st_mode & S_IFREG) == S_IFREG;
#else
	struct stat s;
	int r = stat(path, &s);
	return (r == 0) && (s.st_mode & S_IFREG) == S_IFREG;
#endif
}

static TiXmlElement* FindXmlNode(TiXmlNode* node, const char* szNodeName)
{
	TiXmlNode* c = node->FirstChild();
	while (c)
	{
		if (c->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			TiXmlElement* e = c->ToElement();
			if (strcmp(e->Value(),szNodeName) == 0)
			{
				return e;
			}
		}
		c = c->NextSibling();
	}
	return NULL;
}

static void GetChildXmlElement(TiXmlElement* e, std::vector<TiXmlElement*>& childs)
{
	TiXmlNode* c = e->FirstChild();
	while (c)
	{
		if (c->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			TiXmlElement* e = c->ToElement();
			childs.push_back(e);
		}
		c = c->NextSibling();
	}
}

static CString GetXmlAttribute(TiXmlElement* e, const char* name)
{
	TiXmlAttribute* a = e->FirstAttribute();
	while (a)
	{
		if (strcmp(a->Name(), name) == 0)
		{
			return Utf82Unicode(a->Value()).c_str();
		}
		a = a->Next();
	}
	return CString();
}

static int GetXmlAttributeInt(TiXmlElement* e, const char* name)
{
	TiXmlAttribute* a = e->FirstAttribute();
	while (a)
	{
		if (strcmp(a->Name(), name) == 0)
		{
			return atoi(a->Value());
		}
		a = a->Next();
	}
	return -1;
}

static Gdiplus::Color ParseColor(CString str, Gdiplus::Color default_color)
{
	int pos = str.Find(_T('('));
	int pos2 = str.Find(_T(')'));
	if (pos2 > pos && pos >= 0)
	{
		CString rgb = str.Mid(pos+1, pos2 - pos - 1);
		int r = 0;
		int g = 0;
		int b = 0;
		int pos3 = rgb.Find(_T(','));
		if (pos3 >= 0)
		{
			CString str_r = rgb.Mid(0, pos3);
			r = _wtoi(str_r);
			pos3 += 1;
		}
		else
		{
			return default_color;
		}
		pos = rgb.Find(_T(','), pos3);
		if (pos >= 0)
		{
			CString str_g = rgb.Mid(pos3, pos - pos3);
			g = _wtoi(str_g);
			pos += 1;
		}
		else
		{
			return default_color;
		}
		CString str_b = rgb.Mid(pos);
		b = _wtoi(str_b);

		return Gdiplus::Color(255,r,g,b);
	}
	return default_color;
}

struct HTMLLoadContext
{
	CString path;
	std::list<UIRichEdit::Item*>	m_save_states;

	UIRichEdit::Item*	m_state;

	HTMLLoadContext()
	{
		m_state = NULL;
	}
	~HTMLLoadContext()
	{
		std::list<UIRichEdit::Item*>::iterator iter = m_save_states.begin();
		for (;iter != m_save_states.end();iter++)
		{
			UIRichEdit::Item* pItem = *iter;
			if(pItem)
			{
				delete pItem;
				pItem = NULL;
			}
		}
	}

	void push_state(UIRichEdit::Item* state)
	{
		m_save_states.push_back(state);
		m_state = state;
	}

	void pop_state()
	{
		if (m_state)
		{
			delete m_state;
			m_state = NULL;
		}
		if (m_save_states.size() > 0)
		{
			m_save_states.pop_back();
			m_state = m_save_states.back();
		}
	}
};

static void SplitCSS(CString& str, std::map<CString,CString>& v)
{
	int pos = 0;
	while (pos >= 0 && pos < str.GetLength())
	{
		int pos2 = str.Find(_T(';'), pos);
		CString data;
		if (pos2 >= 0)
		{
			data = str.Mid(pos, pos2 - pos);
			pos = pos2 + 1;
		}
		else
		{
			data = str.Mid(pos);
			pos = -1;
		}

		int pos3 = data.Find(_T(':'));
		if (pos3 >= 0)
		{
			CString key = data.Mid(0, pos3);
			CString value = data.Mid(pos3 + 1);

			key.Trim();
			value.Trim();

			v[key] = value;
		}
	}
}

static void SplitFontFamily(CString& str, std::vector<CString>& v)
{
	int pos = 0;
	while (pos >= 0 && pos < str.GetLength())
	{
		int pos2 = str.Find(_T(','), pos);
		CString data;
		if (pos2 >= 0)
		{
			data = str.Mid(pos, pos2 - pos);
			pos = pos2 + 1;
		}
		else
		{
			data = str.Mid(pos);
			pos = -1;
		}
		data.Trim();
		if (data.GetLength() > 0)
		{
			data.Replace(_T("\'"), _T(""));
		}
		v.push_back(data);
	}
}

void ParseCSSStyle(UIRichEdit::Item* state, CString& css_style)
{
	std::map<CString, CString> datas;
	SplitCSS(css_style, datas);

	std::map<CString, CString>::iterator iter = datas.begin();
	for (; iter != datas.end(); ++iter)
	{
		CString key = iter->first;
		CString value = iter->second;

		if (key == _T("font-size"))
		{
			state->font_size = _wtoi(value);
		}
		else if (key == _T("font-family"))
		{
			value.Replace(_T("&#39;"), _T(""));
			std::vector<CString> fonts;
			SplitFontFamily(value, fonts);
			if (fonts.size() > 0)
			{
				state->font_family = fonts[0];
			}
		}
		else if (key == _T("color"))
		{
			state->color = ParseColor(value, Gdiplus::Color(255,0,0,0));
			state->link_color = state->color;
		}
		else if (key == _T("text-align"))
		{
			if (value == _T("left"))
			{
				state->align = UIRichEdit::Align_Left;
			}
			else if (value == _T("center"))
			{
				state->align = UIRichEdit::Align_Center;
			}
			else if (value == _T("right"))
			{
				state->align = UIRichEdit::Align_Right;
			}
		}
		else if (key == _T("text-decoration"))
		{
			if (value == _T("none"))
			{
				state->font_style &= (~Gdiplus::FontStyleUnderline);
			}
			else if (value == _T("underline"))
			{
				state->font_style |= Gdiplus::FontStyleUnderline;
			}
		}
	}
}

static CString FindImageFilePath(HTMLLoadContext& context, CString filepath)
{
	if (IsFile(filepath))
	{
		return filepath;
	}
	CString file = GetFileName(filepath);
	filepath = context.path + "/" + file;
	if (IsFile(filepath))
	{
		return filepath;
	}
	filepath = context.path + "/" + file;
	if (IsFile(filepath))
	{
		return filepath;
	}
	return CString();
}

void LoadElement(std::vector<UIRichEdit::Item*>& items, HTMLLoadContext& context, TiXmlElement* element)
{
	if (strcmpi(element->Value(), "p") == 0 || strcmpi(element->Value(), "span") == 0 
		|| strcmpi(element->Value(), "em") == 0 || strcmpi(element->Value(), "strong") == 0
		|| strcmpi(element->Value(), "a") == 0 )
	{
		UIRichEdit::Item* state = new UIRichEdit::Item();
		state->Copy(context.m_state);

		if (strcmpi(element->Value(), "em") == 0)
		{
			state->font_style |= Gdiplus::FontStyleItalic; 
		}
		else if (strcmpi(element->Value(), "strong") == 0)
		{
			state->font_style |= Gdiplus::FontStyleBold;
		}

		CString css_style = GetXmlAttribute(element, "style");

		ParseCSSStyle(state, css_style);

		if (strcmpi(element->Value(), "a") == 0)
		{
			state->type = UIRichEdit::Type_Link;
			state->link = GetXmlAttribute(element, "href");
		}

		if (state->type == UIRichEdit::Type_None)
		{
			state->type = UIRichEdit::Type_Text;
		}

		context.push_state(state);

		TiXmlNode* c = element->FirstChild();
		while (c)
		{
			if (c->Type() == TiXmlNode::TINYXML_ELEMENT)
			{
				TiXmlElement* e = c->ToElement();

				LoadElement(items, context, e);
			}
			else if (c->Type() == TiXmlNode::TINYXML_TEXT)
			{
				CString text = Utf82Unicode(c->ToText()->Value()).c_str();

				for (int i = 0; i < text.GetLength();)
				{
					wchar_t c = text.GetAt(i);
					if (c == '\n' || c == ' ')
					{
						++i;
						continue;
					}
					if (c == '&')
					{
						// HTML转义字符
						int pos = text.Find(_T(';'), i + 1);
						if (pos > i + 1)
						{
							int count = pos - i + 1;
							if (count <= 7)
							{
								CString escape = text.Mid(i, count);
								CString value = UnEscapeHTML(escape);
								if (value.GetLength() > 0)
								{
									UIRichEdit::Item* item = new UIRichEdit::Item();
									item->Copy(context.m_state);

									item->txt = value;
									items.push_back(item);

									i = pos + 1;
									continue;
								}
							}
						}
					}

					// 非HTML转义字符
					UIRichEdit::Item* item = new UIRichEdit::Item();
					item->Copy(context.m_state);

					item->txt = CString(c);
					items.push_back(item);
					++i;

				}
			}
			c = c->NextSibling();
		}

		if (strcmpi(element->Value(), "p") == 0)
		{
			UIRichEdit::Item* item = new UIRichEdit::Item();
			item->Copy(context.m_state);

			item->type = UIRichEdit::Type_Text;

			item->txt = _T("\n");
			items.push_back(item);
		}

		context.pop_state();
	}
	else if (strcmpi(element->Value(), "br") == 0)
	{
		UIRichEdit::Item* item = new UIRichEdit::Item();
		item->Copy(context.m_state);

		item->type = UIRichEdit::Type_Text;
		item->txt = CString("\n");
		items.push_back(item);
	}
	else if (strcmpi(element->Value(), "img") == 0)
	{
		CString file = GetXmlAttribute(element, "src");

		// 查找本地是否有图像文件
		CString filepath = FindImageFilePath(context, file);

		if (IsFile(filepath))
		{
			UIRichEdit::Item* item = UIRichEdit::LoadImageEx(filepath);
			if (item)
			{
				item->link = context.m_state->link;
				item->align = context.m_state->align;

				CString link = GetXmlAttribute(element, "href");
				if (link.GetLength() > 0)
				{
					item->link = link;
				}

				CString css_style = GetXmlAttribute(element, "style");

				ParseCSSStyle(item, css_style);

				items.push_back(item);
			}
		}
	}
	else
	{
		UIRichEdit::Item* state = new UIRichEdit::Item();
		state->Copy(context.m_state);

		CString css_style = GetXmlAttribute(element, "style");

		ParseCSSStyle(state, css_style);

		if (state->type == UIRichEdit::Type_None)
		{
			state->type = UIRichEdit::Type_Text;
		}

		context.push_state(state);

		TiXmlNode* c = element->FirstChild();
		while (c)
		{
			if (c->Type() == TiXmlNode::TINYXML_ELEMENT)
			{
				TiXmlElement* e = c->ToElement();

				LoadElement(items, context, e);
			}
			else if (c->Type() == TiXmlNode::TINYXML_TEXT)
			{
				CString text = Utf82Unicode(c->ToText()->Value()).c_str();

				for (int i = 0; i < text.GetLength();)
				{
					wchar_t c = text.GetAt(i);
					if (c == '\n' || c == ' ')
					{
						++i;
						continue;
					}
					if (c == '&')
					{
						// HTML转义字符
						int pos = text.Find(_T(';'), i + 1);
						if (pos > i + 1)
						{
							int count = pos - i + 1;
							if (count <= 7)
							{
								CString escape = text.Mid(i, count);
								CString value = UnEscapeHTML(escape);
								if (value.GetLength() > 0)
								{
									UIRichEdit::Item* item = new UIRichEdit::Item();
									item->Copy(context.m_state);

									item->txt = value;
									items.push_back(item);

									i = pos + 1;
									continue;
								}
							}
						}
					}

					// 非HTML转义字符
					UIRichEdit::Item* item = new UIRichEdit::Item();
					item->Copy(context.m_state);

					item->txt = CString(c);
					items.push_back(item);
					++i;

				}
			}
			c = c->NextSibling();
		}
	}
}

bool UIRichEdit::LoadHTML(CString filepath)
{
	unsigned char* data = NULL;
	unsigned int size = 0;
	if (!ReadFileData(filepath, data, size))
	{
		return false;
	}

	TiXmlDocument* doc = new TiXmlDocument();
	doc->SetCondenseWhiteSpace(true);
	const char* p = doc->Parse((const char*)data, NULL, TIXML_ENCODING_UTF8);
	if (doc->ErrorId() != 0)
	{
		int error_col = doc->ErrorCol();
		int error_row = doc->ErrorRow();
		int error_id = doc->ErrorId();
		//const char* error = doc->ErrorDesc();
		delete doc;
		free(data);
		return false;
	}

	Clear();

	HTMLLoadContext context;
	context.path = GetFilePath(filepath);

	// 设置默认值
	Item* state = new Item();
	state->font_size = m_font_size;
	state->font_family = m_font_family;
	state->color = m_color;
	state->link_color = m_link_color;

	context.push_state(state);

	TiXmlNode* c = doc->FirstChild();
	while (c != NULL)
	{
		if (c->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			TiXmlElement* e = c->ToElement();
			const char* tag = e->Value();
			LoadElement(m_items, context, e);
		}
		c = c->NextSibling();
	}

	delete doc;
	free(data);

	Item* item = new Item;
	item->type = Type_Text;
	item->txt = _T("\n");
	item->font_size = m_font_size;
	item->font_family = m_font_family;
	item->color = m_color;
	m_items.push_back(item);

	Layout();
	Redraw();

	return true;
}



void SaveTextAndLink(TiXmlDocument* doc, UIRichEdit::Item& item_info, CString& txt)
{
	if (item_info.type == UIRichEdit::Type_Text)
	{
		TiXmlElement* element = new TiXmlElement("span");

		CString css_style;
		css_style.Format(_T("font-size:%ipx; color:rgb(%i,%i,%i);"), 
			item_info.font_size, 
			(int)item_info.color.GetR(), (int)item_info.color.GetG(), (int)item_info.color.GetB());

		if ((item_info.font_style & Gdiplus::FontStyleUnderline) != 0)
		{
			css_style += _T("text-decoration:underline;");
		}

		if (item_info.align != UIRichEdit::Align_Left)
		{
			if (item_info.align == UIRichEdit::Align_Center)
			{
				css_style += _T("text-align:center;");
			}
			else if (item_info.align == UIRichEdit::Align_Right)
			{
				css_style += _T("text-align:right;");
			}
		}

		if (item_info.font_family.GetLength() > 0)
		{
			css_style += _T("font-family:");
			css_style += item_info.font_family;
			css_style += _T(";");
		}

		element->SetAttribute("style", Unicode2Utf8((LPCTSTR)css_style).c_str());

		TiXmlText* txt_node = new TiXmlText(Unicode2Utf8((LPCTSTR)txt).c_str());
		element->LinkEndChild(txt_node);

		if ((item_info.font_style & Gdiplus::FontStyleBold) != 0 || (item_info.font_style & Gdiplus::FontStyleItalic) != 0)
		{
			TiXmlElement* e = element;
			if ((item_info.font_style & Gdiplus::FontStyleBold) != 0)
			{
				TiXmlElement* ee = new TiXmlElement("strong");
				ee->LinkEndChild(e);
				e = ee;
			}
			if ((item_info.font_style & Gdiplus::FontStyleItalic) != 0)
			{
				TiXmlElement* ee = new TiXmlElement("em");
				ee->LinkEndChild(e);
				e = ee;
			}
			doc->LinkEndChild(e);
		}
		else
		{
			doc->LinkEndChild(element);
		}
	}
	else if (item_info.type == UIRichEdit::Type_Link)
	{
		TiXmlElement* element = new TiXmlElement("a");

		TiXmlText* txt_node = new TiXmlText(Unicode2Utf8((LPCTSTR)txt).c_str());
		element->LinkEndChild(txt_node);

		CString css_style;
		css_style.Format(_T("font-size:%ipx; color:rgb(%i,%i,%i);"), 
			item_info.font_size, 
			(int)item_info.color.GetR(), (int)item_info.color.GetG(), (int)item_info.color.GetB());

		if ((item_info.font_style & Gdiplus::FontStyleUnderline) != 0)
		{
			css_style += _T("text-decoration:underline;");
		}

		if (item_info.align != UIRichEdit::Align_Left)
		{
			if (item_info.align == UIRichEdit::Align_Center)
			{
				css_style += _T("text-align:center;");
			}
			else if (item_info.align == UIRichEdit::Align_Right)
			{
				css_style += _T("text-align:right;");
			}
		}

		if (item_info.font_family.GetLength() > 0)
		{
			css_style += _T("font-family:");
			css_style += item_info.font_family;
			css_style += _T(";");
		}

		element->SetAttribute("style", Unicode2Utf8((LPCTSTR)css_style).c_str());

		element->SetAttribute("href", Unicode2Utf8((LPCTSTR)item_info.link).c_str());

		if ((item_info.font_style & Gdiplus::FontStyleBold) != 0 || (item_info.font_style & Gdiplus::FontStyleItalic) != 0)
		{
			TiXmlElement* e = element;
			if ((item_info.font_style & Gdiplus::FontStyleBold) != 0)
			{
				TiXmlElement* ee = new TiXmlElement("strong");
				ee->LinkEndChild(e);
				e = ee;
			}
			if ((item_info.font_style & Gdiplus::FontStyleItalic) != 0)
			{
				TiXmlElement* ee = new TiXmlElement("em");
				ee->LinkEndChild(e);
				e = ee;
			}
			doc->LinkEndChild(e);
		}
		else
		{
			doc->LinkEndChild(element);
		}
	}
}

bool UIRichEdit::SaveHTML(CString filepath)
{
	TiXmlDocument* doc = new TiXmlDocument();

	CString path = GetFilePath(filepath);

	Item item_info;	// 保留相同属性的文本的属性信息

	CString txt;

	size_t count = m_items.size();

	if (m_is_editable)
	{
		// 编辑模式下忽略最后一个换行符
		if (m_items.size() > 0)
		{
			Item* last_item = m_items[m_items.size()-1];
			if (last_item->type == Type_Text || last_item->type == Type_Link)
			{
				if (last_item->txt == _T("\n"))
				{
					count = m_items.size() - 1;
				}
			}
		}
	}

	for (size_t i = 0 ; i < count; ++i)
	{
		Item* item = m_items[i];
		if (item->type == Type_Text || item->type == Type_Link)
		{
			if (item->txt == _T("\n"))
			{
				if (txt.GetLength() > 0)
				{
					SaveTextAndLink(doc, item_info, txt);	
					txt = _T("");
				}

				item_info.type = Type_None;

				TiXmlElement* element = new TiXmlElement("br");

				doc->LinkEndChild(element);
				continue;
			}

			// 判断是否是可以在同一个标签下
			if (item->IsSameType(&item_info))
			{
				//txt += item->txt;
				txt += EscapeHTML(item->txt);
				continue;
			}
			else if (txt.GetLength() > 0)
			{
				SaveTextAndLink(doc, item_info, txt);
				//txt = item->txt;
				txt = EscapeHTML(item->txt);
			}
			else
			{
				//txt += item->txt;
				txt += EscapeHTML(item->txt);
			}
			item_info.Copy(item);
		}
		else if (item->type == Type_AnimateImage || item->type == Type_Image)
		{
			if (txt.GetLength() > 0)
			{
				SaveTextAndLink(doc, item_info, txt);
				txt = _T("");
			}

			TiXmlElement* element = new TiXmlElement("img");

			CString filename = GetFileName(item->txt);
			CString ff = path + "\\" + filename;
			element->SetAttribute("src", Unicode2Utf8((LPCTSTR)filename).c_str());

			if (item->link.GetLength() > 0)
			{
				element->SetAttribute("href", Unicode2Utf8((LPCTSTR)item->link).c_str());
			}

			if (item_info.align != UIRichEdit::Align_Left)
			{
				CString css_style;
				if (item_info.align == UIRichEdit::Align_Center)
				{
					css_style += _T("text-align:center;");
				}
				else if (item_info.align == UIRichEdit::Align_Right)
				{
					css_style += _T("text-align:right;");
				}
				element->SetAttribute("style", Unicode2Utf8((LPCTSTR)css_style).c_str());
			}

			CopyFile(ff, item->txt);

			doc->LinkEndChild(element);

			item_info.Copy(item);
		}
	}

	if (txt.GetLength() > 0)
	{
		SaveTextAndLink(doc, item_info, txt);
		txt = _T("");
	}

	FILE* fp = _wfopen(filepath, _T("wb"));
	if (fp == NULL)
	{
		delete doc;
		return false;
	}
	if (!doc->SaveFile(fp))
	{
		delete doc;
		fclose(fp);
		return false;
	}
	fclose(fp);
	delete doc;
	return true;
}

void UIRichEdit::OnImeComposition()
{
	if (!m_is_editable) return;

	{
		CPoint pt = GetCursorPos();

		SetImePos(pt.x, pt.y);
	}
	return;
}

void UIRichEdit::OnChar(CString& str)
{
	OnImeChar(str);
}

void UIRichEdit::OnImeChar(CString& txt)
{
	if (!m_is_editable) return;

	if (txt.GetAt(0) == VK_BACK || txt.GetAt(0) == VK_RETURN)
	{
		return;
	}

	AddText(txt);

	return;
}

void UIRichEdit::OnMouseWheel(int x, int y, float scroll)
{
	m_vscroll->OnMouseWheel(x, y, scroll);
}

void UIRichEdit::OnSizer()
{
	UIView::OnSizer();
	Layout();
}

void UIRichEdit::EnableEditable(bool is_enable)
{
	// 编辑模式,最后一个对象必须是换行符'\n'
	m_is_editable = is_enable;
	if (m_is_editable)
	{
		if (m_items.size() > 0)
		{
			Item* last_item = m_items.back();
			if ((last_item->type == Type_Text || last_item->type == Type_Link) && last_item->txt == _T("\n"))
			{

			}
			else
			{
				Item* item = new Item;
				item->type = Type_Text;
				item->txt = _T("\n");
				item->font_size = m_font_size;
				item->font_family = m_font_family;
				item->color = m_color;
				m_items.push_back(item);

				Layout();
				Redraw();
			}
		}
		else
		{
			Item* item = new Item;
			item->type = Type_Text;
			item->txt = _T("\n");
			item->font_size = m_font_size;
			item->font_family = m_font_family;
			item->color = m_color;
			m_items.push_back(item);

			Layout();
			Redraw();
		}
	}
}

void UIRichEdit::OnPaste()
{
	if (!m_is_editable) return;

	if (IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		DeleteSelect();
		HWND hwnd = (HWND)GetFrameWindow()-> m_hWnd;
		if (!OpenClipboard(hwnd)) return;
		HANDLE hMem = GetClipboardData(CF_UNICODETEXT);
		if (hMem)
		{
			wchar_t* data = (wchar_t*)GlobalLock(hMem);
			SIZE_T size = GlobalSize(hMem);
			CString txt(data, (int)((size / 2) - 1));

			AddText(txt);
			Redraw();

			GlobalUnlock(hMem);
		}

		CloseClipboard();
	}
}

void UIRichEdit::OnCopy()
{
	HWND hwnd = (HWND)GetFrameWindow()->m_hWnd;
	if (!OpenClipboard(hwnd)) return;
	EmptyClipboard();
	CString data = GetSelectText();

	HANDLE hMem = UnicodeToHandle((LPCTSTR)data, data.GetLength() + 1);
	SetClipboardData(CF_UNICODETEXT, hMem);
	CloseClipboard();
}

void UIRichEdit::OnCut()
{
	if (!m_is_editable) return;

	HWND hwnd = (HWND)GetFrameWindow()->m_hWnd;
	if (!OpenClipboard(hwnd)) return;
	EmptyClipboard();
	CString data = GetSelectText();

	HANDLE hMem = UnicodeToHandle((LPCTSTR)data, data.GetLength() + 1);
	SetClipboardData(CF_UNICODETEXT, hMem);
	CloseClipboard();
	DeleteSelect();
	Layout();
	Redraw();
}

void UIRichEdit::SetVscrollColor( Gdiplus::Color color )
{
	if(m_vscroll)
		m_vscroll->SetColor(color);
}

std::wstring Utf82Unicode(const std::string& utf8string)
{
	int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{

	}
	if (widesize == 0)
	{

	}

	std::vector<wchar_t> resultstring(widesize + 1);
	resultstring.assign(widesize + 1, 0);

	int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);

	if (convresult != widesize)
	{

	}

	return std::wstring(&resultstring[0]);
}

std::string Unicode2Utf8(const std::wstring& widestring)
{
	int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
	if (utf8size == 0)
	{

	}

	std::vector<char> resultstring(utf8size + 1);
	resultstring.assign(utf8size + 1, 0);

	int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);

	if (convresult != utf8size)
	{

	}

	return std::string(&resultstring[0]);
}