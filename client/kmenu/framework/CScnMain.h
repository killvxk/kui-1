#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"
#include "IdcMenuXml.h"
#include <vector>

namespace OpenHMI
{
	class CScnMain 
		: public CPrjBaseScreen
		//, public CAdapterAnimationListener
		, IViewTable
		, IViewList_Simple
		, ITimerListener
	{
	public:// TODO: define const here
		
	private:// TODO: define const here
		const INT  DEFAUT_LEFT_CLASS_ID = 201;
		const INT  DEFAUT_SUB_CLASS_ID = 2001;

		struct subClass : public Object
		{
			INT				id;
			CCtrlButton*	pBtn;
			subClass(CCtrlButton* p, INT i) { pBtn = p; id = i; }
			~subClass() {}
		};


	public:// TODO: define embed class or struct or enum here

	private:// TODO: define embed class or struct or enum here
		enum EResItem
		{
			RES_ITM_UNKNOWN				= 0,

			// TODO: define control ID here
			RES_ITM_IMG_BG								 = 31 ,
			RES_ITM_WEB									 = 52 ,
			RES_ITM_IMG_FRAME							 = 92 ,
			RES_ITM_BTN_ITEM							 = 2 ,
			RES_ITM_IMG_LEFT_BG							 = 8 ,
			RES_ITM_IMG_LINE							 = 97 ,
			RES_ITM_TAB_GAME							 = 1 ,
			RES_ITM_IMG_FOOT_BG							 = 11 ,
			RES_ITM_BTN_LOGIN							 = 24 ,
			RES_ITM_BTN_SKIN							 = 25 ,
			RES_ITM_BTN_MIN								 = 26 ,
			RES_ITM_BTN_QUIT							 = 28 ,
			RES_ITM_TXT_VERSION							 = 29 ,
			RES_ITM_SCROLLBAR_TABLE						 = 33 ,
			RES_ITM_SEARCH_BG							 = 43 ,
			RES_ITM_SEARCH_BOX							 = 45 ,
			RES_ITM_IMG_YUN								 = 47 ,
			RES_ITM_OP_MENU								 = 50 ,
			RES_ITM_MENU								 = 53 ,
			RES_ITM_BTN_MENU							 = 54 ,
			RES_ITM_BTN_S1								 = 55 ,
			RES_ITM_BTN_S2								 = 56 ,
			RES_ITM_BTN_S3								 = 57 ,
			RES_ITM_BTN_S4								 = 58 ,
			RES_ITM_BTN_S5								 = 59 ,
			RES_ITM_BTN_S6								 = 60 ,
			RES_ITM_BTN_S7								 = 61 ,
			RES_ITM_BTN_S8								 = 62 ,
			RES_ITM_FUNC_AD								 = 65 ,
			RES_ITM_BTN_PINYIN							 = 71 ,
			RES_ITM_BTN_UDISK							 = 72 ,
			RES_ITM_BTN_LOCK							 = 73 ,
			RES_ITM_BTN_VOLUME							 = 74 ,
			RES_ITM_BTN_MOUSE							 = 75 ,
			RES_ITM_HOT_SEARCH							 = 84 ,
			RES_ITM_TIPS								 = 88 ,
			RES_ITM_BTN_AD								 = 89 ,
			RES_ITM_IMG_AD_MASK							 = 90 ,
			RES_ITM_BTN_SLOGAN							 = 91 ,
			RES_ITM_IMG_SEARCH							 = 93 ,
			RES_ITM_IMG_LOGO							 = 94 ,
			RES_ITM_TXT_RCM								 = 95 ,
			RES_ITM_BTN_CHANGE							 = 96 ,
			RES_ITM_USER								 = 99 ,
			RES_ITM_NO_RESULT							 = 100 ,
			RES_ITM_AD_REC								 = 101 ,

		};

	public:// method
		CScnMain();
		virtual ~CScnMain();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();
		virtual BOOLEAN	onHardkey(UINT uiKeyCode, INT iKeyStatus);

		virtual void	show();
		virtual void	hide();

		virtual	void	DestroyUserDlg();
		virtual void	UserDlgLoginSuccess(const CString& id, const CString& name, const CString& url, const CString& fcm);
		virtual void	SetUserDlgAttr(const CString& title, int width, int height);
		virtual void	OpenUserDlgWin(const CString& url, const CString& title, int with, int height);
		virtual void	PopToMenu(const CString& url);
		virtual void	UserDlgLoginOut(const CString& id);

		// Declare functions for interface IViewTable
		virtual CComponent* getItemComponent(CCtrlTable* pTable, Object* pObjItem);

		virtual void drawItem(CCtrlTable* pTable, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus,
								BOOLEAN bDisable);


		// Declare functions for interface IViewList_Simple
		virtual CComponent* getItemComponent(CCtrlList_Simple* pList);

		virtual void drawItem(CCtrlList_Simple* pList, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus);

		// TODO: define your public method here
		//virtual	void	onAnimationValueChanged(CBaseAnimation* pSource);
		//virtual	void	onAnimationEnded(CBaseAnimation* pSource);

		virtual	void	onTimer(const CTimer *pTimer, UINT uiCurrentCount);
		virtual	void	onTimerComplete(const CTimer *pTimer);

	protected:// method		

	private:// method

		// TODO: define control event listener here
		void	onBtnLoginTchRel(CComponent *pTarget);
		void	onBtnLoginHoverFocused(CComponent *pTarget);
		void	onBtnUserTchRel(CComponent *pTarget);
		void	onBtnSkinTchPre(CComponent *pTarget);
		void	onBtnMinTchPre(CComponent *pTarget);
		void	onBtnMaxTchPre(CComponent *pTarget);
		void	onBtnQuitTchPre(CComponent *pTarget);
		void	onBtnSearchTchRel(CComponent *pTarget);
		void	onBtnRestoreTchPre(CComponent *pTarget);
		void	onBtnS1TchRel(CComponent *pTarget);
		void	onBtnS2TchRel(CComponent *pTarget);
		void	onBtnS3TchRel(CComponent *pTarget);
		void	onBtnS4TchRel(CComponent *pTarget);
		void	onBtnS5TchRel(CComponent *pTarget);
		void	onBtnS6TchRel(CComponent *pTarget);
		void	onBtnS7TchRel(CComponent *pTarget);
		void	onBtnS8TchRel(CComponent *pTarget);
		void	onBtnRegTchRel(CComponent *pTarget);
		void	onBtnTableGameItemHoverFocused(CComponent *pTarget);			
		void	onBtnTableGameItemHoverCanceled(CComponent *pTarget);
		void	onBtnTableGameWheelStart(CComponent *pTarget);
		void	onBtnTabGameItemTchPre(CComponent *pTarget);
		void	onBtnTabGameItemTchPreR(CComponent *pTarget);
		void	onBtnTableGameItemTchRel(CComponent *pTarget);
		void	onBtnListItemHoverFocused(CComponent *pTarget);
		void	onBtnListItemHoverCanceled(CComponent *pTarget);
		void onBtnListItemSelectLost(CComponent *pTarget);
		void	onBtnListItemSelectChanged(CComponent *pTarget);
		void	onBtnListMenuItemTchRel(CComponent *pTarget);
		void	onBtnChangeHoverFocused(CComponent *pTarget);
		void	onBtnChangeHoverCanceled(CComponent *pTarget);
		void	onBtnChangeTouchRel(CComponent *pTarget);
		void	onBtnPinyinTchRel(CComponent *pTarget);
		void	onBtnUdiskTchRel(CComponent *pTarget);
		void	onBtnLockTchRel(CComponent *pTarget);
		void	onBtnVolumeTchRel(CComponent *pTarget);
		void	onBtnMouseTchRel(CComponent *pTarget);
		void	onChangeSkinDsy(CBasePopup *pTarget);
		void	onUdiskLoginDsy(CBasePopup *pTarget);
		void	onUdiskDsy(CBasePopup *pTarget);
		void	onAdminDsy(CBasePopup *pTarget);
		void	onUploadCfgDsy(CBasePopup *pTarget);
		void	onSaveFileDsy(CBasePopup *pTarget);
		void	onReadFileDsy(CBasePopup *pTarget);

		void	onBtnMouseHoverFocused(CComponent *pTarget);
		void	onBtnVolumeHoverFocused(CComponent *pTarget);
		void	onBtnUdiskHoverFocused(CComponent *pTarget);
		void	onBtnLockHoverFocused(CComponent *pTarget);
		void	onBtnPinyinHoverFocused(CComponent *pTarget);
		void	onBtnSkinHoverFocused(CComponent *pTarget);
		void	onBtnMinHoverFocused(CComponent *pTarget);
		void	onBtnMaxHoverFocused(CComponent *pTarget);
		void	onBtnQuitHoverFocused(CComponent *pTarget);
		void	onBtnRestoreHoverFocused(CComponent *pTarget);
		void	onBtnHoverCanceled(CComponent *pTarget);

		void	onBtnAdHoverFocused(CComponent *pTarget);
		void	onBtnAdHoverCanceled(CComponent *pTarget);
		void	onBtnAdTchRel(CComponent *pTarget);

		void	onBtnSloganTchRel(CComponent *pTarget);
		// TODO: define logic event listener here
		void	onEditSetFocus();
		void	onEditKillFocus();
		void	onEditUpdate();
		void	onCaptionDblClk();
		void	onSetFocus();
		void	onKillFocus();
		void	onDispalyChange();
		void	onDropFiles();
		void	onUpdateData();
		void	onUpdateAllData();
		void	onUpdateWallPaper();
		void	onLeaveUser();
		void	onClickCategroyList();
		void	onRunGame();
		void	onCmdLine();
		void	onRefreshData();
		void	onUploadCfg();
		void	onLoginPopAdminOK();
		void	onLoginUdiskOK();

		void	onSaveFile();
		void	onReadFile();
		void	onLeaveFile();

		// TODO: define your normal private method here
		void	showGame(int id, std::vector<int>& arGid, int order, int maxcount);

		void	onSubClass(INT pos, CComponent *pTarget);

		void	checkGotoDefautLeftClass();

		bool	RunGame(DWORD gid, std::string pos, CString url = _T(""), int type = 0);
		void	OnClickCategroyList(int id, int pos = 0, CString url = _T(""), bool bFromDesk = false);
		void	OnClickLock(bool bFromDesk);
		void	OnClickNetdisk(bool bFromDesk);
		void	onKeyTab();

		void	onDestroyUserDlg();
		void	onUserDlgLoginSuccess();
		void	onSetUserDlgAttr();
		void	onOpenUserDlgWin();
		void	onUserDlgLoginOut();
		void	onUpdateAd();

	protected:// property

	private:// property

		// TODO: define control member here
		CCtrlImageField						m_imgBg;
		CCtrlWebBrowser						m_webBrsWeb;
		CCtrlImageField						m_imgFrame;
		CCtrlImageField						m_imgLeftBg;
		CCtrlImageField						m_imgLine;
		CCtrlTable							m_tableTabGame;
		CCtrlImageField						m_imgFootBg;
		CCtrlButton							m_btnLogin;
		CCtrlButton							m_btnSkin;
		CCtrlButton							m_btnMin;
		CCtrlButton							m_btnQuit;
		CCtrlTextField						m_txtVersion;
		CCtrlScrollbar						m_scrollbarTable;
		CCtrlImageField						m_imgSearchBg;
		CCtrlEdit							m_editSearchBox;
		CCtrlImageField						m_imgYun;
		CFuncWgtOpMenu						m_funcWgtOpMenu;
		CCtrlList_Simple					m_listMenu;
		CCtrlButton							m_btnS1;
		CCtrlButton							m_btnS2;
		CCtrlButton							m_btnS3;
		CCtrlButton							m_btnS4;
		CCtrlButton							m_btnS5;
		CCtrlButton							m_btnS6;
		CCtrlButton							m_btnS7;
		CCtrlButton							m_btnS8;
		CFuncWgtAd							m_funcWgtFuncAd;
		CCtrlButton							m_btnPinyin;
		CCtrlButton							m_btnUdisk;
		CCtrlButton							m_btnLock;
		CCtrlButton							m_btnVolume;
		CCtrlButton							m_btnMouse;
		CFuncWgtHotSearch					m_funcWgtHotSearch;
		CCtrlLabel							m_labTips;
		CCtrlButton							m_btnAd;
		CCtrlImageField						m_imgAdMask;
		CCtrlButton							m_btnSlogan;
		CCtrlImageField						m_imgSearch;
		CCtrlImageField						m_imgLogo;
		CCtrlTextField						m_txtRcm;
		CCtrlButton							m_btnChange;
		CFuncWgtPopUser						m_funcWgtUser;
		CFuncWgtNoResult					m_funcWgtNoResult;
		CFuncWgtAdRec						m_funcWgtAdRec;


		// TODO: define normal private property here
#if 0
		CValueAnimation						m_amtImgOpacity1;
		CValueAnimation						m_amtImgOpacity2;
		CValueAnimation						m_amtMenuImgOpacity1;
		CValueAnimation						m_amtMenuImgOpacity2;

		CValueAnimation						m_amtBgOpacity1;
		CValueAnimation						m_amtBgOpacity2;
		CValueAnimation						m_amtMidBar;

		CAnimationPlayer					m_amtPlayer1;
		CAnimationPlayer					m_amtPlayer2;
		
		CAnimationPlayer					m_amtPlayer4;
		CAnimationPlayer					m_amtPlayer5;
#endif
		Array<Object>						m_aryData;
		Array2<subClass*>					m_arrySc;

		std::vector<hotgame>				m_games;
		std::vector<tagGameInfo*>			m_search_games;
		BOOLEAN								m_bSearch;
		BOOLEAN								m_bSearchInit;
		BOOLEAN								m_bSearchStart;
		CCtrlButton*						m_pBtnSearchResult;

		INT									m_iCurSubClass;

		CString								m_strUserId;
		CString								m_strfcm;
		CString								m_strUserName;
		CString								m_strTitle;
		CString								m_strURL;
		int									m_iUserWidth;
		int									m_iUserHeight;

		CString								m_strUDiskName;
		CString								m_strUDiskPassWord;

		BOOLEAN								m_bOnline;
		HICON								m_hIcon;
		CPopChangeSkin*							m_pPopChangeSkin;
		CPopUdiskLogin*							m_pPopUdiskLogin;
		CPopUdisk*								m_pPopUdisk;
		CPopAdmin*							m_pPopAdmin;
		CPopUploadcfg*						m_pPopUploadCfg;
		CPopSaveFile*						m_pPopSaveFile;
		CPopReadFile*						m_pPopReadFile;
		BOOLEAN								m_bReadFile;

		CString								m_strSearch;

		CString								m_strFhpos;

		border								m_borderItem;
		slogan								m_sloganItem;

		CTimer								m_Timer;
		BOOLEAN                        m_bAdBar;
	};
}