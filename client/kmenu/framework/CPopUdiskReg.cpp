#include "HMI.h"

namespace OpenHMI
{
	CPopUdiskReg::CPopUdiskReg(BOOLEAN bModal)
		: CBasePopup(bModal)
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_funcWgtUdiskReg);


		// TODO: add event listener to children controls here


		// TODO: initialize children controls specific properties here
	}

	CPopUdiskReg::~CPopUdiskReg()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CPopUdiskReg)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LEAVE_POPUDISKREG, onLeavePopUdiskReg)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER + 5, onLeavePopUdiskReg)
	END_EVENT_MAP(CBasePopup)

	// TODO: finish command map here
	BEGIN_CMD_MAP(CPopUdiskReg)
	END_CMD_MAP(CBasePopup)

	BOOLEAN CPopUdiskReg::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		m_funcWgtUdiskReg.initialize();
		CLogicManager::getInstance()->addEventListener(this);
		CScreenTouchServer::getInstance()->addEventListener(this);
		return TRUE;
	}

	void CPopUdiskReg::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_funcWgtUdiskReg.unInitialize();

		CLogicManager::getInstance()->removeEventListener(this);
		CScreenTouchServer::getInstance()->removeEventListener(this);
	}

	void CPopUdiskReg::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info here
		INIT::initPopup(this, RES_POP_UDISK_REG);
		INIT::initFuncWidget(m_funcWgtUdiskReg, RES_ITM_UDISK_REG, this);


		// TODO: set data to children controls or set children controls status here
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		m_funcWgtUdiskReg.refresh();


		UINT sw = pServer->getScreenContext()->uiScreenWidth;
		UINT sh = pServer->getScreenContext()->uiScreenHeight;

		INT x = (sw - m_funcWgtUdiskReg.getPreferredSize().getX()) / 2;
		INT y = (sh - m_funcWgtUdiskReg.getPreferredSize().getY()) / 2;

		m_funcWgtUdiskReg.setX(x);
		m_funcWgtUdiskReg.setY(y);
		m_funcWgtUdiskReg.refresh();

		setRealX(x);
		setRealY(y);

		m_funcWgtUdiskReg.enter();
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here


	// TODO: implement logic event listener here
	void CPopUdiskReg::onLeavePopUdiskReg()
	{
		destroy();
	}
	// TODO: implement your normal private method here
}