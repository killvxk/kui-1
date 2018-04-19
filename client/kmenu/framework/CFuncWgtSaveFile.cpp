#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtSaveFile::CFuncWgtSaveFile()
		: m_Gid(0)
		, m_pPopTips(NULL)
		, m_bSelAll(FALSE)
		, m_bSaveOK(FALSE)
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_txtTitle);
		addComponent(&m_btnSelAll);
		addComponent(&m_btnSave);
		addComponent(&m_btnCancel);
		addComponent(&m_listFile);
		addComponent(&m_txtPath);
		addComponent(&m_btnQuit);
		addComponent(&m_scrollbarList);


		// TODO: add event listener to children controls here
		m_btnSelAll.addEventListener(this);
		m_btnSave.addEventListener(this);
		m_btnCancel.addEventListener(this);
		m_listFile.addEventListener(this);
		m_btnQuit.addEventListener(this);
		m_scrollbarList.addEventListener(this);


		// TODO: initialize children controls specific properties here
		m_listFile.setAssociateScrollbar(&m_scrollbarList, FALSE);
		m_scrollbarList.setTargetComponent(&m_listFile);
		m_listFile.setView(this);
	}

	CFuncWgtSaveFile::~CFuncWgtSaveFile()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtSaveFile)
		CONTROL_EVENT_HANDLER(&m_btnSelAll, CEventButton::TOUCH_RELEASED, onBtnSelAllTchRel)
		CONTROL_EVENT_HANDLER(&m_btnSave, CEventButton::TOUCH_RELEASED, onBtnSaveTchRel)
		CONTROL_EVENT_HANDLER(&m_btnCancel, CEventButton::TOUCH_RELEASED, onBtnCancelTchRel)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::TOUCH_RELEASED, onBtnQuitTchRel)
		CONTROL_EVENT_HANDLER_CHAIN(&m_listFile, CEventButton::TOUCH_RELEASED, onBtnListFileItemTchRel)
		POPUP_EVENT_HANDLER(m_pPopTips, CEventPopup::DESTROYED, onPopTipsDsy)

		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ENTER, onKeyEnter)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ESC, onKeyEsc)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtSaveFile::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener

		m_listFile.setModel(&m_aryData);
		CScreenTouchServer::getInstance()->addEventListener(this);

		return TRUE;
	}

	void CFuncWgtSaveFile::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_listFile.setModel(NULL);
		m_aryData.clear();
		m_aryFileInfo.clear();
		CScreenTouchServer::getInstance()->removeEventListener(this);
	}

	void CFuncWgtSaveFile::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_SAVE_FILE);
		INIT::initTextField(m_txtTitle, RES_ITM_TITLE, this);
		INIT::initButton(m_btnSelAll, RES_ITM_BTN_SEL_ALL, this);
		INIT::initButton(m_btnSave, RES_ITM_BTN_SAVE, this);
		INIT::initButton(m_btnCancel, RES_ITM_BTN_CANCEL, this);
		INIT::initList(m_listFile, RES_ITM_FILE, this);
		INIT::initTextField(m_txtPath, RES_ITM_PATH, this);
		INIT::initButton(m_btnQuit, RES_ITM_BTN_QUIT, this);
		INIT::initScrollbar(m_scrollbarList, RES_ITM_SCROLLBAR_LIST, this);


		// TODO: set data to children controls or set children controls status here
		tagGameInfo* pGame = AfxGetDbMgr()->FindGameInfo(m_Gid);

		if (pGame)
		{
			m_txtTitle.setContent(String::format((const CHAR*)CLanguage::getString(RES_LAG_KEEP_SAVE), pGame->gname));
		}

		FindClienPath(pGame, pGame->gclipath, _countof(pGame->gclipath));

		CString strGamePath = pGame->gclipath;

		CString strFilter = pGame->gprogress;
		strFilter.Trim();

		
		CString info;

		m_aryData.clear();
		m_aryFileInfo.clear();

		if (PathFileExists(strFilter))
		{
			info = strFilter + L"*.*";
			AfxUDGetAllFile2(info, m_aryFileInfo, strFilter, NULL, NULL);
		}
		else
		{
			for (int idx = 0;; idx++)
			{
				CString szItem;
				AfxExtractSubString(szItem, strFilter, idx, TEXT('|'));
				if (szItem.IsEmpty())
					break;

				if (idx > 0)
					info += L"|";
				CString root, match, dir;
				CString path = AfxUDExpandPath2(szItem, strGamePath, root, match, dir);
				info += path;

				AfxUDGetAllFile2(path, m_aryFileInfo, root, match, dir);
			}
		}

		String str = info.GetBuffer();

		if (str.length() > 60)
		{
			String strTmp = str.subString(0, 60) + L"...";
			m_txtPath.setContent(strTmp);
		}
		else
			m_txtPath.setContent(str);

		for (size_t i = 0; i < m_aryFileInfo.size(); i++)
		{
			m_aryData.addItem(new CInt(i));
		}
	}

	// Definition functions for interface IViewList_Simple
	CComponent* CFuncWgtSaveFile::getItemComponent(CCtrlList_Simple* pList)
	{
		CComponent* pView = NULL;
		pView = new CCtrlButton();
		INIT::initButton(*((CCtrlButton*)pView), RES_ITM_BTN_ITEM, this);
		pView->addEventListener(this);
		pView->refresh();

		return pView;
	}

	void CFuncWgtSaveFile::drawItem(CCtrlList_Simple* pList, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus)
	{
		CInt* pData = (CInt*)pObjItem;

		ASSERT(*pData >= 0 && *pData < (INT)m_aryFileInfo.size());

		CCtrlButton *pBtn = (CCtrlButton*)pComponent;

		String str = m_aryFileInfo[*pData].strName.GetString();
		if (str.length() > 48)
		{
			String strTmp = str.subString(0, 48) + L"...";
			pBtn->getTextField(L"NAME")->setContent(strTmp);
		}
		else
			pBtn->getTextField(L"NAME")->setContent(str);

		pBtn->getTextField(L"SIZE")->setContent(m_aryFileInfo[*pData].strSize.GetString());

		if (m_aryFileInfo[*pData].bSel)
			pBtn->getImageField()->setUserMovieClip(ENV::getMovieClip(RES_IMG_COMMONIMAGE_SEL_FILE));
		else
			pBtn->getImageField()->setUserMovieClip(ENV::getMovieClip(RES_IMG_COMMONIMAGE_UNSEL_FILE));
	}


	// TODO: implement your public method here

	// TODO: implement control event listener here
	void CFuncWgtSaveFile::onBtnSelAllTchRel(CComponent *pTarget)
	{
		if (m_bSelAll == FALSE)
		{
			for (size_t i = 0; i < m_aryFileInfo.size(); i++)
			{
				m_aryFileInfo[i].bSel = true;
			}

			m_btnSelAll.setCaption(CLanguage::getString(RES_LAG_DESEL_ALL));
			m_bSelAll = TRUE;
		}
		else
		{
			for (size_t i = 0; i < m_aryFileInfo.size(); i++)
			{
				m_aryFileInfo[i].bSel = false;
			}

			m_btnSelAll.setCaption(CLanguage::getString(RES_LAG_SEL_ALL));
			m_bSelAll = FALSE;
		}

		m_listFile.littleRefresh();
	}

	void CFuncWgtSaveFile::onBtnSaveTchRel(CComponent *pTarget)
	{
		INT iSelCount = 0;

		for (size_t i = 0; i < m_aryFileInfo.size(); i++)
		{
			if (m_aryFileInfo[i].bSel)
				++iSelCount;
		}

		if (iSelCount == 0)
		{
			if (m_pPopTips == NULL)
			{
				m_pPopTips = new CPopTips(FALSE);
				if (m_pPopTips->initialize())
				{
					m_pPopTips->setContent(CLanguage::getString(RES_LAG_SEL_SAVE));
					m_pPopTips->addEventListener(this);
					m_pPopTips->doModal();
				}
				else
				{
					delete m_pPopTips;
					m_pPopTips = NULL;
				}
			}
			return;
		}

		tagGameInfo* pGame = AfxGetDbMgr()->FindGameInfo(m_Gid);

		if (pGame == NULL) return;

		CString strGameName = pGame->gname;
		CString strGamePath = pGame->gclipath;

		CString strTmp;
		CString alldir;
		CString time;
		long count = 0;
		AfxUDGetAllSaveDir2(strGameName, alldir);
		for (int idx = 0;; idx++)
		{
			CString szItem;
			AfxExtractSubString(szItem, alldir, idx, TEXT('|'));
			if (szItem.IsEmpty())
				break;

			count++;
			if (time.IsEmpty() || time > szItem)
			{
				time = szItem;
			}
		}

		strTmp = (const CHAR*)CLanguage::getString(RES_LAG_GAME_SAVE);

		int max_count = AfxGetDbMgr()->GetIntOpt(OPT_M_ARCHIVECOUNT);
		if (count >= max_count)
		{
			TCHAR path[MAX_PATH] = { 0 };
			_stprintf_s(path, TEXT("%c:\\%s\\%s\\%s\\*"), AfxGetDbMgr()->GetUDiskUnmoutDriver(), (LPCTSTR)strTmp, (LPCTSTR)strGameName, (LPCTSTR)time);

			SHFILEOPSTRUCT fo = { 0 };
			fo.wFunc = FO_DELETE;
			fo.pFrom = path;
			fo.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NO_UI;
			SHFileOperation(&fo);

			_stprintf_s(path, TEXT("%c:\\%s\\%s\\%s\\"), AfxGetDbMgr()->GetUDiskUnmoutDriver(), (LPCTSTR)strTmp, (LPCTSTR)strGameName, (LPCTSTR)time);
			RemoveDirectory(path);
		}

		CString dstRoot;
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		dstRoot.Format(TEXT("%c:\\%s\\%s\\%04d%02d%02d%02d%02d%02d"), AfxGetDbMgr()->GetUDiskUnmoutDriver(), (LPCTSTR)strTmp, (LPCTSTR)strGameName,
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		for (size_t nIdx = 0; nIdx < m_aryFileInfo.size(); nIdx++)
		{
			if (!m_aryFileInfo[nIdx].bSel)
			{
				continue;
			}

			CString root, match, dir;
			CString SrcFileName = AfxUDExpandPath2(m_aryFileInfo[nIdx].strDir, strGamePath, root, match, dir);			
			CString DstFileName = SrcFileName;

			CString strPath = pGame->gprogress;

			if (PathFileExists(strPath))
			{
				DstFileName.Delete(0, strPath.GetLength() - 1);
				DstFileName.Insert(0, dstRoot);
			}

			if (!match.IsEmpty())
			{
				DstFileName.Delete(0, dir.GetLength());
				DstFileName.Insert(0, dstRoot);
			}

			DWORD dwAttr = GetFileAttributes(SrcFileName);
			AfxUDCreateDirectory2(DstFileName);
			SetFileAttributes(DstFileName, FILE_ATTRIBUTE_NORMAL);

			HANDLE hFileSrc = CreateFile(SrcFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			HANDLE hFileDst = CreateFile(DstFileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
			if (hFileSrc == INVALID_HANDLE_VALUE || hFileDst == INVALID_HANDLE_VALUE)
			{
				if (hFileSrc != INVALID_HANDLE_VALUE)
					CloseHandle(hFileSrc);
				if (hFileDst != INVALID_HANDLE_VALUE)
					CloseHandle(hFileDst);
				continue;
			}

			FILETIME ft = { 0 };
			GetFileTime(hFileSrc, NULL, NULL, &ft);
			DWORD left = GetFileSize(hFileSrc, NULL);

			DWORD dwReadBytes = 0, dwWriteBytes = 0;
			char buf[0x10000] = { 0 };
			while (left)
			{
				if (!ReadFile(hFileSrc, buf, _countof(buf), &dwReadBytes, NULL))
					break;
				if (!WriteFile(hFileDst, buf, dwReadBytes, &dwWriteBytes, NULL))
					break;
				left -= dwWriteBytes;
			}
			SetEndOfFile(hFileDst);
			SetFileAttributes(DstFileName, dwAttr);
			SetFileTime(hFileDst, NULL, NULL, &ft);
			CloseHandle(hFileSrc);
			CloseHandle(hFileDst);
		}
		m_bSaveOK = TRUE;

		if (m_pPopTips == NULL)
		{
			m_pPopTips = new CPopTips(FALSE);
			if (m_pPopTips->initialize())
			{
				m_pPopTips->setContent(CLanguage::getString(RES_LAG_KEEP_SUCCESS));
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

	void CFuncWgtSaveFile::onBtnCancelTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leaveFile();
	}

	void CFuncWgtSaveFile::onBtnQuitTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leaveFile();
	}

	void CFuncWgtSaveFile::onBtnListFileItemTchRel(CComponent *pTarget)
	{
		INT i = m_listFile.getCurTouchFocus();

		m_aryFileInfo[i].bSel = !m_aryFileInfo[i].bSel;

		m_listFile.setCurTouchFocus(-1);
		m_listFile.littleRefresh();
	}

	// TODO: implement logic event listener here
	void CFuncWgtSaveFile::onKeyEnter()
	{
		onBtnSaveTchRel(NULL);
	}

	void CFuncWgtSaveFile::onKeyEsc()
	{
		onBtnCancelTchRel(NULL);
	}

	void CFuncWgtSaveFile::onPopTipsDsy(CBasePopup *pTarget)
	{
		m_pPopTips = NULL;
		if (m_bSaveOK == TRUE)
		{
			CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 4);
			CMain::getInstance()->pushMessage(pMessage);
		}
	}

	// TODO: implement your normal private method here
}