#include "HMI.h"
#include "./framework/misc/ZipFunction.h"
#include "localconfig/localconfigfun.h"
#include "framework/ygt.hpp"
#include "winhttp\win_http.hpp"
#include "json\json.h"
#include "resource.h"

extern HANDLE	m_hFileMap;
typedef struct tagFileMap
{
	HWND hWnd;
	TCHAR szCmdLine[1024];
}tagFileMap;

namespace OpenHMI
{
	CScnMain::CScnMain()
		: m_bSearch(FALSE)
		, m_bSearchInit(FALSE)
		, m_bSearchStart(FALSE)
		, m_pBtnSearchResult(NULL)
		, m_iCurSubClass(-1)
		, m_bOnline(TRUE)
		, m_hIcon(NULL)
		, m_pPopChangeSkin(NULL)
		, m_pPopUdiskLogin(NULL)
		, m_pPopUdisk(NULL)
		, m_pPopAdmin(NULL)
		, m_pPopUploadCfg(NULL)
		, m_pPopSaveFile(NULL)
		, m_pPopReadFile(NULL)
		, m_bReadFile(FALSE)
		, m_strFhpos()
		, m_bAdBar(FALSE)
	{
		// TODO: setup parent-child relationship with children controls here

		addComponent(&m_imgBg);
		addComponent(&m_webBrsWeb);
		addComponent(&m_imgLeftBg);
		addComponent(&m_imgLine);
		addComponent(&m_tableTabGame);
		addComponent(&m_imgFootBg);
		
		long l = kcore::GetLocalLongFun(L"ENABLE_CLIENTLOGIN", 1);
		if (l == 1)
		{
			addComponent(&m_btnLogin);
		}

		addComponent(&m_btnSkin);
		addComponent(&m_btnMin);

		addComponent(&m_btnQuit);
		addComponent(&m_txtVersion);
		addComponent(&m_scrollbarTable);
		addComponent(&m_imgSearchBg);
		addComponent(&m_editSearchBox);
		addComponent(&m_imgYun);
		addComponent(&m_funcWgtOpMenu);
		addComponent(&m_listMenu);
		addComponent(&m_btnS1);
		addComponent(&m_btnS2);
		addComponent(&m_btnS3);
		addComponent(&m_btnS4);
		addComponent(&m_btnS5);
		addComponent(&m_btnS6);
		addComponent(&m_btnS7);
		addComponent(&m_btnS8);
		addComponent(&m_funcWgtFuncAd);		
		addComponent(&m_btnPinyin);
		addComponent(&m_btnUdisk);
		addComponent(&m_btnLock);
		addComponent(&m_btnVolume);
		addComponent(&m_btnMouse);
		addComponent(&m_funcWgtHotSearch);
		addComponent(&m_labTips);
		addComponent(&m_btnAd);
		addComponent(&m_imgAdMask);
		addComponent(&m_btnSlogan);
		addComponent(&m_imgSearch);
		addComponent(&m_imgLogo);
		addComponent(&m_txtRcm);
		addComponent(&m_btnChange);
		addComponent(&m_funcWgtUser);
		addComponent(&m_funcWgtNoResult);
		addComponent(&m_funcWgtAdRec);
		addComponent(&m_imgFrame);

		// TODO: add event listener to children controls here
		m_tableTabGame.addEventListener(this);
		m_btnLogin.addEventListener(this);
		m_btnSkin.addEventListener(this);
		m_btnMin.addEventListener(this);
		m_btnQuit.addEventListener(this);
		m_scrollbarTable.addEventListener(this);
		m_listMenu.addEventListener(this);
		m_btnS1.addEventListener(this);
		m_btnS2.addEventListener(this);
		m_btnS3.addEventListener(this);
		m_btnS4.addEventListener(this);
		m_btnS5.addEventListener(this);
		m_btnS6.addEventListener(this);
		m_btnS7.addEventListener(this);
		m_btnS8.addEventListener(this);
		m_btnPinyin.addEventListener(this);
		m_btnUdisk.addEventListener(this);
		m_btnLock.addEventListener(this);
		m_btnVolume.addEventListener(this);
		m_btnMouse.addEventListener(this);
		m_btnAd.addEventListener(this);
		m_btnSlogan.addEventListener(this);
		m_btnChange.addEventListener(this);


		// TODO: initialize children controls specific properties here
		m_tableTabGame.setView(this);
		m_tableTabGame.setAssociateScrollbar(&m_scrollbarTable, FALSE);
		m_scrollbarTable.setTargetComponent(&m_tableTabGame);

		m_listMenu.setView(this);
	}

