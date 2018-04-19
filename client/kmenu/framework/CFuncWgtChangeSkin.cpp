#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtChangeSkin::CFuncWgtChangeSkin()
		: m_pPopAdmin(NULL)
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_imgLog);
		addComponent(&m_txtTitle);
		addComponent(&m_scrollbar);
		addComponent(&m_table);
		addComponent(&m_btnQuit);
		addComponent(&m_btnSave);


		// TODO: add event listener to children controls here
		m_scrollbar.addEventListener(this);
		m_table.addEventListener(this);
		m_btnQuit.addEventListener(this);
		m_btnSave.addEventListener(this);


		// TODO: initialize children controls specific properties here
		m_table.setView(this);
		m_table.setAssociateScrollbar(&m_scrollbar, FALSE);
		m_scrollbar.setTargetComponent(&m_table);
	}

	CFuncWgtChangeSkin::~CFuncWgtChangeSkin()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtChangeSkin)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::TOUCH_RELEASED, onBtnQuitTchRel)
		CONTROL_EVENT_HANDLER(&m_btnSave, CEventButton::TOUCH_RELEASED, onBtnSaveTchRel)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::SKIN_CHANGE, onSkinChange)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::SKIN_ADD, onSkinAdd)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::SKIN_DEL, onSkinDel)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LOGIN_POP_ADMIN_OK, onLoginPopAdminOK)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER + 3, onUpdateData)

		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ESC, onKeyEsc)

		POPUP_EVENT_HANDLER(m_pPopAdmin, CEventPopup::DESTROYED, onAdminDsy)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtChangeSkin::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener

		CLogicManager::getInstance()->addEventListener(this);
		CScreenTouchServer::getInstance()->addEventListener(this);
		return TRUE;
	}

	void CFuncWgtChangeSkin::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		CLogicManager::getInstance()->removeEventListener(this);
		CScreenTouchServer::getInstance()->removeEventListener(this);
	}

	void CFuncWgtChangeSkin::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_CHANGE_SKIN);
		INIT::initImageField(m_imgLog, RES_ITM_IMG_LOG, this);
		INIT::initTextField(m_txtTitle, RES_ITM_TXT_TITLE, this);
		INIT::initScrollbar(m_scrollbar, RES_ITM_SCROLLBAR, this);
		INIT::initTable(m_table, RES_ITM_TABLE, this);
		INIT::initButton(m_btnQuit, RES_ITM_BTN_QUIT, this);
		INIT::initButton(m_btnSave, RES_ITM_BTN_SAVE, this);


		// TODO: set data to children controls or set children controls status here
		CLogicManager::getInstance()->buildWallPaper();

		m_table.getModel()->deleteAllItems();
		m_table.getModel()->setRealItemCount(CLogicManager::getInstance()->getWallPaperCount());
		for (INT i = 0; i < CLogicManager::getInstance()->getWallPaperCount(); i++)
		{
			m_table.getModel()->addItem(new CInt(i));
		}
	}

	// Definition functions for interface IViewTable
	CComponent* CFuncWgtChangeSkin::getItemComponent(CCtrlTable* pTable, Object* pObjItem)
	{
		if (pObjItem == NULL)
		{
			return NULL;
		}

		INT iData = *((CInt*)pObjItem);

		CFuncWgtSkinItem* PFuc = new CFuncWgtSkinItem();
		INIT::initFuncWidget(*PFuc, RES_ITM_SKIN_ITEM, this);
		PFuc->refresh();

		PFuc->showAddBtn(FALSE);
		INT iType = CLogicManager::getInstance()->getWallPaperType(iData);

		if (iType == 0)
		{
			PFuc->hideDelBtn();
		}
		else if(iType == 2)
		{
			PFuc->hideDelBtn();
			PFuc->showAddBtn(TRUE);
		}

		CCtrlButton* pButton = PFuc->getBtnItem();

		String name;
		String strName = CLogicManager::getInstance()->getWallPaperName(iData);

		pButton->setCaption(strName);

		if (iType != 2)
		{
			CMovieClip* pMovieClip = new CMovieClip();
			pMovieClip->addImage(CLogicManager::getInstance()->getWallPaperSPath(iData));
			pButton->getImageField()->setUserMovieClip(pMovieClip);
		}

		return PFuc;
	}

	void CFuncWgtChangeSkin::drawItem(CCtrlTable* pTable, 
								Object* pObjItem, 
								CComponent* pComponent, 
								BOOLEAN bSelected, 
								BOOLEAN bFocus,
								BOOLEAN bDisable)
	{
		
		return ;
	}


	void CFuncWgtChangeSkin::quit()
	{
		if (m_pPopAdmin != NULL)
		{
			m_pPopAdmin->destroy();
		}
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here
	void CFuncWgtChangeSkin::onBtnQuitTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leaveChangeSkin();
	}

	void CFuncWgtChangeSkin::onBtnSaveTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, POS_MENU, TOOL_SAVESKIN);

		if (m_pPopAdmin == NULL)
		{
			m_pPopAdmin = new CPopAdmin(FALSE);

			if (m_pPopAdmin->initialize())
			{
				m_pPopAdmin->showBg(FALSE);
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

	void CFuncWgtChangeSkin::onAdminDsy(CBasePopup *pTarget)
	{
		m_pPopAdmin->quit();
		m_pPopAdmin = NULL;
	}

	// TODO: implement logic event listener here

	void CFuncWgtChangeSkin::onSkinChange()
	{
		INT i = m_table.getCurTouchFocus();

		if (i >= 0)
		{
			CLogicManager::getInstance()->setCurWallPaperIdx(i);
			String strName = CLogicManager::getInstance()->getWallPaperName(i);

			INT iType = CLogicManager::getInstance()->getWallPaperType(i);

			if (iType == 0)
			{
				String name;
				CHelpFunction::getFileNameNotExtern(strName, name);
				AfxGetDbMgr()->SetOpt(OPT_M_MENUBG, (const CHAR*)name);
			}
			else
				AfxGetDbMgr()->SetOpt(OPT_M_MENUBG, (const CHAR*)strName);
			AfxGetDbMgr()->SaveOpt();
		}
	}

	void CFuncWgtChangeSkin::onSkinAdd()
	{
		CLogicManager::getInstance()->buildWallPaper();
		CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 3);
		CMain::getInstance()->pushMessage(pMessage);
	}

	void CFuncWgtChangeSkin::onSkinDel()
	{
		INT i = m_table.getCurTouchFocus();

		String strName = CLogicManager::getInstance()->getWallPaperName(i);
		CString strNewBg;
		CString strBg = AfxGetDbMgr()->GetStrOpt(OPT_M_CUSTOMMENUBG);
		vector<tstring> strSplit;
		AfxSplitString(strBg.GetBuffer(), strSplit, _T('|'));

		for (size_t i = 0; i < strSplit.size(); i++)
		{
			CString strFileName = strSplit[i].c_str();
			if (strFileName.CompareNoCase((const CHAR*)strName) != 0)
			{
				if (strNewBg.IsEmpty())
					strNewBg = strFileName;
				else
				{
					strNewBg += _T("|");
					strNewBg += strFileName;
				}
			}
		}
		AfxGetDbMgr()->SetOpt(OPT_M_CUSTOMMENUBG, strNewBg);
		AfxGetDbMgr()->SaveOpt();
		CString strPng = AfxGetDbMgr()->GetStrOpt(OPT_M_MENUBG);
		if (strName.compare(strPng.GetBuffer()) == 0)
		{
			CLogicManager::getInstance()->setCurWallPaperIdx(0);
		}

		onSkinAdd();
	}

	void CFuncWgtChangeSkin::onUpdateData()
	{
		INT iTop = m_table.getCurrentTopItem();

		m_table.getModel()->deleteAllItems();
		m_table.getModel()->setRealItemCount(CLogicManager::getInstance()->getWallPaperCount());
		for (INT i = 0; i < CLogicManager::getInstance()->getWallPaperCount(); i++)
		{
			m_table.getModel()->addItem(new CInt(i));
		}

		m_table.refresh();
		m_table.showTopItem(iTop);
	}

	void CFuncWgtChangeSkin::onLoginPopAdminOK()
	{
		CServer server;
		tstring strCustomBg = AfxGetDbMgr()->GetStrOpt(OPT_M_CUSTOMMENUBG);
		tstring strPng = AfxGetDbMgr()->GetStrOpt(OPT_M_MENUBG);

		std::vector<tstring> strSplit;
		AfxSplitString(strCustomBg, strSplit, _T('|'));
		for (size_t i = 0; i < strSplit.size(); i++)
		{
			CString strFileName = strSplit[i].c_str();
			TCHAR szImage[MAX_PATH] = { 0 };
			AfxGetAppPath(szImage, MAX_PATH, _T("Skin\\Image\\"));
			lstrcat(szImage, strFileName);
			CString strDstPath;
			strDstPath.Format(_T("config\\icon\\%s"), strFileName);
			if (!PathFileExists(szImage))
			{
				continue;
			}
			if (!server.UploadFile(szImage, strDstPath))
			{
				CString strError = (const CHAR*)(CLanguage::getString(RES_LAG_UPLOAD_FAILED));
				strError.AppendFormat(_T(":%s"), szImage);

				CMainFrame::getInstance()->showTips(strError.GetBuffer(), FALSE, FALSE);
				return;
			}
		}

		CString strError;
		if (!server.ModifyOption(OPT_M_CUSTOMMENUBG, strCustomBg.c_str(), strError) ||
			!server.ModifyOption(OPT_M_MENUBG, strPng.c_str(), strError))
		{
			strError.Append((const CHAR*)(CLanguage::getString(RES_LAG_SAVE_OPTION_FAILED)));
			CMainFrame::getInstance()->showTips(strError.GetBuffer(), FALSE, FALSE);
			return;
		}

		CMainFrame::getInstance()->showTips(CLanguage::getString(RES_LAG_UPLOAD_SUCCESS), FALSE, FALSE);
	}

	void CFuncWgtChangeSkin::onKeyEsc()
	{
		if (m_pPopAdmin != NULL)
		{
			m_pPopAdmin->destroy();
		}
		CMainFrame::getInstance()->quitTips();
		onBtnQuitTchRel(NULL);
	}
	// TODO: implement your normal private method here
}