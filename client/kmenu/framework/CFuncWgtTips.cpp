#include "HMI.h"

namespace OpenHMI
{
	CFuncWgtTips::CFuncWgtTips()
		: m_bNotCatchKey(FALSE)
	{
		// TODO: setup parent-child relationship with children controls here
		addComponent(&m_txtTitle);
		addComponent(&m_txt);
		addComponent(&m_btnQuit);
		addComponent(&m_btnOk);


		// TODO: add event listener to children controls here
		m_btnQuit.addEventListener(this);
		m_btnOk.addEventListener(this);


		// TODO: initialize children controls specific properties here
	}

	CFuncWgtTips::~CFuncWgtTips()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CFuncWgtTips)
		CONTROL_EVENT_HANDLER(&m_btnQuit, CEventButton::TOUCH_RELEASED, onBtnQuitTchRel)
		CONTROL_EVENT_HANDLER(&m_btnOk, CEventButton::TOUCH_RELEASED, onBtnOkTchRel)
		
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ENTER, onKeyEnter)
		LOGIC_EVENT_HANDLER(CScreenTouchServer::getInstance(), CScreenTouchServer::KEY_ESC, onKeyEsc)
	END_EVENT_MAP(CBaseFuncWidget)

	BOOLEAN CFuncWgtTips::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener

		CScreenTouchServer::getInstance()->addEventListener(this);
		return TRUE;
	}

	void CFuncWgtTips::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.

		CScreenTouchServer::getInstance()->removeEventListener(this);
	}

	void CFuncWgtTips::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info heree
		INIT::initFuncWidget(this, RES_FUNCWGT_TIPS);
		INIT::initTextField(m_txtTitle, RES_ITM_TITLE, this);
		INIT::initTextField(m_txt, RES_ITM_TXT, this);
		INIT::initButton(m_btnQuit, RES_ITM_BTN_QUIT, this);
		INIT::initButton(m_btnOk, RES_ITM_BTN_OK, this);


		// TODO: set data to children controls or set children controls status here
	}

	// TODO: implement your public method here

	// TODO: implement control event listener here
	void CFuncWgtTips::onBtnQuitTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leavePopTips();
	}

	void CFuncWgtTips::onBtnOkTchRel(CComponent *pTarget)
	{
		CLogicManager::getInstance()->leavePopTips();
	}

	// TODO: implement logic event listener here
	void CFuncWgtTips::onKeyEnter()
	{
#if 0
		if (m_bNotCatchKey == TRUE)
		{
			m_bNotCatchKey = FALSE;
			return;
		}
#endif
		onBtnOkTchRel(NULL);
	}

	void CFuncWgtTips::onKeyEsc()
	{
		onBtnQuitTchRel(NULL);
	}
	// TODO: implement your normal private method here
}