	CScnMain::~CScnMain()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CScnMain)
		CONTROL_EVENT_HANDLER(&m_btnLogin, CEventButton::TOUCH_RELEASED, onBtnLoginTchRel)
		CONTROL_EVENT_HANDLER(&m_btnLogin, CEventButton::HOVER_FOCUSED, onBtnLoginHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnLogin, CEventButton::HOVER_CANCELED, onBtnHoverCanceled)

		CONTROL_EVENT_HANDLER(&m_btnSkin, CEventButton::TOUCH_RELEASED, onBtnSkinTchPre)
		CONTROL_EVENT_HANDLER(&m_btnSkin, CEventButton::HOVER_FOCUSED, onBtnSkinHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnSkin, CEventButton::HOVER_CANCELED, onBtnHoverCanceled)

		CONTROL_EVENT_HANDLER(&m_btnMin, CEventButton::TOUCH_RELEASED, onBtnMinTchPre)
		CONTROL_EVENT_HANDLER(&m_btnMin, CEventButton::HOVER_FOCUSED, onBtnMinHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnMin, CEventButton::HOVER_CANCELED, onBtnHoverCanceled)


		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::TOUCH_PRESSED, onBtnQuitTchPre)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::HOVER_FOCUSED, onBtnQuitHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::HOVER_CANCELED, onBtnHoverCanceled)		

		CONTROL_EVENT_HANDLER(&m_btnS1, CEventButton::TOUCH_RELEASED, onBtnS1TchRel)
		CONTROL_EVENT_HANDLER(&m_btnS2, CEventButton::TOUCH_RELEASED, onBtnS2TchRel)
		CONTROL_EVENT_HANDLER(&m_btnS3, CEventButton::TOUCH_RELEASED, onBtnS3TchRel)
		CONTROL_EVENT_HANDLER(&m_btnS4, CEventButton::TOUCH_RELEASED, onBtnS4TchRel)
		CONTROL_EVENT_HANDLER(&m_btnS5, CEventButton::TOUCH_RELEASED, onBtnS5TchRel)
		CONTROL_EVENT_HANDLER(&m_btnS6, CEventButton::TOUCH_RELEASED, onBtnS6TchRel)
		CONTROL_EVENT_HANDLER(&m_btnS7, CEventButton::TOUCH_RELEASED, onBtnS7TchRel)
		CONTROL_EVENT_HANDLER(&m_btnS8, CEventButton::TOUCH_RELEASED, onBtnS8TchRel)
		CONTROL_EVENT_HANDLER(&m_btnPinyin, CEventButton::TOUCH_RELEASED, onBtnPinyinTchRel)
		CONTROL_EVENT_HANDLER(&m_btnPinyin, CEventButton::HOVER_FOCUSED, onBtnPinyinHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnPinyin, CEventButton::HOVER_CANCELED, onBtnHoverCanceled)

		CONTROL_EVENT_HANDLER(&m_btnUdisk, CEventButton::TOUCH_RELEASED, onBtnUdiskTchRel)
		CONTROL_EVENT_HANDLER(&m_btnUdisk, CEventButton::HOVER_FOCUSED, onBtnUdiskHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnUdisk, CEventButton::HOVER_CANCELED, onBtnHoverCanceled)
		CONTROL_EVENT_HANDLER(&m_btnLock, CEventButton::TOUCH_RELEASED, onBtnLockTchRel)
		CONTROL_EVENT_HANDLER(&m_btnLock, CEventButton::HOVER_FOCUSED, onBtnLockHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnLock, CEventButton::HOVER_CANCELED, onBtnHoverCanceled)
		CONTROL_EVENT_HANDLER(&m_btnVolume, CEventButton::TOUCH_RELEASED, onBtnVolumeTchRel)
		CONTROL_EVENT_HANDLER(&m_btnVolume, CEventButton::HOVER_FOCUSED, onBtnVolumeHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnVolume, CEventButton::HOVER_CANCELED, onBtnHoverCanceled)
		CONTROL_EVENT_HANDLER(&m_btnMouse, CEventButton::TOUCH_RELEASED, onBtnMouseTchRel)
		CONTROL_EVENT_HANDLER(&m_btnMouse, CEventButton::HOVER_FOCUSED, onBtnMouseHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnMouse, CEventButton::HOVER_CANCELED, onBtnHoverCanceled)

		CONTROL_EVENT_HANDLER(&m_btnAd, CEventButton::HOVER_FOCUSED, onBtnAdHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnAd, CEventButton::HOVER_CANCELED, onBtnAdHoverCanceled)
		CONTROL_EVENT_HANDLER(&m_btnAd, CEventButton::TOUCH_RELEASED, onBtnAdTchRel)
		CONTROL_EVENT_HANDLER(&m_btnSlogan, CEventButton::TOUCH_RELEASED, onBtnSloganTchRel)

		CONTROL_EVENT_HANDLER(&m_tableTabGame, CEventTable::ITEM_HOVER_FOCUSED, onBtnTableGameItemHoverFocused)
		CONTROL_EVENT_HANDLER(&m_tableTabGame, CEventTable::ITEM_HOVER_CANCELED, onBtnTableGameItemHoverCanceled)
		CONTROL_EVENT_HANDLER(&m_tableTabGame, CEventTable::HOVER_CANCELED, onBtnTableGameItemHoverCanceled)
		CONTROL_EVENT_HANDLER(&m_tableTabGame, CEventTable::WHEEL_START, onBtnTableGameWheelStart)

		CONTROL_EVENT_HANDLER_CHAIN(&m_tableTabGame, CEventButton::TOUCH_PRESSED, onBtnTabGameItemTchPre)	
		CONTROL_EVENT_HANDLER_CHAIN(&m_tableTabGame, CEventButton::TOUCH_PRESSED_R, onBtnTabGameItemTchPreR)
		CONTROL_EVENT_HANDLER_CHAIN(&m_tableTabGame, CEventButton::TOUCH_RELEASED, onBtnTableGameItemTchRel)

		CONTROL_EVENT_HANDLER(&m_listMenu, CEventList::ITEM_HOVER_FOCUSED, onBtnListItemHoverFocused)
		CONTROL_EVENT_HANDLER(&m_listMenu, CEventList::ITEM_HOVER_CANCELED, onBtnListItemHoverCanceled)
		CONTROL_EVENT_HANDLER(&m_listMenu, CEventList::HOVER_CANCELED, onBtnListItemHoverCanceled)
		CONTROL_EVENT_HANDLER(&m_listMenu, CEventList::CCP_FOCUS_CHANGED, onBtnListItemSelectLost)
		CONTROL_EVENT_HANDLER(&m_listMenu, CEventList::SELECT_CHANGED, onBtnListItemSelectChanged)

		CONTROL_EVENT_HANDLER(&m_btnChange, CEventButton::HOVER_FOCUSED, onBtnChangeHoverFocused)
		CONTROL_EVENT_HANDLER(&m_btnChange, CEventButton::HOVER_CANCELED, onBtnChangeHoverCanceled)
		CONTROL_EVENT_HANDLER(&m_btnChange, CEventButton::TOUCH_PRESSED, onBtnChangeTouchRel)

		CONTROL_EVENT_HANDLER_CHAIN(&m_listMenu, CEventButton::TOUCH_RELEASED, onBtnListMenuItemTchRel)

		POPUP_EVENT_HANDLER(m_pPopChangeSkin, CEventPopup::DESTROYED, onChangeSkinDsy)
		POPUP_EVENT_HANDLER(m_pPopUdiskLogin, CEventPopup::DESTROYED, onUdiskLoginDsy)
		POPUP_EVENT_HANDLER(m_pPopUdisk, CEventPopup::DESTROYED, onUdiskDsy)
		POPUP_EVENT_HANDLER(m_pPopAdmin, CEventPopup::DESTROYED, onAdminDsy)
		POPUP_EVENT_HANDLER(m_pPopUploadCfg, CEventPopup::DESTROYED, onUploadCfgDsy)
		POPUP_EVENT_HANDLER(m_pPopSaveFile, CEventPopup::DESTROYED, onSaveFileDsy)
		POPUP_EVENT_HANDLER(m_pPopReadFile, CEventPopup::DESTROYED, onReadFileDsy)

		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::UPDATE_WALL_PAPER, onUpdateWallPaper)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LEAVE_USER, onLeaveUser)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::CLICK_CATEGROY_LIST, onClickCategroyList)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::RUN_GAME, onRunGame)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::REFRESH_DATA, onRefreshData)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::UPLOAD_CFG, onUploadCfg)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LOGIN_POP_ADMIN_OK, onLoginPopAdminOK)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LOGIN_UDISK_OK, onLoginUdiskOK)

		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::SAVE_FILE, onSaveFile)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::READ_FILE, onReadFile)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LEAVE_FILE, onLeaveFile)

		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER, onCmdLine)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::EDIT_SETFOCUS, onEditSetFocus)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::EDIT_KILLFOCUS, onEditKillFocus)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::EDIT_UPDATE, onEditUpdate)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::SETFOCUS, onSetFocus)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KILLFOCUS, onKillFocus)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::DROPFILES, onDropFiles)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER+1, onUpdateData)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER + 2, onUpdateAllData)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER + 6, onDestroyUserDlg)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER + 7, onUserDlgLoginSuccess)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER + 8, onSetUserDlgAttr)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER + 9, onOpenUserDlgWin)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER + 10, onUserDlgLoginOut)

	END_EVENT_MAP(CPrjBaseScreen)

	// TODO: finish command map here
	BEGIN_CMD_MAP(CScnMain)
	END_CMD_MAP(CPrjBaseScreen)

	BOOLEAN CScnMain::initialize()
	{
		CPrjBaseScreen::initialize();

		// TODO: add your spec initialize code here
		// normally add logic event listener.
		m_funcWgtOpMenu.initialize();
		m_funcWgtFuncAd.initialize();
		m_funcWgtHotSearch.initialize();
		m_funcWgtUser.initialize();
		m_funcWgtNoResult.initialize();
		m_funcWgtAdRec.initialize();

		CLogicManager::getInstance()->addEventListener(this);
		CScreenTouchServer::getInstance()->addEventListener(this);

		m_listMenu.setModel(&m_aryData);
		m_Timer.setListener(this);
		m_Timer.setDelay(1000);
		m_Timer.setRepeatCount(1);
		return TRUE;
	}

	void CScnMain::unInitialize()
	{
		CPrjBaseScreen::unInitialize();

		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_funcWgtOpMenu.unInitialize();
		m_funcWgtFuncAd.unInitialize();
		m_funcWgtHotSearch.unInitialize();
		m_funcWgtUser.unInitialize();
		m_funcWgtNoResult.unInitialize();
		m_funcWgtAdRec.unInitialize();

		CScreenTouchServer::getInstance()->removeEventListener(this);
		CLogicManager::getInstance()->removeEventListener(this);
		m_listMenu.setModel(NULL);
		m_aryData.clear();
		m_arrySc.clear();
		m_Timer.stop();
		m_Timer.setListener(NULL);
	}

	void CScnMain::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info here
		INIT::initScreen(this, RES_SCN_MAIN);
		INIT::initImageField(m_imgBg, RES_ITM_IMG_BG, this);
		INIT::initWebBrowser(m_webBrsWeb, RES_ITM_WEB, this);
		INIT::initImageField(m_imgFrame, RES_ITM_IMG_FRAME, this);
		INIT::initImageField(m_imgLeftBg, RES_ITM_IMG_LEFT_BG, this);
		INIT::initImageField(m_imgLine, RES_ITM_IMG_LINE, this);
		INIT::initTable(m_tableTabGame, RES_ITM_TAB_GAME, this);
		INIT::initImageField(m_imgFootBg, RES_ITM_IMG_FOOT_BG, this);
		INIT::initButton(m_btnLogin, RES_ITM_BTN_LOGIN, this);
		INIT::initButton(m_btnSkin, RES_ITM_BTN_SKIN, this);
		INIT::initButton(m_btnMin, RES_ITM_BTN_MIN, this);
		INIT::initButton(m_btnQuit, RES_ITM_BTN_QUIT, this);
		INIT::initTextField(m_txtVersion, RES_ITM_TXT_VERSION, this);
		INIT::initScrollbar(m_scrollbarTable, RES_ITM_SCROLLBAR_TABLE, this);
		INIT::initImageField(m_imgSearchBg, RES_ITM_SEARCH_BG, this);
		INIT::initEdit(m_editSearchBox, RES_ITM_SEARCH_BOX, this);
		INIT::initImageField(m_imgYun, RES_ITM_IMG_YUN, this);
		INIT::initFuncWidget(m_funcWgtOpMenu, RES_ITM_OP_MENU, this);
		INIT::initList(m_listMenu, RES_ITM_MENU, this);
		INIT::initButton(m_btnS1, RES_ITM_BTN_S1, this);
		INIT::initButton(m_btnS2, RES_ITM_BTN_S2, this);
		INIT::initButton(m_btnS3, RES_ITM_BTN_S3, this);
		INIT::initButton(m_btnS4, RES_ITM_BTN_S4, this);
		INIT::initButton(m_btnS5, RES_ITM_BTN_S5, this);
		INIT::initButton(m_btnS6, RES_ITM_BTN_S6, this);
		INIT::initButton(m_btnS7, RES_ITM_BTN_S7, this);
		INIT::initButton(m_btnS8, RES_ITM_BTN_S8, this);
		INIT::initFuncWidget(m_funcWgtFuncAd, RES_ITM_FUNC_AD, this);
		INIT::initButton(m_btnPinyin, RES_ITM_BTN_PINYIN, this);
		INIT::initButton(m_btnUdisk, RES_ITM_BTN_UDISK, this);
		INIT::initButton(m_btnLock, RES_ITM_BTN_LOCK, this);
		INIT::initButton(m_btnVolume, RES_ITM_BTN_VOLUME, this);
		INIT::initButton(m_btnMouse, RES_ITM_BTN_MOUSE, this);
		INIT::initFuncWidget(m_funcWgtHotSearch, RES_ITM_HOT_SEARCH, this);
		INIT::initLabel(m_labTips, RES_ITM_TIPS, this);
		INIT::initButton(m_btnAd, RES_ITM_BTN_AD, this);
		INIT::initImageField(m_imgAdMask, RES_ITM_IMG_AD_MASK, this);
		INIT::initButton(m_btnSlogan, RES_ITM_BTN_SLOGAN, this);
		INIT::initImageField(m_imgSearch, RES_ITM_IMG_SEARCH, this);
		INIT::initImageField(m_imgLogo, RES_ITM_IMG_LOGO, this);
		INIT::initTextField(m_txtRcm, RES_ITM_TXT_RCM, this);
		INIT::initButton(m_btnChange, RES_ITM_BTN_CHANGE, this);
		INIT::initFuncWidget(m_funcWgtUser, RES_ITM_USER, this);
		INIT::initFuncWidget(m_funcWgtNoResult, RES_ITM_NO_RESULT, this);
		INIT::initFuncWidget(m_funcWgtAdRec, RES_ITM_AD_REC, this);
		// TODO: set data to children controls or set children controls status here

		m_btnAd.setVisible(FALSE);
		m_funcWgtAdRec.setVisible(FALSE);
		m_imgAdMask.setVisible(FALSE);
		m_funcWgtNoResult.setVisible(FALSE);
		

		if (CLogicManager::getInstance()->getMenuXml().GetBorderItem(m_borderItem) == TRUE)
		{
			CMovieClip* pAd = new CMovieClip();

			String strBorderPath = L"config\\res\\";
			strBorderPath = strBorderPath + m_borderItem.fid.GetBuffer();

			CZipAd::getInstance()->LoadZip((const CHAR*)strBorderPath);

			strBorderPath = strBorderPath.subString(0, strBorderPath.length() - 4) + L"\\";

			for (UINT uiIndex = 0; TRUE; ++uiIndex)
			{
				String strFile = String::format(L"%d.png", uiIndex + 1);

				if (CZipAd::getInstance()->FindFileFromZip((const CHAR*)strFile) == FALSE)
					break;

				CMovieClip::tagBitmapInfo info;

				info.strID = String::format(L"%s%d.png", (const CHAR*)strBorderPath, uiIndex + 1);
				info.pImage = CZipAd::getInstance()->LoadImageFromZip((const CHAR*)strFile);

				pAd->addImage(info);

				m_btnAd.setVisible(TRUE);
			}

			m_btnAd.getImageField(L"IMG_DH")->setUserMovieClip(pAd);
			m_btnAd.setHandCursor(TRUE);

			CMovieClip* pAdMask = new CMovieClip();
			CMovieClip::tagBitmapInfo info;

			info.strID = String::format(L"%s%d.png", (const CHAR*)strBorderPath, 0);
			info.pImage = CZipAd::getInstance()->LoadImageFromZip(L"0.png");

			pAdMask->addImage(info);
			m_imgAdMask.setUserMovieClip(pAdMask);			

			CZipAd::getInstance()->UnLoad();
		}

		m_btnSlogan.setVisible(FALSE);
		if (CLogicManager::getInstance()->getMenuXml().GetSloganItem(m_sloganItem) == TRUE)
		{
			String strFile = CPath::getApplicationFolder() + L"config\\res\\" + m_sloganItem.fid.GetBuffer();

			if(CFile::isExist((const CHAR*)strFile) == TRUE)
			{
				CMovieClip* pSlogan = new CMovieClip();
				pSlogan->addImage(strFile);
				m_btnSlogan.getImageField(L"IMG_DH")->setUserMovieClip(pSlogan);
				m_btnSlogan.setHandCursor(TRUE);
				m_btnSlogan.setVisible(TRUE);
			}
		}


		m_labTips.setVisible(FALSE);
		m_arrySc.clear();
		m_arrySc.addItem(new subClass(&m_btnS1, 0));
		m_arrySc.addItem(new subClass(&m_btnS2, 0));
		m_arrySc.addItem(new subClass(&m_btnS3, 0));
		m_arrySc.addItem(new subClass(&m_btnS4, 0));
		m_arrySc.addItem(new subClass(&m_btnS5, 0));
		m_arrySc.addItem(new subClass(&m_btnS6, 0));
		m_arrySc.addItem(new subClass(&m_btnS7, 0));
		m_arrySc.addItem(new subClass(&m_btnS8, 0));

		for (UINT i = 0; i < m_arrySc.size(); i++)
		{
			m_arrySc[i]->pBtn->setVisible(FALSE);
		}

		m_aryData.clear();

		vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();
		for (size_t i = 0; i < arLeft.size(); i++)
		{
			m_aryData.addItem(new CInt(i));
		}

		m_aryData.addItem(new CInt(arLeft.size()));

		m_listMenu.setReachToHead(TRUE);
		m_listMenu.setReachToTail(TRUE);

		m_editSearchBox.setLimitText(15);
		if (CLogicManager::getInstance()->getMenuXml().GetSearchWord().GetLength() > 0)
		{
			m_editSearchBox.setContent(CLogicManager::getInstance()->getMenuXml().GetSearchWord().GetBuffer());
		}
		else
		{
			m_editSearchBox.setContent(CLanguage::getString(RES_LAG_SEARCH_TIPS));
		}
		m_editSearchBox.setFont(15, RGB(156, 156, 156), L"Microsoft YaHei");
		m_editSearchBox.setFontBkClr(RGB(37, 37, 42));
		m_editSearchBox.setBkColor(RGB(37, 37, 42));
		m_funcWgtOpMenu.setVisible(FALSE);
		

		CMovieClip* pMovieClip = new CMovieClip();

		backgroud bk = CLogicManager::getInstance()->getMenuXml().GetBackgroud();

		if (bk.file.GetLength() > 0)
		{
			if (CFile::isExist(CPath::getApplicationFolder() + L"config\\res\\" + bk.file.GetBuffer()) == TRUE)
				pMovieClip->addImage(CPath::getApplicationFolder() + L"config\\res\\" + bk.file.GetBuffer());
			else
				pMovieClip->addImage(L"MenuSkin/default_skin.jpg");
		}
		else
		{
			CString strBg; int nType = 1;
			String strFile;

			if (AfxGetDbMgr()->GetSkinPng(strBg, nType))
			{
				if (nType == 1)
				{
					String strTmp = strBg.GetBuffer();
					strTmp = strTmp.subString(0, strTmp.length() - 4);

					int nIndex = SkinNameToType((const CHAR*)strTmp);
					strFile = String::format(L"%s%d.jpg", L"MenuSkin/Wallpaper/", nIndex + 1);
				}
				else
				{
					strFile = CPath::getApplicationFolder() + L"Skin\\Image\\" + strBg.GetBuffer();
				}

				if (!CZipSkin::getInstance()->FindFileFromZip((const CHAR*)strFile) && CFile::isExist(strFile) == FALSE)
				{
					strFile = L"MenuSkin/default_skin.jpg";
				}
			}
			else
			{
				strFile = L"MenuSkin/default_skin.jpg";
			}

			pMovieClip->addImage(strFile);
		}

		m_imgBg.setUserMovieClip(pMovieClip);
		m_funcWgtHotSearch.setVisible(FALSE);

		TCHAR szVersizon[MAX_PATH] = { 0 };
		AfxGetFileVersion(TEXT("knbmenu.exe"), szVersizon);
		m_txtVersion.setContent(String::format(L"%s %s", (const CHAR*)CLanguage::getString(RES_LAG_LOGO), szVersizon));
		m_funcWgtUser.setVisible(FALSE);
	
		m_btnLogin.setHandCursor(TRUE);
	}

	BOOLEAN CScnMain::onHardkey(UINT uiKeyCode, INT iKeyStatus)
	{
		// TODO: deal with each key code of this screen can identify and return true here

		return FALSE;
	}

	void CScnMain::show()
	{
		CPrjBaseScreen::show();		
		

		ygt_t y;

		std::string ygt = y.read();


		struct handler_t
		{
			void operator()(const std::string &msg)
			{
				throw std::runtime_error(msg);
			}
		};

		String strYgt;

		String::convertFromMBCS(ygt.c_str(), strYgt);

		String strUrl = String::format(L"https://cas.dobest.com/authen/caslogin.json?appId=474&areaId=0&statistics2=&ygt=%s", (const CHAR*)strYgt);

		http::win_http_t win_http((const CHAR*)strUrl, http::post, handler_t());
		win_http.timeout(3);

		std::map<std::wstring, std::wstring> header;
		header[L"Connection"] = L"close";
		header[L"Content-type"] = L"application/x-www-form-urlencoded";

		try
		{
			std::vector<char> data = win_http.send(header, "");
			std::string response_val(data.begin(), data.end());

			Json::Reader reader;
			Json::Value root;
			if (reader.parse(response_val, root, false))
			{
				int nResult = root["return_code"].asInt();
				if (nResult == 0)
				{
					Json::Value data = root["data"];

					std::string ticket = data["ticket"].asString();

					WriteLog(TEXT("get ticket success."));

					http::win_http_t win_http(L"http://menuapi.yungengxin.net.cn/api/yuka/validateSignature", http::post, handler_t());
					win_http.timeout(3);


					std::map<std::wstring, std::wstring> header;
					header[L"Connection"] = L"close";
					header[L"Content-type"] = L"application/x-www-form-urlencoded";

					std::string str = "ticket=";
					str += ticket;
					std::string params = (LPCSTR)ATL::CA2A(str.c_str(), CP_UTF8);

					try
					{
						std::vector<char> rdata = win_http.send(header, params);
						std::string strReponse(rdata.begin(), rdata.end());
						Json::Reader reader;
						Json::Value root;
						if (reader.parse(strReponse, root, false))
						{
							if (nResult == 0)
							{
								Json::Value data = root["data"];

								std::string id = data["sndaId"].asString();

								WriteLog(TEXT("get sndaId success."));

								m_btnLogin.setEnable(FALSE);
								m_btnLogin.refresh();
							}
						}
					}
					catch (std::runtime_error(msg))
					{
						WriteLog(TEXT("HTTP error."));
					}
				}
			}
		}
		catch (std::runtime_error(msg))
		{
			WriteLog(TEXT("HTTP error."));
		}
		HWND hWnd = CScreenTouchServer::getInstance()->getScreenContext()->hWnd;

		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		onBtnRestoreTchPre(NULL);
		WriteLog(TEXT("CScnMain::show."));
		m_Timer.start();		
	}

	void CScnMain::hide()
	{
		CPrjBaseScreen::hide();
	}

	void CScnMain::DestroyUserDlg()
	{
		CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 6);
		CMain::getInstance()->pushMessage(pMessage);
	}

	void CScnMain::onDestroyUserDlg()
	{
		HWND hWnd = CScreenTouchServer::getInstance()->getScreenContext()->hWnd;
		m_funcWgtUser.quit();
		::SetFocus(hWnd);
	}

	void CScnMain::UserDlgLoginSuccess(const CString& id, const CString& name, const CString& url, const CString& fcm)
	{
		m_strUserId = id;
		//m_strfcm = fcm;
		m_strUserName = name;
		m_strURL = url;


		std::string ygt = _bstr_t(fcm);

		ygt_t y;
		
		y.write(ygt);

		CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 7);
		CMain::getInstance()->pushMessage(pMessage);
	}

	void CScnMain::onUserDlgLoginSuccess()
	{
		m_btnLogin.setEnable(FALSE);
		m_btnLogin.refresh();
#if 0
		Gdiplus::Bitmap* pImage = NULL;
		if (DownloadSingleFile(m_strURL, pImage))
		{
			//m_btnLogin.setVisible(FALSE);
		

			CMovieClip* pMovieClip = new CMovieClip();
			CMovieClip::tagBitmapInfo info;

			int j = m_strURL.Find(L".jpg");
			String strUrl = m_strURL.GetString();

			info.strID = strUrl.subString(j - 3, 6);
			info.pImage = pImage;
			pMovieClip->addImage(info);
		}
#endif
		
	}

	void CScnMain::SetUserDlgAttr(const CString& title, int width, int height)
	{
		m_strTitle = title;
		m_iUserWidth = width;
		m_iUserHeight = height;

		CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 8);
		CMain::getInstance()->pushMessage(pMessage);
	}

	void CScnMain::onSetUserDlgAttr()
	{
		checkGotoDefautLeftClass();
		m_funcWgtUser.setVisible(TRUE);
		m_funcWgtUser.show(m_strTitle.GetString(), m_iUserWidth, m_iUserHeight);
	}

	void CScnMain::OpenUserDlgWin(const CString& url, const CString& title, int with, int height)
	{
		m_strURL = url;
		m_strTitle = title;
		m_iUserWidth = with;
		m_iUserHeight = height;

		CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 9);
		CMain::getInstance()->pushMessage(pMessage);		
	}

	void CScnMain::onOpenUserDlgWin()
	{
		checkGotoDefautLeftClass();
	}

	void CScnMain::PopToMenu(const CString& url)
	{

	}

	void CScnMain::UserDlgLoginOut(const CString& id)
	{
		CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 10);
		CMain::getInstance()->pushMessage(pMessage);
	}

	void CScnMain::onUserDlgLoginOut()
	{
		
	}

	void CScnMain::onUpdateAd()
	{
		m_funcWgtFuncAd.refresh();
	}

	// Definition functions for interface IViewTable
	CComponent* CScnMain::getItemComponent(CCtrlTable* pTable, Object* pObjItem)
	{
		if (pObjItem == NULL)
		{
			return NULL;
		}

		INT iData = *((CInt*)pObjItem);
		CCtrlButton* pButton = new CCtrlButton();
		
		INIT::initButton(*pButton, RES_ITM_BTN_ITEM, this);

		CMovieClip* pMovieClip = new CMovieClip();

		CMovieClip::tagBitmapInfo info;

		tagGameInfo* pGameInfo = NULL;
		customgame cust;
		CCtrlTextField* pText = pButton->getTextField(L"TXT");

		if (m_bSearch == TRUE)
		{
			pGameInfo = m_search_games[iData];
			cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGameInfo->gid);
		}
		else
		{
			hotgame hGame = m_games[iData];

			if (hGame.tag.GetLength() > 0)
			{
				CMovieClip* pTag = new CMovieClip();
				pTag->addImage(CPath::getApplicationFolder() + L"config\\res\\" + hGame.tag.GetBuffer());
				pButton->getImageField(L"IMG_TAG")->setUserMovieClip(pTag);
			}
			
			if (hGame.text.GetLength() > 0)
			{
				String strColor = L"0xFF";
				strColor = strColor + hGame.text.GetBuffer();

				int n;
				::StrToIntEx((const CHAR*)strColor, STIF_SUPPORT_HEX, &n);
				if (n >= 0xFF000000 && n <= 0xFFFFFFFF)
					pText->setFontColor(n);
			}

			if (hGame.effect.GetLength() > 0)
			{
				CMovieClip* pEffect = new CMovieClip();

				String strBorderPath = L"config\\res\\";
				strBorderPath = strBorderPath + hGame.effect.GetBuffer();

				CZipAd::getInstance()->LoadZip((const CHAR*)strBorderPath);

				strBorderPath = strBorderPath.subString(0, strBorderPath.length() - 4) + L"\\";

				for (UINT uiIndex = 0; TRUE; ++uiIndex)
				{
					String strFile = String::format(L"%d.png", uiIndex + 1);

					if (CZipAd::getInstance()->FindFileFromZip((const CHAR*)strFile) == FALSE)
						break;

					CMovieClip::tagBitmapInfo info;

					info.strID = String::format(L"%s%d.png", (const CHAR*)strBorderPath, uiIndex + 1);
					info.pImage = CZipAd::getInstance()->LoadImageFromZip((const CHAR*)strFile);

					pEffect->addImage(info);
				}
				
				pButton->getImageField(L"IMG_DH")->setUserMovieClip(pEffect);
				pButton->getImageField(L"IMG_DH")->play(40);
			}
			cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(hGame.gid);
			{
				pGameInfo = AfxGetDbMgr()->FindGameInfo(hGame.gid);
			}

		}

		if (pGameInfo != NULL && pGameInfo->gid <= 100000)
		{
			info.strID = String::format(L"%d", pGameInfo->gid);
			info.pImage = GetGameIcon(pGameInfo->gid, 48);

			if (info.pImage != NULL)
			{
				pMovieClip->addImage(info);

			}
			else
			{
				BYTE* buffer = NULL;
				UINT size = 0;
				info.strID = L"MenuSkin/default.ico";

				if (AfxGetDbMgr()->m_defaultImage)
				{
					info.pImage = AfxGetDbMgr()->m_defaultImage;
				}
				else
				{
					BYTE* buffer = NULL;
					UINT size = 0;
					if (CZipSkin::getInstance()->LoadFileFromZipEx(L"MenuSkin/default.ico", &buffer, &size))
					{
						AfxGetDbMgr()->m_defaultImage = ReadIcon(buffer, size, 48);
						if (AfxGetDbMgr()->m_defaultImage)
						{
								info.pImage = AfxGetDbMgr()->m_defaultImage;
						}
					}
				}
				pMovieClip->addImage(info);
			}
			String str = pGameInfo->gname;
			if (str.length() >= 14)
			{
				str = str.subString(0, 13);
			}
			if (str.length() >= 7)
			{
				String s = str.subString(0, 6);
				CString szNew((const CHAR*)s);
				szNew.TrimRight();

				String strNew = szNew.GetBuffer();
				
				pText->setContent(strNew + L"\n" + str.subString(6));
			}
			else
			{
				pText->setContent(str);
			}			
		}
		else
		{
			BYTE* buffer = NULL;
			UINT size = 0;
			
			if (CZipAd::getInstance()->LoadFileFromZipEx(cust.fid.GetBuffer(), &buffer, &size))
			{
				info.strID = cust.fid.GetBuffer();
				info.pImage = ReadIcon(buffer, size, 48);
			}
			else
			{
				info.strID = L"MenuSkin/default.ico";

				if (AfxGetDbMgr()->m_defaultImage)
				{
					info.pImage = AfxGetDbMgr()->m_defaultImage;
				}
				else
				{
					BYTE* buffer = NULL;
					UINT size = 0;
					if (CZipSkin::getInstance()->LoadFileFromZipEx(L"MenuSkin/default.ico", &buffer, &size))
					{
						AfxGetDbMgr()->m_defaultImage = ReadIcon(buffer, size, 48);
						if (AfxGetDbMgr()->m_defaultImage)
						{
							info.pImage = AfxGetDbMgr()->m_defaultImage;
						}
					}
				}

			}
			pMovieClip->addImage(info);

			String str = cust.name.GetBuffer();
			if (str.length() >= 14)
			{
				str = str.subString(0, 13);
			}
			if (str.length() >= 7)
			{
				String s = str.subString(0, 6);
				CString szNew((const CHAR*)s);
				szNew.TrimRight();

				String strNew = szNew.GetBuffer();

				pButton->setCaption(strNew + L"\n" + str.subString(6));
			}
			else
			{
				pButton->setCaption(str);
			}
		}

		pButton->getImageField()->setUserMovieClip(pMovieClip);

		pButton->addEventListener(this);
		
		pButton->refresh();
		return pButton;
	}

	void CScnMain::drawItem(CCtrlTable* pTable, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus,
								BOOLEAN bDisable)
	{
		return ;
	}


	// TODO: implement your public method here
