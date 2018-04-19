#pragma once

#include "Gdiplus.h"
#include "ReferenceObj.h"
#include "FastDelegate.h"
#include "UIDC.h"

#include <vector>

class UIFrame;
struct UIFrameInterface;

class UIView : public ReferenceObj
{
public:

	UIView(AutoRef<UIView> parent, int id, Gdiplus::Rect rc);
	UIView(AutoRef<UIView> parent, int id);
	virtual ~UIView(void);

	virtual void Destroy();

	bool IsEnbale();
	bool IsVisible();
	bool IsActive();
	virtual bool IsWindowView();

	virtual void DoInit(); // 初始化函数，窗口创建完成时创建

	AutoRef<UIView> Parent();

	Gdiplus::Rect Rect();			// 自己的大小,x,y总是为0
	const Gdiplus::Rect& FrameRect();		// 相对父窗口的位置
	Gdiplus::Rect WindowRect();		// 相对窗口的位置
	Gdiplus::Rect ClientRect();		// 相对窗口客户区的位置
	Gdiplus::Rect GlobalRect();		// 相对屏幕的位置
	void SetFrameRect(Gdiplus::Rect rc);

	virtual void Show(bool is_visible = true);
	virtual void Hide();
	virtual void SetEnable(bool is_enable);
	virtual void EnableClip(bool is_enable);

	virtual bool SetTimer(DWORD timer_id, DWORD elapse_millsec);
	virtual bool KillTimer(DWORD timer_id);

	virtual void SetFocus();

	AutoRef<UIView> GetRootView();
	CWnd* GetFrameWindow();
	UIFrameInterface* GetFrameInterface();
	virtual HWND GetHWND();

	AutoRef<UIView> FindView(int x, int y);
	void AddChildView(AutoRef<UIView> child);
	void RemoveFromParentView();
	virtual UIFrameInterface* ToFrameInterface();
	virtual CWnd* ToWnd();

	virtual void SetSizer(int sizer_left, int sizer_top, int sizer_right, int sizer_bottom, int width, int height);
	virtual void OnSizer();

	virtual void OnPosChanged();

	virtual void OnClip(UIDC* dc);		// 重载，自定义剪切区域
	virtual void OnPaint(UIDC* dc);
	virtual void OnPostPaint(UIDC* dc); // 子View绘制完成后调用

	virtual void OnLButtonDown(int x, int y);
	virtual void OnLButtonUp(int x, int y);

	virtual void OnLButtonDblClk(int x, int y);

	virtual void OnRButtonDown(int x, int y);
	virtual void OnRButtonUp(int x, int y);

	virtual void OnMouseMove(int x, int y);
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();

	virtual void OnMouseWheel(int x, int y, float scroll);
	virtual void OnSetCursor(int x, int y);

	virtual void OnKeyDown(unsigned int key);
	virtual void OnKeyUp(unsigned int key);

	virtual void OnSetFocus();
	virtual void OnKillFocus();

	virtual void OnChar(CString& str);
	virtual void OnImeChar(CString& str);
	virtual void OnImeComposition();

	virtual bool IsCanDrag(int x, int y);	// 控件中的这个点是否能拖动，如果返回true，表示WM_NCHITTEST 返回 HTCAPTION，为FASLE表示返回 HTCLIENT

	virtual bool OnClose();

	virtual bool OnTimer(DWORD timer_id);

	virtual void Redraw();
	virtual void RedrawAll();

	virtual void SetImePos(int x, int y);

	virtual void OnDrawThemeBackgroud(UIDC* dc);
	virtual void OnDrawThemeBackgroudDst(UIDC* dc,Gdiplus::Rect dstRc);

	void ShowError(DWORD error);

private:
	Gdiplus::Rect					m_rc;		// 相对父View的位置
public:
	int								m_id;
	bool							m_is_visible;
	bool							m_is_enable;
	bool							m_is_clip;
	std::vector<AutoRef<UIView>>	m_childs;

	AutoRef<UIView>					m_parent;

	int								m_sizer_left;
	int								m_sizer_top;
	int								m_sizer_right;
	int								m_sizer_bottom;
	int								m_sizer_width;
	int								m_sizer_height;
};


HFONT GetDefaultFont(int fontsize = -12, bool is_bold = false,bool is_italic = false,bool is_underline = false);
Gdiplus::Font* GetDefaultFontEx(int fontsize = 14, bool is_bold = false,bool is_italic = false,bool is_underline = false);
Gdiplus::Font* GetFamilyFont(CString font_family = _T("微软雅黑"),int fontsize = 14, bool is_bold = false,bool is_italic = false,bool is_underline = false);
void SetDefaultFontFamily(CString font_family);