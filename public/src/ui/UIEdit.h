
#ifndef __EDIT_H__
#define __EDIT_H__

#include "UIView.h"
#include "FastDelegate.h"
#include "UIMenu.h"
#include "UItip.h"



HANDLE BytesToHandle(const char *szText, int nTextLen);

HANDLE UnicodeToHandle(const wchar_t *szText, int nTextLen);

#define IDC_COPY		3001
#define IDC_PASTE		3002
#define IDC_CUT			3003

class UIEdit : public UIView
{
public:

	int				m_select_index[2];
	int				m_cursor_index;
	int				m_max_text_length;

	CString			m_txt;
	CString			m_placeholder;
	CFont			m_font;

	bool			m_is_draw_frame;
	Gdiplus::Color	m_frame_color;
	float			m_frame_round;		// 边框圆角半径
	int				m_font_size;
	bool			m_is_down;

	int				m_pos; // text scroll pos
	bool			m_is_center_txt;
	bool			m_is_focus;
	bool			m_is_button_down;
	bool			m_is_password;

	bool			m_is_enable_edit;

	//cursor pos
	POINT			m_cursorpoint;
	FastDelegate<void()>	m_enter_delegate;
	FastDelegate<void()>	m_change_delegate;

	AutoRef<UIMenu>		m_menu;
	AutoRef<UItip>		m_tip;

	UIEdit(AutoRef<UIView> parent, int id, Gdiplus::Rect rc, 
		UINT uiCopy, UINT uiPaste, UINT uiCut)
		:UIView(parent, id, rc)
	{
		m_select_index[0] = -1;
		m_select_index[1] = -1;

		m_max_text_length = -1;

		m_cursor_index = 0;

		m_is_draw_frame = false;

		m_txt = _T("");

		m_pos = 0; // text scroll pos
		m_is_center_txt = false;
		m_is_focus = false;
		m_is_button_down = false;
		m_is_password = false;

		m_is_enable_edit = true;


		m_font_size = 12;
		SetFontSize(12);

		m_is_down = false;

		m_menu = new UIMenu();
		CString strTmp;
		strTmp.LoadString(uiCopy);
		m_menu->AddMenu(strTmp, IDC_COPY);
		strTmp.LoadString(uiPaste);
		m_menu->AddMenu(strTmp, IDC_PASTE);
		strTmp.LoadString(uiCut);
		m_menu->AddMenu(strTmp, IDC_CUT);
	}

	~UIEdit()
	{
		m_font.DeleteObject();
	}

	void Destroy()
	{
		UIView::Destroy();
		if (m_menu != NULL)
		{
			m_menu->Destroy();
			m_menu.Delete();
			m_menu = NULL;
		}
		if(m_tip != NULL)
		{
			m_tip->Destroy();
			m_tip.Delete();
			m_tip = NULL;
		}
	}

	void EnablePassword(bool is_enable)
	{
		m_is_password = is_enable;
	}
	void SetTextMaxLength(int nLength)
	{
		m_max_text_length = nLength;
	}
	void SetText(CString& txt, bool is_notify = true)
	{
		if (txt != m_txt)
		{
			m_txt = txt;
			m_select_index[0] = -1;
			m_select_index[1] = -1;
			m_cursor_index = m_txt.GetLength();
			Update();
			Redraw();

			if (m_change_delegate)
			{
				m_change_delegate();
			}
		}
	}

	CString GetText()
	{
		return m_txt;
	}

	void SetPlaceHolder(CString& txt)
	{
		m_placeholder = txt;
		Redraw();
	}

	void SetFontSize(int font_size)
	{
		m_font_size = font_size;
		Gdiplus::Font* font = GetDefaultFontEx(m_font_size);
		LOGFONTW logFont;
		UIDC dc(1,1);
		font->GetLogFontW(dc.m_dc, &logFont);
		m_font.DeleteObject();
		m_font.CreateFontIndirect(&logFont);
		delete font;

		Update();
		Redraw();
	}

	void SetFrame(Gdiplus::Color color, float frame_round = 0)
	{
		m_is_draw_frame = true;
		m_frame_color = color;
		m_frame_round = frame_round;
	}

