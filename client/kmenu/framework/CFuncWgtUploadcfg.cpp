#include "HMI.h"
#include "OSFDlg.h"
#include "resource.h"

bool AfxIsValidDirName(tstring strDir)
{
	if (strDir.size() < 2)
		return false;
	if (!(_T('A') <= strDir[0] && strDir[0] <= _T('Z') ||
		_T('a') <= strDir[0] && strDir[0] <= _T('z')))
		return false;
	if (!isalpha(strDir[0]) || strDir[1] != _T(':'))
		return false;
	size_t nDoubleSlash = strDir.find(_T("\\\\"));
	if (nDoubleSlash != tstring::npos)
		return false;

	if (*strDir.rbegin() != _T('\\'))
		strDir += _T("\\");
	size_t nStart = strDir.find(_T('\\'));
	if (nStart != tstring::npos)
	{
		size_t nEnd = 0;
		while ((nEnd = strDir.find_first_of(_T('\\'), nStart + 1)) != tstring::npos)
		{
			tstring sub = strDir.substr(nStart + 1, nEnd - nStart - 1);
			if (sub.size() == 0 || sub.find_first_of(_T("<>:\"/|?*'")) != tstring::npos)
				return false;
			nStart = nEnd;
		}
	}
	return true;
}


namespace OpenHMI
{
	CFuncWgtUploadcfg::CFuncWgtUploadcfg()
		: m_pPopTips(NULL)
		, m_Gid(0)
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_txtTitle);
		addComponent(&m_imgName);
		addComponent(&m_btnQuit);
		addComponent(&m_btnUpload);
		addComponent(&m_btnCancel);
		addComponent(&m_txtName);
		addComponent(&m_txtPath);
		addComponent(&m_editName);
		addComponent(&m_txtTips);
		addComponent(&m_btnSel);
		addComponent(&m_imgBg);
		addComponent(&m_txtPathName);


		// TODO: add event listener to children controls here
		m_btnQuit.addEventListener(this);
		m_btnUpload.addEventListener(this);
		m_btnCancel.addEventListener(this);
		m_btnSel.addEventListener(this);


		// TODO: initialize children controls specific properties here
	}

	CFuncWgtUploadcfg::~CFuncWgtUploadcfg()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtUploadcfg)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::TOUCH_RELEASED, onBtnQuitTchRel)
		CONTROL_EVENT_HANDLER(&m_btnUpload, CEventButton::TOUCH_RELEASED, onBtnUploadTchRel)
		CONTROL_EVENT_HANDLER(&m_btnCancel, CEventButton::TOUCH_RELEASED, onBtnCancelTchRel)
		CONTROL_EVENT_HANDLER(&m_btnSel, CEventButton::TOUCH_RELEASED, onBtnSelTchRel)
		POPUP_EVENT_HANDLER(m_pPopTips, CEventPopup::DESTROYED, onPopTipsDsy)

		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ENTER, onKeyEnter)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ESC, onKeyEsc)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_TAB, onKeyTab)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtUploadcfg::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener


		return TRUE;
	}

	void CFuncWgtUploadcfg::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.


	}

	void CFuncWgtUploadcfg::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_UPLOADCFG);
		INIT::initTextField(m_txtTitle, RES_ITM_TITLE, this);
		INIT::initImageField(m_imgName, RES_ITM_IMG_NAME, this);
		INIT::initButton(m_btnQuit, RES_ITM_BTN_QUIT, this);
		INIT::initButton(m_btnUpload, RES_ITM_BTN_UPLOAD, this);
		INIT::initButton(m_btnCancel, RES_ITM_BTN_CANCEL, this);
		INIT::initTextField(m_txtName, RES_ITM_TXT_NAME, this);
		INIT::initTextField(m_txtPath, RES_ITM_TXT_PATH, this);
		INIT::initEdit(m_editName, RES_ITM_EDIT_NAME, this);
		INIT::initTextField(m_txtTips, RES_ITM_TXT_TIPS, this);
		INIT::initButton(m_btnSel, RES_ITM_BTN_SEL, this);
		INIT::initTextField(m_txtPathName, RES_ITM_TXT_PATH_NAME, this);
		INIT::initImageField(m_imgBg, RES_ITM_IMG_BG, this);


		// TODO: set data to children controls or set children controls status here
		m_editName.setContent(L"");
		m_editName.setLimitText(12);
		m_editName.setFont(15, RGB(0, 0, 0), L"Microsoft YaHei");
		m_editName.setBkColor(RGB(255, 255, 255));
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here
	void CFuncWgtUploadcfg::onBtnQuitTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leaveCfg();
	}

	void CFuncWgtUploadcfg::onBtnUploadTchRel(CComponent *pTarget)
	{
		CString strName = m_editName.getContent();
		strName.Trim();

		if (strName.IsEmpty())
		{
			m_txtTips.setContent(CLanguage::getString(RES_LAG_NAME_EMPTY));
			m_editName.setFocus(TRUE);

			return;
		}

		CString strPath = m_strPath;
		strPath.Trim();

		if (strPath.IsEmpty())
		{
			m_txtTips.setContent(CLanguage::getString(RES_LAG_PATH_EMPTY));
			return;
		}

		if (!PathFileExists(strPath))
		{
			m_txtTips.setContent(CLanguage::getString(RES_LAG_FILE_NOT_EXIST));
			return;
		}

		quit();
		CString FileName = PathFindFileName(strPath);
		CString strDstPath;
		strDstPath.Format(_T("geffect\\%d\\%u\\%s"), m_Gid, GetTickCount(), FileName);
		CServer server;
		if (!server.UploadFile(strPath, strDstPath))
		{
			showTips(CLanguage::getString(RES_LAG_UPLOAD_FAILED));
			return;
		}
		TCHAR szPcName[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
		DWORD dwSize = _countof(szPcName);
		GetComputerName(szPcName, &dwSize);
		_tcsupr_s(szPcName);
		CString strError;
		if (!server.AddGameConfig(m_Gid, strName, strDstPath, szPcName, strError))
		{
			showTips(strError.GetBuffer());
			return;
		}
		
		showTips(CLanguage::getString(RES_LAG_UPLOAD_SUCCESS));
	}

	void CFuncWgtUploadcfg::onBtnCancelTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leaveCfg();
	}

	void CFuncWgtUploadcfg::onBtnSelTchRel(CComponent *pTarget)
	{
		HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, threadProc, this, 0, NULL));

		if (hThread != NULL)
		{
			::SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
		}
	}


	// TODO: implement logic event listener here
	void CFuncWgtUploadcfg::onKeyEnter()
	{
		onBtnUploadTchRel(NULL);
	}

	void CFuncWgtUploadcfg::onKeyEsc()
	{
		CMainFrame::getInstance()->quitTips();
		onBtnCancelTchRel(NULL);
	}

	void CFuncWgtUploadcfg::onKeyTab()
	{
		m_editName.setFocus(TRUE);
	}

	void CFuncWgtUploadcfg::onPopTipsDsy(CBasePopup *pTarget)
	{
		onBtnCancelTchRel(NULL);
		m_pPopTips = NULL;
	}

	// TODO: implement your normal private method here

	void CFuncWgtUploadcfg::showTips(const String& str)
	{
		if (m_pPopTips == NULL)
		{
			m_pPopTips = new CPopTips(FALSE);
			if (m_pPopTips->initialize())
			{
				m_pPopTips->setContent(str, FALSE);
				m_pPopTips->addEventListener(this);
				m_pPopTips->doModal();
			}
			else
			{
				delete m_pPopTips;
				m_pPopTips = NULL;
			}
		}
	}

	UINT __stdcall CFuncWgtUploadcfg::threadProc(void *pParam)
	{
		CFuncWgtUploadcfg *pFunc = static_cast<CFuncWgtUploadcfg*>(pParam);

		HWND hWnd = CScreenTouchServer::getInstance()->getScreenContext()->hWnd;
		tagGameInfo*	pGame = AfxGetDbMgr()->FindGameInfo(pFunc->m_Gid);
		if (pGame == NULL)
		{
			pFunc->m_txtTips.setContent(CLanguage::getString(RES_LAG_GAME_INFO_ERROR));
			return 0;
		}
		
		CString strFilter = pGame->gConfig;
		CString strName = PathFindFileName(strFilter);
		TCHAR szFilter[MAX_PATH] = { 0 };
		TCHAR szEdfExt[MAX_PATH] = { 0 };

		TCHAR szTest[MAX_PATH] = { 0 };

		::LoadString(::GetModuleHandle(NULL), IDS_ALL_FILE, szTest, MAX_PATH);
		
		_stprintf_s(szFilter, MAX_PATH, szTest, strName, strName);
		_stprintf_s(szEdfExt, MAX_PATH, _T("%s| *.*||"), strName);
		size_t length = _tcslen(szFilter);
		for (size_t i = 0; i < length; i++)
		{
			if (szFilter[i] == _T('|'))
				szFilter[i] = _T('\0');
		}
		length = _tcslen(szEdfExt);
		for (size_t i = 0; i < _tcslen(szEdfExt); i++)
		{
			if (szEdfExt[i] == _T('|'))
				szEdfExt[i] = _T('\0');
		}

		CString strGameDir = GetGameConfigFilePath(pGame);
		CString root, match, dir;
		CString strAllPath = AfxUDExpandPath2(strFilter, strGameDir, root, match, dir);
	
		if (!AfxIsValidDirName((LPCTSTR)strAllPath))
		{
			root = strGameDir;
			strAllPath = strGameDir + pGame->gConfig;
		}
		
		CString str;
		
		if(PathFileExists(strAllPath))
			str = strAllPath;

		COSFDialog osfDlg(hWnd);
		//ShowWindow(hWnd, SW_HIDE);
		//CScreenTouchServer::getInstance()->setForceHide(TRUE);
		if(osfDlg.FileOpenDlg(szFilter, szEdfExt,str.GetBuffer(),root.GetBuffer(), NULL, FALSE))
		{
			CString str;
			str = osfDlg.GetFileName();
			pFunc->m_strPath = str;

			String strName = str.GetBuffer();
			if (strName.length() >= 22)
			{
				String s = strName.subString(0, 21) + L"...";
				pFunc->m_txtPathName.setContent(s);
			}
			else
			{
				pFunc->m_txtPathName.setContent(strName);
			}
		}
		//CScreenTouchServer::getInstance()->setForceHide(FALSE);
		//ShowWindow(hWnd, SW_SHOW);
		return 0;
	}
}