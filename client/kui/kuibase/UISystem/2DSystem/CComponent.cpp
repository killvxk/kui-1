#include "OpenHMI.h"

namespace OpenHMI
{
	CComponent::CComponent()
		: m_pParent(NULL)
		, m_pBaseView(NULL)
		, m_bIsContainer(FALSE)
		, m_uiID(0)		
		, m_bHasInitFromResource(FALSE)
		, m_bLock(FALSE)
		, m_bDestroyed(FALSE)
		, m_iX (0)
		, m_iY(0)
		, m_fRotateAngle(0.0f)
		, m_eRotateAxis(CRenderGroup::RA_Z)
		, m_iAnchorX(0)
		, m_iAnchorY(0)
		, m_fScaleWidth(1.0f)
		, m_fScaleHeight(1.0f)
		, m_uiOpacity(100)
		, m_bVisible(TRUE)		
		, m_bEnable(TRUE)
		, m_bHoverFocus(FALSE)
		, m_bTouchFocus(FALSE)
		, m_bSelected(FALSE)
		, m_bSoundSwitch(TRUE)
		, m_iSoundType(0)
		, m_bCanSetFocus(TRUE)
		, m_bCanMoveFocusOut(TRUE)
		, m_bCanMoveFocus(TRUE)
		, m_bCCPFocus(FALSE)
		, m_bCCPBelowComponent(FALSE)
		, m_pTargetComponent(NULL)
		, m_pSuperiorComponent(NULL)
		, m_pJuniorComponent(NULL)
		, m_bCanHitTest(TRUE)
		, m_bCanDisabledTouch(FALSE)
		, m_bSupportMultiComponentTouched(FALSE)
		, m_bCanDrag(FALSE)		
		, m_bIsParentMoving(FALSE)
		, m_uiTouch1ID(0)
		, m_eTouch1Status(TS_INIT)
		, m_uiTouch1Time(0)			
		, m_uiActiveCursor(0)
		, m_bCanCatchGesture(FALSE)
		, m_uiGestureGroupID(0)
	{
		m_uiID = reinterpret_cast<UINT>(this);
	}

	CComponent::~CComponent()
	{
		;
	}

	BOOLEAN	CComponent::contains(CComponent *pComponent)
	{
		return FALSE;
	}

	void CComponent::lock()
	{
		if (!m_bLock)
		{
			m_bLock = TRUE;
			setDirty();
		}
	}

	void CComponent::unlock()
	{
		if (m_bLock)
		{
			m_bLock = FALSE;
			setDirty();
		}
	}

	void CComponent::setX(INT iValue)
	{
		if (m_iX != iValue)
		{
			m_iX = iValue; 
			setDirty();
		}
	}

	void CComponent::setY(INT iValue)
	{
		if (m_iY != iValue)
		{
			m_iY = iValue; 
			setDirty();
		}
	}

	CPoint CComponent::globalToLocal(INT iX, INT iY) const
	{
		CPoint pt = localToGlobal(0, 0);

		return CPoint(iX - pt.getX(), iY - pt.getY());
	}

	CPoint CComponent::globalToLocal(const CPoint &pt) const
	{
		return globalToLocal(pt.getX(), pt.getY());
	}

	CPoint CComponent::localToGlobal(INT iX, INT iY) const
	{
		if (m_pParent == NULL)
		{
			return CPoint(m_iX + iX, m_iY + iY);
		}
		else
		{
			CPoint pt = m_pParent->localToGlobal(m_iX, m_iY);

			pt.offset(iX, iY);

			return pt;
		}
	}

	CPoint CComponent::localToGlobal(const CPoint &pt) const
	{
		return localToGlobal(pt.getX(), pt.getY());
	}

	void CComponent::setPreferredSize(const CSize &szValue)
	{ 
		if (m_szPreferredSize != szValue)
		{
			m_szPreferredSize = szValue;
			setDirty();
		}
	}

	void CComponent::setPreferredSize(UINT uiWidth, UINT uiHeight)
	{
		m_szPreferredSize.setX(uiWidth);
		m_szPreferredSize.setY(uiHeight);
		setDirty();
	}
	
