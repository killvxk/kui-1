#include "HMI.h"

namespace OpenHMI
{
	CPopReadFile::CPopReadFile(BOOLEAN bModal)
		: CBasePopup(bModal)
	{
		// TODO: setup parent-child relationship with children controls here
		//addComponent(&m_imgBg);
		addComponent(&m_funcWgtReadFile);


		// TODO: add event listener to children controls here


		// TODO: initialize children controls specific properties here
	}

	CPopReadFile::~CPopReadFile()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CPopReadFile)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LEAVE_FILE, onLeaveFile)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::USER + 4, onLeaveFile)
	END_EVENT_MAP(CBasePopup)

	// TODO: finish command map here
	BEGIN_CMD_MAP(CPopReadFile)
	END_CMD_MAP(CBasePopup)

	BOOLEAN CPopReadFile::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		m_funcWgtReadFile.initialize();
		CLogicManager::getInstance()->addEventListener(this);
		CScreenTouchServer::getInstance()->addEventListener(this);
		return TRUE;
	}

	void CPopReadFile::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_funcWgtReadFile.unInitialize();
		CLogicManager::getInstance()->removeEventListener(this);
		CScreenTouchServer::getInstance()->removeEventListener(this);

	}

	void CPopReadFile::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info here
		INIT::initPopup(this, RES_POP_READ_FILE);
		INIT::initImageField(m_imgBg, RES_ITM_BG, this);
		INIT::initFuncWidget(m_funcWgtReadFile, RES_ITM_READ_FILE, this);


		// TODO: set data to children controls or set children controls status here
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		m_funcWgtReadFile.refresh();


		UINT sw = pServer->getScreenContext()->uiScreenWidth;
		UINT sh = pServer->getScreenContext()->uiScreenHeight;

		INT x = (sw - m_funcWgtReadFile.getPreferredSize().getX()) / 2;
		INT y = (sh - m_funcWgtReadFile.getPreferredSize().getY()) / 2;

		m_funcWgtReadFile.setX(x);
		m_funcWgtReadFile.setY(y);

		setRealX(x);
		setRealY(y);
		m_imgBg.setPreferredSize(sw, sh);
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here


	// TODO: implement logic event listener here
	void CPopReadFile::onLeaveFile()
	{
		destroy();
	}

	// TODO: implement your normal private method here
}