	void SetCursorPos(int pos, bool is_redraw = true)
	{
		if (pos <= m_txt.GetLength() && pos >= 0)
		{
			m_cursor_index = pos;
			if (is_redraw)
			{
				Redraw();
			}
		}
	}

	void EnableEdit(bool is_enable)
	{
		m_is_enable_edit = is_enable;
	}

	void EnableCenterText(bool is_enable)
	{
		m_is_center_txt = is_enable;
	}

	virtual bool IsMultiLine()
	{
		return false;
	}

	virtual void Update()
	{
		MakeIndexVisible(m_cursor_index);
	}

	virtual void MakeIndexVisible(int index)
	{
		// 宽度太小就不计算了
		if (FrameRect().Width < 10)
		{
			m_pos = 0;
			return;
		}

		UIDC dc(1,1);
		std::vector<float> ww;
		int font_height = (int)dc.GetFontHeight(m_font);

		if (m_is_password)
		{
			CString txt = _T("");
			for (int i = 0; i < m_txt.GetLength(); ++i)
			{
				txt += _T("●");
			}
			dc.GetTextExtend(ww, txt, m_font);
		}
		else
		{
			dc.GetTextExtend(ww, m_txt, m_font);
		}
		
		float all_width = 0;
		float width = 0;
		int cursor_pos = -1;
		int i = 0;
		if (index < 0)
		{
			cursor_pos = 0;
			std::vector<float>::iterator iter = ww.begin();
			for (; iter != ww.end(); ++iter)
			{
				width += (*iter);
			}

			all_width = width;
		}
		else
		{
			std::vector<float>::iterator iter = ww.begin();
			for (; iter != ww.end(); ++iter,++i)
			{
				if (i == index)
				{
					cursor_pos = (int)width;
				}
				width += (*iter);
			}

			if (i == index)
				cursor_pos = (int)width;


			iter = ww.begin();
			for (; iter != ww.end(); ++iter)
			{
				all_width += (*iter);
			}
		}

		Gdiplus::Rect rc = _GetDrawRect();
		if (cursor_pos == -1)
			cursor_pos = (int)width;
		if (m_is_center_txt)
		{
			if (rc.Width >= width)
			{
				m_pos = -(rc.Width - (int)width) / 2;
			}
			else
			{
				if (cursor_pos + font_height >= m_pos + rc.Width)
				{
					m_pos = (cursor_pos + (font_height * 3)) - rc.Width;
				}
				else if (cursor_pos - font_height < m_pos)
				{
					int pos =  cursor_pos - (font_height * 3);
					if (pos < 0)
						pos = 0;
					m_pos = pos;
				}
				else if (m_pos < 0)
				{
					m_pos = 0;
				}
			}
		}
		else
		{
			if (cursor_pos + font_height >= m_pos + rc.Width)
			{
				m_pos = (cursor_pos + (font_height * 3)) - rc.Width;
			}
			else if (cursor_pos - font_height < m_pos)
			{
				int pos =  cursor_pos - (font_height * 3);
				if (pos < 0)
					pos = 0;
				m_pos = pos;
			}

			if (all_width < rc.Width)
			{
				m_pos = 0;
			}
		}
	}

	int Hittest(int x, int y)
	{
		UIDC dc(1,1);
		Gdiplus::Rect rc = _GetDrawRect();
		int xx = x - rc.X;
		int yy = y - rc.Y;

		int index = 0;
		if (m_is_password)
		{
			CString txt = _T("");
			for (int i = 0; i < m_txt.GetLength(); ++i)
			{
				txt += _T("●");
			}
			index = dc.HittestText(txt, Gdiplus::Rect(-m_pos,0,rc.Width+m_pos,rc.Height), xx, yy, 0, m_font);
		}
		else
		{
			index = dc.HittestText(m_txt, Gdiplus::Rect(-m_pos,0,rc.Width+m_pos,rc.Height), xx, yy, 0, m_font);
		}
		return index;
	}

