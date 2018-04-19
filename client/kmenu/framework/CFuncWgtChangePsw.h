#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtChangePsw
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
			RES_ITM_TITLE								 = 1 ,
			RES_ITM_IMG_PSW								 = 2 ,
			RES_ITM_IMG_PSW_Q							 = 3 ,
			RES_ITM_BTN_QUIT							 = 4 ,
			RES_ITM_BTN_OK								 = 5 ,
			RES_ITM_BTN_CANCEL							 = 6 ,
			RES_ITM_TXT_PSW								 = 9 ,
			RES_ITM_TXT_PSW_Q							 = 11 ,
			RES_ITM_EDIT_PSW							 = 12 ,
			RES_ITM_EDIT_PSW_Q							 = 13 ,
			RES_ITM_TXT_TIPS							 = 14 ,

		};

	public:// method
		CFuncWgtChangePsw();
		virtual ~CFuncWgtChangePsw();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void enter() 
		{ 
			CScreenTouchServer::getInstance()->addEventListener(this);
			m_editPsw.setContent(L"");
			m_editPsw.show(TRUE);
			m_editPsw.setFocus(TRUE);

			m_editPswQ.setContent(L"");
			m_editPswQ.show(TRUE);
		}

		void quit() 
		{ 
			CScreenTouchServer::getInstance()->removeEventListener(this);
			m_editPsw.show(FALSE);
			m_editPswQ.show(FALSE);
		}

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void	onBtnQuitTchRel(CComponent *pTarget);
		void	onBtnOkTchRel(CComponent *pTarget);
		void	onBtnCancelTchRel(CComponent *pTarget);
		void	onPopTipsDsy(CBasePopup *pTarget);

		// TODO: define logic event listener here
		void	onKeyTab();
		void	onKeyEnter();
		void	onKeyEsc();

		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlTextField						m_txtTitle;
		CCtrlImageField						m_imgPsw;
		CCtrlImageField						m_imgPswQ;
		CCtrlButton							m_btnQuit;
		CCtrlButton							m_btnOk;
		CCtrlButton							m_btnCancel;
		CCtrlTextField						m_txtPsw;
		CCtrlTextField						m_txtPswQ;
		CCtrlEdit							m_editPsw;
		CCtrlEdit							m_editPswQ;
		CCtrlTextField						m_txtTips;


		// TODO: define normal private property here
		CPopTips*							m_pPopTips;
	};
}
