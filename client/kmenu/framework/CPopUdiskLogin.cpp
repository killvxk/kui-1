#include "HMI.h"

namespace OpenHMI
{
	CPopUdiskLogin::CPopUdiskLogin(BOOLEAN bModal)
		: CBasePopup(bModal)
	{
		// TODO: setup parent-child relationship with children controls here
		//addComponent(&m_imgBg);
		addComponent(&m_funcWgtUdiskLogin);


		// TODO: add event listener to children controls here


		// TODO: initialize children controls specific properties here
	}

	CPopUdiskLogin::~CPopUdiskLogin()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CPopUdiskLogin)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LEAVE_POPUDISKLOGIN, onLeavePopUdiskLogin)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER + 4, onLeavePopUdiskLogin)
	END_EVENT_MAP(CBasePopup)

	// TODO: finish command map here
	BEGIN_CMD_MAP(CPopUdiskLogin)
	END_CMD_MAP(CBasePopup)

	BOOLEAN CPopUdiskLogin::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		m_funcWgtUdiskLogin.initialize();

		CLogicManager::getInstance()->addEventListener(this);
		CScreenTouchServer::getInstance()->addEventListener(this);
		return TRUE;
	}

	void CPopUdiskLogin::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_funcWgtUdiskLogin.unInitialize();

		CLogicManager::getInstance()->removeEventListener(this);
		CScreenTouchServer::getInstance()->removeEventListener(this);
	}

	void CPopUdiskLogin::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info here
		INIT::initPopup(this, RES_POP_UDISK_LOGIN);
		INIT::initImageField(m_imgBg, RES_ITM_BG, this);
		INIT::initFuncWidget(m_funcWgtUdiskLogin, RES_ITM_UDISK_LOGIN, this);


		// TODO: set data to children controls or set children controls status here
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		m_funcWgtUdiskLogin.refresh();


		UINT sw = pServer->getScreenContext()->uiScreenWidth;
		UINT sh = pServer->getScreenContext()->uiScreenHeight;

		INT x = (sw - m_funcWgtUdiskLogin.getPreferredSize().getX()) / 2;
		INT y = (sh - m_funcWgtUdiskLogin.getPreferredSize().getY()) / 2;

		m_funcWgtUdiskLogin.setX(x);
		m_funcWgtUdiskLogin.setY(y);
		m_funcWgtUdiskLogin.refresh();

		setRealX(x);
		setRealY(y);
		m_imgBg.setPreferredSize(sw, sh);

		m_funcWgtUdiskLogin.enter();
	}

	// TODO: implement your public method here
#if 0
	void CPopUdiskLogin::startShowAnimation(UINT uiIntervalTime, UINT uiRepeatCount)
	{
		CBasePopup::startShowAnimation(10, 1);
	}

	void CPopUdiskLogin::completeShowAnimaiton()
	{		
		m_funcWgtUdiskLogin.enter();
		CBasePopup::completeShowAnimaiton();
	}
#endif
	// TODO: implement control event listener here
	void CPopUdiskLogin::onLeavePopUdiskLogin()
	{
		destroy();
	}
	// TODO: implement logic event listener here

	// TODO: implement your normal private method here
}