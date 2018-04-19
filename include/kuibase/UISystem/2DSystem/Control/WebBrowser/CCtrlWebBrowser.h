#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{	
	class OPENHMI_API CEventWebBrowser
	{
	public:
		
	};

	class OPENHMI_API CCtrlWebBrowser : public CComponent
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CCtrlWebBrowser();
		virtual ~CCtrlWebBrowser();

		virtual void	onMove(INT iX, INT iY);
		virtual void	onLostHover();

		BOOL navigate2(const String& strURL);
		void stopUrl();
		virtual	void	refresh();
		virtual	void	littleRefresh();
		void	show(BOOLEAN bValue);
		BOOLEAN getFocus();
		void setWebBrsID(UINT uiID);

	protected:// method
		CCtrlWebBrowser(const CCtrlWebBrowser&){}
		CCtrlWebBrowser& operator =(const CCtrlWebBrowser&){return *this;}

	private:// method

	protected:// property

	private:// property
		const tagWebBrowserContext* m_pContext;
		String	m_StrCurURL;
	};
}