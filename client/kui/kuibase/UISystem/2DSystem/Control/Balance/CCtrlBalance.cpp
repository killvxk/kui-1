#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlBalance::CCtrlBalance()
		: m_iZoneX(0)
		, m_iZoneY(0)
		, m_iZoneWidth(0)
		, m_iZoneHeight(0)
		, m_bDragSlider(FALSE)
		, m_bCCCHoritontal(FALSE)
		, m_bCircleAdjust(FALSE)
		, m_bVLineHighLight(FALSE)
		, m_bHLineHighLight(FALSE)
		
	{
		m_model.setComponent(this);
		setBaseView(&m_view);
	}

	CCtrlBalance::~CCtrlBalance()
	{
		;
	}	

	void CCtrlBalance::setSkinVerticalLine(UINT uiImageID)
	{
		m_view.setSkinVerticalLine(uiImageID);
	}

	void CCtrlBalance::setSkinHorizontalLine(UINT uiImageID)
	{
		m_view.setSkinHorizontalLine(uiImageID);
	}

	void CCtrlBalance::setSkinSlider(UINT uiImageID)
	{
		m_view.setSkinSlider(uiImageID);
	}

	void CCtrlBalance::setZoneX(UINT uiValue)
	{
		m_iZoneX = (INT)uiValue;
	}

	void CCtrlBalance::setZoneY(UINT uiValue)
	{
		m_iZoneY = (INT)uiValue;
	}

	void CCtrlBalance::setZoneWidth(UINT uiValue)
	{
		m_iZoneWidth  = (INT)uiValue;
	}

	void CCtrlBalance::setZoneHeight(UINT uiValue)
	{
		m_iZoneHeight  = (INT)uiValue;
	}

	void CCtrlBalance::circleAdjust(BOOLEAN bValue)
	{
		m_bCircleAdjust = bValue;
	}

	void CCtrlBalance::setVLineHighLight(BOOLEAN bValue)
	{
		m_bHLineHighLight = FALSE;

		m_bVLineHighLight = bValue;
		setDirty();	
	}

	void CCtrlBalance::setHLineHighLight(BOOLEAN bValue)
	{
		m_bVLineHighLight = FALSE;

		m_bHLineHighLight = bValue;
		setDirty();	
	}

	void CCtrlBalance::refresh()
	{
		CComponent::refresh();
		
		if (m_pMCVerticalLine.isNull()) 
		{
			m_pMCVerticalLine = m_view.getSkinVerticalLine();
		}

		if (m_pMCHorizontalLine.isNull())
		{
			m_pMCHorizontalLine = m_view.getSkinHorizontalLine();
		}

		if (m_pMCSlider.isNull())
		{
			m_pMCSlider = m_view.getSkinSlider();
		}

		compose();
		updateView();
	}

	void CCtrlBalance::littleRefresh()
	{
		CComponent::littleRefresh();

 		compose();
 		updateView();
	}

	void CCtrlBalance::destroySkin()
	{
		m_pMCVerticalLine.setNull();
		m_pMCHorizontalLine.setNull();
		m_pMCSlider.setNull();

		CComponent::destroySkin();
	}

	void CCtrlBalance::destroy()
	{	
		if (isDestroyed())
		{
			return;
		}

		CComponent::destroy();
	}

	void CCtrlBalance::buildRenderTree(CRenderGroup &renderGroup)
	{
		if (getVisible())
		{
			CPoint pt = localToGlobal(0, 0);
			m_renderGroup.setX(pt.getX());
			m_renderGroup.setY(pt.getY());
			m_renderGroup.setWidth(getPreferredSize().getX());
			m_renderGroup.setHeight(getPreferredSize().getY());
			renderGroup.addRenderItem(&m_renderGroup);

			buildRenderTreeFromBackground(m_renderGroup);

			// add VLine
			pt = localToGlobal( m_pMCVerticalLine->getX(), m_pMCVerticalLine->getY() );
			m_renderImageVLine.setFilePath(m_pMCVerticalLine->getCurrentImage());			
			m_renderImageVLine.setX(pt.getX());
			m_renderImageVLine.setY(pt.getY());
			m_renderImageVLine.setWidth(m_pMCVerticalLine->getWidth());
			m_renderImageVLine.setHeight(m_pMCVerticalLine->getScaleHeight());
			m_renderImageVLine.setLayout(CRenderImage::L_NORMAL);

			m_renderGroup.addRenderItem((CRenderItem*)&m_renderImageVLine);

			// Add HLine
			pt = localToGlobal(m_pMCHorizontalLine->getX(), m_pMCHorizontalLine->getY());

			m_renderImageHLine.setFilePath(m_pMCHorizontalLine->getCurrentImage());			
			m_renderImageHLine.setX(pt.getX());
			m_renderImageHLine.setY(pt.getY());
			m_renderImageHLine.setWidth(m_pMCHorizontalLine->getScaleWidth());
			m_renderImageHLine.setHeight(m_pMCHorizontalLine->getHeight());
			m_renderImageHLine.setLayout(CRenderImage::L_NORMAL);

			m_renderGroup.addRenderItem((CRenderItem*)&m_renderImageHLine);

			// Add Slider
			pt = localToGlobal( m_pMCSlider->getX(), m_pMCSlider->getY() );

			m_renderImageSlider.setFilePath(m_pMCSlider->getCurrentImage());			
			m_renderImageSlider.setX(pt.getX());
			m_renderImageSlider.setY(pt.getY());
			m_renderImageSlider.setWidth(m_pMCSlider->getWidth());
			m_renderImageSlider.setHeight(m_pMCSlider->getHeight());
			m_renderImageSlider.setLayout(CRenderImage::L_NORMAL);

			m_renderGroup.addRenderItem((CRenderItem*)&m_renderImageSlider);

			if (m_skin.isNotNull())
			{
				buildRenderTreeFromCCP(m_renderGroup, 3);
			}
			else
			{
				buildRenderTreeFromCCP(m_renderGroup, 2);
			}
		}
	}	

	void CCtrlBalance::setFocus(ECCPOperation eCCPOperation)
	{
		ccpHighLightLine();

		CComponent::setFocus(eCCPOperation);
	}

	void CCtrlBalance::releaseFocus()
	{
		m_bHLineHighLight = FALSE;
		m_bVLineHighLight = FALSE;

		CComponent::releaseFocus();
	}
	
	BOOLEAN CCtrlBalance::turnFocusToLeft(UINT uiStep)
	{
		if (m_bCCCHoritontal)
		{
			if (m_model.getXPos() == m_model.getXMin())
			{
				if (m_bCircleAdjust)
				{
					m_model.setXPos( m_model.getXMax() );
				}
				else
				{
					return TRUE;
				}
			}
			else
			{
				m_model.setXPos( m_model.getXPos() - uiStep * m_model.getXStep() );
			}
		}
		else
		{
			if (m_model.getYPos() == m_model.getYMax())
			{
				if (m_bCircleAdjust)
				{
					m_model.setYPos( m_model.getYMin() );
				}
				else
				{
					return TRUE;
				}
			}
			else
			{
				m_model.setYPos( m_model.getYPos() + uiStep * m_model.getYStep() );
			}
		}

		compose();
		updateView();

		setDirty();

		dispatchEvent(CEventBalance::POS_CHANGED);

		return TRUE;
	}

	BOOLEAN CCtrlBalance::turnFocusToRight(UINT uiStep)
	{
		if (m_bCCCHoritontal)
		{
			if (m_model.getXPos() == m_model.getXMax())
			{
				if (m_bCircleAdjust)
				{
					m_model.setXPos( m_model.getXMin() );
				}
				else
				{
					return true;
				}
			}
			else
			{
				m_model.setXPos( m_model.getXPos() + uiStep * m_model.getXStep() );
			}
		}
		else
		{
			if (m_model.getYPos() == m_model.getYMin())
			{
				if (m_bCircleAdjust)
				{
					m_model.setYPos( m_model.getYMax() );
				}
				else
				{
					return true;
				}
			}
			else
			{
				m_model.setYPos( m_model.getYPos() - uiStep * m_model.getYStep() );
			}
		}

		compose();
		updateView();

		setDirty();

		dispatchEvent(CEventBalance::POS_CHANGED);

		return true;
	}

	// Input Event
	void CCtrlBalance::doFocusEnterPress()
	{
		m_bCCCHoritontal = !m_bCCCHoritontal;

		ccpHighLightLine();

		littleRefresh();
		setDirty();
	}

	void CCtrlBalance::onTouchPress(INT iX, INT iY)
	{
		// inside the zone.
		CPoint pt = globalToLocal(iX, iY);
		if (pt.getX() >= m_iZoneX && pt.getX() <= (m_iZoneX + m_iZoneWidth) && pt.getY() >= m_iZoneY && pt.getY() <= (m_iZoneY + m_iZoneHeight) )
		{
			setTouchFocus(TRUE);
			m_bHLineHighLight = TRUE;
			m_bVLineHighLight = TRUE;
			
			if ( m_pMCSlider->hitTest(pt.getX(), pt.getY()) )
			{
				updateView();

				m_bDragSlider = TRUE;

				return;
			}

			resetPos(iX, iY);
		}
	}

	void CCtrlBalance::onTouchPressRelease(INT iX, INT iY)
	{
		onTouchRelease();
	}

	void CCtrlBalance::onTouchLongPressRelease(INT iX, INT iY)
	{
		onTouchRelease();
	}

	void CCtrlBalance::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
	{
		// inside the zone.
		CPoint pt = globalToLocal(iX, iY);
		//if (pt.getX() >= m_iZoneX && pt.getX() <= (m_iZoneX + m_iZoneWidth) && pt.getY() >= m_iZoneY && pt.getY() <= (m_iZoneY + m_iZoneHeight) )
		{
			if (m_bDragSlider)
			{
				resetPos(iX, iY);
			}
		}
	}
 
	void CCtrlBalance::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
	{
		onTouchRelease();
	}

	void CCtrlBalance::resetPos(INT iX, INT iY)
	{
		if (m_pMCVerticalLine == NULL || m_pMCHorizontalLine == NULL || m_pMCSlider == NULL)
		{
			return;
		}

		CPoint pt = globalToLocal(iX, iY);

		m_model.setXPos( m_model.getXMin() + (INT)( m_model.getXRange() * (FLOAT)(pt.getX() - m_iZoneX) / m_iZoneWidth ) );
		m_model.setYPos( m_model.getYMin() + (INT)( m_model.getYRange() * (FLOAT)(1.0f - (FLOAT)(pt.getY() - m_iZoneY) / m_iZoneHeight ) ) );

		littleRefresh();

		setDirty();

		dispatchEvent(CEventBalance::POS_CHANGED);
	}

	void CCtrlBalance::compose()
	{
		if (m_pMCVerticalLine == NULL || m_pMCHorizontalLine == NULL || m_pMCSlider == NULL)
		{
			return;
		}

		INT i = (INT)m_pMCVerticalLine->getWidth() / 2;
		INT j = (INT)m_pMCHorizontalLine->getHeight() / 2;

		m_pMCVerticalLine->setX( (INT)(m_iZoneX + m_model.getXPosRatio() * m_iZoneWidth) - i);
		m_pMCVerticalLine->setY(m_iZoneY);
		m_pMCVerticalLine->setScaleHeight(m_iZoneHeight);

		m_pMCHorizontalLine->setY( (INT)(m_iZoneY + (FLOAT)(1.0f - m_model.getYPosRatio()) * m_iZoneHeight) - j);
		m_pMCHorizontalLine->setX(m_iZoneX);
		m_pMCHorizontalLine->setScaleWidth(m_iZoneWidth);

		m_pMCSlider->setX( m_pMCVerticalLine->getX() + i - (INT)m_pMCSlider->getWidth() / 2 );
		m_pMCSlider->setY( m_pMCHorizontalLine->getY() + j - (INT)m_pMCSlider->getHeight() / 2);
	}
	
	void CCtrlBalance::onTouchRelease()
	{
		ENV::switchCCPFocus(this);

		if (innerGetFocus()) 
		{
			ccpHighLightLine();
		}
		else
		{
			m_bVLineHighLight = FALSE;
			m_bHLineHighLight = FALSE;
		}

		m_bDragSlider = FALSE;

		setTouchFocus(FALSE);

		updateView();

		setDirty();
	}

	void CCtrlBalance::updateView()
	{
		if (m_pMCVerticalLine == NULL || m_pMCHorizontalLine == NULL || m_pMCSlider == NULL)
		{
			return;
		}

		if (innerGetFocus())
		{
			if (m_CCPSkin == NULL)
			{
				m_view.drawSkin(m_pMCSlider, TRUE);
			}
			else
			{
				m_view.drawSkin(m_pMCSlider, getTouchFocus());

				m_CCPSkin->setX( m_pMCSlider->getX() - ( (INT)m_CCPSkin->getWidth() - (INT)m_pMCSlider->getWidth() ) / 2 );
				m_CCPSkin->setY( m_pMCSlider->getY() - ( (INT)m_CCPSkin->getHeight() -  (INT)m_pMCSlider->getHeight() ) / 2 );
			}
		}
		else
		{
			m_view.drawSkin(m_pMCSlider, getTouchFocus());			
		}

		m_view.drawSkin(m_pMCVerticalLine, m_bVLineHighLight);
		m_view.drawSkin(m_pMCHorizontalLine, m_bHLineHighLight);
	}

	void CCtrlBalance::ccpHighLightLine()
	{
		if (m_bCCCHoritontal)
		{
			m_bVLineHighLight = TRUE;
			m_bHLineHighLight = FALSE;
		}
		else
		{
			m_bHLineHighLight = TRUE;
			m_bVLineHighLight = FALSE;
		}
	}		
}