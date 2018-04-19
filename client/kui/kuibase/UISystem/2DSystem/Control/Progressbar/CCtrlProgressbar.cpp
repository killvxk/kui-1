#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlProgressbar::CCtrlProgressbar()
		: m_iClipY(0)
		, m_bCircleAdjust(TRUE)
		, m_bIsAdjustMode(FALSE)
		, m_bIsPosAlterWhileAdjustCCP(FALSE)
		, m_iAdjustStep(0)
		, m_bDragSlider(FALSE)
		, m_bIsHorizontalPGB(TRUE)
		, m_iPosRecordNew(0x0fffffff)
		, m_bUpdateLockOn(FALSE)
	{
		m_model.setComponent(this);
		setBaseView(&m_view);
		
		notAcceptFocus();
	}

	CCtrlProgressbar::~CCtrlProgressbar()
	{
		;
	}

	void CCtrlProgressbar::setSkinRegularBar(UINT uiImageID)
	{
		m_view.setSkinRegularBar(uiImageID);
	}

	void CCtrlProgressbar::setSkinRegularSlider(UINT uiImageID)
	{
		m_view.setSkinRegularSlider(uiImageID);
	}

	void CCtrlProgressbar::setSkinIrregularBar(UINT uiImageID)
	{
		m_view.setSkinIrregularBar(uiImageID);
	}

	void CCtrlProgressbar::setSkinIrregularSlider(UINT uiImageID)
	{
		m_view.setSkinIrregularSlider(uiImageID);
	}

	void CCtrlProgressbar::setBarArea(UINT uiX, UINT uiY, UINT uiWidth, UINT uiHeight)
	{
		m_rcBarArea.setX((INT)uiX);
		m_rcBarArea.setY((INT)uiY);
		m_rcBarArea.setWidth(uiWidth);
		m_rcBarArea.setHeight(uiHeight);
	}

	void CCtrlProgressbar::setIsAdjustMode(BOOLEAN bValue)
	{ 
		m_bIsAdjustMode = bValue;

		if (m_bIsAdjustMode)
		{
			setAcceptFocus();
		}
		else
		{
			notAcceptFocus();
		}
	}

	void CCtrlProgressbar::setProgressInfo(INT iMin, INT iMax, INT iPos, INT iStep)
	{
		if ( m_bIsAdjustMode && m_bUpdateLockOn )
		{
			if ( m_bDragSlider )
			{
				return;
			}
			else
			{
				m_bUpdateLockOn = (iPos == m_iPosRecordNew) ? FALSE : TRUE;
				
				if ( m_bUpdateLockOn && m_iPosRecordNew != 0x0fffffff )
				{
					return ;
				}
			}
		}
		
		m_model.setProgressInfo(iMin, iMax, iPos, iStep);
	}

	void CCtrlProgressbar::setPos(INT iValue)
	{
		if ( m_bIsAdjustMode && m_bUpdateLockOn )
		{
			if ( m_bDragSlider )
			{
				return;
			}
			else
			{
				m_bUpdateLockOn = (iValue == m_iPosRecordNew) ? FALSE : TRUE;
				
				if ( m_bUpdateLockOn && m_iPosRecordNew != 0x0fffffff )
				{
					return ;
				}
			}
		}
		
		m_model.setPos(iValue);
	}

	void CCtrlProgressbar::refresh()
	{
		CComponent::refresh();
		
		if (m_pMCRegularBar.isNull())
		{
			m_pMCRegularBar = m_view.getSkinRegularBar();
		}

		if (m_pMCRegularSlider.isNull())
		{
			m_pMCRegularSlider = m_view.getSkinRegularSlider();
		}

		if (m_pMCIrregularBar.isNull())
		{
			m_pMCIrregularBar = m_view.getSkinIrregularBar();
		}

		if (m_pMCIrregularSlider.isNull())
		{
			m_pMCIrregularSlider = m_view.getSkinIrregularSlider();
		}	

		m_view.drawSkinSlider(m_pMCRegularSlider, FALSE);
		m_view.drawSkinSlider(m_pMCIrregularSlider, FALSE);

		compose();
	}

	void CCtrlProgressbar::littleRefresh()
	{
		refresh();
	}

	void CCtrlProgressbar::destroySkin()
	{
		m_pMCRegularBar.setNull();
		m_pMCRegularSlider.setNull();
		m_pMCIrregularBar.setNull();
		m_pMCIrregularSlider.setNull();

		CComponent::destroySkin();
	}

	void CCtrlProgressbar::destroy()
	{
		if (isDestroyed())
		{
			return;
		}

		CComponent::destroy();
	}

	void CCtrlProgressbar::buildRenderTree(CRenderGroup &renderGroup)
	{
		if (getVisible() == FALSE)
		{
			return;
		}

		CPoint pt = localToGlobal(0, 0);
		m_renderGroup.setX(pt.getX());
		m_renderGroup.setY(pt.getY());
		m_renderGroup.setWidth(getPreferredSize().getX());
		m_renderGroup.setHeight(getPreferredSize().getY());
		m_renderGroup.setFrequentChangeFlag(TRUE);
		renderGroup.addRenderItem((CRenderItem*)&m_renderGroup);

		buildRenderTreeFromBackground(m_renderGroup);

		if (m_pMCIrregularBar.isNotNull())
		{
			pt = localToGlobal( m_pMCIrregularBar->getX(), m_pMCIrregularBar->getY() );

			m_renderImageIrregularBar.setFilePath(m_pMCIrregularBar->getCurrentImage());
			m_renderImageIrregularBar.setX(pt.getX());
			m_renderImageIrregularBar.setY(pt.getY());
			m_renderImageIrregularBar.setWidth(m_pMCIrregularBar->getScaleWidth());
			m_renderImageIrregularBar.setHeight(m_pMCIrregularBar->getScaleHeight());
			m_renderImageIrregularBar.setLayout(CRenderImage::L_NORMAL);

			if (!m_bIsHorizontalPGB)
			{
				m_renderImageIrregularBar.setNormalLayoutClipOffsetY(m_iClipY);
			}

			m_renderGroup.addRenderItem((CRenderItem*)&m_renderImageIrregularBar);
		}
		else if (m_pMCRegularBar.isNotNull())
		{
			pt = localToGlobal( m_pMCRegularBar->getX(), m_pMCRegularBar->getY() );

			m_renderImageRegularBar.setFilePath(m_pMCRegularBar->getCurrentImage());
			m_renderImageRegularBar.setX(pt.getX());
			m_renderImageRegularBar.setY(pt.getY());
			m_renderImageRegularBar.setWidth(m_pMCRegularBar->getScaleWidth());
			m_renderImageRegularBar.setHeight(m_pMCRegularBar->getScaleHeight());
			m_renderImageRegularBar.setLayout(CRenderImage::L_NORMAL);

			if (!m_bIsHorizontalPGB)
			{
				m_renderImageRegularBar.setNormalLayoutClipOffsetY(m_iClipY);
			}

			m_renderGroup.addRenderItem((CRenderItem*)&m_renderImageRegularBar);			
		}

		if (m_pMCIrregularSlider.isNotNull())
		{
			pt = localToGlobal( m_pMCIrregularSlider->getX(), m_pMCIrregularSlider->getY() );

			m_renderImageIrregularSlider.setFilePath(m_pMCIrregularSlider->getCurrentImage());
			m_renderImageIrregularSlider.setX(pt.getX());
			m_renderImageIrregularSlider.setY(pt.getY());
			m_renderImageIrregularSlider.setWidth(m_pMCIrregularSlider->getScaleWidth());
			m_renderImageIrregularSlider.setHeight(m_pMCIrregularSlider->getScaleHeight());
			m_renderImageIrregularSlider.setLayout(CRenderImage::L_NORMAL);

			m_renderGroup.addRenderItem((CRenderItem*)&m_renderImageIrregularSlider);
		}
		else if (m_pMCRegularSlider.isNotNull())
		{
			pt = localToGlobal( m_pMCRegularSlider->getX(), m_pMCRegularSlider->getY() );

			m_renderImageRegularSlider.setFilePath(m_pMCRegularSlider->getCurrentImage());
			m_renderImageRegularSlider.setX(pt.getX());
			m_renderImageRegularSlider.setY(pt.getY());
			m_renderImageRegularSlider.setWidth(m_pMCRegularSlider->getScaleWidth());
			m_renderImageRegularSlider.setHeight(m_pMCRegularSlider->getScaleHeight());
			m_renderImageRegularSlider.setLayout(CRenderImage::L_NORMAL);

			m_renderGroup.addRenderItem((CRenderItem*)&m_renderImageRegularSlider);
		}

		buildRenderTreeFromCCP(m_renderGroup);
	}

	BOOLEAN CCtrlProgressbar::turnFocusToLeft(UINT uiStep)
	{
		if ( !m_bIsAdjustMode )
		{
			return FALSE;
		}

		if ( m_bUpdateLockOn )
		{
			return TRUE;
		}
		
		if (m_model.getPos() == m_model.getMin())
		{
			if (m_bCircleAdjust)
			{
				m_model.setPos( m_bIsPosAlterWhileAdjustCCP ? m_model.getMax() : m_model.getPos());
				m_iAdjustStep = m_bIsPosAlterWhileAdjustCCP ? 0 : m_model.getMax() - m_model.getMin();
			}
			else
			{
				return TRUE;
			}
		}
		else
		{
			if ( m_model.getPos() - m_model.getStep() * (INT)uiStep >= m_model.getMin() )
			{
				m_model.setPos( m_bIsPosAlterWhileAdjustCCP ? (m_model.getPos() - m_model.getStep() * uiStep) : m_model.getPos() );
				m_iAdjustStep = m_bIsPosAlterWhileAdjustCCP ? 0 : ( - m_model.getStep() * uiStep );
			}
			else
			{
				m_iAdjustStep = m_bIsPosAlterWhileAdjustCCP ? 0 : ( m_model.getMin() - m_model.getPos() );
				m_model.setPos( m_bIsPosAlterWhileAdjustCCP ? m_model.getMin() : m_model.getPos() );
			}
		}
		
		if ( !m_bIsPosAlterWhileAdjustCCP )
		{
			dispatchEvent(CEventProgressbar::POS_ADJUSTED);
			return TRUE;
		}
		
		compose();
		setDirty();
		
		dispatchEvent(CEventProgressbar::POS_CHANGED);
		
		return TRUE;
	}

	BOOLEAN CCtrlProgressbar::turnFocusToRight(UINT uiStep)
	{
		if ( !m_bIsAdjustMode )
		{
			return FALSE;
		}
		
		if ( m_bUpdateLockOn )
		{
			return TRUE;
		}

		if (m_model.getPos() == m_model.getMax())
		{
			if (m_bCircleAdjust)
			{
				m_model.setPos( m_bIsPosAlterWhileAdjustCCP ? m_model.getMin() : m_model.getPos() );
				m_iAdjustStep = m_bIsPosAlterWhileAdjustCCP ? 0 : m_model.getMin() - m_model.getMax();
			}
			else
			{
				return TRUE;
			}
		}
		else
		{
			if ( m_model.getPos() + m_model.getStep() * (INT)uiStep <= m_model.getMax() )
			{
				m_model.setPos( m_bIsPosAlterWhileAdjustCCP ? (m_model.getPos() + uiStep * m_model.getStep()) : m_model.getPos() );
				m_iAdjustStep = m_bIsPosAlterWhileAdjustCCP ? 0 : uiStep * m_model.getStep();
			}
			else
			{
				m_iAdjustStep = m_bIsPosAlterWhileAdjustCCP ? 0 : m_model.getMax() - m_model.getPos();
				m_model.setPos( m_bIsPosAlterWhileAdjustCCP ? m_model.getMax() : m_model.getPos());
			}
		}
		
		if ( !m_bIsPosAlterWhileAdjustCCP )
		{
			dispatchEvent(CEventProgressbar::POS_ADJUSTED);
			return TRUE;
		}
		
		compose();
		setDirty();
		
		dispatchEvent(CEventProgressbar::POS_CHANGED);
		
		return TRUE;
	}

	void CCtrlProgressbar::onTouchPress(INT iX, INT iY)
	{
		if ( !m_bIsAdjustMode )
		{
			return;
		}
		
		CPoint pt = globalToLocal(iX, iY);

		if (m_pMCIrregularSlider.isNotNull() && m_pMCIrregularSlider->hitTest(pt.getX(), pt.getY())) 
		{
			m_view.drawSkinSlider(m_pMCIrregularSlider, TRUE);

			m_bDragSlider = TRUE;

			setDirty();

			return;
		}
		else if (m_pMCRegularSlider.isNotNull() && m_pMCRegularSlider->hitTest(pt.getX(), pt.getY())) 
		{
			m_view.drawSkinSlider(m_pMCRegularSlider, TRUE);
			
			m_bDragSlider = TRUE;

			setDirty();
			
			return;
		}
		
		if (m_rcBarArea.ptInRect(pt))
		{
			if (m_bIsHorizontalPGB)
			{
				m_model.setPos( m_model.getMin() + (INT)( (m_model.getRange()) * (FLOAT)(pt.getX() - m_rcBarArea.getX()) / m_rcBarArea.getWidth() ) );
			}
			else
			{
				m_model.setPos( m_model.getMin() + (INT)( m_model.getRange() * (FLOAT)(1.0f - (FLOAT)(pt.getY() - m_rcBarArea.getY()) / m_rcBarArea.getHeight() ) ) );
			}
				
			compose();
			setDirty();

			m_bDragSlider = TRUE;
			m_bUpdateLockOn = TRUE;
			
			return;
		}
	}

	void CCtrlProgressbar::onTouchPressRelease(INT iX, INT iY)
	{
		onTouchRelease();
	}

	void CCtrlProgressbar::onTouchLongPressRelease(INT iX, INT iY)
	{
		onTouchRelease();
	}

	void CCtrlProgressbar::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
	{
		if ( !m_bIsAdjustMode )
		{
			return;
		}

		if (m_bDragSlider)
		{
			CPoint pt = globalToLocal(iX, iY);

			if (m_bIsHorizontalPGB)
			{
				m_model.setPos( m_model.getMin() + (INT)( (m_model.getRange()) * (FLOAT)(pt.getX() - m_rcBarArea.getX()) / m_rcBarArea.getWidth() ) );
			}
			else
			{
				m_model.setPos( m_model.getMin() + (INT)( m_model.getRange() * (FLOAT)(1.0f - (FLOAT)(pt.getY() - m_rcBarArea.getY()) / m_rcBarArea.getHeight() ) ) );
			}

			compose();
			setDirty();

			m_bUpdateLockOn = TRUE;

			return ;
		}
	}

	void CCtrlProgressbar::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
	{
		onTouchRelease();
	}

	void CCtrlProgressbar::compose()
	{
		if (m_bIsHorizontalPGB)
		{
			if (m_pMCIrregularBar.isNotNull())
			{
				m_view.drawSkinIrregularBar(m_pMCIrregularBar, m_model.getPosRatio());

				m_pMCIrregularBar->setX(m_rcBarArea.getX());
				m_pMCIrregularBar->setY(m_rcBarArea.getY() + (INT)m_rcBarArea.getHeight() / 2 - (INT)m_pMCIrregularBar->getHeight() / 2);
				m_pMCIrregularBar->setScaleWidth(m_rcBarArea.getWidth());
				m_pMCIrregularBar->setScaleHeight(m_pMCIrregularBar->getHeight());				
			}
			else if (m_pMCRegularBar.isNotNull())
			{
				m_pMCRegularBar->setX(m_rcBarArea.getX());
				m_pMCRegularBar->setY(m_rcBarArea.getY() + (INT)m_rcBarArea.getHeight() / 2 - (INT)m_pMCRegularBar->getHeight() / 2);
				m_pMCRegularBar->setScaleWidth((UINT)(m_rcBarArea.getWidth() * m_model.getPosRatio()));
				m_pMCRegularBar->setScaleHeight(m_pMCRegularBar->getHeight());
			}

			if (m_pMCIrregularSlider.isNotNull())
			{
				m_pMCIrregularSlider->setX((INT)(m_rcBarArea.getX() + m_rcBarArea.getWidth() * m_model.getPosRatio()) - (INT)m_pMCIrregularSlider->getWidth() / 2);
				m_pMCIrregularSlider->setY(m_rcBarArea.getY() + (INT)m_rcBarArea.getHeight() / 2 - (INT)m_pMCIrregularSlider->getHeight() / 2);
				m_pMCIrregularSlider->setScaleWidth(m_pMCIrregularSlider->getWidth());
				m_pMCIrregularSlider->setScaleHeight(m_pMCIrregularSlider->getHeight());
			}
			else if (m_pMCRegularSlider.isNotNull())
			{
				m_pMCRegularSlider->setX((INT)(m_rcBarArea.getX() + m_rcBarArea.getWidth() * m_model.getPosRatio()) - (INT)m_pMCRegularSlider->getWidth() / 2);
				m_pMCRegularSlider->setY(m_rcBarArea.getY() + (INT)m_rcBarArea.getHeight() / 2 - (INT)m_pMCRegularSlider->getHeight() / 2);
				m_pMCRegularSlider->setScaleWidth(m_pMCRegularSlider->getWidth());
				m_pMCRegularSlider->setScaleHeight(m_pMCRegularSlider->getHeight());
			}
		}
		else
		{
			if (m_pMCIrregularBar.isNotNull())
			{
				m_view.drawSkinIrregularBar(m_pMCIrregularBar, m_model.getPosRatio());

				m_pMCIrregularBar->setX(m_rcBarArea.getX() + (INT)m_rcBarArea.getWidth() / 2 - (INT)m_pMCIrregularBar->getWidth() / 2);
				m_pMCIrregularBar->setY(m_rcBarArea.getY());
				m_pMCIrregularBar->setScaleWidth(m_pMCIrregularBar->getWidth());
				m_pMCIrregularBar->setScaleHeight(m_rcBarArea.getHeight());

				m_iClipY = (INT)m_rcBarArea.getHeight() - (INT)m_pMCIrregularBar->getHeight();	
			}
			else if (m_pMCRegularBar.isNotNull())
			{
				INT iCurValue = (INT)(m_rcBarArea.getHeight() *  m_model.getPosRatio());

				m_pMCRegularBar->setX(m_rcBarArea.getX() + (INT)m_rcBarArea.getWidth() / 2 - (INT)m_pMCRegularBar->getWidth() / 2);
				m_pMCRegularBar->setY(m_rcBarArea.getY() + m_rcBarArea.getHeight() - iCurValue);
				m_pMCRegularBar->setScaleWidth(m_pMCRegularBar->getWidth());
				m_pMCRegularBar->setScaleHeight(iCurValue);

				m_iClipY = iCurValue - (INT)m_pMCRegularBar->getHeight();
			}

			if (m_pMCIrregularSlider.isNotNull())
			{
				m_pMCIrregularSlider->setX(m_rcBarArea.getX() + (INT)m_rcBarArea.getWidth() / 2 - (INT)m_pMCIrregularSlider->getWidth() / 2);
				m_pMCIrregularSlider->setY((INT)(m_rcBarArea.getY() + m_rcBarArea.getHeight() * (1.0f - m_model.getPosRatio())) - (INT)m_pMCIrregularSlider->getHeight() / 2);
				m_pMCIrregularSlider->setScaleWidth(m_pMCIrregularSlider->getWidth());
				m_pMCIrregularSlider->setScaleHeight(m_pMCIrregularSlider->getHeight());
			}
			else if (m_pMCRegularSlider.isNotNull())
			{
				m_pMCRegularSlider->setX(m_rcBarArea.getX() + (INT)m_rcBarArea.getWidth() / 2 - (INT)m_pMCRegularSlider->getWidth() / 2);
				m_pMCRegularSlider->setY((INT)(m_rcBarArea.getY() + m_rcBarArea.getHeight() * (1.0f - m_model.getPosRatio())) - (INT)m_pMCRegularSlider->getHeight() / 2);
				m_pMCRegularSlider->setScaleWidth(m_pMCRegularSlider->getWidth());
				m_pMCRegularSlider->setScaleHeight(m_pMCRegularSlider->getHeight());
			}
		}
	}

	void CCtrlProgressbar::onTouchRelease()
	{
		ENV::switchCCPFocus(this);

		if ( !m_bIsAdjustMode )
		{
			return;
		}

		m_bDragSlider = FALSE;
		m_bUpdateLockOn = FALSE;

		m_view.drawSkinSlider(m_pMCRegularSlider, FALSE);
		m_view.drawSkinSlider(m_pMCIrregularSlider, FALSE);

		if ( m_model.getPos() != m_iPosRecordNew )
		{
			m_iPosRecordNew = m_model.getPos();

			dispatchEvent(CEventProgressbar::POS_CHANGED);
		}

		setDirty();
	}
}