	void CComponent::setRotateAngle(FLOAT fValue, INT iAnchorX, INT iAnchorY, CRenderGroup::ERotateAxis eRA)
	{
		if ( m_eRotateAxis != eRA || !Math::isZero(fValue - m_fRotateAngle) || iAnchorX != m_iAnchorX || m_iAnchorY != iAnchorY)
		{
			m_fRotateAngle = fValue;
			m_iAnchorX = iAnchorX;
			m_iAnchorY = iAnchorY;
			m_eRotateAxis = eRA;

			m_renderGroup.setRotateRadian(Math::toRadian(fValue), m_iAnchorX, m_iAnchorY, m_eRotateAxis);

			setDirty();
		}
	}

	void CComponent::setScaleWidth(FLOAT fValue)
	{
		if ( !Math::isZero(fValue - m_fScaleWidth) && fValue >= 0.0f )
		{
			m_fScaleWidth = fValue;

			m_renderGroup.setScaleWidth(m_fScaleWidth);

			setDirty();
		}
	}

	void CComponent::setScaleHeight(FLOAT fValue)
	{
		if ( !Math::isZero(fValue - m_fScaleHeight) && fValue >= 0.0f )
		{
			m_fScaleHeight = fValue;

			m_renderGroup.setScaleHeight(m_fScaleHeight);

			setDirty();
		}
	}

	void CComponent::setSkin(UINT uiImageID)
	{
		if (m_pBaseView != NULL)
		{
			m_pBaseView->setSkin(uiImageID);
		}
	}

	void CComponent::setOpacity(UINT uiValue)
	{
		if( uiValue != m_uiOpacity )
		{
			m_uiOpacity = uiValue > 100 ? 100 : uiValue;

			m_renderGroup.setAlpha((UINT)((FLOAT)m_uiOpacity / 100.0F * 255));

			setDirty();
		}
	}

	void CComponent::setVisible(BOOLEAN bValue)	
	{
		if (m_bVisible != bValue)
		{
			m_bVisible = bValue;
			ENV::setDirty();
		}
	}	

	void CComponent::setEnable(BOOLEAN bValue)
	{
		if (m_bEnable != bValue)
		{
			m_bEnable = bValue;
			setDirty();
		}
	}

	void CComponent::setHoverFocus(BOOLEAN bValue)
	{
		if (m_bHoverFocus != bValue)
		{
			m_bHoverFocus = bValue;
			setDirty();
		}
	}

	void CComponent::setTouchFocus(BOOLEAN bValue)
	{
		if (m_bTouchFocus != bValue)
		{
			m_bTouchFocus = bValue;
			setDirty();
		}
	}

	void CComponent::setSelected(BOOLEAN bValue)
	{
		if (m_bSelected != bValue)
		{
			m_bSelected = bValue;
			setDirty();
		}
	}

	void CComponent::refresh()
	{
		if (m_skin.isNull() && m_pBaseView != NULL)
		{
			m_skin = m_pBaseView->getSkin();

// 			if (m_skin.isNotNull())
// 			{
// 				if (m_szPreferredSize.getX() == 0 && m_szPreferredSize.getY() == 0)
// 				{
// 					m_szPreferredSize.setX(m_skin->getWidth());
// 					m_szPreferredSize.setY(m_skin->getHeight());
// 				}
// 			}
		}

		refreshCCPSkin();

		drawCCPFocs();

		setDirty();
	}

	void CComponent::littleRefresh()
	{
		drawCCPFocs();
	}

	void CComponent::destroySkin()
	{
		m_skin.setNull();

		if (m_szPreferredSize.getX() != (INT)ENV::getScreenWidth() && m_szPreferredSize.getY() != (INT)ENV::getScreenHeight())
		{
			m_szPreferredSize.setX(0);
			m_szPreferredSize.setY(0);
		}

		destroyCCPSkin();

		m_aryHitArea.clear();
		m_aryFilter.clear();
		m_renderGroup.clearAllFilter();

		m_bHasInitFromResource = FALSE;
	}

	void CComponent::destroy()
	{
		if (m_bDestroyed)
		{
			return;
		}

		CScreenTouchManager::getInstance()->removeCursorTracker(this);
		ENV::deleteGestureCatcher(this);

		destroySkin();

		m_pParent = NULL;
		m_pBaseView = NULL;
		m_pTargetComponent = NULL;
		m_pSuperiorComponent = NULL;
		m_pJuniorComponent = NULL;		

		m_bDestroyed = TRUE;
	}

