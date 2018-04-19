#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtOpMenu::CFuncWgtOpMenu()
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_btnRun);
		addComponent(&m_btnOpen);
		addComponent(&m_btnIntro);
		addComponent(&m_btnSaveProgress);
		addComponent(&m_btnReadProgress);
		addComponent(&m_btnUploadConfig);
		//addComponent(&m_imgJiange);


		// TODO: add event listener to children controls here
		m_btnRun.addEventListener(this);
		m_btnOpen.addEventListener(this);
		m_btnIntro.addEventListener(this);
		m_btnSaveProgress.addEventListener(this);
		m_btnReadProgress.addEventListener(this);
		m_btnUploadConfig.addEventListener(this);


		// TODO: initialize children controls specific properties here
	}

	CFuncWgtOpMenu::~CFuncWgtOpMenu()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtOpMenu)
		CONTROL_EVENT_HANDLER(&m_btnRun, CEventButton::TOUCH_RELEASED, onBtnRunTchRel)
		CONTROL_EVENT_HANDLER(&m_btnOpen, CEventButton::TOUCH_RELEASED, onBtnOpenTchRel)
		CONTROL_EVENT_HANDLER(&m_btnIntro, CEventButton::TOUCH_RELEASED, onBtnIntroTchRel)
		CONTROL_EVENT_HANDLER(&m_btnSaveProgress, CEventButton::TOUCH_RELEASED, onBtnSaveProgressTchRel)
		CONTROL_EVENT_HANDLER(&m_btnReadProgress, CEventButton::TOUCH_RELEASED, onBtnReadProgressTchRel)
		CONTROL_EVENT_HANDLER(&m_btnUploadConfig, CEventButton::TOUCH_RELEASED, onBtnUploadConfigTchRel)

	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtOpMenu::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener


		return TRUE;
	}

	void CFuncWgtOpMenu::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.


	}

	void CFuncWgtOpMenu::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_OP_MENU);
		INIT::initButton(m_btnRun, RES_ITM_BTN_RUN, this);
		INIT::initButton(m_btnOpen, RES_ITM_BTN_OPEN, this);
		INIT::initButton(m_btnIntro, RES_ITM_BTN_INTRO, this);
		INIT::initButton(m_btnSaveProgress, RES_ITM_BTN_SAVE_PROGRESS, this);
		INIT::initButton(m_btnReadProgress, RES_ITM_BTN_READ_PROGRESS, this);
		INIT::initButton(m_btnUploadConfig, RES_ITM_BTN_UPLOAD_CONFIG, this);
		INIT::initImageField(m_imgJiange, RES_ITM_JIANGE, this);


		// TODO: set data to children controls or set children controls status here
	}

	ICursorTracker* CFuncWgtOpMenu::probe(INT iX, INT iY)
	{
		if(hitTest(iX, iY) == TRUE)
		{
			return this;
		}
		else
		{
			if(getVisible() == TRUE)
			{
				setVisible(FALSE);
			}
			return NULL;
		}
	}

	// TODO: implement your public method here
	void CFuncWgtOpMenu::showGameMenu(int gid, const String& name, const String& url)
	{
		m_iGid = gid;
		m_strName = name;
		m_strUrl = url;

		m_btnOpen.setEnable(FALSE);
		m_btnIntro.setEnable(FALSE);
		m_btnSaveProgress.setEnable(FALSE);
		m_btnReadProgress.setEnable(FALSE);
		m_btnUploadConfig.setEnable(FALSE);

		if (m_strUrl.length() > 0)
		{
			;
		}
		else
		{
			m_btnOpen.setEnable(TRUE);
			m_btnIntro.setEnable(TRUE);

			tagGameInfo* pGame = AfxGetDbMgr()->FindGameInfo(gid);

			if (lstrlen(pGame->gprogress) > 0)
			{
				m_btnSaveProgress.setEnable(TRUE);
				m_btnReadProgress.setEnable(TRUE);
			}
			if (lstrlen(pGame->gConfig) > 0)
			{
				m_btnUploadConfig.setEnable(TRUE);
			}
		}

		m_btnOpen.littleRefresh();
		m_btnIntro.littleRefresh();
		m_btnSaveProgress.littleRefresh();
		m_btnReadProgress.littleRefresh();
		m_btnUploadConfig.littleRefresh();
	}

	// TODO: implement control event listener here
	void CFuncWgtOpMenu::onBtnRunTchRel(CComponent *pTarget)
	{
		tagGameInfo*	pGame = AfxGetDbMgr()->FindGameInfo(m_iGid);

		CLogicManager::getInstance()->RunGame(true, m_iGid, (const CHAR*)m_strUrl);
		this->setVisible(FALSE);
	}

	void CFuncWgtOpMenu::onBtnOpenTchRel(CComponent *pTarget)
	{
		CString			strTmp;
		tagGameInfo*	pGame = AfxGetDbMgr()->FindGameInfo(m_iGid);
		HWND hWnd = CScreenTouchServer::getInstance()->getScreenContext()->hWnd;

		this->setVisible(FALSE);
		if (pGame != NULL)
		{
			if (pGame->runtype == GAME_RUNTYPE_UPDATE)
			{
				// 本地更新的游戏，打开更新目录，而不是挂盘游戏目录
				DWORD_PTR n = (DWORD_PTR)ShellExecute(hWnd, TEXT("open"), pGame->gclipath, TEXT(""), TEXT(""), SW_SHOWNORMAL);
				if (n <= 32)
				{
					String str = String::format(L"%s\n %s", (const CHAR*)m_strName, (const CHAR*)CLanguage::getString(RES_LAG_FILE_NOT_EXIST));

					CMainFrame::getInstance()->showTips(str);
				}
				return;
			}

			CString szWorkDir = GetGameConfigFilePath(pGame);
			if (!PathFileExists(szWorkDir))
			{
				String str = String::format(L"%s\n %s", (const CHAR*)m_strName, (const CHAR*)CLanguage::getString(RES_LAG_FILE_NOT_EXIST));

				CMainFrame::getInstance()->showTips(str);
			}
			else
			{
				DWORD_PTR n = (DWORD_PTR)ShellExecute(hWnd, TEXT("open"), szWorkDir, TEXT(""), TEXT(""), SW_SHOWNORMAL);
				if (n <= 32)
				{
					String str = String::format(L"%s\n %s", (const CHAR*)m_strName, (const CHAR*)CLanguage::getString(RES_LAG_FILE_NOT_EXIST));
					CMainFrame::getInstance()->showTips(str);					
				}
			}
		};
	}

	void CFuncWgtOpMenu::onBtnIntroTchRel(CComponent *pTarget)
	{
		HWND hWnd = CScreenTouchServer::getInstance()->getScreenContext()->hWnd;
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, POS_MENU, TOOL_SEARCHGAME);
		CString str;
		str.Format(TEXT("http://www.baidu.com/s?wd=%s"), (LPCTSTR)m_strName);
		HINSTANCE result = ShellExecute(hWnd, TEXT("open"), str, NULL, NULL, SW_SHOWNORMAL);

		if ((int)result <= HINSTANCE_ERROR)
			result = ::ShellExecute(NULL, NULL, L"iexplore.exe", str, NULL, SW_SHOWNORMAL);

		this->setVisible(FALSE);
	}

	void CFuncWgtOpMenu::onBtnSaveProgressTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, POS_MENU, TOOL_SAVEPROFILE);
		this->setVisible(FALSE);
		CLogicManager::getInstance()->saveFile();
	}

	void CFuncWgtOpMenu::onBtnReadProgressTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, POS_MENU, TOOL_READPROFILE);
		tagGameInfo*	pGame = AfxGetDbMgr()->FindGameInfo(m_iGid);
		
		this->setVisible(FALSE);
		if (lstrlen(pGame->gprogress) < 1)
		{
			CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_FILE_NOT_EXIST));
			return;
		}

		CLogicManager::getInstance()->readFile();
	}

	void CFuncWgtOpMenu::onBtnUploadConfigTchRel(CComponent *pTarget)
	{
		this->setVisible(FALSE);
		CLogicManager::getInstance()->uploadCfg();		
	}
	// TODO: implement logic event listener here

	// TODO: implement your normal private method here
}