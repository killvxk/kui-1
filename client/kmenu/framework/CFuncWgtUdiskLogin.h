#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtUdiskLogin
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
			RES_ITM_TXT_USER							 = 2 ,
			RES_ITM_IMG_USER							 = 3 ,
			RES_ITM_EDIT_USER							 = 4 ,
			RES_ITM_BTN_QUIT							 = 5 ,
			RES_ITM_BTN_OK								 = 6 ,
			RES_ITM_BTN_CANCEL							 = 7 ,
			RES_ITM_TXT_PSW								 = 9 ,
			RES_ITM_EDIT_PSW							 = 10 ,
			RES_ITM_IMG_PSW								 = 11 ,
			RES_ITM_TXT_TIPS							 = 12 ,
			RES_ITM_BTN_REG								 = 13 ,

		};

	public:// method
		CFuncWgtUdiskLogin();
		virtual ~CFuncWgtUdiskLogin();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void enter() 
		{ 
			CScreenTouchServer::getInstance()->addEventListener(this);
			m_editUser.setContent(L"");
			m_editPsw.setContent(L"");
			m_txtTips.setContent(L"");			

			m_editUser.show(TRUE); 
			m_editUser.setFocus(TRUE);

			m_editPsw.show(TRUE); 
		}

		void quit() { CScreenTouchServer::getInstance()->removeEventListener(this); m_editUser.show(FALSE); m_editPsw.show(FALSE); }

		void setAutoQuit() { m_bAutoQuit = TRUE; }

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void	onBtnQuitTchRel(CComponent *pTarget);
		void	onBtnOkTchRel(CComponent *pTarget);
		void	onBtnCancelTchRel(CComponent *pTarget);
		void	onBtnRegTchRel(CComponent *pTarget);
		void	onPopTipsDsy(CBasePopup *pTarget);
		void	onPopUdiskDsy(CBasePopup *pTarget);
		void	onPopUdiskRegDsy(CBasePopup *pTarget);


		// TODO: define logic event listener here
		void	onKeyTab();
		void	onKeyEnter();
		void	onKeyEsc();

		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlTextField						m_txtTitle;
		CCtrlTextField						m_txtUser;
		CCtrlImageField						m_imgUser;
		CCtrlEdit							m_editUser;
		CCtrlButton							m_btnQuit;
		CCtrlButton							m_btnOk;
		CCtrlButton							m_btnCancel;
		CCtrlTextField						m_txtPsw;
		CCtrlEdit							m_editPsw;
		CCtrlImageField						m_imgPsw;
		CCtrlTextField						m_txtTips;
		CCtrlButton							m_btnReg;


		// TODO: define normal private property here
		CPopTips*							m_pPopTips;
		CPopUdisk*							m_pPopUdisk;
		CPopUdiskReg*						m_pPopUdiskReg;

		BOOLEAN								m_bAutoQuit;
	};
}
