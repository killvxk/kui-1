#include "HMI.h"

namespace OpenHMI
{
	CPopChangePsw::CPopChangePsw(BOOLEAN bModal)
		: CBasePopup(bModal)
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_funcWgtChangePsw);


		// TODO: add event listener to children controls here


		// TODO: initialize children controls specific properties here
	}

	CPopChangePsw::~CPopChangePsw()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CPopChangePsw)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LEAVE_CHANGE_PSW, onLeaveChangePsw)
	END_EVENT_MAP(CBasePopup)

	// TODO: finish command map here
	BEGIN_CMD_MAP(CPopChangePsw)
	END_CMD_MAP(CBasePopup)

	BOOLEAN CPopChangePsw::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		m_funcWgtChangePsw.initialize();
		CLogicManager::getInstance()->addEventListener(this);

		return TRUE;
	}

	void CPopChangePsw::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_funcWgtChangePsw.unInitialize();
		CLogicManager::getInstance()->removeEventListener(this);

	}

	void CPopChangePsw::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info here
		INIT::initPopup(this, RES_POP_CHANGE_PSW);
		INIT::initFuncWidget(m_funcWgtChangePsw, RES_ITM_CHANGE_PSW, this);


		// TODO: set data to children controls or set children controls status here
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		m_funcWgtChangePsw.refresh();


		UINT sw = pServer->getScreenContext()->uiScreenWidth;
		UINT sh = pServer->getScreenContext()->uiScreenHeight;

		INT x = (sw - m_funcWgtChangePsw.getPreferredSize().getX()) / 2;
		INT y = (sh - m_funcWgtChangePsw.getPreferredSize().getY()) / 2;

		m_funcWgtChangePsw.setX(x);
		m_funcWgtChangePsw.setY(y);
		m_funcWgtChangePsw.refresh();

		setRealX(x);
		setRealY(y);

		m_funcWgtChangePsw.enter();
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here

	// TODO: implement logic event listener here
	void CPopChangePsw::onLeaveChangePsw()
	{
		destroy();
	}

	// TODO: implement your normal private method here
}