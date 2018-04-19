#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtUploadcfg
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
			RES_ITM_IMG_NAME							 = 2 ,
			RES_ITM_BTN_QUIT							 = 4 ,
			RES_ITM_BTN_UPLOAD							 = 5 ,
			RES_ITM_BTN_CANCEL							 = 6 ,
			RES_ITM_TXT_NAME							 = 7 ,
			RES_ITM_TXT_PATH							 = 8 ,
			RES_ITM_EDIT_NAME							 = 9 ,
			RES_ITM_TXT_TIPS							 = 11 ,
			RES_ITM_BTN_SEL								 = 12 ,
			RES_ITM_TXT_PATH_NAME						 = 15 ,
			RES_ITM_IMG_BG								 = 16 ,

		};

	public:// method
		CFuncWgtUploadcfg();
		virtual ~CFuncWgtUploadcfg();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void showCfg(DWORD gid) { m_Gid = gid; }
		void enter() { CScreenTouchServer::getInstance()->addEventListener(this); m_editName.show(TRUE); m_editName.setFocus(TRUE); }
		void quit() { CScreenTouchServer::getInstance()->removeEventListener(this); m_editName.show(FALSE); }

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void	onBtnQuitTchRel(CComponent *pTarget);
		void	onBtnUploadTchRel(CComponent *pTarget);
		void	onBtnCancelTchRel(CComponent *pTarget);
		void	onBtnSelTchRel(CComponent *pTarget);
		void	onPopTipsDsy(CBasePopup *pTarget);

		// TODO: define logic event listener here
		void	onKeyEnter();
		void	onKeyEsc();
		void	onKeyTab();

		// TODO: define your normal private method here
		void	showTips(const String& str);
		static unsigned int __stdcall threadProc(void *pParam);

	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlTextField						m_txtTitle;
		CCtrlImageField						m_imgName;
		CCtrlButton							m_btnQuit;
		CCtrlButton							m_btnUpload;
		CCtrlButton							m_btnCancel;
		CCtrlTextField						m_txtName;
		CCtrlTextField						m_txtPath;
		CCtrlEdit							m_editName;
		CCtrlTextField						m_txtTips;
		CCtrlButton							m_btnSel;
		CCtrlTextField						m_txtPathName;
		CCtrlImageField						m_imgBg;


		// TODO: define normal private property here
		DWORD								m_Gid;
		CPopTips*							m_pPopTips;
		CString								m_strPath;
	};
}