#if 0
	void CScnMain::onAnimationValueChanged(CBaseAnimation* pSource)
	{
		if(pSource == &m_amtImgOpacity1 || pSource == &m_amtImgOpacity2)
		{
			CAmtInt* pAmtInt = static_cast<CAmtInt*>(pSource->getAnimationData());

			m_imgMask.setOpacity(pAmtInt->m_iValue);
		}
		else if (pSource == &m_amtMenuImgOpacity1 || pSource == &m_amtMenuImgOpacity2)
		{
			CAmtInt* pAmtInt = static_cast<CAmtInt*>(pSource->getAnimationData());

			for (ULONG i = 0; i < m_aryData.size(); i++)
			{

				CCtrlButton *pBtn = (CCtrlButton *)m_listMenu.getItemComponent(m_aryData[i]);
				CCtrlImageField *pImg = pBtn->getImageField();
				pImg->setOpacity(pAmtInt->m_iValue);

			}
		}
		else if (pSource == &m_amtMidBar)
		{
			CAmtInt* pAmtInt = static_cast<CAmtInt*>(pSource->getAnimationData());
			m_imgMidBar.setX(pAmtInt->m_iValue * m_imgMidBar.getPreferredSize().getX() / 10);
			m_imgMidBar1.setX(pAmtInt->m_iValue * m_imgMidBar.getPreferredSize().getX() / 10 - m_imgMidBar1.getPreferredSize().getX());
		}

		else if( pSource == &m_amtBgOpacity1 || pSource == &m_amtBgOpacity2)
		{
			CAmtInt* pAmtInt = static_cast<CAmtInt*>(pSource->getAnimationData());
			
			m_imgBg.setOpacity(pAmtInt->m_iValue);
		}
	}

	void CScnMain::onAnimationEnded(CBaseAnimation* pSource)
	{
		;
	}
