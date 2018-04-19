#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtAdmin::CFuncWgtAdmin()
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_txt);
		addComponent(&m_txtTitle);
		addComponent(&m_imgEdit);
		addComponent(&m_editPsw);
		addComponent(&m_btnQuit);
		addComponent(&m_btnLogin);
		addComponent(&m_btnCancel);
		addComponent(&m_txtTips);


		// TODO: add event listener to children controls here
		m_btnQuit.addEventListener(this);
		m_btnLogin.addEventListener(this);
		m_btnCancel.addEventListener(this);


		// TODO: initialize children controls specific properties here
	}

	CFuncWgtAdmin::~CFuncWgtAdmin()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtAdmin)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::TOUCH_RELEASED, onBtnQuitTchRel)
		CONTROL_EVENT_HANDLER(&m_btnLogin, CEventButton::TOUCH_RELEASED, onBtnLoginTchRel)
		CONTROL_EVENT_HANDLER(&m_btnCancel, CEventButton::TOUCH_RELEASED, onBtnCancelTchRel)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_TAB, onKeyTab)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ESC, onKeyEsc)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ENTER, onKeyEnter)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtAdmin::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener


		return TRUE;
	}

	void CFuncWgtAdmin::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.


	}

	void CFuncWgtAdmin::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_ADMIN);
		INIT::initTextField(m_txt, RES_ITM_TXT, this);
		INIT::initTextField(m_txtTitle, RES_ITM_TITLE, this);
		INIT::initImageField(m_imgEdit, RES_ITM_IMG_EDIT, this);
		INIT::initEdit(m_editPsw, RES_ITM_EDIT_PSW, this);
		INIT::initButton(m_btnQuit, RES_ITM_BTN_QUIT, this);
		INIT::initButton(m_btnLogin, RES_ITM_BTN_LOGIN, this);
		INIT::initButton(m_btnCancel, RES_ITM_BTN_CANCEL, this);
		INIT::initTextField(m_txtTips, RES_ITM_TXT_TIPS, this);
		
		// TODO: set data to children controls or set children controls status here
		m_editPsw.setContent(L"");
		m_editPsw.setLimitText(20);
		m_editPsw.setFont(16, RGB(0, 0, 0), L"Microsoft YaHei");
		m_editPsw.setBkColor(RGB(255, 255, 255));
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here
	void CFuncWgtAdmin::onBtnQuitTchRel(CComponent *pTarget)
	{
		m_editPsw.show(FALSE);
		CLogicManager::getInstance()->leaveAdmin(FALSE);
	}

	void CFuncWgtAdmin::onBtnLoginTchRel(CComponent *pTarget)
	{
		String txt = m_editPsw.getContent();
		if (txt.length() == 0)
		{
			m_txtTips.setContent(CLanguage::getString(RES_LAG_ENTER_PASSWORD));
			return;
		}

		TCHAR szVersizon[MAX_PATH] = { 0 };
		AfxGetFileVersion(TEXT("knbmenu.exe"), szVersizon);

		CString strVer = szVersizon;
		strVer = strVer.Right(4);

		bool bOnline = AfxGetDbMgr()->ClientServiceIsRunning();
		if (strVer.CompareNoCase(L"5498") > 0)
		{
			CServer server;
			tstring errinfo;
			server.AdminTestPassword((const CHAR*)txt, errinfo);
			if (!bOnline || !server.AdminTestPassword((const CHAR*)txt, errinfo))
			{
				if (!bOnline)
				{
					m_txtTips.setContent(CLanguage::getString(RES_LAG_SERVER_CONNECTION_FAILED));
					return;
				}
				else
				{
					m_txtTips.setContent(errinfo.c_str());
					return;
				}
			}
		}
		else
		{
			DWORD dwPassword = AfxGetDbMgr()->GetIntOpt(OPT_M_CLIPWD);
			DWORD dwCrc = AfxCrc32Encode(0, (LPCTSTR)((const CHAR*)txt), txt.length()*sizeof(TCHAR));

			if (!bOnline || dwCrc != dwPassword)
			{
				if (!bOnline)
				{
					m_txtTips.setContent(CLanguage::getString(RES_LAG_SERVER_CONNECTION_FAILED));
					return;
				}
				else
				{
					m_txtTips.setContent(CLanguage::getString(RES_LAG_PASSWORD_ERROR));
					return;
				}
			}
		}

		CLogicManager::getInstance()->leaveAdmin(TRUE);
		CLogicManager::getInstance()->loginPopAdminOK();		
	}

	void CFuncWgtAdmin::onBtnCancelTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leaveAdmin(FALSE);
	}

	// TODO: implement logic event listener here
	void CFuncWgtAdmin::onKeyTab()
	{
		m_editPsw.setFocus(TRUE);
	}

	void CFuncWgtAdmin::onKeyEnter()
	{
		onBtnLoginTchRel(NULL);
	}

	void CFuncWgtAdmin::onKeyEsc()
	{
		onBtnQuitTchRel(NULL);
	}
	// TODO: implement your normal private method here
}