	void CComponent::setDirty()
	{
		if( !getVisible())
		{
			return;
		}

		CComponent* pParent = getParent();
		while( pParent != NULL )
		{
			if( !pParent->getVisible() )
			{
				return;
			}
			
			pParent = pParent->getParent();
		}

		ENV::setDirty();
	}

	void CComponent::buildRenderTree(CRenderGroup &renderGroup)
	{
		CPoint pt = localToGlobal(0, 0);
		m_renderGroup.setX(pt.getX());
		m_renderGroup.setY(pt.getY());
		m_renderGroup.setWidth(getPreferredSize().getX());
		m_renderGroup.setHeight(getPreferredSize().getY());
		renderGroup.addRenderItem((CRenderItem*)&m_renderGroup);

		buildRenderTreeFromBackground(m_renderGroup);

		buildRenderTreeFromCCP(m_renderGroup);
	}

	void CComponent::addFilter(CRenderBaseFilter *pFilter)
	{
		m_aryFilter.addItem(pFilter);

		m_renderGroup.addFilter(pFilter);

		pFilter->setAssociateComponent(this);

		setDirty();
	}

	void CComponent::updateFilter()
	{
		m_renderGroup.updateFilter();

		setDirty();
	}

	void CComponent::playFilter()
	{
		for (UINT i = 0; i < m_aryFilter.size(); i++)
		{
			m_aryFilter[i]->play();
		}
	}

	void CComponent::stopFilter()
	{
		for (UINT i = 0; i < m_aryFilter.size(); i++)
		{
			m_aryFilter[i]->stop();
		}
	}

	void CComponent::setSoundSwitch(BOOLEAN bValue)	
	{
		if (m_bSoundSwitch != bValue)
		{
			m_bSoundSwitch = bValue;
		}
	}

	void CComponent::setSoundType(INT iValue)	
	{
		if (m_iSoundType != iValue)
		{
			m_iSoundType = iValue;
		}
	}	

	BOOLEAN CComponent::canSetFocus()
	{
		if (   m_bCanSetFocus == FALSE
			|| m_bVisible == FALSE 
			|| m_bEnable == FALSE 
			|| isLock()   )
			//|| m_szPreferredSize.getX() == 0
			//|| m_szPreferredSize.getY() == 0   )
		{
			return FALSE;
		}
		else 
		{
			return TRUE;
		}
	}

	void CComponent::setUniqueFocus()
	{
		ENV::switchCCPFocusTo(this);		
	}

	void CComponent::setFocus(ECCPOperation eCCPOperation)
	{
		if (!m_bCCPFocus)
		{
			m_bCCPFocus = TRUE;
			littleRefresh();
			setDirty();
		}
	}

	void CComponent::releaseFocus()
	{
		if (m_bCCPFocus)
		{
			m_bCCPFocus = FALSE;
			littleRefresh();
			setDirty();
		}
	}

	void CComponent::doFocusEnterIn()
	{
		if (m_pJuniorComponent != NULL && m_pJuniorComponent->canSetFocus())
		{
			releaseFocus();

			m_pJuniorComponent->setFocus(CCP_ROTATE_RIGHT);
		}
	}

	BOOLEAN	CComponent::doFocusBack()
	{
		if (m_bCCPFocus)
		{
			//m_bCCPFocus = FALSE;
			//littleRefresh();
			releaseFocus();
		}

		if (m_pSuperiorComponent != NULL && m_pSuperiorComponent->canSetFocus())
		{
			// has move CCP focus to its superior component, so should return true.
			m_pSuperiorComponent->setFocus(CCP_ROTATE_RIGHT);
			return TRUE;
		}

		// the parent container should continue do back CCP focus operation.
		return FALSE;
	}

	void CComponent::addTouchCursor(const CCursor& cursor)
	{
		m_aryCursor.addItem(cursor);
	}

