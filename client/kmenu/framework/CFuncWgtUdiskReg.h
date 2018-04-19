#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtUdiskReg
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
			RES_ITM_IMG_PSW								 = 14 ,
			RES_ITM_IMG_PSW_Q							 = 17 ,
			RES_ITM_BTN_QUIT							 = 5 ,
			RES_ITM_BTN_OK								 = 6 ,
			RES_ITM_BTN_CANCEL							 = 7 ,
			RES_ITM_TXT_USER							 = 11 ,
			RES_ITM_EDIT_USER							 = 12 ,
			RES_ITM_TXT_PSW								 = 13 ,
			RES_ITM_IMG_USER							 = 15 ,
			RES_ITM_TXT_PSW_Q							 = 16 ,
			RES_ITM_EDIT_PSW							 = 18 ,
			RES_ITM_EDIT_PSW_Q							 = 19 ,
			RES_ITM_TXT_TIPS							 = 20 ,

		};

	public:// method
		CFuncWgtUdiskReg();
		virtual ~CFuncWgtUdiskReg();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void enter()
		{
			CScreenTouchServer::getInstance()->addEventListener(this);

			m_editUser.setContent(L"");			
			m_editUser.show(TRUE);
			m_editUser.setFocus(TRUE);

			m_editPsw.setContent(L"");
			m_editPsw.show(TRUE);

			m_editPswQ.setContent(L"");
			m_editPswQ.show(TRUE);

			m_txtTips.setContent(L"");
		}

		void quit() { CScreenTouchServer::getInstance()->removeEventListener(this); m_editUser.show(FALSE); m_editPsw.show(FALSE); m_editPswQ.show(FALSE); }

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void	onBtnQuitTchRel(CComponent *pTarget);
		void	onBtnOkTchRel(CComponent *pTarget);
		void	onBtnCancelTchRel(CComponent *pTarget);
		void	onPopTipsDsy(CBasePopup *pTarget);

		// TODO: define logic event listener here
		void	onPopUdiskDsy(CBasePopup *pTarget);
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
		CCtrlTextField						m_txtUser;
		CCtrlEdit							m_editUser;
		CCtrlTextField						m_txtPsw;
		CCtrlImageField						m_imgUser;
		CCtrlTextField						m_txtPswQ;
		CCtrlEdit							m_editPsw;
		CCtrlEdit							m_editPswQ;
		CCtrlTextField						m_txtTips;


		// TODO: define normal private property here
		CPopTips*							m_pPopTips;
		CPopUdisk*							m_pPopUdisk;
	};
}
