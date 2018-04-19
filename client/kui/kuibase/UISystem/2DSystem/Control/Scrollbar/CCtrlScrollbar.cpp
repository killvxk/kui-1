#include "OpenHMI.h"

namespace OpenHMI
{
	static const UINT MIN_SLIDER_HEIGHT = 20;

	CCtrlScrollbar::CCtrlScrollbar()
		: m_ePosChangedType(PC_UNKNOW)
		, m_bCanDragSlider(TRUE)
		, m_iFocusSelBeginPosY(0)
		, m_iFocusSelModelBeginPos(0)
		, m_bDragSlider(FALSE)
		, m_bScaleSlider(FALSE)
		, m_bLongPressHitUpArrow(FALSE)
		, m_bLongPressHitDownArrow(FALSE)
		, m_bMoveByPage(TRUE)
		, m_bUpArrowFocus(FALSE)
		, m_bDownArrowFocus(FALSE)
	{
		m_model.setComponent(this);
		setBaseView(&m_view);
		notAcceptFocus();
	}

	CCtrlScrollbar::~CCtrlScrollbar()
	{
		;
	}

	void CCtrlScrollbar::setSkinUpArrow(UINT uiImageID)
	{
		m_view.setSkinUpArrow(uiImageID);
	}

	void CCtrlScrollbar::setSkinDownArrow(UINT uiImageID)
	{
		m_view.setSkinDownArrow(uiImageID);
	}

	void CCtrlScrollbar::setSkinBar(UINT uiImageID)
	{
		m_view.setSkinBar(uiImageID);
	}

	void CCtrlScrollbar::setSkinSlider(UINT uiImageID)
	{
		m_view.setSkinSlider(uiImageID);
	}

	void CCtrlScrollbar::setScaleSlider(BOOLEAN bValue)
	{
		m_bScaleSlider = bValue;
	}

	BOOLEAN CCtrlScrollbar::isAdjustMode()
	{
		return getEnable();
	}

	void CCtrlScrollbar::setIsAdjustMode(BOOLEAN bValue)	
	{
		setEnable(bValue);
	}

	void CCtrlScrollbar::moveToPreviousStep()
	{
		m_model.moveStepPrevious();
		compose();

		setPosChangedType(CCtrlScrollbar::PC_LINE_UP);
		dispatchEvent(CEventScrollbar::POS_CHANGED);

		setDirty();
	}

	void CCtrlScrollbar::moveToNextStep()
	{
		m_model.moveStepNext();
		compose();

		setPosChangedType(CCtrlScrollbar::PC_LINE_DOWN);
		dispatchEvent(CEventScrollbar::POS_CHANGED);

		setDirty();
	}

	void CCtrlScrollbar::moveToPreviousPage()
	{
		m_model.movePagePrevious();
		compose();

		setPosChangedType(CCtrlScrollbar::PC_LINE_UP);
		dispatchEvent(CEventScrollbar::POS_CHANGED);

		setDirty();
	}

	void CCtrlScrollbar::moveToNextPage()
	{
		m_model.movePageNext();
		compose();

		setPosChangedType(CCtrlScrollbar::PC_LINE_DOWN);
		dispatchEvent(CEventScrollbar::POS_CHANGED);

		setDirty();
	}

	void CCtrlScrollbar::refresh()
	{
		CComponent::refresh();

		if (m_pMCUpArrow.isNull())
		{
			m_pMCUpArrow = m_view.getSkinUpArrow();
		}

		if (m_pMCDownArrow.isNull())
		{
			m_pMCDownArrow = m_view.getSkinDownArrow();
		}

		if (m_pMCBar.isNull())
		{
			m_pMCBar = m_view.getSkinBar();
		}

		if (m_pMCSlider.isNull())
		{
			m_pMCSlider = m_view.getSkinSlider();
		}

		compose();
		
		m_view.drawSkinUpArrow(m_pMCUpArrow, m_bUpArrowFocus);
		m_view.drawSkinDownArrow(m_pMCDownArrow, m_bDownArrowFocus);
		m_view.drawSkinBar(m_pMCBar, FALSE);
		m_view.drawSkinSlider(m_pMCSlider, FALSE);
	}