#endif
	// Definition functions for interface IViewList_Simple
	CComponent* CScnMain::getItemComponent(CCtrlList_Simple* pList)
	{
		CComponent* pView = NULL;
		pView = new CCtrlButton();
		INIT::initButton(*((CCtrlButton*)pView), RES_ITM_BTN_MENU, this);
		pView->addEventListener(this);
		pView->refresh();

		return pView;
	}

	void CScnMain::drawItem(CCtrlList_Simple* pList, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus)
	{
		CInt* pData = (CInt*)pObjItem;

		vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();
		CCtrlButton *pBtn = (CCtrlButton*)pComponent;

		if (*pData >= (INT)arLeft.size())
		{
			TCHAR szTest[MAX_PATH] = { 0 };
			::LoadString(::GetModuleHandle(NULL), IDS_NB_NAME, szTest, MAX_PATH);

			pBtn->setCaption(szTest);
		}
		else
		{			
			leftclass c = arLeft[*pData];
			pBtn->setCaption(c.name.GetString());

			CMovieClip* pMovieClip = new CMovieClip();
			pMovieClip->addImage(CPath::getApplicationFolder() + L"config\\res\\" + c.ico1.GetBuffer());
			pMovieClip->addImage(CPath::getApplicationFolder() + L"config\\res\\" + c.ico2.GetBuffer());
			
			pBtn->getImageField(L"IMG")->setUserMovieClip(pMovieClip);

			if (bSelected || bFocus)
			{
				pBtn->getImageField(L"IMG")->showImage(2);
			}
		}
		return ;
	}
	// TODO: implement control event listener here
	void CScnMain::onBtnUserTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, POS_MENU, TOOL_HEAD);
		m_funcWgtUser.openUrl(_T("http://www.feihuo.com/yunuser"));
	}


	void CScnMain::onBtnSkinTchPre(CComponent *pTarget)
	{		
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, POS_MENU, TOOL_SKIN);
		checkGotoDefautLeftClass();
		
		
		if (m_pPopChangeSkin == NULL)
		{
			m_pPopChangeSkin = new CPopChangeSkin(FALSE);

			if (m_pPopChangeSkin->initialize())
			{
				m_pPopChangeSkin->addEventListener(this);
				m_pPopChangeSkin->doModal();
			}
			else
			{
				delete m_pPopChangeSkin;
				m_pPopChangeSkin = NULL;
			}
		}
	}

	void CScnMain::onBtnMinTchPre(CComponent *pTarget)
	{
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		::ShowWindow(pServer->getScreenContext()->hWnd, SW_MINIMIZE);
	}

	void CScnMain::onBtnMaxTchPre(CComponent *pTarget)
	{
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

		RECT rt;
		SystemParametersInfo(SPI_GETWORKAREA,   0,   &rt,   0) ;

		UINT w = rt.right - rt.left;
		UINT h = rt.bottom - rt.top;

		::SetWindowPos(pServer->getScreenContext()->hWnd, HWND_TOP, 0, 0, rt.right, rt.bottom, 0);

		CMainFrame::getInstance()->setPreferredSize(w, h);
		setPreferredSize(w, h);

		m_editSearchBox.setX((w - m_editSearchBox.getPreferredSize().getX()) / 2);
		m_editSearchBox.refresh();
		m_webBrsWeb.setPreferredSize(w - m_webBrsWeb.getX(), \
			h - m_webBrsWeb.getY() - m_imgFootBg.getPreferredSize().getY());
		m_webBrsWeb.littleRefresh();
		
	
		m_imgBg.setPreferredSize(w, h);
		m_imgLeftBg.setPreferredSize(m_imgLeftBg.getPreferredSize().getX(), h);
		m_imgFootBg.setY(h - m_imgFootBg.getPreferredSize().getY());
		m_imgFootBg.setPreferredSize(w, m_imgFootBg.getPreferredSize().getY());
		m_imgYun.setY(h - m_imgYun.getPreferredSize().getY());

		m_txtVersion.setY(h - m_txtVersion.getPreferredSize().getY());
		m_btnSkin.setX(w - (pServer->getScreenContext()->uiScreenWidth - m_btnSkin.getX()));
		m_btnMin.setX(w - (pServer->getScreenContext()->uiScreenWidth - m_btnMin.getX()));

		m_btnQuit.setX(w - (pServer->getScreenContext()->uiScreenWidth - m_btnQuit.getX()));
	
		m_imgSearchBg.setX((w - m_imgSearchBg.getPreferredSize().getX()) / 2);		
	

		m_btnPinyin.setX(w - (pServer->getScreenContext()->uiScreenWidth - m_btnPinyin.getX()));
		m_btnPinyin.setY(h - (pServer->getScreenContext()->uiScreenHeight - m_btnPinyin.getY()));
		m_btnUdisk.setX(w - (pServer->getScreenContext()->uiScreenWidth - m_btnUdisk.getX()));
		m_btnUdisk.setY(h - (pServer->getScreenContext()->uiScreenHeight - m_btnUdisk.getY()));
		m_btnLock.setX(w - (pServer->getScreenContext()->uiScreenWidth - m_btnLock.getX()));
		m_btnLock.setY(h - (pServer->getScreenContext()->uiScreenHeight - m_btnLock.getY()));
		m_btnVolume.setX(w - (pServer->getScreenContext()->uiScreenWidth - m_btnVolume.getX()));
		m_btnVolume.setY(h - (pServer->getScreenContext()->uiScreenHeight - m_btnVolume.getY()));
		m_btnMouse.setX(w - (pServer->getScreenContext()->uiScreenWidth - m_btnMouse.getX()));
		m_btnMouse.setY(h - (pServer->getScreenContext()->uiScreenHeight - m_btnMouse.getY()));
		

		m_tableTabGame.setPreferredSize(w - m_tableTabGame.getX(), \
			h - m_tableTabGame.getY() - m_imgFootBg.getPreferredSize().getY());
		m_scrollbarTable.setX(w - (pServer->getScreenContext()->uiScreenWidth - m_scrollbarTable.getX()));
		m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), h - m_scrollbarTable.getY() - m_imgFootBg.getPreferredSize().getY());
		m_tableTabGame.resetTableLayout();


		m_funcWgtFuncAd.changeSize(w - m_funcWgtFuncAd.getX(), \
			m_funcWgtFuncAd.getPreferredSize().getY());

		m_funcWgtHotSearch.setX(w / 2 - m_funcWgtHotSearch.getPreferredSize().getX() / 2);

		CConfigParser::tagScreenInfo* pTagScreenInfo = CConfigParser::getInstance()->getScreenInfo();
		ENV::setScreenCaptionRect(CRectangle(pTagScreenInfo->iCaptionX, pTagScreenInfo->iCaptionY, w, pTagScreenInfo->uiCaptionH));	

		pServer->setScreenSize(w, h);

		CRenderEngine::getInstance()->getRenderSession(1)->swapBuffer();
	}

	void CScnMain::onBtnQuitTchPre(CComponent *pTarget)
	{
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		::ShowWindow(pServer->getScreenContext()->hWnd, SW_MINIMIZE);
		::ShowWindow(pServer->getScreenContext()->hWnd, SW_HIDE);
	}

	void CScnMain::onBtnS1TchRel(CComponent *pTarget)
	{
		INT i = m_listMenu.getCurSel();

		vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();

		if (i < 0 || i >= (INT)arLeft.size())
		{
			m_iCurSubClass = 0;

			if (m_pBtnSearchResult != NULL)
			{
				m_pBtnSearchResult->getImageField(L"IMG")->setVisible(FALSE);
				m_pBtnSearchResult->setSelected(False);
				m_pBtnSearchResult->littleRefresh();

				m_funcWgtAdRec.setVisible(FALSE);
				m_funcWgtNoResult.setVisible(FALSE);
				m_txtRcm.setVisible(FALSE);
				m_btnChange.setVisible(FALSE);
				m_funcWgtFuncAd.setVisible(FALSE);
				m_tableTabGame.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
				m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), m_imgFootBg.getY() - m_tableTabGame.getY());
				m_scrollbarTable.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
				m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
			}
			m_arrySc[0]->pBtn->getImageField(L"IMG")->setVisible(TRUE);
			m_arrySc[0]->pBtn->setSelected(TRUE);
			m_arrySc[0]->pBtn->littleRefresh();

			vector<tagGameInfo*>	gameinfo;

			vector<hotgame>().swap(m_games);

			m_games.clear();

			std::map<DWORD, tagGameInfo*>& game_infos = AfxGetDbMgr()->m_mapGameInfo;
			std::map<DWORD, tagGameInfo*>::iterator iter = game_infos.begin();
			for (; iter != game_infos.end(); ++iter)
			{
				tagGameInfo* pGame = iter->second;
				if (AfxIsIdcClassGUID(pGame->cid))
				{
					TCHAR szName[MAX_PATH] = { 0 };
					AfxGetIdcClassName(pGame->cid, szName, MAX_PATH);
					_tcscpy_s(pGame->Class, szName);
				}

				gameinfo.push_back(pGame);
			}
			//这里需要先排序按传入的参数
			SortGameList<SORT_NAME>(gameinfo);

			for (int i = 0; i < (int)gameinfo.size(); ++i)
			{
				tagGameInfo* pGame = gameinfo[i];

				if (pGame->gid < 10000)
				{
					hotgame item;
					item.gid = pGame->gid;
					item.tag = L"";
					item.effect = L"";
					item.text = L"";
					item.pos = i + 1;
					item.type = 0;
					m_games.push_back(item);
				}
			}

			// 重新排序
			std::sort(m_games.begin(), m_games.end());

			// 放入控件
			m_tableTabGame.getModel()->deleteAllItems();
			for (INT i = 0; i < (int)m_games.size(); i++)
			{
				m_tableTabGame.getModel()->addItem(new CInt(i));
			}
			m_bSearch = FALSE;
			m_tableTabGame.refresh();
			return;
		}
		onSubClass(0, pTarget);
	}

	void CScnMain::onBtnS2TchRel(CComponent *pTarget)
	{
		onSubClass(1, pTarget);
	}

	void CScnMain::onBtnS3TchRel(CComponent *pTarget)
	{
		onSubClass(2, pTarget);
	}

	void CScnMain::onBtnS4TchRel(CComponent *pTarget)
	{
		onSubClass(3, pTarget);
	}

	void CScnMain::onBtnS5TchRel(CComponent *pTarget)
	{
		onSubClass(4, pTarget);
	}

	void CScnMain::onBtnS6TchRel(CComponent *pTarget)
	{
		onSubClass(5, pTarget);
	}

	void CScnMain::onBtnS7TchRel(CComponent *pTarget)
	{
		onSubClass(6, pTarget);
	}

	void CScnMain::onBtnS8TchRel(CComponent *pTarget)
	{
		onSubClass(7, pTarget);
	}

	void CScnMain::onBtnLoginTchRel(CComponent *pTarget)
	{
		m_funcWgtUser.openUrl(L"http://menuapi.yungengxin.net.cn/yuka/login.html");

		return;
		if (m_strUserId.IsEmpty())
		{
			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_LOGIN, POS_MENU, TOOL_LOGIN);
		}

		if (m_strUserId.IsEmpty())
			m_funcWgtUser.openUrl(L"http://advert.yungengxin.com/yuka/login.html");
		else
			m_funcWgtUser.openUrl(L"http://www.feihuo.com/yunuser");
	}

	void CScnMain::onBtnRegTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_LOGIN, POS_MENU, TOOL_REGISTER);
	}

	void CScnMain::onBtnTableGameItemHoverFocused(CComponent *pTarget)
	{
		CCtrlButton *pBtn = (CCtrlButton *)m_tableTabGame.getHoverFocusedItem();

		if (pBtn != NULL && m_tableTabGame.getHoverFocus() == TRUE)
		{
			CCtrlImageField* pImg = pBtn->getImageField(L"IMG");

			if (pImg == NULL)
				return;

			INT x = pBtn->getX() + pImg->getX() + m_tableTabGame.getX() - m_tableTabGame.getScrollRectangle().getX();
			INT y = pBtn->getY() + pImg->getY() + m_tableTabGame.getY() - m_tableTabGame.getScrollRectangle().getY();
		

	
		}
	}

	void CScnMain::onBtnTableGameItemHoverCanceled(CComponent *pTarget)
	{
		
	}

	void CScnMain::onBtnListItemHoverFocused(CComponent *pTarget)
	{
		CCtrlButton *pBtn = (CCtrlButton *)m_listMenu.getHoverFocusedItem();

		if (pBtn != NULL )
		{
			CCtrlImageField* pImg = pBtn->getImageField(L"IMG");

			if (pImg == NULL)
				return;

			pImg->showImage(2);
		}
	}

	void CScnMain::onBtnListItemHoverCanceled(CComponent *pTarget)
	{
		CCtrlButton *pBtn = (CCtrlButton *)m_listMenu.getHoverFocusedItem();

		if (pBtn != NULL && pBtn != m_listMenu.getSelectedItem())
		{
			CCtrlImageField* pImg = pBtn->getImageField(L"IMG");

			if (pImg == NULL)
				return;

			pImg->showImage(1);
		}
	}
	
	void CScnMain::onBtnListItemSelectLost(CComponent *pTarget)
	{
		CCtrlButton *pBtn = (CCtrlButton *)m_listMenu.getSelectedItem();

		if (pBtn != NULL)
		{
			CCtrlImageField* pImg = pBtn->getImageField(L"IMG");

			if (pImg == NULL)
				return;

			pImg->showImage(1);
		}
	}

	void CScnMain::onBtnListItemSelectChanged(CComponent *pTarget)
	{
		CCtrlButton *pBtn = (CCtrlButton *)m_listMenu.getSelectedItem();

		if (pBtn != NULL )
		{
			CCtrlImageField* pImg = pBtn->getImageField(L"IMG");

			if (pImg == NULL)
				return;

			pImg->showImage(2);
		}
	}

	void CScnMain::onBtnTableGameWheelStart(CComponent *pTarget)
	{
		m_funcWgtOpMenu.setVisible(FALSE);
		
	}

	void CScnMain::onBtnSearchTchRel(CComponent *pTarget)
	{
		
	}

	void CScnMain::onBtnRestoreTchPre(CComponent *pTarget)
	{
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		CConfigParser::tagScreenInfo* pTagScreenInfo = CConfigParser::getInstance()->getScreenInfo();

		RECT rt;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);

		UINT sw = rt.right - rt.left;
		UINT sh = rt.bottom - rt.top;

		UINT w = CLogicManager::getInstance()->getMenuXml().GetResW();
		UINT h = CLogicManager::getInstance()->getMenuXml().GetResH();

		if(sw < w)
			w = sw;
		if(sh < h)
			h = sh;

		UINT pw = pServer->getScreenContext()->uiScreenWidth;
		UINT ph = pServer->getScreenContext()->uiScreenHeight;

		pServer->setScreenSize(w, h);

		::SetWindowPos(pServer->getScreenContext()->hWnd, HWND_TOP, (sw - w) / 2, \
			(sh - h) / 2, w, h, 0);

		ENV::setScreenCaptionRect(CRectangle(pTagScreenInfo->iCaptionX, pTagScreenInfo->iCaptionY, w, pTagScreenInfo->uiCaptionH));
		CMainFrame::getInstance()->setPreferredSize(w, h);


		m_tableTabGame.setPreferredSize(w - m_scrollbarTable.getPreferredSize().getX() - m_tableTabGame.getX(), m_tableTabGame.getPreferredSize().getY());
		m_tableTabGame.resetTableLayout();
		m_imgFrame.setPreferredSize(w, h);
		m_imgBg.setPreferredSize(w, h);
		m_funcWgtFuncAd.changeSize(w - m_funcWgtFuncAd.getX(), m_funcWgtFuncAd.getPreferredSize().getY());
		m_funcWgtAdRec.changeSize(w - m_funcWgtAdRec.getX(), m_funcWgtAdRec.getPreferredSize().getY());
		m_funcWgtNoResult.changeSize(w, h);
		m_btnPinyin.setX(w - (pw - m_btnPinyin.getX()));
		m_btnPinyin.setY(h - (ph - m_btnPinyin.getY()));
		m_btnUdisk.setX(w - (pw - m_btnUdisk.getX()));
		m_btnUdisk.setY(h - (ph - m_btnUdisk.getY()));
		m_btnLock.setX(w - (pw - m_btnLock.getX()));
		m_btnLock.setY(h - (ph - m_btnLock.getY()));
		m_btnVolume.setX(w - (pw - m_btnVolume.getX()));
		m_btnVolume.setY(h - (ph - m_btnVolume.getY()));
		m_btnMouse.setX(w - (pw - m_btnMouse.getX()));
		m_btnMouse.setY(h - (ph - m_btnMouse.getY()));

		m_btnLogin.setX(w - (pw - m_btnLogin.getX()));
		m_btnSkin.setX(w - (pw - m_btnSkin.getX()));
		m_btnMin.setX(w - (pw - m_btnMin.getX()));
		m_btnQuit.setX(w - (pw - m_btnQuit.getX()));
		m_btnChange.setX(w - (pw - m_btnChange.getX()));
		setPreferredSize(w, h);
	}

	void CScnMain::onBtnTabGameItemTchPre(CComponent *pTarget)
	{
		
	}

	void CScnMain::onBtnTabGameItemTchPreR(CComponent *pTarget)
	{
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

		POINT pt;
		::GetCursorPos(&pt);
		::ScreenToClient(pServer->getScreenContext()->hWnd, &pt);

		m_funcWgtOpMenu.setVisible(TRUE);

		INT x = pt.x + m_funcWgtOpMenu.getPreferredSize().getX();
		INT tX = m_tableTabGame.getX() + m_tableTabGame.getPreferredSize().getX();

		if (x > tX)
		{
			m_funcWgtOpMenu.setX(tX - m_funcWgtOpMenu.getPreferredSize().getX());
		}
		else
		{
			m_funcWgtOpMenu.setX(pt.x);
		}

		INT y = pt.y + m_funcWgtOpMenu.getPreferredSize().getY();
		INT tY = m_tableTabGame.getY() + m_tableTabGame.getPreferredSize().getY();

		if (y > tY)
		{
			m_funcWgtOpMenu.setY(tY - m_funcWgtOpMenu.getPreferredSize().getY());
		}
		else
		{
			m_funcWgtOpMenu.setY(pt.y);
		}


		INT i = m_tableTabGame.getCurTouchFocusR();

		tagGameInfo* pGameInfo = NULL;
		customgame cust;

		if (m_bSearch == TRUE)
		{
			pGameInfo = m_search_games[i];
			cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGameInfo->gid);
		}
		else
		{
			hotgame pGame = m_games[i];

			cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGame.gid);
			
			pGameInfo = AfxGetDbMgr()->FindGameInfo(pGame.gid);
			

		}

		if (pGameInfo != NULL)
		{
			m_funcWgtOpMenu.showGameMenu(pGameInfo->gid, pGameInfo->gname, L"");
		}
		else
		{
			m_funcWgtOpMenu.showGameMenu(cust.gid, cust.name.GetBuffer(), cust.url.GetBuffer());
		}
	}

	void CScnMain::onBtnListMenuItemTchRel(CComponent *pTarget)
	{		
		m_funcWgtOpMenu.setVisible(FALSE);

		INT i = m_listMenu.getCurTouchFocus();		

		if (i < 0)
			return;
		vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();
		if (i < (INT)arLeft.size() )

		{
			leftclass c = arLeft[i];

			if (c.adbar)
			{
				m_funcWgtFuncAd.setVisible(TRUE);
				m_tableTabGame.setY(m_imgFootBg.getY() - 4 * m_tableTabGame.getCellHeight());
				m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), 4 * m_tableTabGame.getCellHeight());
				m_scrollbarTable.setY(m_tableTabGame.getY());
				m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
				m_txtRcm.setVisible(TRUE);
				m_btnChange.setVisible(TRUE);
				m_bAdBar = TRUE;
			}
			else
			{
				m_txtRcm.setVisible(FALSE);
				m_btnChange.setVisible(FALSE);
				m_funcWgtFuncAd.setVisible(FALSE);
				m_tableTabGame.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
				m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), m_imgFootBg.getY() - m_tableTabGame.getY());
				m_scrollbarTable.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
				m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
				m_bAdBar = FALSE;
			}
			OnClickCategroyList(c.id, 0);
		}
		else
		{
			m_iCurSubClass = 0;
			m_webBrsWeb.show(FALSE);
			m_funcWgtAdRec.setVisible(FALSE);
			m_funcWgtNoResult.setVisible(FALSE);
			m_funcWgtFuncAd.setVisible(FALSE);
			m_tableTabGame.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
			m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), m_imgFootBg.getY() - m_tableTabGame.getY());
			m_scrollbarTable.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
			m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
			m_tableTabGame.setVisible(TRUE);

			m_txtRcm.setVisible(FALSE);
			m_btnChange.setVisible(FALSE);

			for (UINT i = 0; i < m_arrySc.size(); i++)
			{
				m_arrySc[i]->pBtn->setVisible(FALSE);
				m_arrySc[i]->pBtn->getImageField(L"IMG_TAG")->setUserMovieClip(NULL);
			}

			TCHAR szTest[MAX_PATH] = { 0 };
			::LoadString(::GetModuleHandle(NULL), IDS_NB_NAME, szTest, MAX_PATH);

			CCtrlButton *pBtn = m_arrySc[0]->pBtn;

			pBtn->setCaption(szTest);

			CCtrlTextField *pText = pBtn->getTextField();
			INT w = pText->getContent().length()*pText->getFontSize();

			pText->setPreferredSize(w, pText->getPreferredSize().getY());
			pBtn->getImageField(L"IMG")->setPreferredSize(w, pBtn->getPreferredSize().getY());
			pBtn->getImageField(L"IMG")->setVisible(TRUE);
			pBtn->setPreferredSize(w, pBtn->getPreferredSize().getY());

			pBtn->setVisible(TRUE);
			pBtn->setSelected(TRUE);
			pBtn->littleRefresh();

			vector<tagGameInfo*>	gameinfo;

			vector<hotgame>().swap(m_games);

			m_games.clear();

			std::map<DWORD, tagGameInfo*>& game_infos = AfxGetDbMgr()->m_mapGameInfo;
			std::map<DWORD, tagGameInfo*>::iterator iter = game_infos.begin();
			for (; iter != game_infos.end(); ++iter)
			{
				tagGameInfo* pGame = iter->second;

				gameinfo.push_back(pGame);
			}
			//这里需要先排序按传入的参数
			SortGameList<SORT_NAME>(gameinfo);

			for (int i = 0; i < (int)gameinfo.size(); ++i)
			{
				tagGameInfo* pGame = gameinfo[i];

				if (pGame->gid < 10000)
				{
					hotgame item;
					item.gid = pGame->gid;
					item.tag = L"";
					item.effect = L"";
					item.text = L"";
					item.pos = i + 1;
					item.type = 0;
					m_games.push_back(item);
				}
			}

				// 重新排序
				std::sort(m_games.begin(), m_games.end());

				// 放入控件
				m_tableTabGame.getModel()->deleteAllItems();
				for (INT i = 0; i < (int)m_games.size(); i++)
				{
					m_tableTabGame.getModel()->addItem(new CInt(i));
				}
				m_bSearch = FALSE;
				m_bAdBar = FALSE;
				m_tableTabGame.refresh();
			
		}
		if (pTarget != NULL)
		{
			m_bSearchInit = FALSE;
			m_bSearchStart = FALSE;
			if (CLogicManager::getInstance()->getMenuXml().GetSearchWord().GetLength() > 0)
			{
				m_editSearchBox.setContent(CLogicManager::getInstance()->getMenuXml().GetSearchWord().GetBuffer());
			}
			else
			{
				m_editSearchBox.setContent(CLanguage::getString(RES_LAG_SEARCH_TIPS));
			}
			m_editSearchBox.setFont(15, RGB(156, 156, 156), L"Microsoft YaHei");
			m_funcWgtHotSearch.setVisible(FALSE);
		}
	}

	void CScnMain::onBtnTableGameItemTchRel(CComponent *pTarget)
	{
		INT i = m_tableTabGame.getCurTouchFocus();
		DWORD gid = 0;

		if (m_bSearch == TRUE)
		{
			gid = m_search_games[i]->gid;
		}
		else
		{
			gid = m_games[i].gid;
		}

		char szGid[10] = { 0 };
		char szPosId[10] = { 0 };
		_ltoa_s(m_arrySc[m_iCurSubClass]->id, szPosId, 10);

		RunGame(gid, m_bSearch ? POS_MENUSEARCH : szPosId);

		_ltoa_s(gid, szGid, 10);
		if (m_bSearch == TRUE)
			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_GAME, POS_MENUSEARCH, szGid);
		else
		{
			char szRow[10] = { 0 };
			char szColumn[10] = { 0 };
			char szPer_row_cnt[10] = { 0 };

			UINT uiRow, uiColumn;
			m_tableTabGame.getRowAndClomn(uiRow, uiColumn);

			
			_ltoa_s(i / uiColumn + 1, szRow, 10);
			_ltoa_s(i % uiColumn + 1, szColumn, 10);
			_ltoa_s(uiColumn, szPer_row_cnt, 10);
			
			std::string strTag = _bstr_t(m_games[i].tagid);
			std::string strEffect = _bstr_t(m_games[i].effectid);

			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_GAME, szPosId, szGid, szRow, szColumn, szPer_row_cnt, strTag, strEffect);
		}

		m_tableTabGame.setCurTouchFocus(-1);

	}

	void CScnMain::onBtnChangeHoverFocused(CComponent *pTarget)
	{
		m_btnChange.getImageField(L"IMG")->showImage(2);
	}

	void CScnMain::onBtnChangeHoverCanceled(CComponent *pTarget)
	{
		m_btnChange.getImageField(L"IMG")->showImage(1);
	}

	void CScnMain::onBtnChangeTouchRel(CComponent *pTarget)
	{
		UINT row = 0, clomn = 0;

		m_tableTabGame.getRowAndClomn(row, clomn);

		if ((UINT)m_tableTabGame.getCurrentTopItem() + 4 * clomn >= m_tableTabGame.getModel()->getItemCount())
		{
			m_tableTabGame.showTopItem(0);
		}
		else
		{
			m_tableTabGame.showTopItem(m_tableTabGame.getCurrentTopItem() + 64);
		}
	}

	void CScnMain::onBtnPinyinTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, POS_MENU, TOOL_INPUTMETHOD);

		// 处理输入法
		tagGameInfo* pGameInfo = AfxGetDbMgr()->FindGameInfo(42922);
		if (pGameInfo == NULL)
		{
			checkGotoDefautLeftClass();
			CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_GAME_NOT_DOWNLOAD));
		}
		else
		{
			RunGame(42922, "", L"", 2);
		}
	}

	void CScnMain::onBtnUdiskTchRel(CComponent *pTarget)
	{
		OnClickNetdisk(FALSE);
	}

	void CScnMain::onBtnLockTchRel(CComponent *pTarget)
	{
		OnClickLock(false);
	}

	void CScnMain::onBtnVolumeTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, POS_MENU, TOOL_VOICE);
		OSVERSIONINFO info = { sizeof(OSVERSIONINFO) };
		GetVersionEx(&info);
		if (info.dwMajorVersion >= 6)
			WinExec("sndvol.exe", SW_SHOWNORMAL);
		else
			WinExec("sndvol32.exe", SW_SHOWNORMAL);
	}

	void CScnMain::onBtnMouseTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, POS_MENU, TOOL_MOUSE);
		WinExec("rundll32.exe shell32.dll, Control_RunDLL main.cpl @0", SW_SHOWNORMAL);
	}

	void CScnMain::onChangeSkinDsy(CBasePopup *pTarget)
	{
		m_pPopChangeSkin = NULL;
		//m_editSearchBox.show(TRUE);
	}

	void CScnMain::onUdiskLoginDsy(CBasePopup *pTarget)
	{
		m_pPopUdiskLogin->quit();
		m_pPopUdiskLogin = NULL;
	}

	void CScnMain::onUdiskDsy(CBasePopup *pTarget)
	{
		m_pPopUdisk = NULL;
	}

	void CScnMain::onAdminDsy(CBasePopup *pTarget)
	{		
		m_pPopAdmin->quit();
		m_pPopAdmin = NULL;
	}

	void CScnMain::onUploadCfgDsy(CBasePopup *pTarget)
	{
		m_pPopUploadCfg->quit();
		m_pPopUploadCfg = NULL;
	}

	void CScnMain::onSaveFileDsy(CBasePopup *pTarget)
	{
		m_pPopSaveFile = NULL;
	}

	void CScnMain::onReadFileDsy(CBasePopup *pTarget)
	{
		m_pPopReadFile = NULL;
	}

	void CScnMain::onBtnMouseHoverFocused(CComponent *pTarget)
	{
		if (m_webBrsWeb.getVisible() == TRUE)
			return;
		m_labTips.setX(pTarget->getX() + pTarget->getPreferredSize().getX() - m_labTips.getPreferredSize().getX());
		m_labTips.setY(pTarget->getY() - m_labTips.getPreferredSize().getY());
		m_labTips.setCaption(CLanguage::getString(RES_LAG_MOUSE));
		m_labTips.setVisible(TRUE);
	}

	void CScnMain::onBtnVolumeHoverFocused(CComponent *pTarget)
	{
		if (m_webBrsWeb.getVisible() == TRUE)
			return;
		m_labTips.setX(pTarget->getX() + pTarget->getPreferredSize().getX() - m_labTips.getPreferredSize().getX());
		m_labTips.setY(pTarget->getY() - m_labTips.getPreferredSize().getY());
		m_labTips.setCaption(CLanguage::getString(RES_LAG_VOLUME));
		m_labTips.setVisible(TRUE);
	}

	void CScnMain::onBtnUdiskHoverFocused(CComponent *pTarget)
	{
		if (m_webBrsWeb.getVisible() == TRUE)
			return;
		m_labTips.setX(pTarget->getX() + pTarget->getPreferredSize().getX() - m_labTips.getPreferredSize().getX());
		m_labTips.setY(pTarget->getY() - m_labTips.getPreferredSize().getY());
		m_labTips.setCaption(CLanguage::getString(RES_LAG_PERSONAL_DISK));
		m_labTips.setVisible(TRUE);
	}

	void CScnMain::onBtnLockHoverFocused(CComponent *pTarget)
	{
		if (m_webBrsWeb.getVisible() == TRUE)
			return;
		m_labTips.setX(pTarget->getX() + pTarget->getPreferredSize().getX() - m_labTips.getPreferredSize().getX());
		m_labTips.setY(pTarget->getY() - m_labTips.getPreferredSize().getY());
		m_labTips.setCaption(CLanguage::getString(RES_LAG_HANG_UP_LOCK));
		m_labTips.setVisible(TRUE);
	}

	void CScnMain::onBtnPinyinHoverFocused(CComponent *pTarget)
	{
		if (m_webBrsWeb.getVisible() == TRUE)
			return;
		m_labTips.setX(pTarget->getX() + pTarget->getPreferredSize().getX() - m_labTips.getPreferredSize().getX());
		m_labTips.setY(pTarget->getY() - m_labTips.getPreferredSize().getY());
		m_labTips.setCaption(CLanguage::getString(RES_LAG_INPUT_METHOD));
		m_labTips.setVisible(TRUE);
	}

	void CScnMain::onBtnLoginHoverFocused(CComponent *pTarget)
	{
		m_labTips.setX(pTarget->getX() + (pTarget->getPreferredSize().getX() - m_labTips.getPreferredSize().getX())/2);
		m_labTips.setY(pTarget->getY() + pTarget->getPreferredSize().getY());
		m_labTips.setCaption(L"用户登录");
		m_labTips.setVisible(TRUE);
	}

	void CScnMain::onBtnSkinHoverFocused(CComponent *pTarget)
	{
		m_labTips.setX(pTarget->getX() + pTarget->getPreferredSize().getX() - m_labTips.getPreferredSize().getX());
		m_labTips.setY(pTarget->getY() + m_labTips.getPreferredSize().getY());
		m_labTips.setCaption(CLanguage::getString(RES_LAG_CHANGE_SKIN));
		m_labTips.setVisible(TRUE);
	}

	void CScnMain::onBtnMinHoverFocused(CComponent *pTarget)
	{
		m_labTips.setX(pTarget->getX() + pTarget->getPreferredSize().getX() - m_labTips.getPreferredSize().getX());
		m_labTips.setY(pTarget->getY() + m_labTips.getPreferredSize().getY());
		m_labTips.setCaption(CLanguage::getString(RES_LAG_MIN));
		m_labTips.setVisible(TRUE);
	}

	void CScnMain::onBtnMaxHoverFocused(CComponent *pTarget)
	{
		m_labTips.setX(pTarget->getX() + pTarget->getPreferredSize().getX() - m_labTips.getPreferredSize().getX());
		m_labTips.setY(pTarget->getY() + m_labTips.getPreferredSize().getY());
		m_labTips.setCaption(CLanguage::getString(RES_LAG_MAX));
		m_labTips.setVisible(TRUE);
	}

	void CScnMain::onBtnQuitHoverFocused(CComponent *pTarget)
	{
		m_labTips.setX(pTarget->getX() + pTarget->getPreferredSize().getX() - m_labTips.getPreferredSize().getX());
		m_labTips.setY(pTarget->getY() + m_labTips.getPreferredSize().getY());
		m_labTips.setCaption(CLanguage::getString(RES_LAG_QUIT));
		m_labTips.setVisible(TRUE);
	}

	void CScnMain::onBtnRestoreHoverFocused(CComponent *pTarget)
	{
		m_labTips.setX(pTarget->getX() + pTarget->getPreferredSize().getX() - m_labTips.getPreferredSize().getX());
		m_labTips.setY(pTarget->getY() + m_labTips.getPreferredSize().getY());
		m_labTips.setCaption(CLanguage::getString(RES_LAG_WINDOW_MODE));
		m_labTips.setVisible(TRUE);
	}

	void CScnMain::onBtnHoverCanceled(CComponent *pTarget)
	{
		m_labTips.setVisible(FALSE);
	}

	void CScnMain::onBtnAdHoverFocused(CComponent *pTarget)
	{
		m_imgAdMask.setVisible(TRUE);		
	}

	void CScnMain::onBtnAdHoverCanceled(CComponent *pTarget)
	{
		m_imgAdMask.setVisible(FALSE);
	}

	void CScnMain::onBtnAdTchRel(CComponent *pTarget)
	{
		std::string strPos = "s22";

		char szAid[10] = { 0 }, szGid[10];
		_ltoa_s(m_borderItem.id, szAid, 10);
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_AD, strPos, szAid);
		_ltoa_s(m_borderItem.gid, szGid, 10);
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_GAME, strPos, szGid);

		if (m_borderItem.action == 3  && !m_borderItem.url.IsEmpty())
		{
			CLogicManager::getInstance()->OnClickCategroyList(m_borderItem.gid, m_borderItem.url);
		}
		else if (m_borderItem.action == 2)
		{
			CLogicManager::getInstance()->RunGame(false, m_borderItem.gid, m_borderItem.url, 2, strPos);
		}
		else if (m_borderItem.action == 1)
		{
			CLogicManager::getInstance()->RunGame(false, m_borderItem.gid, m_borderItem.url, 1, strPos);
		}
		else
			CLogicManager::getInstance()->RunGame(false, m_borderItem.gid, m_borderItem.url, 0, strPos);
	}

	void CScnMain::onBtnSloganTchRel(CComponent *pTarget)
	{
		std::string strPos = "s23";

		char szAid[10] = { 0 }, szGid[10];
		_ltoa_s(m_sloganItem.id, szAid, 10);
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_AD, strPos, szAid);
		_ltoa_s(m_sloganItem.gid, szGid, 10);
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_GAME, strPos, szGid);

		if (m_sloganItem.action == 3  && !m_sloganItem.url.IsEmpty())
		{
			CLogicManager::getInstance()->OnClickCategroyList(m_sloganItem.gid, m_sloganItem.url);
		}
		else if (m_sloganItem.action == 2)
		{
			CLogicManager::getInstance()->RunGame(false, m_sloganItem.gid, m_sloganItem.url, 2, strPos);
		}
		else if (m_sloganItem.action == 1)
		{
			CLogicManager::getInstance()->RunGame(false, m_sloganItem.gid, m_sloganItem.url, 1, strPos);
		}
		else
			CLogicManager::getInstance()->RunGame(false, m_sloganItem.gid, m_sloganItem.url, 0, strPos);
	}

	// TODO: implement logic event listener here
	void CScnMain::onEditSetFocus()
	{
		if (m_editSearchBox.getFocus() == FALSE )
			return;

		checkGotoDefautLeftClass();

		if (m_bSearchInit == FALSE)
		{
			m_editSearchBox.setContent(L"");
		}
		m_editSearchBox.setFont(15, RGB(255, 255, 255), L"Microsoft YaHei");
		m_bSearchInit = TRUE;
		m_funcWgtOpMenu.setVisible(FALSE);
		m_funcWgtHotSearch.enter();
		m_funcWgtHotSearch.setVisible(TRUE);
		m_imgSearch.showImage(2);
	}

	void CScnMain::onEditKillFocus()
	{
		m_editSearchBox.setFocus(FALSE);

		if (m_bSearchInit == TRUE)
		{
			CString txt = m_editSearchBox.getContent();

			if (txt.GetLength() == 0)
			{
				if (CLogicManager::getInstance()->getMenuXml().GetSearchWord().GetLength() > 0)
				{
					m_editSearchBox.setContent(CLogicManager::getInstance()->getMenuXml().GetSearchWord().GetBuffer());
				}
				else
				{
					m_editSearchBox.setContent(CLanguage::getString(RES_LAG_SEARCH_TIPS));
				}
				m_editSearchBox.setFont(15, RGB(156, 156, 156), L"Microsoft YaHei");
				m_bSearchInit = FALSE;
				m_bSearchStart = FALSE;
				if (m_pBtnSearchResult != NULL)
				{
					m_pBtnSearchResult->setVisible(FALSE);
				}

				m_funcWgtHotSearch.setVisible(FALSE);
			}
		}
		m_imgSearch.showImage(1);
	}

	void CScnMain::onEditUpdate()
	{
		if (m_bSearchInit == FALSE)
		{
			return;
		}

		CString txt = m_editSearchBox.getContent();
		
		if (m_strSearch.Compare(txt) == 0)
			return;

		if (txt.GetLength() == 0)
		{
			if (m_bSearch == TRUE)
			{				
				if (m_pBtnSearchResult != NULL)
				{
					m_pBtnSearchResult->setVisible(FALSE);
				}
				INT i = m_listMenu.getCurSel();

				vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();

				m_funcWgtFuncAd.setVisible(TRUE);
				m_tableTabGame.setY(m_imgFootBg.getY() - 4*m_tableTabGame.getCellHeight());
				m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), 4 * m_tableTabGame.getCellHeight());
				m_tableTabGame.setVisible(TRUE);
				m_scrollbarTable.setY(m_tableTabGame.getY());
				m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
				m_txtRcm.setVisible(TRUE);
				m_btnChange.setVisible(TRUE);
				m_funcWgtNoResult.setVisible(FALSE);
				m_funcWgtAdRec.setVisible(FALSE);

				if (i >= 0 && i < (INT)arLeft.size())
				{					
					onSubClass(0, NULL);
				}
				else
				{
					vector<tagGameInfo*>	gameinfo;

					vector<hotgame>().swap(m_games);

					m_games.clear();

					std::map<DWORD, tagGameInfo*>& game_infos = AfxGetDbMgr()->m_mapGameInfo;
					std::map<DWORD, tagGameInfo*>::iterator iter = game_infos.begin();
					for (; iter != game_infos.end(); ++iter)
					{
						tagGameInfo* pGame = iter->second;
						if (AfxIsIdcClassGUID(pGame->cid))
						{
							TCHAR szName[MAX_PATH] = { 0 };
							AfxGetIdcClassName(pGame->cid, szName, MAX_PATH);
							_tcscpy_s(pGame->Class, szName);
						}

						gameinfo.push_back(pGame);
					}
					//这里需要先排序按传入的参数
					SortGameList<SORT_NAME>(gameinfo);

					for (int i = 0; i < (int)gameinfo.size(); ++i)
					{
						tagGameInfo* pGame = gameinfo[i];

						if (pGame->gid >= 10000)
						{
							hotgame item;
							item.gid = pGame->gid;
							item.tag = L"";
							item.effect = L"";
							item.text = L"";
							item.pos = i + 1;
							item.type = 0;
							m_games.push_back(item);
						}
					}

					// 重新排序
					std::sort(m_games.begin(), m_games.end());

					// 放入控件
					m_tableTabGame.getModel()->deleteAllItems();
					for (INT i = 0; i < (int)m_games.size(); i++)
					{
						m_tableTabGame.getModel()->addItem(new CInt(i));
					}
					m_bSearch = FALSE;
					m_arrySc[0]->pBtn->setSelected(TRUE);
					m_arrySc[0]->pBtn->getImageField(L"IMG")->setVisible(TRUE);
					m_arrySc[0]->pBtn->littleRefresh();
					m_tableTabGame.refresh();
				}
				
			}
			m_strSearch = L"";
			return;
		}		

		m_funcWgtHotSearch.setVisible(FALSE);
		m_strSearch = txt;
		m_bSearchStart = TRUE;

		if (txt.GetLength() > 0)
		{
			txt.MakeLower();

			vector<tagGameInfo*>().swap(m_search_games);
			m_search_games.clear();
			
			std::map<DWORD, tagGameInfo*>& games = AfxGetDbMgr()->m_mapGameInfo;
			std::map<DWORD, tagGameInfo*>::iterator iter = games.begin();
			for (; iter != games.end(); ++iter)
			{
				tagGameInfo* pGame = iter->second;
				CString strName = pGame->gname;
				CString strPy = pGame->szPy;
				CString strMultiPy = pGame->szMultiPy;
				CString strGalias = pGame->galias;

				if (pGame->gid == 0)
				{
					continue;
				}
				if (strGalias.CompareNoCase(txt) == 0)
				{
					pGame->bGalias = true;
					m_search_games.push_back(pGame);
				}
				else if ((pGame->dwFindPos = strName.Find(txt)) != -1)
				{
					if (pGame->dwFindPos != 0) pGame->dwFindPos = 0;
					else pGame->dwFindPos = -1; //在名字中查找到的优先级高过拼音中查找到的
					m_search_games.push_back(pGame);
				}
				else if ((pGame->dwFindPos = strPy.Find(txt)) != -1
					|| (pGame->dwFindPos = strMultiPy.Find(txt)) != -1)
				{
					if (pGame->dwFindPos != 0) pGame->dwFindPos = 1;
					m_search_games.push_back(pGame);
				}
			}
			SortGameList<SORT_FINDPOS>(m_search_games);

			m_funcWgtFuncAd.setVisible(FALSE);
			m_funcWgtAdRec.setVisible(TRUE);
			m_txtRcm.setVisible(FALSE);
			m_tableTabGame.setY(m_funcWgtFuncAd.getY());
			m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), 3*m_tableTabGame.getCellHeight());
			m_scrollbarTable.setY(m_funcWgtFuncAd.getY());
			m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
			m_btnChange.setVisible(FALSE);

			m_bSearch = TRUE;
			result r = CLogicManager::getInstance()->getMenuXml().GetResult();
			if (m_search_games.size() > 0)
			{
				for (unsigned int i = 0; i < r.arrGameRcR.size(); i++)
				{
					m_search_games.push_back(r.arrGameRcR[i]);
				}

				// 放入控件
				m_tableTabGame.getModel()->deleteAllItems();
				for (int i = 0; i < (int)m_search_games.size(); ++i)
				{
					m_tableTabGame.getModel()->addItem(new CInt(i));
				}

				m_tableTabGame.setVisible(TRUE);
				m_funcWgtNoResult.setVisible(FALSE);
				m_tableTabGame.refresh();
			}
			else
			{
				m_scrollbarTable.setVisible(FALSE);
				m_tableTabGame.setVisible(FALSE);
				m_funcWgtNoResult.setVisible(TRUE);
			}			

			INT i = m_listMenu.getCurSel();

			vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();
			
			if (i < 0 || i >= (INT)arLeft.size())
			{
				m_arrySc[0]->pBtn->getImageField(L"IMG")->setVisible(FALSE);
				m_arrySc[0]->pBtn->setSelected(FALSE);
				m_arrySc[0]->pBtn->littleRefresh();

				m_pBtnSearchResult = m_arrySc[1]->pBtn;
			}
			else
			{
				ASSERT(i >= 0 && i < (INT)arLeft.size());
				leftclass c = arLeft[i];
				size_t j = 0;

				for (; j < c.arSubClass.size(); j++)
				{
					m_arrySc[j]->pBtn->getImageField(L"IMG")->setVisible(FALSE);
					m_arrySc[j]->pBtn->setSelected(FALSE);
					m_arrySc[j]->pBtn->littleRefresh();
				}

				m_pBtnSearchResult = m_arrySc[j]->pBtn;
			}			

			m_pBtnSearchResult->setCaption(CLanguage::getString(RES_LAG_SEARCH_RESULT));
			CCtrlTextField *pText = m_pBtnSearchResult->getTextField();
			INT w = pText->getContent().length()*pText->getFontSize();

			pText->setPreferredSize(w, pText->getPreferredSize().getY());
			m_pBtnSearchResult->getImageField(L"IMG")->setPreferredSize(w, m_pBtnSearchResult->getPreferredSize().getY());
			m_pBtnSearchResult->setPreferredSize(w, m_pBtnSearchResult->getPreferredSize().getY());
			m_pBtnSearchResult->setVisible(TRUE);
			m_pBtnSearchResult->setSelected(TRUE);
			m_pBtnSearchResult->getImageField(L"IMG")->setVisible(TRUE);
			m_pBtnSearchResult->littleRefresh();

		}
	}

	void CScnMain::onCaptionDblClk()
	{
		
	}

	void CScnMain::onSetFocus()
	{
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		if (!IsIconic(pServer->getScreenContext()->hWnd))
		{
			CTimerManager::getInstance()->start();
			//OutputDebugString(L"开始动画\n");
		}
		m_editSearchBox.show(TRUE);
	}

	void CScnMain::onKillFocus()
	{
		CTimerManager::getInstance()->stop();
		//OutputDebugString(L"结束动画\n");
	}

	void CScnMain::onDispalyChange()
	{
		
	}

	void CScnMain::onDropFiles()
	{
		if (m_pPopChangeSkin != NULL)
		{
			m_pPopChangeSkin->destroy();
		}

		HDROP hDropInfo = CScreenTouchServer::getInstance()->getHDrop();


		TCHAR szFilePathName[MAX_PATH * 2 + 1] = { 0 };

		//得到文件个数 
		UINT nNumOfFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

		for (UINT nIndex = 0; nIndex < nNumOfFiles; ++nIndex)
		{
			//得到文件名 
			DragQueryFile(hDropInfo, nIndex, szFilePathName, MAX_PATH * 2);

			CString strPathName = szFilePathName;
			int nPos = strPathName.ReverseFind(_T('\\'));
			CString strFileName = strPathName.Right(strPathName.GetLength() - nPos - 1);

			nPos = strFileName.ReverseFind(_T('.'));
			if (nPos == -1)
				continue;
			CString strExt = strFileName.Right(strFileName.GetLength() - nPos);
			if (strExt.CompareNoCase(_T(".png")) == 0 ||
				strExt.CompareNoCase(_T(".jpg")) == 0)
			{
				CString strBg = AfxGetDbMgr()->GetStrOpt(OPT_M_CUSTOMMENUBG);
				vector<tstring> strSplit;
				AfxSplitString(strBg.GetBuffer(), strSplit, _T('|'));
				if (strSplit.size() >= 10)
				{
					CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_SKINS_EXCEED_10));
					break;
				}
				bool bExist = false;
				for (size_t i = 0; i < strSplit.size(); i++)
				{
					CString strOldFileName = strSplit[i].c_str();
					if (strOldFileName.CompareNoCase(strFileName) == 0)
					{
						bExist = true;
						break;
					}
				}

				TCHAR szDstPath[MAX_PATH] = { 0 };
				AfxGetAppPath(szDstPath, MAX_PATH, _T("Skin\\Image\\"));
				CString	strImagePath = szDstPath;
				strImagePath += strFileName;
				AfxCreateDirectory(strImagePath);

				Gdiplus::Bitmap *pBmp = Gdiplus::Bitmap::FromFile(strPathName.GetBuffer());
				if (pBmp == NULL)
				{
					CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_FILE_OPEN_ERROR));
					break;
				}

				bool bRet;

				if (pBmp->GetWidth() > 1920 && pBmp->GetHeight() > 1920)
				{
					Gdiplus::Bitmap* pDstBmp = CLogicManager::getInstance()->scaleBitmap(pBmp, 1920, 1920);

					bRet = CLogicManager::getInstance()->saveAsImageFile(pDstBmp, strImagePath, L"image/png");
					delete pDstBmp;

				}
				else
				{
					bRet = CopyFile(strPathName, strImagePath, FALSE) == TRUE;
				}

				Gdiplus::Bitmap* pShortCutBmp = CLogicManager::getInstance()->scaleBitmap(pBmp, 246, 136);
				String* pStrName = new String(CPath::getApplicationFolder() + L"skin\\customS\\" + strFileName.GetBuffer());
				AfxCreateDirectory((const CHAR*)(*pStrName));
				CLogicManager::getInstance()->saveAsImageFile(pShortCutBmp, (const CHAR*)(*pStrName), L"image/png");
				delete pShortCutBmp;

				delete pBmp;

				if (strPathName.CompareNoCase(strImagePath) == 0 || bRet || GetLastError()==0)
				{
					AfxGetDbMgr()->SetOpt(OPT_M_MENUBG, strFileName);
					AfxGetDbMgr()->SaveOpt();

					CMovieClip* pMovieClip = new CMovieClip();

					pMovieClip->addImage(strImagePath.GetBuffer());

					m_imgBg.setUserMovieClip(pMovieClip);
					CLogicManager::getInstance()->skinAdd();

					CString strError;
					if (!bExist)
					{
						if (strBg.IsEmpty())
							strBg = strFileName;
						else
						{
							strBg += _T("|");
							strBg += strFileName;
						}

						AfxGetDbMgr()->SetOpt(OPT_M_CUSTOMMENUBG, strBg);
						if (!AfxGetDbMgr()->SaveOpt())
						{
							CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_SAVE_OPTION_FAILED));
							break;
						}						
					}
					else
					{
						CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_SKIN_EXIST), FALSE);
						break;
					}					
				}
				else
				{
					DWORD nError = GetLastError();
					TCHAR szBufer[MAX_PATH] = { 0 };
					AfxGetWinErrorText(szBufer, MAX_PATH, nError);
					
					String str = String::format(L"%s:%u，%s", (const CHAR*)CLanguage::getString(RES_LAG_COPY_FILE_FAILED), nError, szBufer);
					CMainFrame::getInstance()->showTips(str);
					break;
				}
			}
			break;
		}

		DragFinish(hDropInfo);
	}

	void CScnMain::onUpdateData()
	{
		INT iCurSel = -1;
		INT iPos = -1;
		INT id = -1;
		INT type = -1;

		vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();

		for (size_t i = 0; i < arLeft.size(); i++)
		{
			for (size_t j = 0; j < arLeft[i].arSubClass.size(); j++)
			{
				if (arLeft[i].arSubClass[j].id == DEFAUT_SUB_CLASS_ID)
				{
					iCurSel = (INT)i;
					iPos = (INT)j;
					id = arLeft[i].id;
					type = arLeft[i].arSubClass[j].type;
					break;
				}
			}

			if (iCurSel >= 0)
				break;
		}

		if (iCurSel < 0 || type == 1)
			return;

		if (arLeft[iCurSel].adbar)
		{
			m_funcWgtFuncAd.setVisible(TRUE);
			m_tableTabGame.setY(m_imgFootBg.getY() - 4 * m_tableTabGame.getCellHeight());
			m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), 4 * m_tableTabGame.getCellHeight());
			m_scrollbarTable.setY(m_tableTabGame.getY());
			m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
			m_txtRcm.setVisible(TRUE);
			m_btnChange.setVisible(TRUE);
			m_bAdBar = TRUE;
		}
		else
		{
			m_txtRcm.setVisible(FALSE);
			m_btnChange.setVisible(FALSE);
			m_funcWgtFuncAd.setVisible(FALSE);
			m_tableTabGame.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
			m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), m_imgFootBg.getY() - m_tableTabGame.getY());
			m_scrollbarTable.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
			m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
			m_bAdBar = FALSE;
		}
		m_webBrsWeb.show(FALSE);
		m_listMenu.setCurSel(iCurSel);
		m_listMenu.setCurTouchFocus(iCurSel);
		m_listMenu.littleRefresh();

		OnClickCategroyList(id, iPos);
	}

	void CScnMain::onUpdateAllData()
	{
		AfxGetDbMgr()->RefreshOfflineData();
#if 0
		if (!AfxGetDbMgr()->GetBoolOpt(OPT_M_WINDOWMODE))
		{
			if (m_btnMax.getVisible() == FALSE)
				onBtnMaxTchPre(NULL);
			else
				onBtnRestoreTchPre(NULL);
		}
		if (CMainFrame::getInstance()->getUseGdi() == FALSE)
		{
			if (AfxGetDbMgr()->GetBoolOpt(OPT_M_MENUANIMATION))
			{
				m_amtPlayer1.play(&m_amtBgOpacity1, TRUE);
				m_amtPlayer4.play(&m_amtMenuImgOpacity1, TRUE);
				m_amtPlayer5.play(&m_amtMidBar, TRUE);
			}
			else
			{
				m_amtPlayer1.stop();
				m_amtPlayer4.stop();
				m_amtPlayer5.stop();
				m_imgMidBar1.setVisible(FALSE);
			}
		}
#endif
		if (!CLogicManager::getInstance()->getMenuXml().LoadIdcXml())
			CLogicManager::getInstance()->getMenuXml().LoadDefautXml();

		m_aryData.clear();

		INT iCurSel = -1;

		vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();
		for (size_t i = 0; i < arLeft.size(); i++)
		{
			m_aryData.addItem(new CInt(i));
			if (arLeft[i].id == DEFAUT_LEFT_CLASS_ID)
			{
				iCurSel = (INT)i;
			}
		}
		m_listMenu.refresh();
		m_listMenu.setCurSel(iCurSel);
		m_listMenu.setCurTouchFocus(iCurSel);
		m_listMenu.littleRefresh();
		onBtnListMenuItemTchRel(NULL);

		CMovieClip* pMovieClip = new CMovieClip();
		CString strBg; int nType = 1;
		String strFile;

		if (AfxGetDbMgr()->GetSkinPng(strBg, nType))
		{
			if (nType == 1)
			{
				String strTmp = strBg.GetBuffer();
				strTmp = strTmp.subString(0, strTmp.length() - 4);

				int nIndex = SkinNameToType((const CHAR*)strTmp);
				strFile = String::format(L"%s%d.jpg", L"MenuSkin/Wallpaper/", nIndex + 1);
			}
			else
			{
				strFile = CPath::getApplicationFolder() + L"Skin\\Image\\" + strBg.GetBuffer();
			}

			if (!CZipSkin::getInstance()->FindFileFromZip((const CHAR*)strFile) && CFile::isExist(strFile) == FALSE)
			{
				strFile = L"MenuSkin/default_skin.jpg";
			}
		}
		else
		{
			strFile = L"MenuSkin/default_skin.jpg";
		}

		pMovieClip->addImage(strFile);
		m_imgBg.setUserMovieClip(pMovieClip);
	}

	void CScnMain::onUpdateWallPaper()
	{
		CMovieClip* pMovieClip = new CMovieClip();
		pMovieClip->addImage(CLogicManager::getInstance()->getWallPaperPath(CLogicManager::getInstance()->getCurWallPaperIdx()));
		m_imgBg.setUserMovieClip(pMovieClip);
	}

	void CScnMain::onLeaveUser()
	{
		m_funcWgtUser.setVisible(FALSE);
	}

	void CScnMain::onClickCategroyList()
	{
		CLogicManager::CClickCategroyList& c = CLogicManager::getInstance()->getClickCategroyList();

		INT iCurSel = -1;
		INT iPos = -1;
		INT id = -1;

		vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();

		for (size_t i = 0; i < arLeft.size(); i++)
		{
			for (size_t j = 0; j < arLeft[i].arSubClass.size(); j++)
			{
				if (arLeft[i].arSubClass[j].id == c.m_id)
				{
					iCurSel = (INT)i;
					iPos = (INT)j;
					id = arLeft[i].id;
					break;
				}
			}

			if (iCurSel >= 0)
				break;
		}


		if (iCurSel < 0 || iPos < 0)
			return;

		m_bSearchInit = FALSE;
		m_bSearchStart = FALSE;
		m_bSearch = FALSE;
		if (CLogicManager::getInstance()->getMenuXml().GetSearchWord().GetLength() > 0)
		{
			m_editSearchBox.setContent(CLogicManager::getInstance()->getMenuXml().GetSearchWord().GetBuffer());
		}
		else
		{
			m_editSearchBox.setContent(CLanguage::getString(RES_LAG_SEARCH_TIPS));
		}
		m_editSearchBox.setFont(15, RGB(156, 156, 156), L"Microsoft YaHei");
		m_funcWgtHotSearch.setVisible(FALSE);


		if (arLeft[iCurSel].adbar)
		{
			m_funcWgtFuncAd.setVisible(TRUE);
			m_tableTabGame.setY(m_imgFootBg.getY() - 4 * m_tableTabGame.getCellHeight());
			m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), 4 * m_tableTabGame.getCellHeight());
			m_scrollbarTable.setY(m_tableTabGame.getY());
			m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
			m_txtRcm.setVisible(TRUE);
			m_btnChange.setVisible(TRUE);
			m_bAdBar = TRUE;
		}
		else
		{
			m_txtRcm.setVisible(FALSE);
			m_btnChange.setVisible(FALSE);
			m_funcWgtFuncAd.setVisible(FALSE);
			m_tableTabGame.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
			m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), m_imgFootBg.getY() - m_tableTabGame.getY());
			m_scrollbarTable.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
			m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
			m_bAdBar = FALSE;
		}

		m_listMenu.setCurSel(iCurSel);
		m_listMenu.setCurTouchFocus(iCurSel);
		m_listMenu.littleRefresh();

		OnClickCategroyList(id, iPos, c.m_url, c.m_bFromDesk);
	}

	void CScnMain::onRunGame()
	{
		CLogicManager::CRunGame& c = CLogicManager::getInstance()->getRunGame();

		if (c.bFromGame)
		{
			char szGid[10] = { 0 };
			_ltoa_s(c.gid, szGid, 10);
	
			char szPosId[10] = { 0 };
			_ltoa_s(m_arrySc[m_iCurSubClass]->id, szPosId, 10);

			RunGame(c.gid, m_bSearch ? POS_MENUSEARCH : szPosId, c.url, c.type);

			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_GAME, m_bSearch ? POS_MENUSEARCH : szPosId, szGid);
		}
		else
		{
			RunGame(c.gid, c.pos, c.url, c.type);
		}
	}

	void CScnMain::onRefreshData()
	{
		INT iWP, iLP;

		CLogicManager::getInstance()->getRefreshData(iWP, iLP);

		if (iLP == 1)
		{
			BOOLEAN bOnline = (iWP == 1 && iLP == 1);

			if (m_bOnline == bOnline)
			{
				return;
			}

			m_bOnline = bOnline;

			if (m_hIcon)
			{
				DestroyIcon(m_hIcon);
				m_hIcon = NULL;
			}

			if (bOnline)
			{
				m_hIcon = LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(101));
			}
			else
			{
				m_hIcon = LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(106));
			}


			CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
			NOTIFYICONDATA data = { 0 };

			data.cbSize = sizeof(data);
			data.uFlags = NIF_ICON;
			data.hIcon = m_hIcon;
			data.hWnd = pServer->getScreenContext()->hWnd;
			Shell_NotifyIcon(NIM_MODIFY, &data);

			SendMessage(pServer->getScreenContext()->hWnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);
			SendMessage(pServer->getScreenContext()->hWnd, WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);
			SendMessage(pServer->getScreenContext()->hWnd, WM_SETICON, ICON_SMALL2, (LPARAM)m_hIcon);
		}
		else
		{
			//CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 2);
			//CMain::getInstance()->pushMessage(pMessage);			
		}
	}

	void CScnMain::onUploadCfg()
	{
		if (m_pPopAdmin == NULL)
		{
			m_pPopAdmin = new CPopAdmin(FALSE);

			if (m_pPopAdmin->initialize())
			{
				m_pPopAdmin->addEventListener(this);
				m_pPopAdmin->doModal();
			}
			else
			{
				delete m_pPopAdmin;
				m_pPopAdmin = NULL;
			}
		}
	}

	void CScnMain::onLoginPopAdminOK()
	{
		if (m_pPopChangeSkin != NULL)
		{
			return;
		}

		INT i = m_tableTabGame.getCurTouchFocusR();

		tagGameInfo* pGameInfo = NULL;
		customgame cust;

		if (m_bSearch == TRUE)
		{
			pGameInfo = m_search_games[i];
			cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGameInfo->gid);
		}
		else
		{
			hotgame pGame = m_games[i];

			cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGame.gid);
			if (pGame.type == 0)
			{
				pGameInfo = AfxGetDbMgr()->FindGameInfo(pGame.gid);
			}

		}

		if (m_pPopUploadCfg == NULL)
		{
			m_pPopUploadCfg = new CPopUploadcfg(FALSE);

			if (m_pPopUploadCfg->initialize())
			{				
				if (pGameInfo != NULL)
				{
					m_pPopUploadCfg->showCfg(pGameInfo->gid);
				}
				else
				{
					m_pPopUploadCfg->showCfg(cust.gid);
				}
				
				m_pPopUploadCfg->addEventListener(this);
				m_pPopUploadCfg->doModal();
			}
			else
			{
				delete m_pPopUploadCfg;
				m_pPopUploadCfg = NULL;
			}
		}
	}

	void CScnMain::onLoginUdiskOK()
	{
		INT i = m_tableTabGame.getCurTouchFocusR();
		tagGameInfo* pGameInfo = NULL;
		customgame cust;

		if (i < 0)
			return;

		if (m_bSearch == TRUE)
		{
			pGameInfo = m_search_games[i];
			cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGameInfo->gid);
		}
		else
		{
			hotgame pGame = m_games[i];

			cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGame.gid);
			if (pGame.type == 0)
			{
				pGameInfo = AfxGetDbMgr()->FindGameInfo(pGame.gid);
			}

		}

		if (m_bReadFile == TRUE)
		{
			if (m_pPopReadFile == NULL)
			{
				m_pPopReadFile = new CPopReadFile(FALSE);

				if (m_pPopReadFile->initialize())
				{
					if (pGameInfo != NULL)
					{
						m_pPopReadFile->readFile(pGameInfo->gid);
					}
					else
					{
						m_pPopReadFile->readFile(cust.gid);
					}

					m_pPopReadFile->addEventListener(this);
					m_pPopReadFile->doModal();
				}
				else
				{
					delete m_pPopReadFile;
					m_pPopReadFile = NULL;
				}
			}
		}
		else
		{
			if (m_pPopSaveFile == NULL)
			{
				m_pPopSaveFile = new CPopSaveFile(FALSE);

				if (m_pPopSaveFile->initialize())
				{
					if (pGameInfo != NULL)
					{
						m_pPopSaveFile->saveFile(pGameInfo->gid);
					}
					else
					{
						m_pPopSaveFile->saveFile(cust.gid);
					}

					m_pPopSaveFile->addEventListener(this);
					m_pPopSaveFile->doModal();
				}
				else
				{
					delete m_pPopSaveFile;
					m_pPopSaveFile = NULL;
				}
			}
		}
	}

	void CScnMain::onSaveFile()
	{
		if (!AfxGetDbMgr()->GetBoolOpt(OPT_M_ENABLEUDISK))
		{
			CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_NOT_USE_PERSONAL_DISK));
			return;
		}

		GetPassword(m_strUDiskName, m_strUDiskPassWord);

		if (!m_strUDiskName.IsEmpty())
		{
			INT i = m_tableTabGame.getCurTouchFocusR();
			tagGameInfo* pGameInfo = NULL;
			customgame cust;

			if (m_bSearch == TRUE)
			{
				pGameInfo = m_search_games[i];
				cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGameInfo->gid);
			}
			else
			{
				hotgame pGame = m_games[i];

				cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGame.gid);
				if (pGame.type == 0)
				{
					pGameInfo = AfxGetDbMgr()->FindGameInfo(pGame.gid);
				}

			}

			if (m_pPopSaveFile == NULL)
			{
				m_pPopSaveFile = new CPopSaveFile(FALSE);

				if (m_pPopSaveFile->initialize())
				{
					if (pGameInfo != NULL)
					{
						m_pPopSaveFile->saveFile(pGameInfo->gid);
					}
					else
					{
						m_pPopSaveFile->saveFile(cust.gid);
					}

					m_pPopSaveFile->addEventListener(this);
					m_pPopSaveFile->doModal();
				}
				else
				{
					delete m_pPopSaveFile;
					m_pPopSaveFile = NULL;
				}
			}
		}
		else
		{
			CServer server;
			if (server.UDiskUnMount(false))
			{
			}
		
			if (m_pPopUdiskLogin == NULL)
			{
				m_bReadFile = FALSE;

				m_pPopUdiskLogin = new CPopUdiskLogin(FALSE);

				if (m_pPopUdiskLogin->initialize())
				{
					m_pPopUdiskLogin->setAutoQuit();
					m_pPopUdiskLogin->addEventListener(this);
					m_pPopUdiskLogin->doModal();
				}
				else
				{
					delete m_pPopUdiskLogin;
					m_pPopUdiskLogin = NULL;
				}
			}
		}
	}

	void CScnMain::onReadFile()
	{
		if (!AfxGetDbMgr()->GetBoolOpt(OPT_M_ENABLEUDISK))
		{
			CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_NOT_USE_PERSONAL_DISK));
			return;
		}

		GetPassword(m_strUDiskName, m_strUDiskPassWord);

		if (!m_strUDiskName.IsEmpty())
		{
			INT i = m_tableTabGame.getCurTouchFocusR();
			tagGameInfo* pGameInfo = NULL;
			customgame cust;

			if (m_bSearch == TRUE)
			{
				pGameInfo = m_search_games[i];
				cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGameInfo->gid);
			}
			else
			{
				hotgame pGame = m_games[i];

				cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(pGame.gid);
				if (pGame.type == 0)
				{
					pGameInfo = AfxGetDbMgr()->FindGameInfo(pGame.gid);
				}

			}

			if (m_pPopReadFile == NULL)
			{
				m_pPopReadFile = new CPopReadFile(FALSE);

				if (m_pPopReadFile->initialize())
				{
					if (pGameInfo != NULL)
					{
						m_pPopReadFile->readFile(pGameInfo->gid);
					}
					else
					{
						m_pPopReadFile->readFile(cust.gid);
					}

					m_pPopReadFile->addEventListener(this);
					m_pPopReadFile->doModal();
				}
				else
				{
					delete m_pPopReadFile;
					m_pPopReadFile = NULL;
				}
			}
		}
		else
		{
			CServer server;
			if (server.UDiskUnMount(false))
			{
			}

			if (m_pPopUdiskLogin == NULL)
			{
				m_bReadFile = TRUE;

				m_pPopUdiskLogin = new CPopUdiskLogin(FALSE);

				if (m_pPopUdiskLogin->initialize())
				{
					m_pPopUdiskLogin->setAutoQuit();
					m_pPopUdiskLogin->addEventListener(this);
					m_pPopUdiskLogin->doModal();
				}
				else
				{
					delete m_pPopUdiskLogin;
					m_pPopUdiskLogin = NULL;
				}
			}
		}

	}

	void CScnMain::onLeaveFile()
	{
	}

	void CScnMain::onCmdLine()
	{
		CString szCmdLine;

		if (m_hFileMap != NULL)
		{
			tagFileMap* pFileMap = (tagFileMap*)MapViewOfFile(m_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE,
				0, 0, sizeof(tagFileMap));
			if (pFileMap != NULL)
			{
				szCmdLine = pFileMap->szCmdLine;
				UnmapViewOfFile(pFileMap);
			}
		}

		Array2<CComponent*> aryEmpty;
		CContainer* pTopOverlay = dynamic_cast<CContainer*>(CBaseMainFrame::getInstance()->getTopOverlay(aryEmpty));

		if (pTopOverlay != NULL && !pTopOverlay->isLock())
		{
			if (pTopOverlay->getType() == CONFIG::CONTAINER_TYPE_POPUP)
			{
				pTopOverlay->destroy();
			}
		}

		CString param, szGid, szClass;
		AfxExtractSubString(param, szCmdLine.GetBuffer(), 0, TEXT('='));

		if (param.CompareNoCase(TEXT("/gid")) == 0)
		{
			AfxExtractSubString(szGid, szCmdLine.GetBuffer(), 1, TEXT('='));
			int gid = _ttoi(szGid);

			if (gid == -6)
			{
				OnClickLock(true);
				return;
			}
			else
			{
				char szdeskGid[10] = { 0 };
				_ltoa_s(gid, szdeskGid, 10);
				CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_GAME, POS_DESK, szdeskGid);
				char szPosId[10] = { 0 };
				_ltoa_s(m_arrySc[m_iCurSubClass]->id, szPosId, 10);

				//CString strParam;
				//AfxExtractSubString(strParam, szCmdLine, 2, TEXT('='));
				//if (_tcsnicmp(strParam, _T("silent"), 6) == 0)
					if(RunGame(gid, szPosId, _T(""), 2))
						return;
				//else
				//	RunGame(gid, szPosId);
			}

			
		}

		else if (param.CompareNoCase(TEXT("cgm://gid")) == 0)
		{
			//szCmdLine = L"cgm://gid=43655&param=silent&fh_position=yun_top_1";
			AfxExtractSubString(szGid, szCmdLine.GetBuffer(), 1, TEXT('='));
			int gid = _ttoi(szGid);

			char szdeskGid[10] = { 0 };
			_ltoa_s(gid, szdeskGid, 10);
			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_GAME, POS_DESK, szdeskGid);
			char szPosId[10] = { 0 };
			

			INT i = m_listMenu.getCurSel();

			vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();

			ASSERT(i >= 0 && i < (INT)arLeft.size());

			leftclass c = arLeft[i];

			_ltoa_s(c.id, szPosId, 10);

			BOOLEAN bFhpos = FALSE;

			CString	strFhpos;
			std::string strPosId;
			AfxExtractSubString(strFhpos, szCmdLine, 3, TEXT('='));

			if (strFhpos.GetLength() > 0)
			{
				if(strFhpos[strFhpos.GetLength() - 1] == _T('/'))
					strFhpos = strFhpos.Left(strFhpos.GetLength() - 1);

				strPosId = _bstr_t(strFhpos);
				bFhpos = TRUE;
			}
			CString strParam;
			AfxExtractSubString(strParam, szCmdLine, 2, TEXT('='));

			

			if (_tcsnicmp(strParam, _T("silent"), 6) == 0)
			{
				if (bFhpos == TRUE)
				{
					RunGame(gid, strPosId, _T(""), 2);
				}
				else
				{
					RunGame(gid, szPosId, _T(""), 2);
				}
			}
			else
			{
				if (bFhpos == TRUE)
					RunGame(gid, strPosId);
				else
					RunGame(gid, szPosId);
			}

			return;
		}

		else if (param.CompareNoCase(TEXT("/mouse")) == 0)
		{
			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, POS_DESK, TOOL_MOUSE);
			WinExec("rundll32.exe shell32.dll, Control_RunDLL main.cpl @0", SW_SHOWNORMAL);
			return;
		}

		else if (param.CompareNoCase(TEXT("/udisk")) == 0)
		{
			OnClickNetdisk(true);
		}
		else if (param.CompareNoCase(TEXT("top://class")) == 0)
		{
			AfxExtractSubString(szClass, szCmdLine.GetBuffer(), 1, TEXT('='));
			int id = _ttoi(szClass);
			CString	strUrl;
			AfxExtractSubString(strUrl, szCmdLine.GetBuffer(), 2, TEXT('='));

			CLogicManager::getInstance()->OnClickCategroyList(id, strUrl);

			m_funcWgtOpMenu.setVisible(FALSE);
		}
		else if (param.CompareNoCase(TEXT("/classid")) == 0)
		{
			AfxExtractSubString(szClass, szCmdLine.GetBuffer(), 1, TEXT('='));
			int id = _ttoi(szClass);
			CString	strUrl;
			AfxExtractSubString(strUrl, szCmdLine.GetBuffer(), 2, TEXT('='));
			CLogicManager::getInstance()->OnClickCategroyList(id, strUrl);

			m_funcWgtOpMenu.setVisible(FALSE);
		}

		else if (param.CompareNoCase(TEXT("/class")) == 0)
		{
			AfxExtractSubString(szClass, szCmdLine.GetBuffer(), 1, TEXT('='));			

			TCHAR szTest[MAX_PATH] = { 0 };
			::LoadString(::GetModuleHandle(NULL), IDS_NB_NAME, szTest, MAX_PATH);

			if (szClass.CompareNoCase(szTest) == 0)
			{
				m_listMenu.setCurSel(CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass().size());
				m_listMenu.setCurTouchFocus(CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass().size());
				m_listMenu.littleRefresh();
				onBtnListMenuItemTchRel(NULL);
			}

			int leftid = 0, subid = 0;
			if (CLogicManager::getInstance()->getMenuXml().GetClassNavBarId(szClass, leftid, subid))
			{
				INT iCurSel = -1;

				vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();

				for (size_t i = 0; i < arLeft.size(); i++)
				{
					if (arLeft[i].id == leftid)
					{
						iCurSel = (INT)i;
						break;
					}
				}

				m_listMenu.setCurSel(iCurSel);
				m_listMenu.setCurTouchFocus(iCurSel);
				m_listMenu.littleRefresh();
				m_funcWgtOpMenu.setVisible(FALSE);

				if (subid != 0)
				{
					OnClickCategroyList(leftid, 0, _T(""), true);

					char szDstid[256] = { 0 };
					_ltoa_s(subid, szDstid, 10);

					CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_NAV, POS_MENU, szDstid);

					leftclass c = arLeft[iCurSel];

					int ClassId = -1;
					vector<int> arGid;
					int order = 0, maxcount = 0;

					for (size_t i = 0; i < c.arSubClass.size(); i++)
					{
						m_arrySc[i]->pBtn->setSelected(FALSE);

						if (c.arSubClass[i].id == subid)
						{
							m_arrySc[i]->pBtn->setSelected(TRUE);
							ClassId = c.arSubClass[i].id;
							arGid = c.arSubClass[i].arGid;
							order = c.arSubClass[i].order;
							maxcount = c.arSubClass[i].maxcount;
						}

						m_arrySc[i]->pBtn->littleRefresh();
					}

					if (m_pBtnSearchResult != NULL)
					{
						m_pBtnSearchResult->setSelected(False);
						m_pBtnSearchResult->littleRefresh();
					}
					showGame(ClassId, arGid, order, maxcount);
				}
				else
				{
					onBtnListMenuItemTchRel(NULL);
				}
			}
		}

		HWND hWnd = CScreenTouchServer::getInstance()->getScreenContext()->hWnd;
		::ShowWindow(hWnd, SW_SHOWNORMAL);
		::UpdateWindow(hWnd);

		::SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		::SetForegroundWindow(hWnd);
		::SetFocus(hWnd);
	}

	// TODO: implement your normal private method here
	void CScnMain::showGame(INT id, vector<int>& arGid, INT order, INT maxcount)
	{
		vector<tagGameInfo*>	gameinfo;
		vector<hotgame>& games2 = CLogicManager::getInstance()->getMenuXml().GetHotGame(id);

		vector<hotgame>().swap(m_games);

		m_games.clear();

		std::map<DWORD, tagGameInfo*>& game_infos = AfxGetDbMgr()->m_mapGameInfo;
		std::map<DWORD, tagGameInfo*>::iterator iter = game_infos.begin();
		for (; iter != game_infos.end(); ++iter)
		{
			tagGameInfo* pGame = iter->second;
			
			for (size_t i = 0; i < arGid.size(); i++)
			{
				if (arGid[i] == pGame->gid)
				{
					gameinfo.push_back(pGame);
					break;
				}

			}
		}
		//这里需要先排序按传入的参数
		SortGameList<SORT_NAME>(gameinfo);
#if 1
		if (order == SORT_LCLICK)
			SortGameList<SORT_LCLICK>(gameinfo);
		else if (order == SORT_SCLICK)
			SortGameList<SORT_SCLICK>(gameinfo);
		else if (order == SORT_ADDTIME)
			SortGameList<SORT_ADDTIME>(gameinfo);
		else
			SortGameList<SORT_NAME>(gameinfo);
#endif

		int nStaticNpos = 1;
		int count = maxcount;


		for (int j = 0; j < (int)games2.size(); ++j)
		{
			hotgame item = games2[j];

			if (item.gid <= 100000 && AfxGetDbMgr()->FindGameInfo(item.gid) != NULL)
			{
				m_games.push_back(item);
			}
		}

		for (int i = 0; i < (int)gameinfo.size(); ++i)
		{
			tagGameInfo* pGame = gameinfo[i];

			bool bFind = false;

			for (int j = 0; j < (int)games2.size(); ++j)
			{
				hotgame item = games2[j];

				if (item.gid == pGame->gid)
				{
					bFind = true;
					break;
				}
			}

			if (bFind)
				continue;

			hotgame item;
			item.gid = pGame->gid;
			item.tag = L"";
			item.effect = L"";
			item.text = L"";

			for (int nPos = nStaticNpos; nPos <= count || count <= 0; nPos++)
			{
				bool bFindPos = false;

				for (int nSize = 0; nSize < (int)games2.size(); nSize++)
				{
					hotgame item = games2[nSize];
					if (item.pos == nPos)
					{
						bFindPos = true;
						break;
					}
				}
				if (!bFindPos)
				{
					item.pos = nPos;
					nStaticNpos = nPos + 1;
					break;
				}
			}

			item.type = 0;
			m_games.push_back(item);

			if (m_games.size() == count && count > 0)
				break;
		}

		// 重新排序
		std::sort(m_games.begin(), m_games.end());

		if (maxcount <= 0)
			maxcount = m_games.size();

		// 放入控件
		m_tableTabGame.getModel()->deleteAllItems();
		for (INT i = 0; i < maxcount && i<(int)m_games.size(); i++)
		//for (INT i = 0; i < 100; i++)
		{
			m_tableTabGame.getModel()->addItem(new CInt(i));
		}
		m_bSearch = FALSE;
		m_tableTabGame.refresh();
	}

	bool CScnMain::RunGame(DWORD gid, std::string pos, CString url, int type)
	{
#if 1
		if (type == 1)
		{
			HINSTANCE result = ::ShellExecute(NULL, L"open", url, NULL, NULL, SW_SHOW);
			if ((int)result <= HINSTANCE_ERROR)
				result = ::ShellExecute(NULL, NULL, L"iexplore.exe", url, NULL, SW_SHOW);
			return 0;
		}

		if (gid <= 100000)
		{
			CLogicManager::getInstance()->getUpLoadGameClick().addItemSvr(gid, 1);

			tagGameInfo*	pGame = AfxGetDbMgr()->FindGameInfo(gid);

			if (pGame != NULL)
			{
				TCHAR szVersizon[MAX_PATH] = { 0 };
				AfxGetFileVersion(TEXT("knbmenu.exe"), szVersizon);

				CString strVer = szVersizon;
				strVer = strVer.Right(4);

				if (strVer.CompareNoCase(L"4976") >= 0)
				{
					if (pGame->iWait == 0)
					{
						type = 2;
					}
				}
				CString szWorkDir = GetGameConfigFilePath(pGame);
				if (!PathFileExists(szWorkDir))
				{
					String str = String::format(L"%s\n %s", pGame->gname, (const CHAR*)CLanguage::getString(RES_LAG_FILE_NOT_EXIST));
					if (m_webBrsWeb.getVisible() == FALSE )
						CMainFrame::getInstance()->showTips(str);
					return false;
				}
			}
			else
			{
				if (m_webBrsWeb.getVisible() == FALSE )
					CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_GAME_NOT_DOWNLOAD));
				return false;
			}
		}
		else
		{
			customgame cust = CLogicManager::getInstance()->getMenuXml().GetCustomGame(gid);
			if (cust.gid == 0)
			{
				if (m_webBrsWeb.getVisible() == FALSE )
					CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_GAME_NOT_DOWNLOAD));
				return false;
			}
			else
			{				
				HINSTANCE result = ::ShellExecute(NULL, L"open", cust.url, NULL, NULL, SW_SHOW);
				if ((int)result <= HINSTANCE_ERROR)
					result = ::ShellExecute(NULL, NULL, L"iexplore.exe", cust.url, NULL, SW_SHOW);
				return 0;
			}
		}