	void CComponent::removeTouchCursor(const CCursor& cursor)
	{
		for ( UINT ui = 0; ui < m_aryCursor.size(); ui++ )
		{
			if ( m_aryCursor[ui].getID() == cursor.getID() )
			{
				m_aryCursor.deleteItemByIndex(ui);
				break;
			}
		}		
	}

	BOOLEAN CComponent::hitTest(INT iX, INT iY)
	{
		if (!m_bVisible || !m_bCanHitTest || m_szPreferredSize.getX() == 0 || m_szPreferredSize.getY() == 0)
		{
			return FALSE;
		}

		CPoint ptLeftTop = localToGlobal(0, 0);

		//var rect:Rectangle = this.scrollRect;
		//if (rect != null)
		//{
		//	leftTop.x += rect.x;
		//	leftTop.y += rect.y;
		//}

		if ( m_aryHitArea.size() == 0 )
		{
			if (   ptLeftTop.getX() <= iX && iX <= ptLeftTop.getX() + m_szPreferredSize.getX() - 1
				&& ptLeftTop.getY() <= iY && iY <= ptLeftTop.getY() + m_szPreferredSize.getY() - 1 )
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			m_aryHitArea.resetIterator();
			while(m_aryHitArea.hasMore())
			{
				CBaseHitArea* pHitArea = m_aryHitArea.next();

				if( pHitArea->hitTest(iX - ptLeftTop.getX(), iY - ptLeftTop.getY()) )
				{
					return TRUE;
				}
			}
		}

		return FALSE;
	}	

	void CComponent::move(INT iX, INT iY)
	{
		onTouchMove(iX, iY, 0, 0);
	}

	void CComponent::hoverCheck(INT iX, INT iY)
	{
		if(m_bLock == FALSE)
		{
			onMove(iX, iY);
		}
	}

	void CComponent::lostHover()
	{
		onLostHover();
	}

	ICursorTracker* CComponent::probe(INT iX, INT iY)
	{
		return hitTest(iX, iY) ? this : NULL;
	}

	void CComponent::wheel(CCursor::EWheelType eType)
	{
		if(m_bLock == FALSE)
		{
			onWheel(eType);
		}
	}

	void CComponent::addCursor(const CCursor &cursor)
	{
		CComponent *pComponent = getGestureCatcher();
		if ( pComponent != NULL )
		{
			UINT uiID = 0;
			if ( pComponent->getCursorCnt() > 0 )
			{
				uiID = ENV::getGestureGroupID( pComponent, FALSE);				
			}
			else
			{
				uiID = ENV::getGestureGroupID( pComponent, TRUE );
				if( pComponent != this )
				{
					pComponent->addTouchCursor(cursor);
				}
			}

			pComponent->setGestureGroupID(uiID);
			ENV::getGestureRecognizer()->addCursor(uiID, cursor);
		}

		if (!m_bEnable && !m_bCanDisabledTouch)
		{
			return;
		}

		if (m_bLock)
		{
			return;
		}			

		BOOLEAN bIsInDoubleTapRange = !checkMove(cursor, m_ptTouch1LastPoint, ENV::getDoubleTapWaverDistance());
		m_ptTouch1LastPoint.setX(cursor.getX());
		m_ptTouch1LastPoint.setY(cursor.getY());

		m_aryCursor.addItem(cursor);
		m_uiActiveCursor = cursor.getID();

		if( !m_bEnable && m_bCanDisabledTouch )
		{
			m_eTouch1Status = TS_DISABLED_PRESS;
			return ;
		}

		playFilter();

		if ( m_aryCursor.size() == 1 )
		{
			if ( m_uiTouch1Time > 0 && (CTime::getTickCount() - m_uiTouch1Time) < ENV::getDoubleTapInterval() && bIsInDoubleTapRange)
			{
				m_eTouch1Status = TS_DOUBLE_PRESS;
				dispatchEvent(CEventGesture::DOUBLE_PRESS);
				onTouchDoublePress(cursor.getX(), cursor.getY());	
			}
			else
			{
				m_eTouch1Status = TS_PRESS;
				m_uiTouch1Time = CTime::getTickCount();
				dispatchEvent(CEventGesture::PRESS);
				onTouchPress(cursor.getX(), cursor.getY());
			}
		}
		else
		{
			if ( m_eTouch1Status == TS_DRAG && m_bCanDrag )
			{
				dispatchEvent(CEventGesture::DRAG);
				onTouchDrag(cursor.getX(), cursor.getY());
			}

			m_uiTouch1Time = CTime::getTickCount();
		}
	}

