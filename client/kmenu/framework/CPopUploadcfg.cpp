#include "HMI.h"

namespace OpenHMI
{
	CPopUploadcfg::CPopUploadcfg(BOOLEAN bModal)
		: CBasePopup(bModal)
	{
		// TODO: setup parent-child relationship with children controls here
		//addComponent(&m_imgBg);
		addComponent(&m_funcWgtUploadcfg);


		// TODO: add event listener to children controls here


		// TODO: initialize children controls specific properties here
	}

	CPopUploadcfg::~CPopUploadcfg()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CPopUploadcfg)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LEAVE_CFG, onLeaveCfg)
	END_EVENT_MAP(CBasePopup)

	// TODO: finish command map here
	BEGIN_CMD_MAP(CPopUploadcfg)
	END_CMD_MAP(CBasePopup)

	BOOLEAN CPopUploadcfg::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		m_funcWgtUploadcfg.initialize();
		CLogicManager::getInstance()->addEventListener(this);

		return TRUE;
	}

	void CPopUploadcfg::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_funcWgtUploadcfg.unInitialize();
		CLogicManager::getInstance()->removeEventListener(this);

	}

	void CPopUploadcfg::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info here
		INIT::initPopup(this, RES_POP_UPLOADCFG);
		INIT::initFuncWidget(m_funcWgtUploadcfg, RES_ITM_UPLOADCFG, this);
		INIT::initImageField(m_imgBg, RES_ITM_BG, this);


		// TODO: set data to children controls or set children controls status here
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		m_funcWgtUploadcfg.refresh();


		UINT sw = pServer->getScreenContext()->uiScreenWidth;
		UINT sh = pServer->getScreenContext()->uiScreenHeight;

		INT x = (sw - m_funcWgtUploadcfg.getPreferredSize().getX()) / 2;
		INT y = (sh - m_funcWgtUploadcfg.getPreferredSize().getY()) / 2;

		m_funcWgtUploadcfg.setX(x);
		m_funcWgtUploadcfg.setY(y);
		m_funcWgtUploadcfg.refresh();

		setRealX(x);
		setRealY(y);
		m_imgBg.setPreferredSize(sw, sh);
		m_funcWgtUploadcfg.enter();
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here


	// TODO: implement logic event listener here
	void CPopUploadcfg::onLeaveCfg()
	{
		destroy();
	}

	// TODO: implement your normal private method here
}