#endif
		TCHAR szPath[MAX_PATH] = { 0 };
		AfxGetAppPath(szPath, MAX_PATH, _T("prerun.exe"));

		CString strParamer;
		OpenHMI::String strPos;

		OpenHMI::String::convertFromMBCS(pos.c_str(), strPos);

		strParamer.Format(_T("%d|%d|%s"), gid, type, (const CHAR*)strPos);
		//bool bResult = AfxRunProcess(szPath, strParamer);
		TCHAR szRootPath[MAX_PATH] = { 0 };
		AfxGetAppPath(szRootPath);

		HINSTANCE result = ShellExecute(NULL, _T("open"), szPath, strParamer, szRootPath, SW_SHOW);

		if ((int)result > HINSTANCE_ERROR)
		{
			bool is_hide_menu = AfxGetDbMgr()->GetIntOpt(OPT_M_HIDEMENU, 0L) >= 1;
			if (is_hide_menu)
			{
				CScreenTouchServer *pServer = CScreenTouchServer::getInstance();

				::ShowWindow(pServer->getScreenContext()->hWnd, SW_MINIMIZE);
			}
		}

		return true;
	}

	void CScnMain::onSubClass(INT pos, CComponent *pTarget)
	{
		m_iCurSubClass = pos;

		m_funcWgtOpMenu.setVisible(FALSE);
		if (m_pBtnSearchResult == pTarget)
		{
			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_NAV, POS_MENU, POS_MENUSEARCH);

			if (m_search_games.size() > 0)
			{
				m_tableTabGame.getModel()->deleteAllItems();
				for (int i = 0; i < (int)m_search_games.size(); ++i)
				{
					m_tableTabGame.getModel()->addItem(new CInt(i));
				}

				m_tableTabGame.setY(m_funcWgtFuncAd.getY());
				m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), 3 * m_tableTabGame.getCellHeight());
				m_scrollbarTable.setY(m_funcWgtFuncAd.getY());
				m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
				m_txtRcm.setVisible(FALSE);
				m_btnChange.setVisible(FALSE);
				m_bSearch = TRUE;
				m_tableTabGame.refresh();
			}
			else
			{
				m_txtRcm.setVisible(FALSE);
				m_btnChange.setVisible(FALSE);
				m_tableTabGame.setVisible(FALSE);
				m_scrollbarTable.setVisible(FALSE);
				m_funcWgtNoResult.setVisible(TRUE);
			}

			INT i = m_listMenu.getCurSel();
			vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();

			if (i < 0 || i >= (INT)arLeft.size())
			{
				m_iCurSubClass = 0;
				m_arrySc[0]->pBtn->getImageField(L"IMG")->setVisible(FALSE);
				m_arrySc[0]->pBtn->setSelected(FALSE);
				m_arrySc[0]->pBtn->littleRefresh();
			}
			else
			{
				leftclass c = arLeft[i];

				for (size_t i = 0; i < c.arSubClass.size(); i++)
				{
					m_arrySc[i]->pBtn->setSelected(FALSE);
					m_arrySc[i]->pBtn->getImageField(L"IMG")->setVisible(FALSE);
					m_arrySc[i]->pBtn->littleRefresh();
				}
			}
			m_webBrsWeb.show(FALSE);
			m_funcWgtAdRec.setVisible(TRUE);
			m_funcWgtFuncAd.setVisible(FALSE);
			m_pBtnSearchResult->getImageField(L"IMG")->setVisible(TRUE);
			m_pBtnSearchResult->setSelected(TRUE);
			m_pBtnSearchResult->littleRefresh();
		}
		else
		{
			char szDstid[256] = { 0 };
			_ltoa_s(m_arrySc[pos]->id, szDstid, 10);

			CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_NAV, POS_MENU, szDstid);

			INT i = m_listMenu.getCurSel();

			vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();

			ASSERT(i >= 0 && i < (INT)arLeft.size());
			leftclass c = arLeft[i];

			int ClassId = -1;
			vector<int> arGid;
			int order = 0, maxcount = 0, type = 0;
			CString url;

			for (size_t i = 0; i < c.arSubClass.size(); i++)
			{
				m_arrySc[i]->pBtn->setSelected(FALSE);
				m_arrySc[i]->pBtn->getImageField(L"IMG")->setVisible(FALSE);
				if (i == pos)
				{
					m_arrySc[i]->pBtn->setSelected(TRUE);
					m_arrySc[i]->pBtn->getImageField(L"IMG")->setVisible(TRUE);
					ClassId = c.arSubClass[i].id;
					arGid = c.arSubClass[i].arGid;
					order = c.arSubClass[i].order;
					maxcount = c.arSubClass[i].maxcount;
					type = c.arSubClass[i].type;
					url = c.arSubClass[i].url;
				}

				m_arrySc[i]->pBtn->littleRefresh();
			}

			m_tableTabGame.setVisible(TRUE);
			m_funcWgtNoResult.setVisible(FALSE);
			m_funcWgtFuncAd.setVisible(TRUE);
			m_funcWgtAdRec.setVisible(FALSE);

			if (m_bAdBar == TRUE)
			{		
				m_tableTabGame.setY(m_imgFootBg.getY() - 4 * m_tableTabGame.getCellHeight());
				m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), 4 * m_tableTabGame.getCellHeight());
				m_scrollbarTable.setY(m_tableTabGame.getY());
				m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
				m_txtRcm.setVisible(TRUE);
				m_btnChange.setVisible(TRUE);
			}
			else
			{
				m_txtRcm.setVisible(FALSE);
				m_btnChange.setVisible(FALSE);
				m_funcWgtFuncAd.setVisible(FALSE);
				m_tableTabGame.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
				m_tableTabGame.setPreferredSize(m_tableTabGame.getPreferredSize().getX(), m_imgFootBg.getY() - m_tableTabGame.getY());
				m_scrollbarTable.setY(m_imgLine.getY() + m_imgLine.getPreferredSize().getY());
				m_scrollbarTable.setPreferredSize(m_scrollbarTable.getPreferredSize().getX(), m_tableTabGame.getPreferredSize().getY());
			}

			if (m_pBtnSearchResult != NULL)
			{
				m_pBtnSearchResult->getImageField(L"IMG")->setVisible(FALSE);
				m_pBtnSearchResult->setSelected(False);
				m_pBtnSearchResult->littleRefresh();
			}

			if (type == 1)
			{
				m_webBrsWeb.show(TRUE);
				m_webBrsWeb.navigate2(url.GetBuffer());
				m_tableTabGame.getModel()->deleteAllItems();
				m_tableTabGame.refresh();
			}
			else
			{
				m_webBrsWeb.show(FALSE);
				showGame(ClassId, arGid, order, maxcount);
			}
		}
	}

	void CScnMain::checkGotoDefautLeftClass()
	{
		if (m_webBrsWeb.getVisible() == TRUE)
		{
			CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 1);
			CMain::getInstance()->pushMessage(pMessage);
		}
	}

	void CScnMain::OnClickCategroyList(int id, int pos, CString url, bool bFromDesk)
	{
		char szDstid[256] = { 0 };
		_ltoa_s(id, szDstid, 10);
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_NAV, bFromDesk ? POS_DESK : POS_MENU, szDstid);

		m_webBrsWeb.show(FALSE);
		for (UINT i = 0; i < m_arrySc.size(); i++)
		{
			m_arrySc[i]->pBtn->setVisible(FALSE);
		}

		vector<int> arGid;
		int order = 0, maxcount = 0, type = 0, ClassId = -1;
		CString strUrl;

		vector<subclass> arSubClass = CLogicManager::getInstance()->getMenuXml().GetSubClass(id);

		INT  pos_x = m_arrySc[0]->pBtn->getX();

		for (size_t i = 0; i < arSubClass.size(); i++)
		{
			m_arrySc[i]->id = arSubClass[i].id;

			CCtrlTextField *pText = m_arrySc[i]->pBtn->getTextField();
			m_arrySc[i]->pBtn->setCaption(arSubClass[i].name.GetBuffer());
			INT w = pText->getContent().length()*pText->getFontSize();

			pText->setPreferredSize(w, pText->getPreferredSize().getY());
			m_arrySc[i]->pBtn->getImageField(L"IMG")->setPreferredSize(w, m_arrySc[i]->pBtn->getPreferredSize().getY());
			m_arrySc[i]->pBtn->getImageField(L"IMG")->setVisible(FALSE);
			m_arrySc[i]->pBtn->setPreferredSize(w, m_arrySc[i]->pBtn->getPreferredSize().getY());
			m_arrySc[i]->pBtn->setX(pos_x);

			if (arSubClass[i].effect.GetLength() > 0)
			{
				CMovieClip* pTag = new CMovieClip();
				pTag->addImage(CPath::getApplicationFolder() + L"config\\res\\" + arSubClass[i].effect.GetBuffer());
				m_arrySc[i]->pBtn->getImageField(L"IMG_TAG")->setUserMovieClip(pTag);
				m_arrySc[i]->pBtn->getImageField(L"IMG_TAG")->setX(w);
			}
			else
			{
				m_arrySc[i]->pBtn->getImageField(L"IMG_TAG")->setUserMovieClip(NULL);
			}

			pos_x = pos_x + w + 40;

			m_arrySc[i]->pBtn->setVisible(TRUE);
			m_arrySc[i]->pBtn->setSelected(FALSE);

			if (i == pos)
			{
				m_arrySc[i]->pBtn->setSelected(TRUE);
				m_arrySc[i]->pBtn->getImageField(L"IMG")->setVisible(TRUE);
				ClassId = arSubClass[i].id;
				arGid = arSubClass[i].arGid;
				order = arSubClass[i].order;
				maxcount = arSubClass[i].maxcount;
				type = arSubClass[i].type;
				strUrl = arSubClass[i].url;
			}
			m_arrySc[i]->pBtn->littleRefresh();
		}

		m_arrySc[arSubClass.size()]->pBtn->setX(pos_x);

		m_iCurSubClass = 0;
		m_bSearch = FALSE;
		m_pBtnSearchResult = NULL;
		m_funcWgtAdRec.setVisible(FALSE);
		m_funcWgtNoResult.setVisible(FALSE);

		if (type == 1 || url.GetLength() > 0)
		{
			m_webBrsWeb.show(TRUE);
			if (url.GetLength() > 0)
				m_webBrsWeb.navigate2(url.GetBuffer());
			else
				m_webBrsWeb.navigate2(strUrl.GetBuffer());

			m_tableTabGame.getModel()->deleteAllItems();
			m_tableTabGame.refresh();
		}
		else
		{		
			showGame(ClassId, arGid, order, maxcount);
		}
	}

	void CScnMain::OnClickLock(bool bFromDesk)
	{
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, bFromDesk ? POS_DESK : POS_MENU, TOOL_LOCK);
		TCHAR szFile[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, szFile, MAX_PATH);
		PathRemoveFileSpec(szFile);
		PathAddBackslash(szFile);
		lstrcat(szFile, TEXT("LockScreen.exe"));

		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		ShellExecute(pServer->getScreenContext()->hWnd, TEXT("open"), szFile, TEXT(""), TEXT(""), SW_SHOWNORMAL);
	}

	void CScnMain::OnClickNetdisk(bool bFromDesk)
	{
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, bFromDesk ? POS_DESK : POS_MENU, TOOL_OPENUDISK);

		checkGotoDefautLeftClass();

		if (!AfxGetDbMgr()->GetBoolOpt(OPT_M_ENABLEUDISK))
		{
			CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_NOT_USE_PERSONAL_DISK));
			return;
		}

		GetPassword(m_strUDiskName, m_strUDiskPassWord);

		if (!m_strUDiskName.IsEmpty())
		{
			if (m_pPopUdisk == NULL)
			{
				m_pPopUdisk = new CPopUdisk(FALSE);
				if (m_pPopUdisk->initialize())
				{
					m_pPopUdisk->addEventListener(this);
					m_pPopUdisk->doModal();
				}
				else
				{
					delete m_pPopUdisk;
					m_pPopUdisk = NULL;
				}
			}
		}
		else
		{
			CServer server;
			if (server.UDiskUnMount(false))
			{
			}

			if (m_pPopUdiskLogin == NULL)
			{
				m_pPopUdiskLogin = new CPopUdiskLogin(FALSE);

				if (m_pPopUdiskLogin->initialize())
				{
					m_pPopUdiskLogin->addEventListener(this);
					m_pPopUdiskLogin->doModal();
				}
				else
				{
					delete m_pPopUdiskLogin;
					m_pPopUdiskLogin = NULL;
				}
			}
		}

	}

	void CScnMain::onKeyTab()
	{	
		if (m_editSearchBox.getVisible() == TRUE)
		{
			m_editSearchBox.setFocus(TRUE);
		}
	}

	void CScnMain::onTimer(const CTimer *pTimer, UINT uiCurrentCount)
	{

	}

	void CScnMain::onTimerComplete(const CTimer *pTimer)
	{
		if (m_btnAd.getVisible() == TRUE)
			m_btnAd.getImageField(L"IMG_DH")->play(40);

		long l = kcore::GetLocalLongFun(L"ENABLE_CLIENTLOGIN", 1);
		
		

		INT iCurSel = -1;

		vector<leftclass> arLeft = CLogicManager::getInstance()->getMenuXml().GetXmlLeftClass();
		for (size_t i = 0; i < arLeft.size(); i++)
		{
			if (arLeft[i].id == DEFAUT_LEFT_CLASS_ID)
			{
				iCurSel = (INT)i;
			}
		}

		if (iCurSel < 0)
			iCurSel = arLeft.size();

		m_listMenu.setCurSel(iCurSel);
		m_listMenu.setCurTouchFocus(iCurSel);
		onBtnListMenuItemTchRel(NULL);
		m_listMenu.littleRefresh();

		onCmdLine();
	}
}