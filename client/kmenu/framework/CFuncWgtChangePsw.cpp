#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtChangePsw::CFuncWgtChangePsw()
		: m_pPopTips(NULL)
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_txtTitle);
		addComponent(&m_imgPsw);
		addComponent(&m_imgPswQ);
		addComponent(&m_btnQuit);
		addComponent(&m_btnOk);
		addComponent(&m_btnCancel);
		addComponent(&m_txtPsw);
		addComponent(&m_txtPswQ);
		addComponent(&m_editPsw);
		addComponent(&m_editPswQ);
		addComponent(&m_txtTips);


		// TODO: add event listener to children controls here
		m_btnQuit.addEventListener(this);
		m_btnOk.addEventListener(this);
		m_btnCancel.addEventListener(this);


		// TODO: initialize children controls specific properties here
	}

	CFuncWgtChangePsw::~CFuncWgtChangePsw()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtChangePsw)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::TOUCH_RELEASED, onBtnQuitTchRel)
		CONTROL_EVENT_HANDLER(&m_btnOk, CEventButton::TOUCH_RELEASED, onBtnOkTchRel)
		CONTROL_EVENT_HANDLER(&m_btnCancel, CEventButton::TOUCH_RELEASED, onBtnCancelTchRel)
		POPUP_EVENT_HANDLER(m_pPopTips, CEventPopup::DESTROYED, onPopTipsDsy)

		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_TAB, onKeyTab)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ENTER, onKeyEnter)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ESC, onKeyEsc)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtChangePsw::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener


		return TRUE;
	}

	void CFuncWgtChangePsw::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.


	}

	void CFuncWgtChangePsw::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_CHANGE_PSW);
		INIT::initTextField(m_txtTitle, RES_ITM_TITLE, this);
		INIT::initImageField(m_imgPsw, RES_ITM_IMG_PSW, this);
		INIT::initImageField(m_imgPswQ, RES_ITM_IMG_PSW_Q, this);
		INIT::initButton(m_btnQuit, RES_ITM_BTN_QUIT, this);
		INIT::initButton(m_btnOk, RES_ITM_BTN_OK, this);
		INIT::initButton(m_btnCancel, RES_ITM_BTN_CANCEL, this);
		INIT::initTextField(m_txtPsw, RES_ITM_TXT_PSW, this);
		INIT::initTextField(m_txtPswQ, RES_ITM_TXT_PSW_Q, this);
		INIT::initEdit(m_editPsw, RES_ITM_EDIT_PSW, this);
		INIT::initEdit(m_editPswQ, RES_ITM_EDIT_PSW_Q, this);
		INIT::initTextField(m_txtTips, RES_ITM_TXT_TIPS, this);


		// TODO: set data to children controls or set children controls status here
		m_editPsw.setContent(L"");
		m_editPsw.setLimitText(20);
		m_editPsw.setFont(14, RGB(0, 0, 0), L"Microsoft YaHei");
		m_editPsw.setBkColor(RGB(255, 255, 255));

		m_editPswQ.setContent(L"");
		m_editPswQ.setLimitText(20);
		m_editPswQ.setFont(14, RGB(0, 0, 0), L"Microsoft YaHei");
		m_editPswQ.setBkColor(RGB(255, 255, 255));
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here
	void CFuncWgtChangePsw::onBtnQuitTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leavePopChangePsw();
	}

	void CFuncWgtChangePsw::onBtnOkTchRel(CComponent *pTarget)
	{
		String password2 = m_editPswQ.getContent();
		String password = m_editPsw.getContent();

		CString user;
		CString old_password;

		GetPassword(user, old_password);

		if (password.length() == 0)
		{
			m_txtTips.setContent(CLanguage::getString(RES_LAG_ENTER_PASSWORD));
			return;
		}

		if (password2.length() == 0)
		{
			m_txtTips.setContent(CLanguage::getString(RES_LAG_CONFIRM_PASSWORD));
			return;
		}

		if (password != password2)
		{
			m_txtTips.setContent(CLanguage::getString(RES_LAG_PASSWORD_DIFF));
			return;
		};

		GetPassword(user, old_password);

		CServer server;
		tstring errinfo;

		quit();

		if (!server.UDiskModifyPassword(user, old_password, password, errinfo))
		{
			if (m_pPopTips == NULL)
			{
				m_pPopTips = new CPopTips(FALSE);
				if (m_pPopTips->initialize())
				{
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

		SavePassword(user, (const CHAR*)password);
		
		CLogicManager::getInstance()->leavePopChangePsw();
	}

	void CFuncWgtChangePsw::onBtnCancelTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leavePopChangePsw();
	}

	void CFuncWgtChangePsw::onPopTipsDsy(CBasePopup *pTarget)
	{
		enter();
		m_pPopTips = NULL;
	}

	// TODO: implement logic event listener here
	void CFuncWgtChangePsw::onKeyTab()
	{
		if (m_editPsw.getFocus() == TRUE)
		{
			m_editPsw.setFocus(FALSE);
			m_editPswQ.setFocus(TRUE);
		}
		else
		{
			m_editPsw.setFocus(TRUE);
			m_editPswQ.setFocus(FALSE);
		}
	}

	void CFuncWgtChangePsw::onKeyEnter()
	{
		onBtnOkTchRel(NULL);
	}

	void CFuncWgtChangePsw::onKeyEsc()
	{
		onBtnCancelTchRel(NULL);
	}
	// TODO: implement your normal private method here
}