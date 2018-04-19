#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtReadFile::CFuncWgtReadFile()
		: m_Gid(0)
		, m_pPopTips(NULL)
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_btnQuit);
		addComponent(&m_btnRead);
		addComponent(&m_txtTitle);
		addComponent(&m_btnCancel);
		addComponent(&m_listFile);
		addComponent(&m_scrollbarList);


		// TODO: add event listener to children controls here
		m_btnQuit.addEventListener(this);
		m_btnRead.addEventListener(this);
		m_btnCancel.addEventListener(this);
		m_listFile.addEventListener(this);
		m_scrollbarList.addEventListener(this);

		m_listFile.setAssociateScrollbar(&m_scrollbarList, FALSE);
		m_scrollbarList.setTargetComponent(&m_listFile);

		// TODO: initialize children controls specific properties here
		m_listFile.setView(this);
	}

	CFuncWgtReadFile::~CFuncWgtReadFile()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtReadFile)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::TOUCH_RELEASED, onBtnQuitTchRel)
		CONTROL_EVENT_HANDLER(&m_btnRead, CEventButton::TOUCH_RELEASED, onBtnReadTchRel)
		CONTROL_EVENT_HANDLER(&m_btnCancel, CEventButton::TOUCH_RELEASED, onBtnCancelTchRel)
		CONTROL_EVENT_HANDLER_CHAIN(&m_listFile, CEventButton::TOUCH_RELEASED, onBtnListFileItemTchRel)

		POPUP_EVENT_HANDLER(m_pPopTips, CEventPopup::DESTROYED, onPopTipsDsy)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ENTER, onKeyEnter)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ESC, onKeyEsc)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtReadFile::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener

		m_listFile.setModel(&m_aryData);
		CScreenTouchServer::getInstance()->addEventListener(this);
		return TRUE;
	}

	void CFuncWgtReadFile::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.

		m_listFile.setModel(NULL);
		m_aryData.clear();
		m_aryFileInfo.clear();

		CScreenTouchServer::getInstance()->removeEventListener(this);
	}

	void CFuncWgtReadFile::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_READ_FILE);
		INIT::initButton(m_btnQuit, RES_ITM_BTN_QUIT, this);
		INIT::initButton(m_btnRead, RES_ITM_BTN_READ, this);
		INIT::initTextField(m_txtTitle, RES_ITM_TITLE, this);
		INIT::initButton(m_btnCancel, RES_ITM_BTN_CANCEL, this);
		INIT::initList(m_listFile, RES_ITM_FILE, this);
		INIT::initScrollbar(m_scrollbarList, RES_ITM_SCROLLBAR_LIST, this);


		// TODO: set data to children controls or set children controls status here
		tagGameInfo* pGame = AfxGetDbMgr()->FindGameInfo(m_Gid);

		if (pGame)
		{
			m_txtTitle.setContent(String::format((const CHAR*)CLanguage::getString(RES_LAG_LOAD_SAVE), pGame->gname));
		}

		FindClienPath(pGame, pGame->gclipath, _countof(pGame->gclipath));

		CString strGamePath = pGame->gclipath;
		CString strGameName = pGame->gname;

		CString dir;

		AfxUDGetAllSaveDir2(strGameName, dir);
		m_listFile.setReachToHead(TRUE);
		m_listFile.setReachToTail(TRUE);

		m_aryData.clear();
		m_aryFileInfo.clear();

		for (int idx = 0;; idx++)
		{
			CString szItem;
			AfxExtractSubString(szItem, dir, idx, TEXT('|'));
			if (szItem.IsEmpty())
				break;
			if(szItem.GetLength() != 14)
				continue;
			szItem.Insert(12, TEXT(":"));
			szItem.Insert(10, TEXT(":"));
			szItem.Insert(8, TEXT(" "));
			szItem.Insert(6, TEXT("-"));
			szItem.Insert(4, TEXT("-"));
			m_aryFileInfo.addItem(szItem);
		}

		for (size_t i = 0; i < m_aryFileInfo.size(); i++)
		{
			m_aryData.addItem(new CInt(i));
		}
	}

	// Definition functions for interface IViewList_Simple
	CComponent* CFuncWgtReadFile::getItemComponent(CCtrlList_Simple* pList)
	{
		CComponent* pView = NULL;
		pView = new CCtrlButton();
		INIT::initButton(*((CCtrlButton*)pView), RES_ITM_BTN_ITEM, this);
		pView->addEventListener(this);
		pView->refresh();

		return pView;
	}

	void CFuncWgtReadFile::drawItem(CCtrlList_Simple* pList, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus)
	{
		CInt* pData = (CInt*)pObjItem;

		ASSERT(*pData >= 0 && *pData < (INT)m_aryFileInfo.size());

		CCtrlButton *pBtn = (CCtrlButton*)pComponent;

		String str = m_aryFileInfo[*pData].GetString();

		if (str.length() > 48)
		{
			String strTmp = str.subString(0, 48) + L"...";
			pBtn->setCaption(strTmp);
		}
		else
			pBtn->setCaption(str);

		return;
	}


	// TODO: implement your public method here

	// TODO: implement control event listener here
	void CFuncWgtReadFile::onBtnQuitTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leaveFile();
	}

	void CFuncWgtReadFile::onBtnReadTchRel(CComponent *pTarget)
	{
		CString szDate;
		INT i = m_listFile.getCurSel();

		szDate = m_aryFileInfo[i];
		if (szDate.GetLength() == 0)
		{
			return;
		}

		szDate.Delete(4);
		szDate.Delete(6);
		szDate.Delete(8);
		szDate.Delete(10);
		szDate.Delete(12);

		tagGameInfo* pGame = AfxGetDbMgr()->FindGameInfo(m_Gid);

		if (pGame == NULL) return;

		CString strGameName = pGame->gname;
		CString strGamePath = pGame->gclipath;
		CString strFilter = pGame->gprogress;

		CString strTmp = (const CHAR*)CLanguage::getString(RES_LAG_GAME_SAVE);

		if (PathFileExists(strFilter))
		{
			CString root, match;
			match.Format(TEXT("%c:\\%s\\%s\\%s"), AfxGetDbMgr()->GetUDiskUnmoutDriver(), (LPCTSTR)strTmp, (LPCTSTR)strGameName, (LPCTSTR)szDate);
			root = match + TEXT("\\");

			if (!AfxUDCopyFile2(root, match, strFilter))
			{
				if (m_pPopTips == NULL)
				{
					m_pPopTips = new CPopTips(FALSE);
					if (m_pPopTips->initialize())
					{
						m_pPopTips->setContent(CLanguage::getString(RES_LAG_READ_FAILED));
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
		}
		else
		{
			for (int idx = 0;; idx++)
			{
				CString szItem;
				AfxExtractSubString(szItem, strFilter, idx, TEXT('|'));
				if (szItem.IsEmpty())
					break;
				CString root, match, dir;
				AfxUDExpandPath2(szItem, strGamePath, root, match, dir);

				match.Format(TEXT("%c:\\%s\\%s\\%s"), AfxGetDbMgr()->GetUDiskUnmoutDriver(), (LPCTSTR)strTmp, (LPCTSTR)strGameName, (LPCTSTR)szDate);
				root = match + TEXT("\\");
				if (!AfxUDCopyFile2(root, match, dir))
				{
					if (m_pPopTips == NULL)
					{
						m_pPopTips = new CPopTips(FALSE);
						if (m_pPopTips->initialize())
						{
							m_pPopTips->setContent(CLanguage::getString(RES_LAG_READ_FAILED));
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
			}
		}

		if (m_pPopTips == NULL)
		{
			m_pPopTips = new CPopTips(FALSE);
			if (m_pPopTips->initialize())
			{
				m_pPopTips->setContent(CLanguage::getString(RES_LAG_READ_SUCESS));
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

	void CFuncWgtReadFile::onBtnCancelTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leaveFile();;
	}

	void CFuncWgtReadFile::onBtnListFileItemTchRel(CComponent *pTarget)
	{
		INT i = m_listFile.getCurTouchFocus();
	}
	// TODO: implement logic event listener here

	void CFuncWgtReadFile::onKeyEnter()
	{
		onBtnReadTchRel(NULL);
	}

	void CFuncWgtReadFile::onKeyEsc()
	{
		onBtnCancelTchRel(NULL);
	}

	void CFuncWgtReadFile::onPopTipsDsy(CBasePopup *pTarget)
	{
		m_pPopTips = NULL;

		CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 4);
		CMain::getInstance()->pushMessage(pMessage);
	}

	// TODO: implement your normal private method here
}