	void CComponent::addCursorR(const CCursor &cursor)
	{
		onTouchPressR(cursor.getX(), cursor.getY());
	}

	void CComponent::updateCursor(const CCursor &cursor)
	{
		CComponent *pComponent = getGestureCatcher();
		if ( pComponent != NULL )
		{
			ENV::getGestureRecognizer()->updateCursor( ENV::getGestureGroupID( pComponent ), cursor );
		}

		for ( UINT ui = 0; ui < m_aryCursor.size(); ui++ )
		{
			if( m_aryCursor[ui].getID() == cursor.getID() )
			{
				m_aryCursor[ui] = cursor;
			}
		}

		if ( cursor.getID() != m_uiActiveCursor )
		{
			playFilter();
			return;
		}

		switch (m_eTouch1Status)
		{
		case TS_PRESS:
			if (checkMove(cursor, m_ptTouch1LastPoint, ENV::getWaverDistance()))
			{
				INT iDeltaX = cursor.getX() - m_ptTouch1LastPoint.getX();
				INT iDeltaY = cursor.getY() - m_ptTouch1LastPoint.getY();
				m_ptTouch1LastPoint.setX(cursor.getX());
				m_ptTouch1LastPoint.setY(cursor.getY());

				playFilter();
				m_eTouch1Status = TS_MOVE;
				dispatchEvent(CEventGesture::MOVE);
				onTouchMove(cursor.getX(), cursor.getY(), iDeltaX, iDeltaY);
			}
			else if (CTime::getTickCount() - m_uiTouch1Time > ENV::getLongPressDelay())
			{
				m_eTouch1Status = TS_LONG_PRESS;
				m_uiTouch1Time = CTime::getTickCount();
				dispatchEvent(CEventGesture::LONG_PRESS);
				onTouchLongPress(cursor.getX(), cursor.getY());
			}
			break;

		case TS_MOVE:
			if (checkMove(cursor, m_ptTouch1LastPoint, ENV::getMoveWaverDistance()))
			{
				INT iDeltaX = cursor.getX() - m_ptTouch1LastPoint.getX();
				INT iDeltaY = cursor.getY() - m_ptTouch1LastPoint.getY();
				m_ptTouch1LastPoint.setX(cursor.getX());
				m_ptTouch1LastPoint.setY(cursor.getY());

				playFilter();
				dispatchEvent(CEventGesture::MOVE);
				onTouchMove(cursor.getX(), cursor.getY(),iDeltaX, iDeltaY);
			}
			break;

		case TS_LONG_PRESS:
		case TS_CONTINUE_PRESS:
			if (checkMove(cursor, m_ptTouch1LastPoint, ENV::getWaverDistance()))
			{
				if (m_bCanDrag)
				{
					m_ptTouch1LastPoint.setX(cursor.getX());
					m_ptTouch1LastPoint.setY(cursor.getY());

					playFilter();
					m_eTouch1Status = TS_DRAG;
					dispatchEvent(CEventGesture::DRAG);
					onTouchDrag(cursor.getX(), cursor.getY());
				}
				else
				{
					// transfer to TS_MOVE or TS_CONTINUE_PRESS ?
					if (CTime::getTickCount() - m_uiTouch1Time > ENV::getContinuePressInterval())
					{
						m_eTouch1Status = TS_CONTINUE_PRESS;
						m_uiTouch1Time = CTime::getTickCount();
						dispatchEvent(CEventGesture::CONTINUE_PRESS);
						onTouchContinuePress(cursor.getX(), cursor.getY());
					}
				}
			}
			else if (CTime::getTickCount() - m_uiTouch1Time > ENV::getContinuePressInterval())
			{
				m_eTouch1Status = TS_CONTINUE_PRESS;
				m_uiTouch1Time = CTime::getTickCount();
				dispatchEvent(CEventGesture::CONTINUE_PRESS);
				onTouchContinuePress(cursor.getX(), cursor.getY());
			}
			break;

		case TS_DRAG:
			if (checkMove(cursor, m_ptTouch1LastPoint, ENV::getMoveWaverDistance()))
			{
				m_ptTouch1LastPoint.setX(cursor.getX());
				m_ptTouch1LastPoint.setY(cursor.getY());

				playFilter();
				dispatchEvent(CEventGesture::DRAG);
				onTouchDrag(cursor.getX(), cursor.getY());
			}
			break;

		case TS_DOUBLE_PRESS:
			break;
		}
	}