	Gdiplus::Rect _GetDrawRect()
	{
		UIDC dc(1,1);
		Gdiplus::Rect rc = Rect();
		rc.X = 5;
		rc.Y = 2;
		rc.Width -= 5 + 1;
		rc.Height -= 4;
		int font_height = (int)dc.GetFontHeight(m_font);
		if (rc.Height > font_height)
		{
			rc.Y += ((rc.Height - font_height) / 2) + 1;
			rc.Height -= rc.Height - font_height;
		}
		
		return rc;
	}

	virtual void OnSizer()
	{
		UIView::OnSizer();
		Update();
		return;
	}

	void DeleteSelectString()
	{
		if (m_select_index[0] >= 0)
		{
			m_txt = m_txt.Mid(m_select_index[0], m_select_index[1] - m_select_index[0]);
			m_cursor_index = m_select_index[0];
		}
		m_select_index[0] = -1;
		m_select_index[1] = -1;
	}

	void OnLButtonDown(int x, int y)
	{
		m_is_down = true;

		int index = Hittest(x, y);
		m_cursor_index = index;
		m_select_index[0] = -1;
		m_select_index[1] = -1;
		RedrawAll();
	}

	void OnLButtonUp(int x, int y)
	{
		m_is_down = false;
		Redraw();
	}

	void OnMouseMove(int x, int y)
	{
		if (m_is_down)
		{
			int index = Hittest(x, y);
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
			MakeIndexVisible(index);
			Redraw();
		}
	}

	virtual void OnLButtonDblClk(int x, int y)
	{
		m_select_index[0] = 0;
		m_select_index[1] = m_txt.GetLength();
		m_cursor_index = m_select_index[1];
		Redraw();
		return;
	}

	virtual void OnRButtonDown(int x, int y)
	{
		Gdiplus::Rect rc = GlobalRect();
		int index = m_menu->ShowMenu(rc.X+x, rc.Y+y);
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
		Redraw();
		return;
	}

	virtual void OnKeyDown(unsigned int key)
	{
		if(m_tip != NULL)
			m_tip->ShowWindow(false);

		if (key == VK_BACK)
		{
			if (!m_is_enable_edit) return;
			if (m_select_index[0] >= 0)
			{
				_DeleteSelectString();
				Update();
				Redraw();

				if (m_change_delegate)
				{
					m_change_delegate();
				}

				return;
			}
			else
			{
				if (m_cursor_index > 0)
				{
					m_txt = m_txt.Mid(0, m_cursor_index - 1) + m_txt.Mid(m_cursor_index);
					m_cursor_index -= 1;
					Update();
					Redraw();

					if (m_change_delegate)
					{
						m_change_delegate();
					}
					return;
				}
			}
		}
		else if (key == VK_DELETE)
		{
			if (!m_is_enable_edit) return;
			if (m_select_index[0] >= 0)
			{
				_DeleteSelectString();
				Update();
				Redraw();
				if (m_change_delegate)
				{
					m_change_delegate();
				}
				return;
			}
			else
			{
				if (m_cursor_index < m_txt.GetLength())
				{
					m_txt = m_txt.Mid(0, m_cursor_index) + m_txt.Mid(m_cursor_index+1);
					Update();
					Redraw();

					if (m_change_delegate)
					{
						m_change_delegate();
					}
					return;
				}
			}
		}
		else if (key == VK_RETURN)
		{
			if (!IsMultiLine())
			{
				if (m_enter_delegate)
				{
					m_enter_delegate();
				}
			}
			else
			{
				if (!m_is_enable_edit) return;
				OnChar(CString(_T("\n")));
			}
			return;
		}
		else if (key == 'A') // ctrl + a
		{
			if ((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000)
			{
				m_select_index[0] = 0;
				m_select_index[1] = m_txt.GetLength();
				m_cursor_index = m_select_index[1];
				Redraw();
				return;
			}
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
			m_is_button_down = false;
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
				else if (m_cursor_index <= m_txt.GetLength())
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
			Update();
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
				else if (m_cursor_index < m_txt.GetLength() - 1)
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
				if (m_cursor_index < m_txt.GetLength())
				{
					m_cursor_index += 1;
				}
				m_select_index[0] = -1;
				m_select_index[1] = -1;
			}

			Update();
			Redraw();
		}
		else if (key == VK_TAB)
		{
		}
		return;
	}

