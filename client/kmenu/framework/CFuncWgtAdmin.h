#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtAdmin
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
			RES_ITM_TXT									 = 6 ,
			RES_ITM_TITLE								 = 4 ,
			RES_ITM_IMG_EDIT							 = 7 ,
			RES_ITM_EDIT_PSW							 = 1 ,
			RES_ITM_BTN_QUIT							 = 3 ,
			RES_ITM_BTN_LOGIN							 = 8 ,
			RES_ITM_BTN_CANCEL							 = 9 ,
			RES_ITM_TXT_TIPS							 = 10 ,

		};

	public:// method
		CFuncWgtAdmin();
		virtual ~CFuncWgtAdmin();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void enter() { CScreenTouchServer::getInstance()->addEventListener(this); m_editPsw.show(TRUE); m_editPsw.setFocus(TRUE); }
		void quit() { CScreenTouchServer::getInstance()->removeEventListener(this); m_editPsw.show(FALSE); }

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void	onBtnQuitTchRel(CComponent *pTarget);
		void	onBtnLoginTchRel(CComponent *pTarget);
		void	onBtnCancelTchRel(CComponent *pTarget);


		// TODO: define logic event listener here
		void	onKeyTab();
		void	onKeyEnter();
		void	onKeyEsc();
		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlTextField						m_txt;
		CCtrlTextField						m_txtTitle;
		CCtrlImageField						m_imgEdit;
		CCtrlEdit							m_editPsw;
		CCtrlButton							m_btnQuit;
		CCtrlButton							m_btnLogin;
		CCtrlButton							m_btnCancel;
		CCtrlTextField						m_txtTips;


		// TODO: define normal private property here
	};
}
