#include "HMI.h"

namespace OpenHMI
{
	CPopUdisk::CPopUdisk(BOOLEAN bModal)
		: CBasePopup(bModal)
		, m_bShowBg(TRUE)
	{
		// TODO: setup parent-child relationship with children controls here
		//addComponent(&m_imgBg);
		addComponent(&m_funcWgtUdisk);


		// TODO: add event listener to children controls here
		

		// TODO: initialize children controls specific properties here
	}

	CPopUdisk::~CPopUdisk()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CPopUdisk)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LEAVE_POPUDISK, onLeavePopUdisk)
	END_EVENT_MAP(CBasePopup)

	// TODO: finish command map here
	BEGIN_CMD_MAP(CPopUdisk)
	END_CMD_MAP(CBasePopup)

	BOOLEAN CPopUdisk::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		m_funcWgtUdisk.initialize();
		CLogicManager::getInstance()->addEventListener(this);

		return TRUE;
	}

	void CPopUdisk::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_funcWgtUdisk.unInitialize();
		CLogicManager::getInstance()->removeEventListener(this);

	}

	void CPopUdisk::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info here
		INIT::initPopup(this, RES_POP_UDISK);
		INIT::initImageField(m_imgBg, RES_ITM_BG, this);
		INIT::initFuncWidget(m_funcWgtUdisk, RES_ITM_UDISK, this);


		// TODO: set data to children controls or set children controls status here
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		m_funcWgtUdisk.refresh();


		UINT sw = pServer->getScreenContext()->uiScreenWidth;
		UINT sh = pServer->getScreenContext()->uiScreenHeight;

		INT x = (sw - m_funcWgtUdisk.getPreferredSize().getX()) / 2;
		INT y = (sh - m_funcWgtUdisk.getPreferredSize().getY()) / 2;

		m_funcWgtUdisk.setX(x);
		m_funcWgtUdisk.setY(y);

		setRealX(x);
		setRealY(y);
		m_imgBg.setPreferredSize(sw, sh);
		m_imgBg.setVisible(m_bShowBg);
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here


	// TODO: implement logic event listener here
	void CPopUdisk::onLeavePopUdisk()
	{
		destroy();
	}

	// TODO: implement your normal private method here
}