	void CComponent::removeCursor(const CCursor &cursor)
	{
		CComponent *pComponent = getGestureCatcher();
		if ( pComponent != NULL )
		{
			if( pComponent != this )
			{
				pComponent->removeTouchCursor(cursor);
			}

			ENV::getGestureRecognizer()->removeCursor( ENV::getGestureGroupID(pComponent) ,cursor );
		}

		UINT uiIndex = m_aryCursor.size();
		for( UINT ui = 0; ui < m_aryCursor.size(); ui++ )
		{
			if( m_aryCursor[ui].getID() == cursor.getID() )
			{
				uiIndex = ui;
				break;
			}
		}

		if ( cursor.getID() != m_uiActiveCursor )
		{
			m_aryCursor.deleteItemByIndex(uiIndex);
			return;
		}

		if (  m_aryCursor.size() > 1 )
		{
			m_aryCursor.deleteItemByIndex(uiIndex);
			m_uiActiveCursor = m_aryCursor[0].getID();

			switch (m_eTouch1Status)
			{
			case TS_PRESS:
				m_uiTouch1Time = CTime::getTickCount();
				break;

			case TS_MOVE:
				m_ptTouch1LastPoint.setX(m_aryCursor[0].getX());
				m_ptTouch1LastPoint.setY(m_aryCursor[0].getY());
				break;

			case TS_LONG_PRESS:
			case TS_CONTINUE_PRESS:
				m_ptTouch1LastPoint.setX(m_aryCursor[0].getX());
				m_ptTouch1LastPoint.setY(m_aryCursor[0].getY());

				m_uiTouch1Time = CTime::getTickCount();
				break;

			case TS_DRAG:
				m_ptTouch1LastPoint.setX(m_aryCursor[0].getX());
				m_ptTouch1LastPoint.setY(m_aryCursor[0].getY());

				dispatchEvent(CEventGesture::DRAG);
				onTouchDrag(m_aryCursor[0].getX(), m_aryCursor[0].getY());
				break;

			case TS_DOUBLE_PRESS:
				break;
			}
			return;
		}

		m_uiTouch1Time = 0;

		switch(m_eTouch1Status)
		{
		case TS_PRESS:
			m_uiTouch1Time = CTime::getTickCount();
			m_eTouch1Status = TS_INIT;
			dispatchEvent(CEventGesture::PRESS_RELEASE);
			onTouchPressRelease(cursor.getX(), cursor.getY());			
			break;

		case TS_LONG_PRESS:
		case TS_CONTINUE_PRESS:
			m_eTouch1Status = TS_INIT;
			dispatchEvent(CEventGesture::LONG_PRESS_RELEASE);
			onTouchLongPressRelease(cursor.getX(), cursor.getY());					
			break;

		case TS_MOVE:
			m_eTouch1Status = TS_INIT;
			dispatchEvent(CEventGesture::MOVE_RELEASE);
			onTouchMoveRelease(cursor.getX(), cursor.getY(), cursor.getXSpeed(), cursor.getYSpeed());					
			break;

		case TS_DRAG:
			m_eTouch1Status = TS_INIT;
			dispatchEvent(CEventGesture::DRAG_RELEASE);
			onTouchDragRelease(cursor.getX(), cursor.getY());
			break;

		case TS_DOUBLE_PRESS:
			m_eTouch1Status = TS_INIT;
			dispatchEvent(CEventGesture::DOUBLE_PRESS_RELEASE);
			onTouchDoublePressRelease(cursor.getX(), cursor.getY());
			break;

		case TS_DISABLED_PRESS:
			m_eTouch1Status = TS_INIT;
			dispatchEvent(CEventGesture::PRESS_RELEASE);
			onTouchPressRelease(cursor.getX(), cursor.getY());
			break;

		default:
			m_eTouch1Status = TS_INIT;
			break;
		}

		m_aryCursor.deleteItemByIndex(uiIndex);
	}

