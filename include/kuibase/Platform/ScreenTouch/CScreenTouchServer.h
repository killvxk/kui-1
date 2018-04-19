/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"
#include "./UISystem/CEventDispatcher.h"
#include "./UISystem/2DSystem/Control/WebBrowser/jscallclient.h"
#include <ShellApi.h>

namespace OpenHMI
{
	struct OPENHMI_API tagScreenContext
	{
		UINT		uiScreenWidth;
		UINT		uiScreenHeight;	

		HWND		hWnd; 
		HDC			hDC;

		tagScreenContext();
		void	reset();

		BOOLEAN	operator == (const tagScreenContext &src)  const;
		BOOLEAN	operator != (const tagScreenContext &src)  const;
	};

	struct OPENHMI_API tagWebBrowserContext
	{
		HWND		hWnd;
		
		CWebForm	*pWeb;

		HDC			hDC;

		tagWebBrowserContext();

		BOOLEAN	operator == (const tagWebBrowserContext &src)  const;
		BOOLEAN	operator != (const tagWebBrowserContext &src)  const;
	};

	struct OPENHMI_API tagEditContext
	{
		HWND		hWnd;
		HDC			hDC;

		tagEditContext();

		BOOLEAN	operator == (const tagWebBrowserContext &src)  const;
		BOOLEAN	operator != (const tagWebBrowserContext &src)  const;
	};

	class OPENHMI_API CScreenTouchServer 
		: public CThread
		, public Object
		, public CEventDispatcher
	{
	public:// const define
		static const	UINT	EDIT_SETFOCUS			= 0;
		static const	UINT	EDIT_KILLFOCUS			= 1;
		static const	UINT	EDIT_UPDATE				= 2;
		static const	UINT	CAPTION_DBLCLK			= 3;
		static const	UINT	SETFOCUS				= 4;
		static const	UINT	KILLFOCUS				= 5;
		static const	UINT	DISPLAYCHANGE			= 6;
		static const	UINT	DROPFILES				= 7;
		static const	UINT	KEY_TAB					= 8;
		static const	UINT	KEY_ENTER				= 9;
		static const	UINT	KEY_ESC					= 10;
		static const	UINT	USER					= 11;

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum
		
	public:// method
		static	CScreenTouchServer*	getInstance();
		static	void				deleteInstance();

		void	setListener(IScreenTouchListener *pListener)	{ m_pListener = pListener; }
		void	setScreenSize(UINT uiWidth, UINT uiHeight)		{ m_context.uiScreenWidth = uiWidth; m_context.uiScreenHeight = uiHeight; }
		void	setScreenName(const String &strName)			{ m_strScreenName = strName; }
		
		const tagScreenContext* getScreenContext()	{ return &m_context; }
		
		const tagWebBrowserContext* getWebBrowserContext(UINT uiID);
		const tagEditContext* getEditContext(UINT uiID);
		HDROP getHDrop() { return m_hDrop; }

		BOOLEAN	initialize();
		void	unInitialize();

		void	dispatchEvent(UINT uiEventID) {CEventDispatcher::dispatchEvent(this, uiEventID);}

		void	setForceHide(BOOLEAN bValue) { m_bForceHide = bValue; }
		BOOLEAN	getForceHide(){ return m_bForceHide; }

	protected:// method
		CScreenTouchServer();
		virtual ~CScreenTouchServer();

		virtual	void	run();

	private:// method
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK EditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


	protected:// property	

	private:// property
		static	CScreenTouchServer		*s_pInstance;

		IScreenTouchListener			*m_pListener;
		tagScreenContext				m_context;
		Array2<tagWebBrowserContext>	m_aryWebBrowserContext;
		Array2<tagEditContext>			m_aryEditContext;
		String							m_strScreenName;

		CCursor							m_cursor;
		CWaitObject						m_waitStart;
		HDROP							m_hDrop;
		HICON							m_hIcon;
		BOOLEAN							m_bForceHide;
	};
}