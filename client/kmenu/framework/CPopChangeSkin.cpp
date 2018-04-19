#include "HMI.h"

namespace OpenHMI
{
	CPopChangeSkin::CPopChangeSkin(BOOLEAN bModal)
		: CBasePopup(bModal)
		, m_iOrignalX(0)
		, m_iOrignalY(0)
	{
		// TODO: setup parent-child relationship with children controls here
		//addComponent(&m_imgBg);
		addComponent(&m_funcWgtChangeSkin);


		// TODO: add event listener to children controls here


		// TODO: initialize children controls specific properties here
	}

	CPopChangeSkin::~CPopChangeSkin()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CPopChangeSkin)
		LOGIC_EVENT_HANDLER(CLogicManager::getInstance(), CLogicManager::LEAVE_CHANGE_SKIN, onLeaveChangeSkin)
	END_EVENT_MAP(CBasePopup)

	// TODO: finish command map here
	BEGIN_CMD_MAP(CPopChangeSkin)
	END_CMD_MAP(CBasePopup)

	BOOLEAN CPopChangeSkin::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener
		m_funcWgtChangeSkin.initialize();
		CLogicManager::getInstance()->addEventListener(this);

		return TRUE;
	}

	void CPopChangeSkin::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
		m_funcWgtChangeSkin.unInitialize();
		CLogicManager::getInstance()->removeEventListener(this);

	}

	void CPopChangeSkin::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info here
		INIT::initPopup(this, RES_POP_CHANGE_SKIN);
		INIT::initImageField(m_imgBg, RES_ITM_BG, this);
		INIT::initFuncWidget(m_funcWgtChangeSkin, RES_ITM_CHANGE_SKIN, this);


		// TODO: set data to children controls or set children controls status here
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		m_funcWgtChangeSkin.refresh();


		UINT sw = pServer->getScreenContext()->uiScreenWidth;
		UINT sh = pServer->getScreenContext()->uiScreenHeight;

		INT x = (sw - m_funcWgtChangeSkin.getPreferredSize().getX()) / 2;
		INT y = (sh - m_funcWgtChangeSkin.getPreferredSize().getY()) / 2;

		m_funcWgtChangeSkin.setX(x);
		m_funcWgtChangeSkin.setY(y);

		setRealX(x);
		setRealY(y);
		m_imgBg.setPreferredSize(sw, sh);
	}

	// TODO: implement your public method here
#if 0
	void CPopChangeSkin::startShowAnimation(UINT uiIntervalTime, UINT uiRepeatCount)
	{
		setScaleWidth(0.0f);
		setScaleHeight(0.0f);
		m_iOrignalX = getRealX();
		m_iOrignalY = getRealY();
		setRealX(getRealX() + getRealWidth() / 2);
		setRealY(getRealY() + getRealHeight() / 2);
		CBasePopup::startShowAnimation(10, 5);
	}

	void CPopChangeSkin::doShowAnimation(UINT uiCurrentCount)
	{
		setScaleWidth(FLOAT(uiCurrentCount) / 5);
		setScaleHeight(FLOAT(uiCurrentCount) / 5);
		setRealX(m_iOrignalX + (5 - uiCurrentCount) * getRealWidth() / 10);
		setRealY(m_iOrignalY + (5 - uiCurrentCount) * getRealHeight() / 10);
		CBasePopup::doShowAnimation(uiCurrentCount);
	}

	void CPopChangeSkin::completeShowAnimaiton()
	{
		setScaleWidth(1.0f);
		setScaleHeight(1.0f);
		setRealX(m_iOrignalX);
		setRealY(m_iOrignalY);
		CBasePopup::completeShowAnimaiton();
	}

	void CPopChangeSkin::startHideAnimation(UINT uiIntervalTime, UINT uiRepeatCount)
	{
		setScaleWidth(1.0f);
		setScaleHeight(1.0f);
		m_iOrignalX = getRealX();
		m_iOrignalY = getRealY();
		CBasePopup::startHideAnimation(10, 5);
	}

	void CPopChangeSkin::doHideAnimation(UINT uiCurrentCount)
	{
		setScaleWidth(1.0f - FLOAT(uiCurrentCount) / 5);
		setScaleHeight(1.0f - FLOAT(uiCurrentCount) / 5);
		setRealX(m_iOrignalX + uiCurrentCount * getRealWidth() / 10);
		setRealY(m_iOrignalY + uiCurrentCount * getRealHeight() / 10);
		CBasePopup::doHideAnimation(uiCurrentCount);
	}

	void CPopChangeSkin::completeHideAnimaiton()
	{
		CBasePopup::completeHideAnimaiton();
	}
#endif
	// TODO: implement control event listener here


	// TODO: implement logic event listener here
	void CPopChangeSkin::onLeaveChangeSkin()
	{
		destroy();
	}

	// TODO: implement your normal private method here
}