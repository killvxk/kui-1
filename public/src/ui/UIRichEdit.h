

#ifndef __UI_RICH_EDIT_H__
#define __UI_RICH_EDIT_H__

#include "UIView.h"
#include "FastDelegate.h"
#include <vector>
#include "UIX.h"

#include "UIVScroll.h"
#include "UIMenu.h"
#include "UIEdit.h"
#include "xml/tinyxml.h"

class UIRichEdit : public UIView
{
public:

	//FastDelegate<void()>			m_click_delegate;
	FastDelegate1<CString, void>	m_link_delegate;

public:

	enum ItemType
	{
		Type_None,
		Type_Text,
		Type_Image,
		Type_AnimateImage,	// 动画图片
		Type_Link,
	};

	enum AlignType
	{
		Align_Left,
		Align_Center,
		Align_Right,
	};

	struct Item
	{
		ItemType						type;
		Gdiplus::Bitmap*				image;
		CString							txt;
		CString							link;
		Gdiplus::Color					color;
		Gdiplus::Color					link_color;
		int								width;
		int								height;
		int								font_size;
		int								font_style;			// 支持 Gdiplus::FontStyleBold Gdiplus::FontStyleItalic Gdiplus::FontStyleUnderline
		CString							font_family;
		AlignType						align;
		std::vector<Gdiplus::Bitmap*>	animate_images;
		int								animate_delay;		// 动画延时
		DWORD							time_ticket;
		int								animate_index;

		Item()
		{
			type = Type_None;
			image = NULL;
			font_size = 0;
			width = 0;
			height = 0;
			font_style = 0;
			font_size = 0;
			align = Align_Left;
			animate_index = 0;
			animate_delay = 0;
			time_ticket = 0;
		}

		~Item()
		{
			if (image)
			{
				delete image;
				image = NULL;
			}
			if (!animate_images.empty())
			{
				for (int i = 0; i < (int)animate_images.size(); ++i)
				{
					delete animate_images[i];
				}
				animate_images.clear();
			}
		}

		bool IsSameType(Item* item)
		{
			if (type != item->type)
			{
				return false;
			}
			if (type == Type_Text)
			{
				return font_size == item->font_size && font_family == item->font_family && font_style == item->font_style 
					&& align == item->align && color.GetValue() == item->color.GetValue();
			}
			else if (type == Type_Link)
			{
				return font_size == item->font_size && font_family == item->font_family && font_style == item->font_style 
					&& align == item->align && link_color.GetValue() == item->link_color.GetValue()
					&& link == item->link;
			}
			return false;
		}

		void Copy(const Item* item)
		{
			type = item->type;
			txt = item->txt;
			link = item->link;
			color = item->color;
			link_color = item->link_color;
			width = item->width;
			height = item->height;
			align = item->align;
			font_size = item->font_size;
			font_style = item->font_style;
			font_family = item->font_family;
			animate_delay = item->animate_delay;
			time_ticket = item->time_ticket;
			animate_index = item->animate_index;
		}
	};

	struct LineInfo
	{
		int		begin_index;
		int		end_index;
		int		height;
	};

protected:
	int				m_max_text_length;

	bool					m_is_down;

	std::vector<Item*>		m_items;
	std::vector<LineInfo>	m_lines;

	int						m_select_index[2];
	int						m_cursor_index;

	bool					m_is_editable;

	bool					m_is_draw_bg;
	Gdiplus::Color			m_bg_color;		// 背景颜色

	bool					m_is_draw_frame;
	Gdiplus::Color			m_frame_color;

	Gdiplus::Color			m_color;		// 默认字体颜色
	int						m_font_size;	// 默认字体大小
	CString					m_font_family;	// 默认字体

	Gdiplus::Color			m_link_color;   // 默认链接颜色

	Gdiplus::Bitmap*		m_bg_image;		// 背景图片

	AutoRef<UIVScroll>		m_vscroll;

	// 资源ID
	UINT m_uiCopy;							
	UINT m_uiPaste;
	UINT m_uiCut;

public:

	UIRichEdit(AutoRef<UIView> parent, int id, Gdiplus::Rect rc,
		UINT uiCopy, UINT uiPaste, UINT uiCut);

	virtual ~UIRichEdit();

	void Destroy();

	void Clear();

	void EnableEditable(bool is_enable);;

	void AddText(CString txt);

	void AddImage(CString filepath);

	void DeleteSelect();

	bool LoadHTML(CString filepath);

	bool SaveHTML(CString filepath);