	void CCtrlScrollbar::littleRefresh()
	{
		refresh();
	}

	void CCtrlScrollbar::destroySkin()
	{
		m_pMCUpArrow.setNull();
		m_pMCDownArrow.setNull();
		m_pMCBar.setNull();
		m_pMCSlider.setNull();

		CComponent::destroySkin();
	}

	void CCtrlScrollbar::destroy()
	{
		if (isDestroyed())
		{
			return;
		}

		CComponent::destroy();
	}

	void CCtrlScrollbar::buildRenderTree(CRenderGroup &renderGroup)
	{
		if (getVisible()) 
		{
			CPoint pt = localToGlobal(0, 0);
			m_renderGroup.setX(pt.getX());
			m_renderGroup.setY(pt.getY());
			m_renderGroup.setWidth(getPreferredSize().getX());
			m_renderGroup.setHeight(getPreferredSize().getY());
			renderGroup.addRenderItem((CRenderItem*)&m_renderGroup);

			//buildRenderTreeFromBackground(m_renderGroup);

			if (m_pMCUpArrow.isNotNull() && m_pMCDownArrow.isNotNull())
			{
				// add up arrow
				pt = localToGlobal( m_pMCUpArrow->getX(), m_pMCUpArrow->getY() );

				m_renderImgUpArrow.setFilePath(m_pMCUpArrow->getCurrentImage());				
				m_renderImgUpArrow.setX(pt.getX());
				m_renderImgUpArrow.setY(pt.getY());
				m_renderImgUpArrow.setWidth(m_pMCUpArrow->getWidth());
				m_renderImgUpArrow.setHeight(m_pMCUpArrow->getHeight());
				m_renderImgUpArrow.setLayout(CRenderImage::L_NORMAL);

				m_renderGroup.addRenderItem((CRenderItem*)&m_renderImgUpArrow);

				// add down arrow
				pt = localToGlobal( m_pMCDownArrow->getX(), m_pMCDownArrow->getY() );

				m_renderImgDownArrow.setFilePath(m_pMCDownArrow->getCurrentImage());			
				m_renderImgDownArrow.setX(pt.getX());
				m_renderImgDownArrow.setY(pt.getY());
				m_renderImgDownArrow.setWidth(m_pMCDownArrow->getWidth());
				m_renderImgDownArrow.setHeight(m_pMCDownArrow->getHeight());
				m_renderImgDownArrow.setLayout(CRenderImage::L_NORMAL);

				m_renderGroup.addRenderItem((CRenderItem*)&m_renderImgDownArrow);
			}
			
			if (m_pMCBar.isNotNull() && m_pMCSlider.isNotNull())
			{
				// add Bar
				pt = localToGlobal( m_pMCBar->getX(), m_pMCBar->getY() );

				m_renderImgBar.setFilePath(m_pMCBar->getCurrentImage());				
				m_renderImgBar.setX(pt.getX());
				m_renderImgBar.setY(pt.getY());
				m_renderImgBar.setWidth(m_pMCBar->getWidth());
				m_renderImgBar.setHeight(m_pMCBar->getScaleHeight());
				m_renderImgBar.setLayout(CRenderImage::L_STRETCH_FILL);

				m_renderGroup.addRenderItem((CRenderItem*)&m_renderImgBar);

				// add Slider
				pt = localToGlobal( m_pMCSlider->getX(), m_pMCSlider->getY() );

				m_renderImgSlider.setFilePath(m_pMCSlider->getCurrentImage());				
				m_renderImgSlider.setX(pt.getX());
				m_renderImgSlider.setY(pt.getY());
				m_renderImgSlider.setWidth(m_pMCSlider->getWidth());
				if (m_bScaleSlider) 
				{
					m_renderImgSlider.setHeight(m_pMCSlider->getScaleHeight());
				}
				else
				{
					m_renderImgSlider.setHeight(m_pMCSlider->getHeight());
				}
				m_renderImgSlider.setLayout(CRenderImage::L_STRETCH_FILL);

				m_renderGroup.addRenderItem((CRenderItem*)&m_renderImgSlider);
			}			
			
			//buildRenderTreeFromCCP(m_renderGroup);
		}

		return;

	}

