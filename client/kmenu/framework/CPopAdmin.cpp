#include "HMI.h"

namespace OpenHMI
{
	CPopAdmin::CPopAdmin(BOOLEAN bModal)
		: CBasePopup(bModal)
		, m_bShowBg(TRUE)
	{
		// TODO: setup parent-child relationship with children controls here
		//addComponent(&m_imgBg);
		addComponent(&m_funcWgtAdmin);


		// TODO: add event listener to children controls here


		// TODO: initialize children controls specific properties here
	}

	CPopAdmin::~CPopAdmin()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CPopAdmin)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LEAVE_ADMIN, onLeaveAdmin)
	END_EVENT_MAP(CBasePopup)

	// TODO: finish command map here
	BEGIN_CMD_MAP(CPopAdmin)
	END_CMD_MAP(CBasePopup)

	BOOLEAN CPopAdmin::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		m_funcWgtAdmin.initialize();
		CLogicManager::getInstance()->addEventListener(this);

		return TRUE;
	}

	void CPopAdmin::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_funcWgtAdmin.unInitialize();
		CLogicManager::getInstance()->removeEventListener(this);

	}

	void CPopAdmin::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info here
		INIT::initPopup(this, RES_POP_ADMIN);
		INIT::initImageField(m_imgBg, RES_ITM_BG, this);
		INIT::initFuncWidget(m_funcWgtAdmin, RES_ITM_ADMIN, this);


		// TODO: set data to children controls or set children controls status here
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		m_funcWgtAdmin.refresh();


		UINT sw = pServer->getScreenContext()->uiScreenWidth;
		UINT sh = pServer->getScreenContext()->uiScreenHeight;

		INT x = (sw - m_funcWgtAdmin.getPreferredSize().getX()) / 2;
		INT y = (sh - m_funcWgtAdmin.getPreferredSize().getY()) / 2;

		m_funcWgtAdmin.setX(x);
		m_funcWgtAdmin.setY(y);
		m_funcWgtAdmin.refresh();

		setRealX(x);
		setRealY(y);
		m_imgBg.setPreferredSize(sw, sh);
		m_imgBg.setVisible(m_bShowBg);
		m_funcWgtAdmin.enter();
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here


	// TODO: implement logic event listener here
	void CPopAdmin::onLeaveAdmin()
	{
		destroy();
	}

	// TODO: implement your normal private method here
}