	virtual void OnKeyUp(unsigned int key)
	{
	
	}

	virtual void OnChar(CString& str)
	{
		OnImeChar(str);
	}

	void _DeleteSelectString()
	{
		if (m_select_index[0] >= 0)
		{
			m_txt = m_txt.Mid(0,m_select_index[0]) + m_txt.Mid(m_select_index[1]);
			m_cursor_index = m_select_index[0];
		}
		m_select_index[0] = -1;
		m_select_index[1] = -1;
	}

	int _GetNextCharacterIndex(int index)
	{
		if (index < m_txt.GetLength())
		{
			index += 1;
		}
		return index;
	}

	int _GetPrevCharacterIndex(int index)
	{
		if (index >= 1)
		{
			index -= 1;
		}
		return index;
	}

	virtual void OnSetCursor(int x, int y)
	{
		SetCursor(LoadCursor(NULL, IDC_IBEAM));
	}

	virtual void OnImeComposition()
	{
		if (!m_is_enable_edit) return;

		//if ((lparam & 0x08) == 0x08) // GCS_COMPSTR
		{
			UIDC dc(1,1);
			Gdiplus::Rect rc = _GetDrawRect();
			rc.X += FrameRect().X;
			rc.Y += FrameRect().Y;
			rc.X -= m_pos;
			Gdiplus::Point pt = dc.GetTextPosByIndex(m_txt, rc, m_cursor_index, 0, m_font);
			pt.Y += FrameRect().Height;
			SetImePos(pt.X, pt.Y);
		}
		return;
	}
	void ShowTip(CString str)
	{
		if(m_tip == NULL) 
			m_tip = new UItip();

		m_tip->SetTipText(str);
		Gdiplus::Rect rc = GlobalRect();
		int xPos = 0 , yPos = 0;
		if(m_cursorpoint.x == 0)
		{
			xPos = rc.X+rc.Width;
			yPos = rc.Y+rc.Height/2;
		}
		else
		{
			xPos =rc.X + m_cursorpoint.x + 5;
			yPos = rc.Y+rc.Height/2;
		}
		m_tip->ShowTip(xPos,yPos,this->GetFrameWindow());
		SetTimer(1, 3000);
	}
	virtual void OnImeChar(CString& txt)
	{
		if (txt.GetAt(0) == VK_BACK || txt.GetAt(0) == VK_DELETE || txt.GetAt(0) == VK_RETURN 
			|| (GetKeyState(VK_CONTROL) & 0x8000) == 0x8000 ||
			txt.GetAt(0) == VK_ESCAPE)
		{
			return;
		}
		if (m_select_index[0] >= 0) _DeleteSelectString();
		if(m_max_text_length != -1 && m_txt.GetLength() >= m_max_text_length)
		{
			if(m_tip == NULL) 
				m_tip = new UItip();

			m_tip->SetTipText(CString(_T("字符串长度超出了限制")));
			Gdiplus::Rect rc = GlobalRect();
			int xPos = 0 , yPos = 0;
			if(m_cursorpoint.x == 0)
			{
				xPos = rc.X+rc.Width;
				yPos = rc.Y+rc.Height/2;
			}
			else
			{
				xPos =rc.X + m_cursorpoint.x + 5;
				yPos = rc.Y+rc.Height/2;
			}
			m_tip->ShowTip(xPos,yPos,this->GetFrameWindow());
			SetTimer(1, 3000);

			return;
		}
		m_txt = m_txt.Mid(0, m_cursor_index) + txt + m_txt.Mid(m_cursor_index);
		int len = txt.GetLength();
		m_cursor_index += len;
		Update();
		Redraw();

		if (m_change_delegate)
		{
			m_change_delegate();
		}
		return;
	}
	bool OnTimer(DWORD timer_id)
	{
		if(m_tip != NULL)
			m_tip->ShowWindow(false);
		return false;
	}
	virtual void OnPosChanged()
	{
		if(m_tip != NULL && IsWindow(m_tip->m_hWnd))
			m_tip->ShowWindow(false);
	}
	virtual bool IsWantReturn()
	{
		return false;
	}

