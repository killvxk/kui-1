#include "OpenHMI.h"

namespace OpenHMI
{
	CBasePopup::CBasePopup(BOOLEAN bModal, BOOLEAN bAutoDestroy)
		: m_bModal(bModal)
		, m_bAutoDestroy(bAutoDestroy)
		, m_iRealX(0)
		, m_iRealY(0)
		, m_uiRealWidth(0)
		, m_uiRealHeight(0)
		, m_iOriginalX(-1)
		, m_iOriginalY(-1)
		, m_bIsShowAnimationRunning(TRUE)
	{
		setType(CONFIG::CONTAINER_TYPE_POPUP);

		m_timerAnimation.setListener(this);

		m_bModal = bModal;
		m_bAutoDestroy = bAutoDestroy;
		
		if (m_bModal)
		{
			setPreferredSize(ENV::getScreenWidth(), ENV::getScreenHeight());
		}
		else
		{
			setPreferredSize(0, 0);
		}
	}

	CBasePopup::~CBasePopup()
	{
		;
	}

	BEGIN_EVENT_MAP(CBasePopup)
	END_EVENT_MAP(CContainer)

	BEGIN_CMD_MAP(CBasePopup)
	END_CMD_MAP_2	

	BOOLEAN CBasePopup::doModal()
	{
		if (initialize())
		{
			refresh();
			resetLayout();
			
			CBaseMainFrame::getInstance()->addOverlay(this);
			
			if (CONFIG::getUseCCP())
			{
				if (hasFocus() == FALSE)
				{
					setFocus();
				}
			}

			startShowAnimation(0,0);
			return TRUE;
		}
		else
		{
			destroy();
			
			return FALSE;
		}
	}

	void CBasePopup::refresh()
	{
		pushDataToUI();

		CContainer::refresh();

		m_iOriginalX = m_iOriginalX == -1 ? m_iRealX : m_iOriginalX;
		m_iOriginalY = m_iOriginalY == -1 ? m_iRealY : m_iOriginalY;
	}

	void CBasePopup::destroy()
	{
		if (isDestroyed())
		{
			return;
		}

		if( m_timerAnimation.isRunning() )
		{
			completeHideAnimaiton();
		}
		else
		{
			startHideAnimation(0,0);
		}
		//delete this;
	}

	BOOLEAN CBasePopup::doFocusBack()
	{
		if (CContainer::doFocusBack() == FALSE)
		{
			destroy();
		}
		
		return TRUE;
	}

	ICursorTracker* CBasePopup::probe(INT iX, INT iY)
	{
		ICursorTracker* pTracker = CContainer::probe(iX, iY);
		
		if (pTracker != NULL)
		{
			return pTracker;
		}
		else
		{
			INT iRealX = m_iRealX - (INT)ENV::getHitOffset();
			INT iRealY = m_iRealY - (INT)ENV::getHitOffset();

			if (   iRealX <= iX && iX <= iRealX + (INT)m_uiRealWidth + (INT)ENV::getHitOffset() * 2
				&& iRealY <= iY && iY <= iRealY + (INT)m_uiRealHeight + (INT)ENV::getHitOffset() * 2   )
			{
				return this;
			}
			
			if (!m_bModal && m_bAutoDestroy)
			{
				destroy();

				Array2<CComponent*> aryEmpty;
				CContainer* pTopOverlay = dynamic_cast<CContainer*>(CBaseMainFrame::getInstance()->getTopOverlay(aryEmpty));

				if (pTopOverlay != NULL && !pTopOverlay->isLock())
				{
					if (pTopOverlay->getType() == CONFIG::CONTAINER_TYPE_POPUP)
					{
						return NULL;
					}
				}

				return this;
			}
			
			return this;
		}
	}

	void CBasePopup::buildRenderTree(CRenderGroup &renderGroup)
	{
		CPoint pt = localToGlobal(0, 0);
		m_renderGroup.setX(pt.getX() + m_iRealX);
		m_renderGroup.setY(pt.getY() + m_iRealY);
		m_renderGroup.setWidth(m_uiRealWidth);
		m_renderGroup.setHeight(m_uiRealHeight);
		renderGroup.addRenderItem(&m_renderGroup);

		buildRenderTreeFromBackground(m_renderGroup);

		setX(m_iRealX - m_iOriginalX);
		setY(m_iRealY - m_iOriginalY);
		buildRenderTreeFromChildren(m_renderGroup);		

		buildRenderTreeFromCCP(m_renderGroup);
		setX(0);
		setY(0);
	}

	void CBasePopup::buildRenderTreeFromBackground(CRenderGroup &renderGroup)
	{
		if (getVisible() && m_skin.isNotNull())
		{
			m_renderImage.setFilePath(m_skin->getCurrentImage());

			CPoint pt = localToGlobal(m_iRealX, m_iRealY);

			m_renderImage.setX(pt.getX());
			m_renderImage.setY(pt.getY());
			m_renderImage.setWidth(m_uiRealWidth);
			m_renderImage.setHeight(m_uiRealHeight);

			m_renderImage.setLayout(CRenderImage::L_NORMAL);

			renderGroup.addRenderItem(&m_renderImage);
		}
	}

	void CBasePopup::startShowAnimation(UINT uiIntervalTime, UINT uiRepeatCount)
	{
		lock();

		if (uiIntervalTime > 0 && uiRepeatCount > 0)
		{
			m_bIsShowAnimationRunning = TRUE;
			m_timerAnimation.stop();
			m_timerAnimation.setDelay(uiIntervalTime);
			m_timerAnimation.setRepeatCount(uiRepeatCount);
			m_timerAnimation.start();
		}
		else
		{
			completeShowAnimaiton();
		}
	}

	void CBasePopup::doShowAnimation(UINT uiCurrentCount)
	{
		;
	}

	void CBasePopup::completeShowAnimaiton()
	{
		unlock();
	}

	void CBasePopup::startHideAnimation(UINT uiIntervalTime, UINT uiRepeatCount)
	{
		lock();

		if (uiIntervalTime > 0 && uiRepeatCount > 0)
		{
			m_bIsShowAnimationRunning = FALSE;
			m_timerAnimation.stop();
			m_timerAnimation.setDelay(uiIntervalTime);
			m_timerAnimation.setRepeatCount(uiRepeatCount);
			m_timerAnimation.start();
		}
		else
		{
			completeHideAnimaiton();
		}
	}

	void CBasePopup::doHideAnimation(UINT uiCurrentCount)
	{
		;
	}

	void CBasePopup::completeHideAnimaiton()
	{
		unlock();

		unInitialize();

		CBaseMainFrame::getInstance()->removeOverlay(this);

		CContainer::destroy();

		dispatchEvent(CEventPopup::DESTROYED);
	}

	void CBasePopup::onTimer(const CTimer *pTimer, UINT uiCurrentCount)
	{
		if( m_bIsShowAnimationRunning )
		{
			doShowAnimation(uiCurrentCount);
		}
		else
		{
			doHideAnimation(uiCurrentCount);
		}
	}

	void CBasePopup::onTimerComplete(const CTimer *pTimer)
	{
		m_timerAnimation.stop();

		if( m_bIsShowAnimationRunning )
		{
			completeShowAnimaiton();
		}
		else
		{
			completeHideAnimaiton();
		}
	}
}