	void CCtrlScrollbar::onMove(INT iX, INT iY)
	{	
		CPoint pt = globalToLocal(iX, iY);

		if ( m_pMCSlider.isNotNull() && !m_pMCSlider->isEmpty())
		{
			BOOLEAN bInInner = m_pMCSlider->hitTest(pt.getX(), pt.getY());
			
			if (bInInner)
			{
				if (getHoverFocus() == FALSE)
				{
					setHoverFocus(TRUE);
					m_view.drawSkinSlider(m_pMCSlider, TRUE);
				}
			}
			else
			{

				if (getHoverFocus() == TRUE)
				{
					setHoverFocus(FALSE);
					m_view.drawSkinSlider(m_pMCSlider, FALSE);
				}
			}

			return;
		}
	}

	void CCtrlScrollbar::onTouchPress(INT iX, INT iY)
	{
		CPoint pt = globalToLocal(iX, iY);

		if (m_pMCSlider.isNotNull() && !m_pMCSlider->isEmpty() && m_pMCSlider->hitTest(pt.getX(), pt.getY()))
		{
			if (m_bCanDragSlider)
			{
				m_view.drawSkinSlider(m_pMCSlider, TRUE);

				m_bDragSlider = TRUE;
				m_iFocusSelBeginPosY = pt.getY();
				m_iFocusSelModelBeginPos = m_model.getPosition();

				setDirty();
			}

			return;
		}

		if ( m_pMCUpArrow.isNotNull() && !m_pMCUpArrow->isEmpty() && m_pMCUpArrow->hitTest(pt.getX(), pt.getY()) ) 
		{
			m_bUpArrowFocus = TRUE;
			m_view.drawSkinUpArrow(m_pMCUpArrow, TRUE);

			if (m_bMoveByPage)
			{
				m_model.movePagePrevious();
			}
			else
			{
				m_model.moveStepPrevious();
			}
			compose();

			setPosChangedType(CCtrlScrollbar::PC_LINE_UP);
			dispatchEvent(CEventScrollbar::POS_CHANGED);

			setDirty();

			return;
		}

		if ( m_pMCDownArrow.isNotNull() && !m_pMCDownArrow->isEmpty() && m_pMCDownArrow->hitTest(pt.getX(), pt.getY()) ) 
		{
			m_bDownArrowFocus = TRUE;
			m_view.drawSkinDownArrow(m_pMCDownArrow, TRUE);

			if (m_bMoveByPage)
			{
				m_model.movePageNext();
			}
			else
			{
				m_model.moveStepNext();
			}
			compose();

			setPosChangedType(CCtrlScrollbar::PC_LINE_DOWN);
			dispatchEvent(CEventScrollbar::POS_CHANGED);

			setDirty();

			return;
		}

		if ( m_pMCSlider.isNotNull() && !m_pMCSlider->isEmpty() && m_pMCSlider->hitTest(pt.getX(), pt.getY()) )
		{
			if (m_bCanDragSlider)
			{
				m_view.drawSkinSlider(m_pMCSlider, TRUE);

				m_bDragSlider = TRUE;
				m_iFocusSelBeginPosY = pt.getY();
				m_iFocusSelModelBeginPos = m_model.getPosition();

				setDirty();
			}

			return;
		}

		if (m_pMCBar.isNotNull() && !m_pMCBar->isEmpty() && m_pMCBar->hitTest(pt.getX(), pt.getY()))
		{
			//m_view.drawSkinBar(m_pMCBar, TRUE);

			// need test
			CPoint pt = globalToLocal(iX, iY);
			if (pt.getY() < m_pMCSlider->getY()) 
			{
				m_model.moveStepPrevious();
				compose();
				
				setPosChangedType(CCtrlScrollbar::PC_PAGE_UP);
				dispatchEvent(CEventScrollbar::POS_CHANGED);
			}
			else 
			{
				m_model.moveStepNext();
				compose();
				
				setPosChangedType(CCtrlScrollbar::PC_PAGE_DOWN);
				dispatchEvent(CEventScrollbar::POS_CHANGED);
			}

			setDirty();

			return;
		}
		
	}

	void CCtrlScrollbar::onTouchPressRelease(INT iX, INT iY)
	{
		onTouchRelease();
	}