	void OnCopy()
	{
		HWND hwnd = (HWND)GetFrameWindow()->m_hWnd;
		if (!OpenClipboard(hwnd)) return;
		EmptyClipboard();
		CString data;
		if (m_select_index[0] >= 0 && m_select_index[1] > 0)
		{
			data = m_txt.Mid(m_select_index[0], m_select_index[1] - m_select_index[0]);
		}

		HANDLE hMem = UnicodeToHandle((LPCTSTR)data, data.GetLength() + 1);
		SetClipboardData(CF_UNICODETEXT, hMem);
		CloseClipboard();
	}

	void OnCut()
	{
		if (!m_is_enable_edit) return;
		if (m_select_index[0] >= 0 && m_select_index[1] > 0)
		{
			HWND hwnd = (HWND)GetFrameWindow()->m_hWnd;
			if (!OpenClipboard(hwnd)) return;
			EmptyClipboard();
			CString data;
			if (m_select_index[0] >= 0 && m_select_index[1] > 0)
			{
				data = m_txt.Mid(m_select_index[0], m_select_index[1] - m_select_index[0]);
			}

			HANDLE hMem = UnicodeToHandle((LPCTSTR)data, data.GetLength() + 1);
			SetClipboardData(CF_UNICODETEXT, hMem);
			CloseClipboard();
			_DeleteSelectString();
			Update();
			Redraw();

			if (m_change_delegate)
			{
				m_change_delegate();
			}
		}
	}

	void OnPaste()
	{
		if (!m_is_enable_edit) return;
		if (IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			_DeleteSelectString();
			HWND hwnd = (HWND)GetFrameWindow()->m_hWnd;
			if (!OpenClipboard(hwnd)) return;
			HANDLE hMem = GetClipboardData(CF_UNICODETEXT);
			if (hMem)
			{
				wchar_t* data = (wchar_t*)GlobalLock(hMem);
				SIZE_T size = GlobalSize(hMem);
				CString txt(data, (int)((size / 2) - 1));
				if (!IsWantReturn())
				{
					txt.Replace(_T("\r"),_T(""));
					txt.Replace(_T("\n"),_T(""));
				}
				txt.Replace(_T("\t"),_T(""));

				m_txt = m_txt.Mid(0, m_cursor_index) + txt + m_txt.Mid(m_cursor_index);
				m_cursor_index += txt.GetLength();
				Update();
				Redraw();

				GlobalUnlock(hMem);

				if (m_change_delegate)
				{
					m_change_delegate();
				}
			}

			CloseClipboard();
		}
	}

	virtual void OnSetFocus()
	{
		m_is_focus = true;
		if (m_cursor_index < 0)
		{
			m_cursor_index = m_txt.GetLength();
			Redraw();
		}
		
		//m_parent->on_notify(this, SET_FOCUS, Attribute());
		return;
	}

	virtual void OnKillFocus()
	{
		if(m_tip != NULL)
			m_tip->ShowWindow(false);

		m_is_focus = false;
		//m_select_index[0] = -1;
		//m_select_index[1] = -1;
		Redraw();
		//m_parent->on_notify(this, KILL_FOCUS, Attribute());
		return;
	}

