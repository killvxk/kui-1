#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtUdiskLogin::CFuncWgtUdiskLogin()
		: m_pPopTips(NULL)
		, m_pPopUdiskReg(NULL)
		, m_pPopUdisk(NULL)
		, m_bAutoQuit(FALSE)
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_txtTitle);
		addComponent(&m_txtUser);
		addComponent(&m_imgUser);
		addComponent(&m_editUser);
		addComponent(&m_btnQuit);
		addComponent(&m_btnOk);
		addComponent(&m_btnCancel);
		addComponent(&m_txtPsw);
		addComponent(&m_editPsw);
		addComponent(&m_imgPsw);
		addComponent(&m_txtTips);
		addComponent(&m_btnReg);


		// TODO: add event listener to children controls here
		m_btnQuit.addEventListener(this);
		m_btnOk.addEventListener(this);
		m_btnCancel.addEventListener(this);
		m_btnReg.addEventListener(this);


		// TODO: initialize children controls specific properties here
	}

	CFuncWgtUdiskLogin::~CFuncWgtUdiskLogin()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtUdiskLogin)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::TOUCH_RELEASED, onBtnQuitTchRel)
		CONTROL_EVENT_HANDLER(&m_btnOk, CEventButton::TOUCH_RELEASED, onBtnOkTchRel)
		CONTROL_EVENT_HANDLER(&m_btnCancel, CEventButton::TOUCH_RELEASED, onBtnCancelTchRel)
		CONTROL_EVENT_HANDLER(&m_btnReg, CEventButton::TOUCH_RELEASED, onBtnRegTchRel)
		POPUP_EVENT_HANDLER(m_pPopTips, CEventPopup::DESTROYED, onPopTipsDsy)
		POPUP_EVENT_HANDLER(m_pPopUdisk, CEventPopup::DESTROYED, onPopUdiskDsy)
		POPUP_EVENT_HANDLER(m_pPopUdiskReg, CEventPopup::DESTROYED, onPopUdiskRegDsy)

		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_TAB, onKeyTab)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ENTER, onKeyEnter)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ESC, onKeyEsc)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtUdiskLogin::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		return TRUE;
	}

	void CFuncWgtUdiskLogin::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
	}

	void CFuncWgtUdiskLogin::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_UDISK_LOGIN);
		INIT::initTextField(m_txtTitle, RES_ITM_TITLE, this);
		INIT::initTextField(m_txtUser, RES_ITM_TXT_USER, this);
		INIT::initImageField(m_imgUser, RES_ITM_IMG_USER, this);
		INIT::initEdit(m_editUser, RES_ITM_EDIT_USER, this);
		INIT::initButton(m_btnQuit, RES_ITM_BTN_QUIT, this);
		INIT::initButton(m_btnOk, RES_ITM_BTN_OK, this);
		INIT::initButton(m_btnCancel, RES_ITM_BTN_CANCEL, this);
		INIT::initTextField(m_txtPsw, RES_ITM_TXT_PSW, this);
		INIT::initEdit(m_editPsw, RES_ITM_EDIT_PSW, this);
		INIT::initImageField(m_imgPsw, RES_ITM_IMG_PSW, this);
		INIT::initTextField(m_txtTips, RES_ITM_TXT_TIPS, this);
		INIT::initButton(m_btnReg, RES_ITM_BTN_REG, this);


		// TODO: set data to children controls or set children controls status here
		m_editUser.setContent(L"");
		m_editUser.setLimitText(20);
		m_editUser.setFont(15, RGB(0, 0, 0), L"Microsoft YaHei");
		m_editUser.setBkColor(RGB(255, 255, 255));

		m_editPsw.setContent(L"");
		m_editPsw.setLimitText(20);
		m_editPsw.setFont(15, RGB(0, 0, 0), L"Microsoft YaHei");
		m_editPsw.setBkColor(RGB(255, 255, 255));
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here
	void CFuncWgtUdiskLogin::onBtnQuitTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leavePopUdiskLogin(FALSE);
	}

	void CFuncWgtUdiskLogin::onBtnRegTchRel(CComponent *pTarget)
	{
		if (m_pPopUdiskReg == NULL)
		{
			quit();
			m_pPopUdiskReg = new CPopUdiskReg(FALSE);
			if (m_pPopUdiskReg->initialize())
			{
				m_pPopUdiskReg->addEventListener(this);
				m_pPopUdiskReg->doModal();
			}
			else
			{
				delete m_pPopUdiskReg;
				m_pPopUdiskReg = NULL;
			}
		}
	}


	void CFuncWgtUdiskLogin::onBtnOkTchRel(CComponent *pTarget)
	{
		String password = m_editPsw.getContent();
		String user = m_editUser.getContent();
		CLogicManager::getInstance()->getUpLoadGameClick().addItem(FROM_CLICK_TOOL, POS_MENU, TOOL_LOGINUDISK);
		
		if (user.length() == 0)
		{
			m_txtTips.setContent(CLanguage::getString(RES_LAG_ENTER_USER_NAME));
			return;
		}

		if (password.length() == 0)
		{
			m_txtTips.setContent(CLanguage::getString(RES_LAG_ENTER_PASSWORD));
			return;
		}

		quit();

		CServer server;
		tstring errinfo;

		if (!server.UDiskLoginUser((const CHAR*)user, (const CHAR*)password, errinfo))
		{
			if (m_pPopTips == NULL)
			{
				CScreenTouchServer::getInstance()->removeEventListener(this);
				m_pPopTips = new CPopTips(FALSE);
				if (m_pPopTips->initialize())
				{
					m_pPopTips->setNotCatchKey();
					m_pPopTips->setContent(errinfo.c_str(), FALSE);
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

		SavePassword((const CHAR*)user, (const CHAR*)password);

		if (m_bAutoQuit == TRUE)
		{
			CLogicManager::getInstance()->leavePopUdiskLogin(TRUE);
			CLogicManager::getInstance()->loginUdiskOK();
		}
		else
		{
			if (m_pPopUdisk == NULL)
			{
				CScreenTouchServer::getInstance()->removeEventListener(this);

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
	}

	void CFuncWgtUdiskLogin::onBtnCancelTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leavePopUdiskLogin(FALSE);
	}

	void CFuncWgtUdiskLogin::onPopTipsDsy(CBasePopup *pTarget)
	{
		enter();
		m_pPopTips = NULL;
	}

	void CFuncWgtUdiskLogin::onPopUdiskDsy(CBasePopup *pTarget)
	{
		m_pPopUdisk = NULL;
		setVisible(FALSE);
		CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 4);
		CMain::getInstance()->pushMessage(pMessage);
	}

	void CFuncWgtUdiskLogin::onPopUdiskRegDsy(CBasePopup *pTarget)
	{
		m_pPopUdiskReg->quit();
		m_pPopUdiskReg = NULL;
		setVisible(FALSE);
		

		//CScreenTouchServer::getInstance()->addEventListener(this);
		CMsgScreen *pMessage = new CMsgScreen(CScreenTouchServer::USER + 4);
		CMain::getInstance()->pushMessage(pMessage);
	}

	void CFuncWgtUdiskLogin::onKeyTab()
	{
		if (m_editPsw.getFocus() == TRUE)
		{
			m_editUser.setFocus(TRUE);
			m_editPsw.setFocus(FALSE);
		}

		else 
		{
			m_editPsw.setFocus(TRUE);
			m_editUser.setFocus(FALSE);
		}

	}

	void CFuncWgtUdiskLogin::onKeyEnter()
	{
		onBtnOkTchRel(NULL);
	}

	void CFuncWgtUdiskLogin::onKeyEsc()
	{
		onBtnCancelTchRel(NULL);
	}
	// TODO: implement logic event listener here

	// TODO: implement your normal private method here
}