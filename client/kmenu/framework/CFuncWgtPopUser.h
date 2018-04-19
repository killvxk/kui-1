#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtPopUser
		: public CBaseFuncWidget
	{
	public:// TODO: define const here

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here

	private:// TODO: define embed class or struct or enum here
		enum EResItem
		{
			RES_ITM_UNKNOWN				= 0,

			// TODO: define control ID here
			RES_ITM_BG									 = 1 ,
			RES_ITM_USER								 = 2 ,

		};

	public:// method
		CFuncWgtPopUser();
		virtual ~CFuncWgtPopUser();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();
		virtual	ICursorTracker*	probe(INT iX, INT iY);

		// TODO: define your public method here
		void openUrl(const String& strURL) { m_funcWgtUser.openUrl(strURL); }
		void show(const String& strName, INT w, INT h) 
		{
			CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

			UINT sw = pServer->getScreenContext()->uiScreenWidth;
			UINT sh = pServer->getScreenContext()->uiScreenHeight;

			INT x = (sw - m_funcWgtUser.getPreferredSize().getX()) / 2;
			INT y = (sh - m_funcWgtUser.getPreferredSize().getY()) / 2;
			m_funcWgtUser.setX(x);
			m_funcWgtUser.setY(y);

			m_imgBg.setPreferredSize(sw, sh);

			m_funcWgtUser.show(strName, w, h); 
		}

		void show(const String& url, const String& strName, INT w, INT h) 
		{ 
			CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

			UINT sw = pServer->getScreenContext()->uiScreenWidth;
			UINT sh = pServer->getScreenContext()->uiScreenHeight;

			INT x = (sw - m_funcWgtUser.getPreferredSize().getX()) / 2;
			INT y = (sh - m_funcWgtUser.getPreferredSize().getY()) / 2;
			m_funcWgtUser.setX(x);
			m_funcWgtUser.setY(y);

			m_imgBg.setPreferredSize(sw, sh);

			m_funcWgtUser.show(url, strName, w, h); 
		}
		void quit() { m_funcWgtUser.quit(); }

	protected:// method		

	private:// method
		
		// TODO: define control event listener here


		// TODO: define logic event listener here

		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgBg;
		CFuncWgtUser						m_funcWgtUser;


		// TODO: define normal private property here
	};
}