	virtual void OnPaint(UIDC* dc)
	{
		Gdiplus::Rect rc = Rect();
		rc.X = 0;
		rc.Y = 0;

		dc->EnableSmooth(true);

		if (m_is_draw_frame)
		{
			dc->FillRoundRect(Gdiplus::Color(255,255,255,255), m_frame_round, rc);
			dc->DrawRoundRect(1, m_frame_color, m_frame_round, rc);
		}
		else
		{
			dc->FillRect(Gdiplus::Color(255,255,255,255), rc);
		}

		rc = _GetDrawRect();
		if (m_txt.GetLength() == 0 && !m_is_focus)
		{
			dc->DrawTextSingleLine(m_placeholder, Gdiplus::Color(255,112,112,112), Gdiplus::Color(255,255,255,255), rc, 0, m_font);
			return;
		}
		
		dc->Save();
		dc->SetClip(rc);
		dc->m_off_x += rc.X;
		dc->m_off_y += rc.Y;

		rc.X = 0;
		rc.Y = 0;

		CString txt = m_txt;
		if (m_is_password)
		{
			txt = "";
			for (int i = 0; i < m_txt.GetLength(); ++i)
			{
				txt += _T("●");
			}
		}

		CString s1; // 前面未选中的部分
		CString s2;	// 中间选中的部分
		CString s3;	// 后面未选中的部分

		if (m_select_index[0] > 0)
		{
			s1 = txt.Mid(0, m_select_index[0]);
			s2 = txt.Mid(m_select_index[0], m_select_index[1] - m_select_index[0]);
			if (m_select_index[1] < txt.GetLength())
			{
				s3 = txt.Mid(m_select_index[1], txt.GetLength() - m_select_index[1]);
			}
		}
		else if (m_select_index[0] == 0)
		{
			s2 = txt.Mid(m_select_index[0], m_select_index[1] - m_select_index[0]);
			if (m_select_index[1] < txt.GetLength())
			{
				s3 = txt.Mid(m_select_index[1], txt.GetLength() - m_select_index[1]);
			}
		}
		else
		{
			s1 = txt;
		}

		int x = -m_pos;
		if (s1.GetLength() > 0)
		{
			Gdiplus::Rect b = dc->GetTextBounding(s1, Gdiplus::Rect(x,0,rc.Width+m_pos,rc.Height), 0, m_font);
			dc->DrawTextSingleLine(s1, Gdiplus::Color(255,0,0,0), Gdiplus::Color(255,255,255,255), Gdiplus::Rect(x,0,b.Width,b.Height), 0, m_font);
			x += b.Width;
		}

		if (s2.GetLength() > 0)
		{
			Gdiplus::Rect b = dc->GetTextBounding(s2, Gdiplus::Rect(x,0,rc.Width+m_pos,rc.Height), 0, m_font);
			dc->FillRect(Gdiplus::Color(255,49,106,197), b);
			dc->DrawTextSingleLine(s2, Gdiplus::Color(255,255,255,255), Gdiplus::Color(255,49,106,197), Gdiplus::Rect(x,0,b.Width,b.Height), 0, m_font);
			x += b.Width;
		}

		if (s3.GetLength() > 0)
		{
			Gdiplus::Rect b = dc->GetTextBounding(s3, Gdiplus::Rect(x,0,rc.Width+m_pos,rc.Height), 0, m_font);
			dc->DrawTextSingleLine(s3, Gdiplus::Color(255,0,0,0), Gdiplus::Color(255,255,255,255), Gdiplus::Rect(x,0,b.Width,b.Height), 0, m_font);
		}

		if (m_is_focus) // draw cursor
		{
			int x = 0;
			int y = 0;
			rc = _GetDrawRect();
			CString s = txt.Mid(0, m_cursor_index);
			if (m_is_center_txt && txt.GetLength() == 0)
			{
				x = rc.Width / 2;
				y = (int)dc->GetFontHeight(m_font);
			}
			else
			{
				Gdiplus::Point pt = dc->GetTextPosByIndex(s, Gdiplus::Rect(-m_pos,0,rc.Width+m_pos,rc.Height), m_cursor_index, 0, m_font);
				x = pt.X;
				y = pt.Y;
			}
			m_cursorpoint.x = 0;
			m_cursorpoint.y = 0;
			if (x < rc.X + rc.Width)
			{
				Gdiplus::Pen pen(Gdiplus::Color(255,0,0,0), 1);
				dc->DrawLine(1, Gdiplus::Color(255,0,0,0), x, y-(int)dc->GetFontHeight(m_font), x, y-1);
				m_cursorpoint.x = x + 5;
				m_cursorpoint.y = y-(int)dc->GetFontHeight(m_font);
			}
		}

		dc->Restore();

		return;
	}
};

#endif