	// 设置边框，不设置不绘制
	void SetFrame(Gdiplus::Color color)
	{
		m_is_draw_frame = true;
		m_frame_color = color;
	}

	// 设置背景颜色，不设置，不绘制
	void SetBgColor(Gdiplus::Color color)
	{
		m_is_draw_bg = true;
		m_bg_color = color;
	}

	// 设置默认字体颜色
	void SetDefaultTextColor(Gdiplus::Color color)
	{
		m_color = color;
	}

	// 设置默认字体
	void SetDefaultFontFamily(CString font_family)
	{
		m_font_family = font_family;
	}

	void SetDefaultLinkColor(Gdiplus::Color color)
	{
		m_link_color = color;
	}

	void SetDefaultFontSize(int font_size)
	{
		m_font_size = font_size;
	}

	// 设置背景图片，如果不设置，不绘制
	void SetBgImage(Gdiplus::Bitmap* image)
	{
		m_bg_image = image;
		Redraw();
	}

	void Layout();

	// 绘制内容的区域
	virtual Gdiplus::Rect GetDrawRect()
	{
		Gdiplus::Rect rc = Rect();
		rc.Inflate(-3, -3);
		return rc;
	}

	void SetTextMaxLength(int nLength)
	{
		m_max_text_length = nLength;
	}

	// 获取当前光标处的链接地址，如果不是链接返回空字符串
	CString GetCurrentLink();
	bool SetSelectLink(CString link);
	bool ClearSelectLink();
	bool SetSelect(int begin_index, int end_index);
	bool IsHaveSelectItem();
	bool SetSelectColor(Gdiplus::Color color);
	bool SetSelectAlign(UIRichEdit::AlignType align);
	bool SetSelectFontFamily(CString font_family);
	bool SetSelectFontSize(int font_size);
	void SetVscrollColor(Gdiplus::Color color);

	bool AddSelectFontStyle(int style);
	bool RemoveSelectFontStyle(int style);
	
	CString GetSelectText();
	UIRichEdit::Item* GetFirstSelectItem(); // 获取选中的第一个对象
	UIRichEdit::Item* GetItem(int index);
	int GetItemCount();

	int GetCursorIndex();
	CPoint GetCursorPos();
	int Hittest(int x, int y);

	static Item* LoadImageEx(CString filepath);

protected:

	virtual void OnImeComposition();

	void OnChar(CString& str);

	virtual void OnImeChar(CString& txt);

	void OnMouseWheel(int x, int y, float scroll);

	void OnSizer();

	virtual void OnKeyDown(unsigned int key);

	bool OnTimer(DWORD timer_id);

	void OnLButtonDown(int x, int y);

	void OnLButtonUp(int x, int y);

	void OnMouseMove(int x, int y);

	void OnMouseLeave();

	void OnSetCursor(int x, int y);

	void OnRButtonDown(int x, int y);

	void OnPaint(UIDC* dc);

	void OnPaste();
	void OnCopy();
	void OnCut();

	int _GetNextCharacterIndex(int index);
	int _GetPrevCharacterIndex(int index);
};


class FontCache
{
public:

	FontCache()
	{
		// 获取默认字体
		Gdiplus::Font* font = GetDefaultFontEx(12, false);
		Gdiplus::FontFamily family;
		font->GetFamily(&family);
		wchar_t name[256] = {0};
		family.GetFamilyName(name, LANG_NEUTRAL);
		m_default_font_family = name;

		delete font;
	}

	~FontCache()
	{
		for (int i = 0; i < (int)m_fonts.size(); ++i)
		{
			FontInfo& info = m_fonts[i];
			delete info.font;
		}
		m_fonts.clear();
	}
	
	Gdiplus::Font* GetFont(CString font_family, int font_size, int font_style)
	{
		if (font_family.GetLength() == 0)
		{
			font_family = m_default_font_family;
		}

		for (int i = 0; i < (int)m_fonts.size(); ++i)
		{
			FontInfo& info = m_fonts[i];
			if (font_size == info.font_size && font_family == info.font_family && font_style == info.font_style)
			{
				return info.font;
			}
		}

		FontInfo info;
		info.font_family = font_family;
		info.font_size = font_size;
		info.font_style = font_style;

		info.font = new Gdiplus::Font(font_family, (float)font_size, font_style, Gdiplus::UnitPixel, NULL);

		m_fonts.push_back(info);
		return info.font;
	}

	struct FontInfo
	{
		CString			font_family;
		int				font_size;
		int				font_style;
		Gdiplus::Font*	font;
	};

	std::vector<FontInfo>	m_fonts;
	CString					m_default_font_family;
};


#endif