	void CComponent::removeCursorR(const CCursor &cursor)
	{
		onTouchPressReleaseR(cursor.getX(), cursor.getY());
	}

	void CComponent::dispatchEvent(UINT uiEventID)
	{
		CEventDispatcher::dispatchEvent(this, uiEventID);
	}

	void CComponent::buildRenderTreeFromBackground(CRenderGroup &renderGroup)
	{
		if (m_bVisible && m_skin.isNotNull() && m_skin->getVisible())
		{
			if(m_skin->getImageType() == CMovieClip::TYPE_FILE)
			{
				m_renderImage.setFilePath(m_skin->getCurrentImage());
			}
			else
			{
				String strID;
				Gdiplus::Bitmap* pBitmap = NULL;

				if (m_skin->getBitmap(strID, &pBitmap) == TRUE)
				{
					m_renderImage.setBitmap(strID, pBitmap);
				}
			}

			CPoint pt = localToGlobal(0, 0);

			m_renderImage.setX(pt.getX());
			m_renderImage.setY(pt.getY());
			m_renderImage.setWidth(m_szPreferredSize.getX());
			m_renderImage.setHeight(m_szPreferredSize.getY());

			if (m_renderImage.getLayout() == CRenderImage::L_UNKNOWN)
			{
				m_renderImage.setLayout(CRenderImage::L_NORMAL);
			}

			renderGroup.addRenderItem(&m_renderImage);
		}
	}

	void CComponent::buildRenderTreeFromCCP(CRenderGroup &renderGroup, INT iCCPPos)
	{
		if (m_bVisible && m_bCCPFocus && m_CCPSkin.isNotNull() && m_CCPSkin->getVisible())
		{
			m_renderImageCCP.setFilePath(m_CCPSkin->getCurrentImage());

			CPoint pt = localToGlobal(m_CCPSkin->getX(), m_CCPSkin->getY());

			m_renderImageCCP.setX(pt.getX());
			m_renderImageCCP.setY(pt.getY());
			m_renderImageCCP.setWidth(m_CCPSkin->getWidth());
			m_renderImageCCP.setHeight(m_CCPSkin->getHeight());
			m_renderImageCCP.setLayout(CRenderImage::L_NORMAL);

			if (getCCPBelowComponent())
			{
				iCCPPos = iCCPPos < 0 ? 0 : iCCPPos;
				renderGroup.insertRenderItem(iCCPPos,&m_renderImageCCP);				
			}
			else
			{
				renderGroup.addRenderItem(&m_renderImageCCP);
			}
		}
	}

	BOOLEAN CComponent::checkMove(const CCursor &cursor, const CPoint &ptLastPoint, INT iDistance)
	{
		INT iDistanceX = cursor.getX() - ptLastPoint.getX();
		INT iDistanceY = cursor.getY() - ptLastPoint.getY();

		if ((Math::abs(iDistanceX) > iDistance) || (Math::abs(iDistanceY) > iDistance))
		{				
			return TRUE;
		}

		return FALSE;
	}

	void CComponent::refreshCCPSkin()
	{
		if (m_CCPSkin.isNull())
		{
			m_CCPSkin = m_CCPView.getSkin();
		}
	}

	void CComponent::drawCCPFocs()
	{
		if (m_CCPSkin.isNotNull())
		{
			m_CCPSkin->setX(-((INT)m_CCPSkin->getWidth() - m_szPreferredSize.getX()) / 2);
			m_CCPSkin->setY(-((INT)m_CCPSkin->getHeight() - m_szPreferredSize.getY()) / 2);
		}		
	}

	void CComponent::destroyCCPSkin()
	{
		m_CCPSkin.setNull();
	}

	CComponent* CComponent::getGestureCatcher()
	{
		if ( m_bCanCatchGesture == TRUE)
		{
			return this;
		}
		else
		{
			if ( m_pParent != NULL )
			{
				return m_pParent->getGestureCatcher();
			}
			else
			{
				return NULL;
			}
		}
	}
}