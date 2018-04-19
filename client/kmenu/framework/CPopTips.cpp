#include "HMI.h"

namespace OpenHMI
{
	CPopTips::CPopTips(BOOLEAN bModal)
		: CBasePopup(bModal)
		, m_iOrignalX(0)
		, m_iOrignalY(0)
	{
		// TODO: setup parent-child relationship with children controls here
		//addComponent(&m_imgBg);
		addComponent(&m_funcWgtTips);


		// TODO: add event listener to children controls here


		// TODO: initialize children controls specific properties here
	}

	CPopTips::~CPopTips()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CPopTips)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LEAVE_POPTIPS, onLeavePopTips)
	END_EVENT_MAP(CBasePopup)

	// TODO: finish command map here
	BEGIN_CMD_MAP(CPopTips)
	END_CMD_MAP(CBasePopup)

	BOOLEAN CPopTips::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		m_funcWgtTips.initialize();
		CLogicManager::getInstance()->addEventListener(this);

		return TRUE;
	}

	void CPopTips::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_funcWgtTips.unInitialize();

		CLogicManager::getInstance()->removeEventListener(this);
	}

	void CPopTips::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info here
		INIT::initPopup(this, RES_POP_TIPS);
		INIT::initImageField(m_imgBg, RES_ITM_BG, this);
		INIT::initFuncWidget(m_funcWgtTips, RES_ITM_TIPS, this);


		// TODO: set data to children controls or set children controls status here
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		m_funcWgtTips.refresh();


		UINT sw = pServer->getScreenContext()->uiScreenWidth;
		UINT sh = pServer->getScreenContext()->uiScreenHeight;

		INT x = (sw - m_funcWgtTips.getPreferredSize().getX()) / 2;
		INT y = (sh - m_funcWgtTips.getPreferredSize().getY()) / 2;

		m_funcWgtTips.setX(x);
		m_funcWgtTips.setY(y);
		m_funcWgtTips.setContent(m_strTips);
		setRealX(x);
		setRealY(y);
		m_imgBg.setPreferredSize(sw, sh);
		m_imgBg.setVisible(m_bShowBg);
	}

	// TODO: implement your public method here
#if 0
	void CPopTips::startShowAnimation(UINT uiIntervalTime, UINT uiRepeatCount)
	{
		setScaleWidth(0.0f);
		setScaleHeight(0.0f);
		m_iOrignalX = getRealX();
		m_iOrignalY = getRealY();
		setRealX(getRealX() + getRealWidth() / 2);
		setRealY(getRealY() + getRealHeight() / 2);
		CBasePopup::startShowAnimation(10, 20);
	}

	void CPopTips::doShowAnimation(UINT uiCurrentCount)
	{
		setScaleWidth(FLOAT(uiCurrentCount) / 20);
		setScaleHeight(FLOAT(uiCurrentCount) / 20);
		setRealX(m_iOrignalX + (20 - uiCurrentCount) * getRealWidth() / 40);
		setRealY(m_iOrignalY + (20 - uiCurrentCount) * getRealHeight() / 40);
		CBasePopup::doShowAnimation(uiCurrentCount);
	}

	void CPopTips::completeShowAnimaiton()
	{
		setScaleWidth(1.0f);
		setScaleHeight(1.0f);
		setRealX(m_iOrignalX);
		setRealY(m_iOrignalY);
		CBasePopup::completeShowAnimaiton();
	}

	void CPopTips::startHideAnimation(UINT uiIntervalTime, UINT uiRepeatCount)
	{
		setScaleWidth(1.0f);
		setScaleHeight(1.0f);
		m_iOrignalX = getRealX();
		m_iOrignalY = getRealY();
		CBasePopup::startHideAnimation(10, 20);
	}

	void CPopTips::doHideAnimation(UINT uiCurrentCount)
	{
		setScaleWidth(1.0f - FLOAT(uiCurrentCount) / 20);
		setScaleHeight(1.0f - FLOAT(uiCurrentCount) / 20);
		setRealX(m_iOrignalX + uiCurrentCount * getRealWidth() / 40);
		setRealY(m_iOrignalY + uiCurrentCount * getRealHeight() / 40);
		CBasePopup::doHideAnimation(uiCurrentCount);
	}

	void CPopTips::completeHideAnimaiton()
	{
		CBasePopup::completeHideAnimaiton();
	}
#endif
	// TODO: implement control event listener here

	// TODO: implement logic event listener here
	void CPopTips::onLeavePopTips()
	{
		destroy();
	}
	// TODO: implement your normal private method here
}