	void CCtrlScrollbar::onTouchLongPress(INT iX, INT iY)
	{
		CPoint pt;
		pt = globalToLocal(iX, iY);

		if ( m_pMCUpArrow.isNotNull() && !m_pMCUpArrow->isEmpty() && m_pMCUpArrow->hitTest(pt.getX(), pt.getY()) )
		{
			m_bLongPressHitUpArrow = TRUE;
		}

		if ( m_pMCUpArrow.isNotNull() && !m_pMCDownArrow->isEmpty() && m_pMCDownArrow->hitTest(pt.getX(), pt.getY()) ) 
		{
			m_bLongPressHitDownArrow = TRUE;
		}
	}

	void CCtrlScrollbar::onTouchContinuePress(INT iX, INT iY)
	{
		if (m_bLongPressHitUpArrow) 
		{
			if (m_bMoveByPage)
			{
				m_model.movePagePrevious();
			}
			else
			{
				m_model.moveStepPrevious();
			}
			compose();
			
			setPosChangedType(CCtrlScrollbar::PC_CONTINUE_LINE_UP);
			dispatchEvent(CEventScrollbar::POS_CHANGED);

			m_view.drawSkinUpArrow(m_pMCUpArrow, TRUE);

			setDirty();

			return;
		}

		if (m_bLongPressHitDownArrow) 
		{
			if (m_bMoveByPage)
			{
				m_model.movePageNext();
			}
			else
			{
				m_model.moveStepNext();
			}
			compose();
			
			setPosChangedType(CCtrlScrollbar::PC_CONTINUE_LINE_DOWN);
			dispatchEvent(CEventScrollbar::POS_CHANGED);

			m_view.drawSkinUpArrow(m_pMCDownArrow, TRUE);

			setDirty();

			return;
		}
	}

	void CCtrlScrollbar::onTouchLongPressRelease(INT iX, INT iY)
	{
		if (m_bLongPressHitUpArrow)
		{
			setPosChangedType(CCtrlScrollbar::PC_CONTINUE_LINE_UP);
			dispatchEvent(CEventScrollbar::POS_CONTINUE_CHANGE_ENDED);
		}

		if (m_bLongPressHitDownArrow)
		{
			setPosChangedType(CCtrlScrollbar::PC_CONTINUE_LINE_DOWN);
			dispatchEvent(CEventScrollbar::POS_CONTINUE_CHANGE_ENDED);
		}

		onTouchRelease();
	}

	void CCtrlScrollbar::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
	{
		if (m_bDragSlider)
		{
			CTimerManager::getInstance()->stop();
			CPoint pt;
			pt = globalToLocal(iX, iY);
			static INT py = pt.getY();

			if (Math::abs(py - pt.getY()) > 5)
			{
				INT yStep = pt.getY() - m_iFocusSelBeginPosY;
				
				if (m_bScaleSlider)
				{
					m_model.setPosition((INT)((FLOAT)yStep / (FLOAT)(m_pMCBar->getScaleHeight() - MIN_SLIDER_HEIGHT) * (FLOAT)(m_model.getRange()) + m_iFocusSelModelBeginPos));
				}
				else
				{
					INT iHeight = m_pMCBar->getHeight() - m_pMCSlider->getHeight();
					m_model.setPosition((INT)((FLOAT)(yStep) / (FLOAT)(iHeight)* (FLOAT)(m_model.getRange() - m_model.getPage()) + m_iFocusSelModelBeginPos));
				}

				compose();

				setPosChangedType(CCtrlScrollbar::PC_SLIDER);
				dispatchEvent(CEventScrollbar::POS_CHANGED);
				py = pt.getY();
			}
			
			//setDirty();
		}
	}

	void CCtrlScrollbar::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
	{
		if (m_bDragSlider)
		{
			setPosChangedType(CCtrlScrollbar::PC_SLIDER);
			dispatchEvent(CEventScrollbar::POS_CONTINUE_CHANGE_ENDED);
		}
		CTimerManager::getInstance()->start();
		onTouchRelease();
	}

