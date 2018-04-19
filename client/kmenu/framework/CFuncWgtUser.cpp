#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtUser::CFuncWgtUser()
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_imgTitle);
		addComponent(&m_btnQuit);
		addComponent(&m_txt);
		addComponent(&m_webBrsWeb);


		// TODO: add event listener to children controls here
		m_btnQuit.addEventListener(this);


		// TODO: initialize children controls specific properties here
	}

	CFuncWgtUser::~CFuncWgtUser()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtUser)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::TOUCH_RELEASED, onBtnQuitTchRel)
		//LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ESC, onKeyEsc)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtUser::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		//CScreenTouchServer::getInstance()->addEventListener(this);

		return TRUE;
	}

	void CFuncWgtUser::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.

		//CScreenTouchServer::getInstance()->removeEventListener(this);
	}

	void CFuncWgtUser::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_USER);
		INIT::initImageField(m_imgTitle, RES_ITM_IMG_TITLE, this);
		INIT::initButton(m_btnQuit, RES_ITM_BTN_QUIT, this);
		INIT::initTextField(m_txt, RES_ITM_TXT, this);
		INIT::initWebBrowser(m_webBrsWeb, RES_ITM_WEB, this);


		// TODO: set data to children controls or set children controls status here
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here
	void CFuncWgtUser::onBtnQuitTchRel(CComponent *pTarget)
	{
		m_webBrsWeb.show(FALSE);
		this->setVisible(FALSE);
		CLogicManager::getInstance()->leaveUser();
	}

	void CFuncWgtUser::openUrl(const String& strURL)
	{
		m_webBrsWeb.navigate2(strURL);
	}

	void CFuncWgtUser::show(const String& strName, INT w, INT h)
	{
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		UINT sw = pServer->getScreenContext()->uiScreenWidth;
		UINT sh = pServer->getScreenContext()->uiScreenHeight;

		m_txt.setContent(strName);

		if (w == 0 || h == 0)
		{
			w = 485;
			h = 385;
		}
		this->setX((sw - w) / 2);
		this->setY((sh - h - m_imgTitle.getPreferredSize().getY()) / 2);
		m_imgTitle.setPreferredSize(CSize(w, m_imgTitle.getPreferredSize().getY()));
		m_btnQuit.setX(w - 30);
		m_webBrsWeb.setPreferredSize(CSize(w, h));
		m_webBrsWeb.littleRefresh();
		m_webBrsWeb.show(TRUE);
		this->setVisible(TRUE);
	}

	void CFuncWgtUser::show(const String& url, const String& strName, INT w, INT h)
	{
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		UINT sw = pServer->getScreenContext()->uiScreenWidth;
		UINT sh = pServer->getScreenContext()->uiScreenHeight;

		m_webBrsWeb.navigate2(url);
		m_txt.setContent(strName);

		this->setX((sw - w) / 2);
		this->setY((sh - h - m_imgTitle.getPreferredSize().getY()) / 2);
		m_imgTitle.setPreferredSize(CSize(w, m_imgTitle.getPreferredSize().getY()));
		m_btnQuit.setX(w - 30);
		m_webBrsWeb.setPreferredSize(CSize(w, h));
		m_webBrsWeb.littleRefresh();
		m_webBrsWeb.show(TRUE);
		this->setVisible(TRUE);
	}

	void CFuncWgtUser::quit()
	{
		m_webBrsWeb.show(FALSE);
		this->setVisible(FALSE);
		CLogicManager::getInstance()->leaveUser();
	}

	// TODO: implement logic event listener here
	void CFuncWgtUser::onKeyEsc()
	{
		if(this->getVisible() == TRUE)
			quit();
	}
	// TODO: implement your normal private method here
}