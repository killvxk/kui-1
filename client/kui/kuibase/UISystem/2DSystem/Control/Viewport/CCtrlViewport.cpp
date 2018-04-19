#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlViewport::CCtrlViewport()
		: m_eTS(TS_UNKNOW)
		, m_iTouchX(0)
		, m_iTouchY(0)
		, m_iSpeedX(0)
		, m_iSpeedY(0)
	{
		m_timer.setListener(this);
	}

	CCtrlViewport::~CCtrlViewport()
	{
		m_timer.stop();
		m_timer.setListener(NULL);
	}

	void CCtrlViewport::setStyle(CRenderViewport::CBaseStyle *pStyle)
	{
		if (m_ptrSytle != pStyle)
		{
			m_ptrSytle = pStyle;
			m_renderViewport.setStyle(pStyle);

			if (pStyle != NULL )
			{
				if (pStyle->getType() == CRenderViewport::ST_WAVER)
				{
					m_renderGroup.setFrequentChangeFlag(TRUE);

					m_timer.setDelay( 30 );
					m_timer.start();
				}
				else
				{
					m_renderGroup.setFrequentChangeFlag(FALSE);

					m_timer.stop();
				}
			}
			else
			{
				m_renderGroup.setFrequentChangeFlag(FALSE);
				m_timer.stop();
			}

			setDirty();
		}	
	}

	void CCtrlViewport::updateStyle()
	{
		m_renderViewport.updateStyle();

		setDirty();
	}

	void CCtrlViewport::onTouchPress(INT iX, INT iY)
	{
		if (getEnable())
		{			
			m_iTouchX = iX;
			m_iTouchY = iY;
			m_iSpeedX = 0;
			m_iSpeedY = 0;
			m_eTS	  = TS_PRESS;

			dispatchEvent(CEventViewport::TOUCHED);
		}
		else
		{
			;
		}
	}

	void CCtrlViewport::onTouchPressRelease(INT iX, INT iY)
	{
		if (getEnable())
		{
			m_iTouchX = iX;
			m_iTouchY = iY;
			m_iSpeedX = 0;
			m_iSpeedY = 0;
			m_eTS	  = TS_PRESS_RELEASE;

			dispatchEvent(CEventViewport::TOUCHED);
		}
		else 
		{
			;
		}	
	}

	void CCtrlViewport::onTouchLongPress(INT iX, INT iY)
	{
		if (getEnable())
		{
			m_iTouchX = iX;
			m_iTouchY = iY;
			m_iSpeedX = 0;
			m_iSpeedY = 0;
			m_eTS	  = TS_LONG_PRESS;

			dispatchEvent(CEventViewport::TOUCHED);
		}
		else
		{
			;
		}

	}

	void CCtrlViewport::onTouchContinuePress(INT iX, INT iY)
	{
		if (getEnable())
		{
			m_iTouchX = iX;
			m_iTouchY = iY;
			m_iSpeedX = 0;
			m_iSpeedY = 0;
			m_eTS	  = TS_CONTINUE_PRESS;

			dispatchEvent(CEventViewport::TOUCHED);
		}
		else
		{
			;
		}
	}

	void CCtrlViewport::onTouchLongPressRelease(INT iX, INT iY)
	{
		if (getEnable())
		{
			m_iTouchX = iX;
			m_iTouchY = iY;
			m_iSpeedX = 0;
			m_iSpeedY = 0;
			m_eTS	  = TS_LONG_PRESS_RELEASE;

			dispatchEvent(CEventViewport::TOUCHED);
		}
		else 
		{
			;
		}	
	}

	void CCtrlViewport::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
	{
		if (getEnable())
		{
			m_iTouchX = iX;
			m_iTouchY = iY;
			m_iSpeedX = 0;
			m_iSpeedY = 0;
			m_eTS	  = TS_MOVE;

			dispatchEvent(CEventViewport::TOUCHED);
		}
		else
		{
			;
		}
	}

	void CCtrlViewport::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
	{
		if (getEnable())
		{
			m_iTouchX = iX;
			m_iTouchY = iY;
			m_iSpeedX = iSpeedX;
			m_iSpeedY = iSpeedY;
			m_eTS     = TS_MOVE_RELEASE;

			dispatchEvent(CEventViewport::TOUCHED);
		}
		else
		{
			;
		}
	}

	void CCtrlViewport::onTouchDrag(INT iX, INT iY)
	{
		if (getEnable())
		{
			m_iTouchX = iX;
			m_iTouchY = iY;
			m_iSpeedX = 0;
			m_iSpeedY = 0;
			m_eTS     = TS_DRAG;

			dispatchEvent(CEventViewport::TOUCHED);
		}
		else
		{
			;
		}
	}

	void CCtrlViewport::onTouchDragRelease(INT iX, INT iY)
	{
		if (getEnable())
		{
			m_iTouchX = iX;
			m_iTouchY = iY;
			m_iSpeedX = 0;
			m_iSpeedY = 0;
			m_eTS     = TS_DRAG_RELEASE;

			dispatchEvent(CEventViewport::TOUCHED);
		}
		else
		{
			;
		}
	}

	void CCtrlViewport::onTouchDoublePress(INT iX, INT iY)
	{
		if (getEnable())
		{
			m_iTouchX = iX;
			m_iTouchY = iY;
			m_iSpeedX = 0;
			m_iSpeedY = 0;
			m_eTS     = TS_DOUBLE_PRESS;

			dispatchEvent(CEventViewport::TOUCHED);
		}
		else
		{
			;
		}
	}

	void CCtrlViewport::onTouchDoublePressRelease(INT iX, INT iY)
	{
		if (getEnable())
		{
			m_iTouchX = iX;
			m_iTouchY = iY;
			m_iSpeedX = 0;
			m_iSpeedY = 0;
			m_eTS     = TS_DOUBLE_PRESS_RELEASE;

			dispatchEvent(CEventViewport::TOUCHED);
		}
		else
		{
			;
		}
	}

	void CCtrlViewport::destroySkin()
	{
		CComponent::destroySkin();

		m_timer.stop();

		m_ptrSytle = NULL;
	}

	void CCtrlViewport::buildRenderTree(CRenderGroup &renderGroup)
	{
		if (getVisible())
		{
			CPoint pt = localToGlobal(0, 0);
			m_renderGroup.setX(pt.getX());
			m_renderGroup.setY(pt.getY());
			m_renderGroup.setWidth(getPreferredSize().getX());
			m_renderGroup.setHeight(getPreferredSize().getY());
			renderGroup.addRenderItem((CRenderItem*)&m_renderGroup);

			m_renderViewport.setX(pt.getX());
			m_renderViewport.setY(pt.getY());
			m_renderViewport.setWidth(getPreferredSize().getX());
			m_renderViewport.setHeight(getPreferredSize().getY());

			m_renderGroup.addRenderItem(&m_renderViewport);
		}
	}

	void CCtrlViewport::onTimer(const CTimer *pTimer, UINT uiCurrentCount)
	{
		m_renderViewport.updateStyle();

		setDirty();
	}
}