	// private
	void CCtrlScrollbar::compose()
	{
		if (m_pMCSlider.isNull() || m_pMCSlider->getHeight() == 0 || m_pMCBar.isNull())
		{
			return;
		}

		if ( m_pMCUpArrow.isNotNull() && m_pMCDownArrow.isNotNull() )
		{
			m_pMCUpArrow->setX( (INT)getPreferredSize().getX() / 2 - (INT)m_pMCUpArrow->getWidth() / 2 );
			m_pMCUpArrow->setY(0);

			m_pMCDownArrow->setX( (INT)getPreferredSize().getX() / 2 - (INT)m_pMCDownArrow->getWidth() / 2 );
			m_pMCDownArrow->setY(getPreferredSize().getY() - m_pMCDownArrow->getHeight());

			m_pMCBar->setX( (INT)getPreferredSize().getX() / 2 - (INT)m_pMCBar->getWidth() / 2 );
			m_pMCBar->setY( m_pMCUpArrow->getHeight() );					
			m_pMCBar->setScaleHeight(getPreferredSize().getY() - m_pMCUpArrow->getHeight() - m_pMCDownArrow->getHeight());					
		}				
		else
		{
			m_pMCBar->setX( (INT)getPreferredSize().getX() / 2 - (INT)m_pMCBar->getWidth() / 2 );
			m_pMCBar->setY(0);
			m_pMCBar->setScaleHeight(getPreferredSize().getY());
		}

		m_pMCSlider->setX( (INT)getPreferredSize().getX() / 2 - (INT)m_pMCSlider->getWidth() / 2 );
		if (m_bScaleSlider)
		{
			m_pMCSlider->setY( m_pMCBar->getY() + (INT) (m_model.getPosRatio() * (m_pMCBar->getScaleHeight() - MIN_SLIDER_HEIGHT)) );
			m_pMCSlider->setScaleHeight(  (INT) ( (FLOAT)(m_model.getPage()) / (FLOAT)(m_model.getRange()) 
										* (FLOAT)(m_pMCBar->getScaleHeight() - MIN_SLIDER_HEIGHT) ) + MIN_SLIDER_HEIGHT );
		}
		else
		{
			m_pMCSlider->setY(	  m_pMCBar->getY()
								+ (INT)( (FLOAT)(m_model.getPosition() - m_model.getMinimum()) / (FLOAT)(m_model.getRange() - m_model.getPage()) 
								* (m_pMCBar->getScaleHeight() - m_pMCSlider->getHeight()) ) );
		}
// 		if (m_bScaleSlider)
// 		{				
// 			INT iHeight = (INT)( (FLOAT)m_model.getPage() / (FLOAT)m_model.getRange() * m_pMCBar->getScaleHeight() );
// 			if (iHeight < MIN_SLIDER_HEIGHT)
// 			{
// 				iHeight = MIN_SLIDER_HEIGHT;
// 			}
// 			m_pMCSlider->setScaleHeight(iHeight);
// 
// 			m_pMCSlider->setY( m_pMCBar->getY() + (INT) (m_model.getPosRatio() * m_pMCBar->getScaleHeight()) );
// 		}
// 		else
// 		{
// 			m_pMCSlider->setY(	  m_pMCBar->getY()
// 				+ (INT)( (FLOAT)(m_model.getPosition() - m_model.getMinimum()) / (FLOAT)(m_model.getRange() - m_model.getPage()) 
// 				* (m_pMCBar->getScaleHeight() - m_pMCSlider->getHeight()) ) );
// 		}	
	}

	void CCtrlScrollbar::setPosChangedType(CCtrlScrollbar::EPosChanged ePosChanged)
	{
		m_ePosChangedType = ePosChanged;
	}

	void CCtrlScrollbar::onTouchRelease()
	{
		ENV::switchCCPFocus(this);

		m_bDragSlider = FALSE;
		m_bLongPressHitUpArrow = FALSE;
		m_bLongPressHitDownArrow = FALSE;

		m_bUpArrowFocus = FALSE;
		m_bDownArrowFocus = FALSE;

		m_view.drawSkinUpArrow(m_pMCUpArrow, FALSE);
		m_view.drawSkinDownArrow(m_pMCDownArrow, FALSE);			
		m_view.drawSkinBar(m_pMCSlider, FALSE);
		m_view.drawSkinSlider(m_pMCBar, FALSE);

		setDirty();
	}
}