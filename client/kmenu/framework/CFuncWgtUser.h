#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtUser
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
			RES_ITM_IMG_TITLE							 = 2 ,
			RES_ITM_BTN_QUIT							 = 4 ,
			RES_ITM_TXT									 = 5 ,
			RES_ITM_WEB									 = 6 ,

		};

	public:// method
		CFuncWgtUser();
		virtual ~CFuncWgtUser();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void openUrl(const String& strURL);
		void show(const String& strName, INT w, INT h);
		void show(const String& url, const String& strName, INT w, INT h);
		void quit();

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void	onBtnQuitTchRel(CComponent *pTarget);


		// TODO: define logic event listener here
		void onKeyEsc();

		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgTitle;
		CCtrlButton							m_btnQuit;
		CCtrlTextField						m_txt;
		CCtrlWebBrowser						m_webBrsWeb;


		// TODO: define normal private property here
	};
}
