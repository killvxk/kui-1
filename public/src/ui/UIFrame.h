#pragma once

#include "UIView.h"
#include "UIDC.h"

#include <map>


#define WM_REDRAW_WINDOW 2001
#define WM_WINDOW_INIT   2002

class UIFrame;

#define UI_FRAME_BASE CFrameWnd
//#define UI_FRAME_BASE CWnd

class UIFrameShadow : public UI_FRAME_BASE, public UIView
{
	DECLARE_DYNCREATE(UIFrameShadow)
public:
	UIFrameShadow();
	virtual ~UIFrameShadow();

	virtual void Destroy();

	BOOL Create(CRect rc, HWND hParent);
	BOOL ShowWindow(int nCmdShow);

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg LRESULT OnRedrawWindow(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow,UINT nStatus);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	void Draw(UIDC* dc);

	DECLARE_MESSAGE_MAP()

public:
	UIDC*				m_dc;

	UIFrame*			m_parent;
	Gdiplus::Image*		m_shadow_image;
};

struct UIFrameInterface
{
	virtual void SetViewFocus(UIView* view) = NULL;
	virtual AutoRef<UIView> GetFocusView() = NULL;

	virtual bool SetTimer(AutoRef<UIView> view, DWORD timer_id, DWORD elapse_millsec) = NULL;
	virtual bool KillTimer(DWORD timer_id) = NULL;

	virtual bool KillTimer(AutoRef<UIView> view) = NULL;

	virtual Gdiplus::Image* GetThemeBackgroud() = NULL;

	virtual void SetThemeBackgroud(Gdiplus::Image* image) = NULL;

	virtual void ClientToView(CPoint& point, UIView* view) = NULL;
	virtual void RedrawView(UIView* view) = NULL;
	virtual void RedrawAllView() = NULL;

	virtual Gdiplus::Rect Rect() = NULL;

	virtual HWND GetHWND() = NULL;
};

class UIFrame : public UI_FRAME_BASE, public UIView, public UIFrameInterface
{
	DECLARE_DYNCREATE(UIFrame)
public:
	UIFrame();
	virtual ~UIFrame();

	virtual void Destroy();

	BOOL CreateDefault(bool is_shadow, Gdiplus::Rect rc, LPCTSTR name);
	BOOL Create(bool is_shadow, Gdiplus::Rect rc, LPCTSTR name, DWORD style, DWORD exstyle,CWnd* pParentWnd=NULL);

	virtual void EnableLayeredWindow(bool is_enable);
	virtual void EnableUserDragSize(bool is_enable);
	void		 SetMinWidthHeight(int nWith,int nHeight);

	virtual bool SetTimer(AutoRef<UIView> view, DWORD timer_id, DWORD elapse_millsec);
	virtual bool KillTimer(DWORD timer_id);

	virtual bool KillTimer(AutoRef<UIView> view);

	virtual void ClientToView(CPoint& point, UIView* view);	// 窗口客户区坐标到视图坐标

	virtual void RedrawView(UIView* view);
	
	virtual void RedrawAllView();

	virtual Gdiplus::Rect Rect();

	BOOL ShowWindow(int nCmdShow);

	virtual bool IsWindowView();

	virtual void SetWindowBottom();

	virtual Gdiplus::Image* GetThemeBackgroud();
	virtual void SetThemeBackgroud(Gdiplus::Image* image);

	virtual void SetViewFocus(UIView* view);
	virtual AutoRef<UIView> GetFocusView();

	virtual HWND GetHWND();

	virtual UIFrameInterface* ToFrameInterface();
	virtual CWnd* ToWnd();

	virtual void SetNoZorder(bool is_no_zorder);

	void DoModal();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

protected:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	virtual afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual afx_msg LRESULT OnMouseLeave2(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd,UINT nHitTest,UINT message);

	afx_msg void OnChar(UINT nChar,UINT nRepCnt,UINT nFlags);
	afx_msg LRESULT OnImeChar2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnImeComposition(WPARAM wParam, LPARAM lParam);

	afx_msg void OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar,UINT nRepCnt,UINT nFlags);

	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* pMMI);

	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS FAR*);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);

	afx_msg LRESULT OnRedrawWindow(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitWindow(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTimer2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCloseWindow(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow,UINT nStatus);

	virtual LRESULT HittestDrag(CPoint point);

	virtual void PostNcDestroy();

	void MoveShadow();
	void MoveShadow(CRect win_rc);

	virtual void DoPaint(UIView* view = NULL);
	void DrawView(UIDC* dc,UIView* viewNeed, UIView* view);
	void DrawView(UIDC* dc, UIView* view);
	bool IsCanDrag(int x, int y);

public:

	int						m_client_off_x;
	int						m_client_off_y;

protected:

	DECLARE_MESSAGE_MAP()

	AutoRef<UIView>			m_hover_view;
	AutoRef<UIView>			m_capture_view;
	AutoRef<UIView>			m_focus_view;
	AutoRef<UIView>			m_prev_focus_view;
	AutoRef<UIView>			m_modal_view;

	bool					m_is_max_fullscreen;		// 最大化时全屏
	bool					m_is_can_user_drag_size;	// 用户是否可以拖动窗口大小
	bool					m_is_user_drag_size;		// 用户是否在拖动窗口大小

	int						m_drag_code;
	CPoint					m_down_pos;		// 鼠标按下时的坐标
	Gdiplus::Rect			m_down_rc;

	int						m_min_width;
	int						m_min_height;

	bool					m_is_no_zorder;

	CString					m_ime_string;

	Gdiplus::Image*			m_theme_bg_image;

	UIDC*					m_dc;
	Gdiplus::Rect			m_need_draw_rc;
	bool					m_bDrawAllView;
	
	AutoRef<UIFrameShadow>	m_shadow_window;

	std::map<DWORD, AutoRef<UIView>>		m_timer_map;
};


