#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlList::CCtrlList()
		: m_pView(NULL)		
		, m_pScrollbar(NULL)
		, m_bControlScrollbarVisible(FALSE)		
		, m_pBubble(NULL)
		, m_bControlBubbleVisible(TRUE)
		, m_bShowBubble(FALSE)
		//, m_pKeyboard(NULL)
		, m_bFirstBuild(TRUE)
		, m_iCurSel(-1)
		, m_iCurTouchFocus(-1)
		, m_iCurBubble(-1)
		, m_iCurCCPFocus(-1)
		, m_iShowTopIndex(0)
		, m_iShowItemIndex(0)
		, m_uiShowItemCount(0)
		, m_iLeftSpace(0)
		, m_iItemSpace(0)
		, m_uiItemsPerPage(0)
		, m_bInAppendPrevPageMode(FALSE)
		, m_bInAppendNextPageMode(FALSE)
		, m_iTouchBeginPosY(0)
		, m_iShowBeginPosY(0)
		, m_iShowCurrentPosY(0)
		, m_bDirectionDown(FALSE)
		, m_bCanScrollByTouchMove(TRUE)
		, m_bOuterChangedData(FALSE)
		, m_eScrollType(CCP_FOCUS_SCROLL)
		, m_uiCCPFocusFixPos(0)
		, m_bCanMoveFocusByUpDown(FALSE)
		, m_bIsEnterAnimationOn(FALSE)
		, m_bIsElasticityOn(FALSE)
		, m_bIsFlingOn(FALSE)
		, m_bIsTouchAnimationOn(FALSE)
	{
		m_model.setComponent(this);
		setBaseView(&m_view);

		m_viewState.pObjSelectedItem = NULL;
		m_viewState.pObjCCPFocusItem = NULL;
		m_viewState.iShowTopIndex = -1;

		m_pAmtEnter = new CListAmtEnterInterdigitation();
		m_pAmtEnter->m_pList = this;

		m_pAmtElasticity = new CListAmtElasticity();
		m_pAmtElasticity->m_pList = this;

		m_pAmtFling = new CListAmtFling();
		m_pAmtFling->m_pList = this;

		m_pAmtTouch = new CListAmtTouchShake();
		m_pAmtTouch->m_pList = this;		
	}

	CCtrlList::~CCtrlList()
	{
		;
	}

	BEGIN_EVENT_MAP(CCtrlList)
		CONTROL_EVENT_HANDLER(m_pScrollbar, CEventScrollbar::POS_CHANGED, onScrollbarPosChanged)
		CONTROL_EVENT_HANDLER(m_pScrollbar, CEventScrollbar::POS_CONTINUE_CHANGE_ENDED, onScrollbarPosContinueChangeEnded)
	END_EVENT_MAP_2

	CModelList* CCtrlList::getModel()
	{
		return &m_model;
	}

	void CCtrlList::setView(IViewList* pView)
	{
		m_pView = pView;
	}

	void CCtrlList::setAssociateScrollbar(CCtrlScrollbar* pScrollBar, BOOLEAN bControlScrollbarVisible, BOOLEAN bMovePageWhenClickUpDownArrow)
	{
		m_bControlScrollbarVisible = bControlScrollbarVisible;

		if (m_pScrollbar == NULL)
		{
			m_pScrollbar = pScrollBar;
		}

		if (m_pScrollbar == NULL)
		{
			return;
		}

		if (m_bControlScrollbarVisible)
		{
			m_pScrollbar->setVisible(FALSE);
			m_pScrollbar->setIsAdjustMode(FALSE);
		}
		else
		{
			m_pScrollbar->movePageWhenClickUpDownArrow(bMovePageWhenClickUpDownArrow);
			m_pScrollbar->addEventListener(this);
		}
	}

	void CCtrlList::setAssociateBubble(CComponent* pBubble, BOOLEAN bControlBubbleVisible)
	{
		m_bControlBubbleVisible = bControlBubbleVisible;

		if (m_pBubble == NULL)
		{
			m_pBubble = pBubble;
		}

		if (m_pBubble == NULL)
		{
			return;
		}

		if (m_bControlBubbleVisible)
		{
			m_pBubble->setVisible(FALSE);
		}
	}

//	void CCtrlList::setAssociateKeyboard(CCtrlIndexKeyboard* pIndexKeyboard);
///	{
//		;
//	}

	void CCtrlList::setCurSel(INT iValue)
	{
		if (iValue < 0)
		{
			m_iCurSel = -1;
		}			
		else if ( iValue >= (INT)m_model.getItemCount())
		{
			m_iCurSel = - 1;
		}
		else
		{
			if (m_iCurSel != iValue)
			{
				m_iCurSel = iValue;
				dispatchEvent(CEventList::SELECT_CHANGED);
			}
		}
	}

	void CCtrlList::setCurTouchFocus(INT iValue)
	{
		if (iValue < 0)
		{
			m_iCurTouchFocus = -1;
		}			
		else if (iValue >= (INT)m_model.getItemCount())
		{
			m_iCurTouchFocus = - 1;
		}
		else
		{
			m_iCurTouchFocus = iValue;
		}
	}

	void CCtrlList::setCurCCPFocus(INT iValue)
	{
		if (m_eScrollType == CCP_FOCUS_SCROLL)
		{
			if (iValue < 0)
			{
				m_iCurCCPFocus = -1;
			}			
			else if (iValue >= (INT)m_model.getItemCount())
			{
				m_iCurCCPFocus = m_model.getItemCount() - 1;
			}
			else
			{
				m_iCurCCPFocus = iValue;
			}
		}
	}

	INT CCtrlList::getCurCCPFocus()
	{
		if (hasFocus())
		{
			return m_iCurCCPFocus;
		}
		
		return -1;
	}

	void CCtrlList::setCCPFocusedItem(Object* pObj)
	{
		if (m_eScrollType == CCP_FOCUS_SCROLL)
		{
			m_iCurCCPFocus = m_model.getItemIndex(pObj);
		}
	}

	Object* CCtrlList::getCCPFocusedItem()
	{
		if (hasFocus())
		{
			if (m_iCurCCPFocus == -1)
			{
				return NULL;
			}
			else
			{
				return m_aryShowItem[m_iCurCCPFocus]->pObject;
			}
		}
		else
		{
			return NULL;
		}
	}

	Object* CCtrlList::getDraggedItem()
	{
		if (getCanDrag() && m_iCurTouchFocus >= 0)
		{
			return m_aryShowItem[m_iCurTouchFocus]->pObject;
		}
		else
		{
			return NULL;
		}
	}

	INT CCtrlList::getItemInsertToPos(CPoint pt)
	{
		if (m_aryShowComponent.size() == 0)
		{
			return -1;
		}

		for (INT i = 0; i < (INT)m_aryShowComponent.size(); i++)
		{
			tagShowComponent* pShowComponent = m_aryShowComponent[i];

			INT iOldX = pShowComponent->pComponent->getX();
			INT iOldY = pShowComponent->pComponent->getY();
			pShowComponent->pComponent->setX(pShowComponent->iShowX - m_scrollRect.getX());
			pShowComponent->pComponent->setY(pShowComponent->iShowY - m_scrollRect.getY());
			
			if (pShowComponent->pComponent->hitTest(pt.getX(), pt.getY()))
			{
				pShowComponent->pComponent->setX(iOldX);
				pShowComponent->pComponent->setY(iOldY);

				return i + m_iShowItemIndex;
			}

			pShowComponent->pComponent->setX(iOldX);
			pShowComponent->pComponent->setY(iOldY);
		}

		return -1;
	}

	CComponent* CCtrlList::getItemComponent(Object* pObj)
	{
		for (INT i = 0; i < (INT)m_aryShowItem.size(); i++)
		{
			if (m_aryShowItem[i]->pObject == pObj)
			{
				return m_aryShowItem[i]->pComponent;
			}
		}
		
		return NULL;
	}

	void CCtrlList::showTopItem(UINT uiIndex)
	{
		if (m_aryShowItem.size() == 0)
		{
			return;
		}
		
		UINT uiMaxShowTopIndex = calculateShowItemIndex(getTotalShowItemHeight() - getPreferredSize().getY() + DISTANCE_OFFSET);
		if (uiIndex > uiMaxShowTopIndex)
		{
			uiIndex = uiMaxShowTopIndex;
		}
		
		m_iShowTopIndex = uiIndex;
		verifyShowInfo();
		showItemState();
		
		INT iPos = m_aryShowItem[m_iShowTopIndex]->pComponent->getY();
		showItems(iPos);
		showBubble();
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->getModel()->setPosition(iPos);
			m_pScrollbar->refresh();
		}
	}

	void CCtrlList::refreshForAppendPrevPageItem(INT iItemCount)
	{
		if (iItemCount <= 0)
		{
			return;
		}

		appendShowItemForPrevPage(iItemCount);
		resetScrollbar(TRUE);

		if (m_bFirstBuild)
		{
			m_bFirstBuild = FALSE;

			updateViewForFirstBuild();
		}
		else
		{
			CComponent *pComponent = m_aryShowItem[iItemCount - 1]->pComponent;
			INT iInsertHeight = pComponent->getY() + pComponent->getPreferredSize().getY() + m_iItemSpace;

			if (m_pScrollbar != NULL)
			{
				m_pScrollbar->getModel()->setPosition(m_pScrollbar->getModel()->getPosition() + iInsertHeight);
				m_pScrollbar->refresh();
			}

			if (m_iCurSel >= 0)
			{
				m_iCurSel += iItemCount;
			}
			
			if (m_iCurTouchFocus >= 0)
			{
				m_iCurTouchFocus += iItemCount;
			}

			if (m_iCurCCPFocus >= 0)
			{
				m_iCurCCPFocus += iItemCount;
			}

			m_iShowTopIndex += iItemCount;
			m_iShowItemIndex += iItemCount;

			m_scrollRect.setY(m_scrollRect.getY() + iInsertHeight);

			m_iShowBeginPosY += iInsertHeight;
			m_iShowCurrentPosY += iInsertHeight;
			if (m_pAmtFling != NULL)
			{
				CListAmtFling *pFling = static_cast<CListAmtFling*>(m_pAmtFling.getRaw());
				pFling->m_iStartValue += iInsertHeight;
			}

			verifyShowInfo();
			showItemState();

			showItems(m_scrollRect.getY());

			m_iCurBubble = -1;
			showBubble();
		}		

		m_bInAppendPrevPageMode = FALSE;

		setDirty();

		m_bOuterChangedData = TRUE;
	}

	void CCtrlList::refreshForAppendNextPageItem()
	{
		appendShowItemForNextPage();
		resetScrollbar(TRUE);
		
		if (m_bFirstBuild)
		{
			m_bFirstBuild = FALSE;

			updateViewForFirstBuild();
		}
		else
		{
			verifyShowInfo();
			showItemState();
			showItems(m_scrollRect.getY());

			m_iCurBubble = -1;
			showBubble();		
		}

		m_bInAppendNextPageMode = FALSE;

		setDirty();

		m_bOuterChangedData = TRUE;
	}	

	void CCtrlList::backupViewState()
	{
		m_viewState.pObjSelectedItem = getSelectedItem();
		m_viewState.pObjCCPFocusItem = getCCPFocusedItem();
		m_viewState.iShowTopIndex = m_iShowTopIndex;
	}

	void CCtrlList::restoreViewState()
	{
		refresh();
		
		setSelectedItem(m_viewState.pObjSelectedItem);
		setCCPFocusedItem(m_viewState.pObjCCPFocusItem);
		showTopItem(m_viewState.iShowTopIndex);

		m_viewState.pObjSelectedItem = NULL;
		m_viewState.pObjCCPFocusItem = NULL;
		m_viewState.iShowTopIndex = -1;
	}

	void CCtrlList::playEnterAnimation()
	{
		startEnterAnimation();
	}

	void CCtrlList::setTouchAnimationType(ETouchAnimation eValue)
	{
		switch(eValue)
		{
		case TOUCH_SHAKE:
			m_pAmtTouch = new CListAmtTouchShake();
			m_pAmtTouch->m_pList = this;
			break;
		}
	}

	void CCtrlList::lock()
	{
		CComponent::lock();
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->lock();
		}
		
		//if (m_keyboard != null)
		//{
		//	m_keyboard.lock();
		//}
		
		if (m_pBubble != NULL)
		{
			m_pBubble->lock();
		}
	}

	void CCtrlList::unlock()
	{
		CComponent::unlock();
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->unlock();
		}
		
		//if (m_keyboard != null)
		//{
		//	m_keyboard.unlock();
		//}
		
		if (m_pBubble != NULL)
		{
			m_pBubble->unlock();
		}
	}

	void CCtrlList::refresh()
	{
		CComponent::refresh();

		buildShowItemList();
		resetScrollbar();		
		
		updateViewForFirstBuild();

		if (m_aryShowItem.size() == 0)
		{
			m_bFirstBuild = TRUE;
		}
		else
		{
			m_bFirstBuild = FALSE;
		}
	}

	void CCtrlList::littleRefresh()
	{
		CComponent::littleRefresh();
		
		showItemState();
	}

	void CCtrlList::destroySkin()
	{
		destroyShowItemList();
		
		CComponent::destroySkin();
	}

	void CCtrlList::destroy()
	{
		if (isDestroyed())
		{
			return;
		}

		CComponent::destroy();
	}

	void CCtrlList::buildRenderTree(CRenderGroup &renderGroup)
	{
		if (!getVisible())
		{
			return;
		}

		CPoint pt = localToGlobal(0, 0);
		m_renderGroup.setX(pt.getX());
		m_renderGroup.setY(pt.getY());
		m_renderGroup.setWidth(getPreferredSize().getX());
		m_renderGroup.setHeight(getPreferredSize().getY());
		m_renderGroup.setNeedClip(TRUE);
		renderGroup.addRenderItem((CRenderItem*)&m_renderGroup);

		buildRenderTreeFromBackground(m_renderGroup);

		INT iOldX = 0, iOldY = 0;
		for (INT j = 0; j < (INT)m_aryShowComponent.size(); j++)
		{
			CComponent* pComponent = m_aryShowComponent[j]->pComponent;
			if (pComponent != NULL)
			{
				iOldX = pComponent->getX();
				iOldY = pComponent->getY();
				pComponent->setX(m_aryShowComponent[j]->iShowX - m_scrollRect.getX());
				pComponent->setY(m_aryShowComponent[j]->iShowY - m_scrollRect.getY());
				pComponent->buildRenderTree(m_renderGroup);
				pComponent->setX(iOldX);
				pComponent->setY(iOldY);
			}
		}

		if (m_skin.isNotNull())
		{
			buildRenderTreeFromCCP(m_renderGroup,1);
		}
		else
		{
			buildRenderTreeFromCCP(m_renderGroup);
		}
	}

	BOOLEAN	CCtrlList::canSetFocus()
	{
		if (CComponent::canSetFocus() == FALSE)
		{
			return FALSE;
		}
		
		if (m_aryShowItem.size() == 0)
		{
			return FALSE;
		}
		
		return TRUE;
	}

	void CCtrlList::setFocus(ECCPOperation eCCPOperation)
	{
		innerSetFocus(TRUE);
		
		if ( m_eScrollType == CCP_FOCUS_SCROLL )
		{
			switch(eCCPOperation)
			{
			case CComponent::CCP_MOVE_DOWN:
			case CComponent::CCP_MOVE_RIGHT:
			case CComponent::CCP_ROTATE_RIGHT:				
					m_iCurCCPFocus = getFirstCCPItemIndex();
					break;
					
			case CComponent::CCP_MOVE_LEFT:
			case CComponent::CCP_MOVE_UP:
			case CComponent::CCP_ROTATE_LEFT:
					m_iCurCCPFocus = getLastCCPItemIndex();
					break;
			}
		}
		else
		{
			//m_iCurCCPFocus = m_iShowTopIndex + m_uiCCPFocusFixPos;
			m_iCurCCPFocus = m_iCurBubble;
		}
		
		adjustCCPFocusDisplayPos();
		
		dispatchEvent(CEventList::CCP_FOCUS_CHANGED);
	}

	void CCtrlList::releaseFocus()
	{
		if (innerGetFocus())
		{
			//m_iCurCCPFocus = -1;
			CComponent::releaseFocus();
			dispatchEvent(CEventList::CCP_FOCUS_CHANGED);
			return;
		}
		
		CComponent* pComponent = getCCPFocusComponent();
		if (pComponent != NULL)
		{
			pComponent->releaseFocus();
		}
	}

	BOOLEAN CCtrlList::hasFocus()
	{
		if ( CComponent::hasFocus() )
		{
			return TRUE;
		}
		
		CComponent* pComponent = getCCPFocusComponent();
		if (pComponent != NULL && pComponent->hasFocus())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOLEAN CCtrlList::moveFocusToLeft()
	{
		if ( innerGetFocus() )
		{
			//return canMoveFocusOut() ? FALSE : TRUE;
			return FALSE;
		}
		
		CComponent* pComponent = getCCPFocusComponent();
		if (pComponent == NULL)
		{
			//return canMoveFocusOut() ? FALSE : TRUE;
			return FALSE;
		}
			
		pComponent->moveFocusToLeft();
		setDirty();
		return TRUE;
	}

	BOOLEAN CCtrlList::moveFocusToRight()
	{
		if ( innerGetFocus() )
		{
			//return canMoveFocusOut() ? FALSE : TRUE;
			return FALSE;
		}
		
		CComponent* pComponent = getCCPFocusComponent();
		if (pComponent == NULL)
		{
			//return canMoveFocusOut() ? FALSE : TRUE;
			return FALSE;
		}
			
		pComponent->moveFocusToRight();
		setDirty();
		return TRUE;
	}

	BOOLEAN CCtrlList::moveFocusToUp()
	{
		if (canMoveFocus() && m_bCanMoveFocusByUpDown)
		{
			return moveCCPFocusIndexDecrease(CComponent::CCP_MOVE_UP);
		}
		else
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
	}

	BOOLEAN CCtrlList::moveFocusToDown()
	{
		if (canMoveFocus() && m_bCanMoveFocusByUpDown)
		{
			return moveCCPFocusIndexIncrease(CComponent::CCP_MOVE_DOWN);
		}
		else
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
	}

	BOOLEAN CCtrlList::turnFocusToLeft(UINT uiStep)
	{
		return moveCCPFocusIndexDecrease(CComponent::CCP_ROTATE_LEFT, uiStep);
	}

	BOOLEAN CCtrlList::turnFocusToRight(UINT uiStep)
	{
		return moveCCPFocusIndexIncrease(CComponent::CCP_ROTATE_RIGHT, uiStep);
	}

	void CCtrlList::doFocusEnterPress()
	{
		CComponent* pComponent = getCCPFocusComponent();
		
		if ( pComponent != NULL )
		{
			if ( innerGetFocus() )
			{
				m_iCurTouchFocus = m_iCurCCPFocus;
				littleRefresh();
			}
			
			m_bOuterChangedData = FALSE;
			pComponent->doFocusEnterPress();
		}
	}

	void CCtrlList::doFocusEnterRelease()
	{
		CComponent* pComponent = getCCPFocusComponent();
		
		if ( pComponent != NULL)
		{
			if (innerGetFocus())
			{					
				if (pComponent->isContainer() && pComponent->canSetFocus())
				{
					innerSetFocus(FALSE);
					pComponent->setFocus(CComponent::CCP_ROTATE_RIGHT);
					
					checkSelectChanged();
				}
				else
				{
					m_bOuterChangedData = FALSE;
					pComponent->doFocusEnterRelease();
					
					if (!m_bOuterChangedData)
					{
						checkSelectChanged();
					}
				}
			}
			else
			{
				pComponent->doFocusEnterRelease();
			}
		}
	}

	BOOLEAN CCtrlList::doFocusBack()
	{
		if (innerGetFocus())
		{
			BOOLEAN bRet = CComponent::doFocusBack();
			dispatchEvent(CEventList::CCP_FOCUS_CHANGED);
			return bRet;
		}
		else
		{
			CComponent* pComponent = getCCPFocusComponent();
			if (pComponent != NULL)
			{
				if (pComponent->doFocusBack())
				{
					return TRUE;
				}
				
				innerSetFocus(TRUE);
				littleRefresh();
				return TRUE;
			}
		}
		
		return FALSE;
	}

	void CCtrlList::onTouchPress(INT iX, INT iY)
	{
		stopFlingAnimation();
		
		m_iTouchBeginPosY = iY;
		m_iShowBeginPosY = m_scrollRect.getY();
		m_iShowCurrentPosY = m_scrollRect.getY();
		
		for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
		{
			tagShowItem* pShowItem = m_aryShowItem[i + m_iShowItemIndex];
			
			if (pShowItem->pComponent->hitTest(iX + m_scrollRect.getX(), iY + m_scrollRect.getY()) == FALSE)
			{
				continue;
			}
			
			if (m_iCurTouchFocus != i + m_iShowItemIndex)
			{
				m_iCurTouchFocus = i + m_iShowItemIndex;
				showItemState();
			}
			else
			{
				stopTouchAnimation();
			}
			
			//if (!m_model.getItemDisable(showItem.objItem))
			{
				m_bOuterChangedData = FALSE;
				pShowItem->pComponent->onTouchPress(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}

			break;
		}
	}

	void CCtrlList::onTouchPressRelease(INT iX, INT iY)
	{
		if (m_iCurTouchFocus >= 0)
		{
			switchCCPFocusForTouch();		
			
			//if (!m_model.getItemDisable(m_aryShowItem[m_iCurTouchFocus]->pObject))
			{
				m_bOuterChangedData = FALSE;
				m_aryShowItem[m_iCurTouchFocus]->pComponent->onTouchPressRelease(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
				
				if (!m_bOuterChangedData)
				{
					startTouchAnimation(m_aryShowItem[m_iCurTouchFocus]->pComponent);
				}
			}
		}
		
		if (!m_bOuterChangedData)
		{
			checkSelectChanged();
		}
	}

	void CCtrlList::onTouchLongPress(INT iX, INT iY)
	{
		if (m_iCurTouchFocus >= 0)
		{
			//if (!m_model.getItemDisable(m_aryShowItem[m_iCurTouchFocus]->pObject))
			{
				m_bOuterChangedData = FALSE;
				m_aryShowItem[m_iCurTouchFocus]->pComponent->onTouchLongPress(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}
		}
	}

	void CCtrlList::onTouchContinuePress(INT iX, INT iY)
	{
		if (m_iCurTouchFocus >= 0)
		{
			//if (!m_model.getItemDisable(m_aryShowItem[m_iCurTouchFocus]->pObject))
			{
				m_bOuterChangedData = FALSE;
				m_aryShowItem[m_iCurTouchFocus]->pComponent->onTouchContinuePress(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}
		}
	}

	void CCtrlList::onTouchLongPressRelease(INT iX, INT iY)
	{
		if (m_iCurTouchFocus >= 0)
		{
			switchCCPFocusForTouch();
			
			//if (!m_model.getItemDisable(m_aryShowItem[m_iCurTouchFocus]->pObject))
			{
				m_bOuterChangedData = FALSE;
				m_aryShowItem[m_iCurTouchFocus]->pComponent->onTouchLongPressRelease(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}
		}
		
		if (!m_bOuterChangedData)
		{
			checkSelectChanged();
		}
	}

	void CCtrlList::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
	{
		if ( !m_bCanScrollByTouchMove )
		{
			return;
		}

		if ((INT)getTotalShowItemHeight() > getPreferredSize().getY())
		{
			INT iPos = 0;
			UINT uiTotalShowItemHeight = getTotalShowItemHeight();

			if (m_scrollRect.getY() <= 0)
			{
				iPos = m_iShowBeginPosY - iDeltaY * (getPreferredSize().getY() + m_scrollRect.getY()) / getPreferredSize().getY();
				if ( !m_bIsElasticityOn && iPos < 0 )
				{
					m_bDirectionDown = FALSE;
 					dispatchEvent(CEventList::ITEM_SCROLLING);
 					checkNeedMorePage();
					return;
				}
			}
			else if ((INT)uiTotalShowItemHeight - getPreferredSize().getY() <= m_scrollRect.getY())
			{
				iPos = m_iShowBeginPosY - iDeltaY * ((INT)uiTotalShowItemHeight - m_scrollRect.getY()) / getPreferredSize().getY();
				
				if ( !m_bIsElasticityOn && iPos > (INT)uiTotalShowItemHeight - getPreferredSize().getY() )
				{
					m_bDirectionDown = TRUE;
 					dispatchEvent(CEventList::ITEM_SCROLLING);
 					checkNeedMorePage();
					return;
				}
			}
			else
			{
				iPos = m_iShowBeginPosY - iDeltaY;
			}

			m_iShowBeginPosY = iPos;

			if (m_pScrollbar != NULL)
			{
				m_pScrollbar->getModel()->setPosition(m_iShowBeginPosY);
				m_pScrollbar->refresh();

				if (m_bControlScrollbarVisible)
				{
					m_pScrollbar->setVisible(TRUE);
				}
			}
				
			if (needVerifyShowInfo(m_iShowBeginPosY))
			{
				verifyShowInfo();				
				showItemState();
			}
			
			showItems(m_iShowBeginPosY);
			m_bShowBubble = TRUE;
			showBubble();
			
			if (m_scrollRect.getY() > m_iShowCurrentPosY)
			{
				m_bDirectionDown = TRUE;
			}
			else
			{
				m_bDirectionDown = FALSE;
			}
			m_iShowCurrentPosY = m_scrollRect.getY();		

			setDirty();

			dispatchEvent(CEventList::ITEM_SCROLLING);

			checkNeedMorePage();
		}
	}

	void CCtrlList::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
	{
		if (m_iCurTouchFocus >= 0)
		{
			//switchCCPFocusForTouch();
			
			//if (!m_model.getItemDisable(m_aryShowItem[m_iCurTouchFocus]->pObject))
			{
				m_aryShowItem[m_iCurTouchFocus]->pComponent->setParentMoving(TRUE);
				m_aryShowItem[m_iCurTouchFocus]->pComponent->onTouchMoveRelease(iX + m_scrollRect.getX(), iY + m_scrollRect.getY(), iSpeedX, iSpeedY);
				m_aryShowItem[m_iCurTouchFocus]->pComponent->setParentMoving(FALSE);
			}
		}
		
		m_iCurTouchFocus = -1;

		showItemState();

		if( !m_bCanScrollByTouchMove )
		{
			return;
		}
		
		hideBubble();
		
		UINT uiTotalShowItemHeight = getTotalShowItemHeight();
		
		if ((INT)uiTotalShowItemHeight <= getPreferredSize().getY())
		{
			return;
		}
		
		if (m_scrollRect.getY() < 0)
		{
			startElasticityAnimation(m_scrollRect.getY(), 0);
		}
		else if ((INT)uiTotalShowItemHeight - getPreferredSize().getY() < m_scrollRect.getY())
		{
			if (m_pScrollbar != NULL)
			{
				m_pScrollbar->getModel()->setPosition(m_pScrollbar->getModel()->getMaxPosition());
				m_pScrollbar->refresh();
			}
			
			if (needVerifyShowInfo(uiTotalShowItemHeight - getPreferredSize().getY()))
			{
				verifyShowInfo();
				showItemState();
			}
			
			startElasticityAnimation(m_scrollRect.getY(), uiTotalShowItemHeight - getPreferredSize().getY());
		}
		else
		{
			startFlingAnimation(m_scrollRect.getY(), iSpeedY);
		}
	}

	void CCtrlList::onTouchDrag(INT iX, INT iY)
	{
		if (m_iCurTouchFocus >= 0)
		{
			m_ptCurrentCursor.setX(iX);
			m_ptCurrentCursor.setY(iY);
			dispatchEvent(CEventList::ITEM_DRAGGING);
		}
	}

	void CCtrlList::onTouchDragRelease(INT iX, INT iY)
	{
		if (m_iCurTouchFocus >= 0)
		{
			m_ptCurrentCursor.setX(iX);
			m_ptCurrentCursor.setY(iY);

			switchCCPFocusForTouch();
			
			//if (!m_model.getItemDisable(m_aryShowItem[m_iCurTouchFocus]->pObject))
			{
				m_aryShowItem[m_iCurTouchFocus]->pComponent->onTouchDragRelease(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}
			
			dispatchEvent(CEventList::ITEM_DROPPED);
			
			m_iCurTouchFocus = -1;
			showItemState();
			m_ptCurrentCursor.setX(0);
			m_ptCurrentCursor.setY(0);
		}
	}

	BOOLEAN CCtrlList::contains(Object *pTarget)
	{
		for (INT j = 0; j < (INT)m_aryShowComponent.size(); j++)
		{
			CComponent* pComponent = m_aryShowComponent[j]->pComponent;
			if (pComponent != NULL && pComponent == pTarget)
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	void CCtrlList::updateViewForFirstBuild()
	{
// 		if (m_keyboard != null)
// 		{
// 			if (getTotalShowItemHeight() <= m_preferredSize.Y)
// 			{
// 				m_keyboard.visible = false;
// 			}
// 			else
// 			{
// 				m_keyboard.visible = true;
// 			}
// 		}

		m_scrollRect.setWidth(getPreferredSize().getX());
		m_scrollRect.setHeight(getPreferredSize().getY());

		m_iCurTouchFocus = -1;
		m_iCurBubble = -1;
		m_iShowTopIndex = 0;

		verifyShowInfo();
		showItemState();

		showItems(0);
		showBubble();
		startEnterAnimation();
	}

	void CCtrlList::buildShowItemList()
	{
		destroyShowItemList();
		
		UINT uiItemCount = (INT)m_model.getItemCount();
		
		INT iY = 0;
		for (UINT i = 0; i < uiItemCount; i++)
		{
			iY += insertShowItem(i, m_model.getItemAt(i), iY);
		}	
	}

	void CCtrlList::appendShowItemForPrevPage(INT iItemCount)
	{
		if (iItemCount <= 0)
		{
			return;
		}

		INT iY = 0;
		for (INT i = 0; i < iItemCount; i++)
		{
			iY += insertShowItem(i, m_model.getItemAt(i), iY);
		}

		UINT uiCount = m_model.getItemCount();
		for (UINT j = iItemCount; j < uiCount; j++)
		{
			CComponent *pComponent = m_aryShowItem[j]->pComponent;
			pComponent->setY(pComponent->getY() + iY);
		}
	}

	void CCtrlList::appendShowItemForNextPage()
	{
		UINT uiItemCount = m_model.getItemCount();
		
		UINT uiIndex = m_aryShowItem.size();
		if (uiIndex >= uiItemCount)
		{
			return;
		}
		
		INT iY = getTotalShowItemHeight();

		for ( ; uiIndex < uiItemCount; uiIndex++)
		{
			iY += insertShowItem((INT)uiIndex, m_model.getItemAt(uiIndex), iY);
		}
	}

	INT CCtrlList::insertShowItem(INT iIndex, Object *pObj, INT iPosY)
	{
		tagShowItem* pShowItem = new tagShowItem(pObj, m_pView->getItemComponent(this, pObj));
		m_aryShowItem.insertItem(iIndex, pShowItem);

		if (!pShowItem->pComponent->isContainer())
		{
			pShowItem->pComponent->notAcceptFocus();
		}

		pShowItem->pComponent->setX(m_iLeftSpace);
		pShowItem->pComponent->setY(iPosY);
		return pShowItem->pComponent->getPreferredSize().getY() + m_iItemSpace;
	}

	void CCtrlList::destroyShowItemList()
	{
		if (m_aryShowItem.size() == 0)
		{
			return;
		}
		
		for (UINT i = 0; i < m_aryShowItem.size(); i++)
		{
			tagShowItem* pShowItem = m_aryShowItem[i];

			pShowItem->pObject = NULL;
			pShowItem->pComponent->destroy();			
			pShowItem->pComponent = NULL;
		}
		m_aryShowItem.clear();

		m_aryShowComponent.clear();
		
		m_bOuterChangedData = TRUE;
	}

	void CCtrlList::checkNeedMorePage()
	{
		if (   m_model.getReachToHead() == FALSE
			&& m_uiItemsPerPage > 0
			&& m_iShowItemIndex < (INT)m_uiItemsPerPage - 1
			&& m_bInAppendPrevPageMode == FALSE )
		{
			m_bInAppendPrevPageMode = TRUE;
			dispatchEvent(CEventList::WANT_PREV_PAGE);
		}

		if (   m_model.getReachToTail() == FALSE
			&& m_uiItemsPerPage > 0
			&& m_aryShowItem.size() > 0
			&& m_iShowItemIndex >= (INT)m_aryShowItem.size() - (INT)m_uiItemsPerPage
			&& m_bInAppendNextPageMode == FALSE )
		{
			m_bInAppendNextPageMode = TRUE;
			dispatchEvent(CEventList::WANT_NEXT_PAGE);
		}
	}

	void CCtrlList::showItems(INT iTop)
	{
		m_scrollRect.setY(iTop);
		drawListCCPFocs();

		if (m_pScrollbar != NULL && m_bControlScrollbarVisible == FALSE)
		{
			tagShowItem *pShowItem = NULL;

// 			if (m_iShowItemIndex < (INT)m_aryShowItem.size())
// 			{
// 				pShowItem = m_aryShowItem[m_iShowItemIndex];
// 				m_pScrollbar->getModel()->setPreviousStep(pShowItem->pComponent->getPreferredSize().getY() + m_iItemSpace);
// 			}
// 
// 			INT iIndex = m_iShowItemIndex + (INT)m_uiShowItemCount - 1;
// 
// 			if (0 < iIndex && iIndex < (INT)m_aryShowItem.size())
// 			{
// 				pShowItem = m_aryShowItem[iIndex];
// 				m_pScrollbar->getModel()->setNextStep(pShowItem->pComponent->getPreferredSize().getY() + m_iItemSpace);
// 			}

			if (m_iShowItemIndex < (INT)m_aryShowItem.size())
			{
				pShowItem = m_aryShowItem[m_iShowItemIndex];
				m_pScrollbar->getModel()->setPreviousStep(pShowItem->pComponent->getPreferredSize().getY() + m_iItemSpace);
			}

			if (m_iShowTopIndex < (INT)m_aryShowItem.size())
			{
				pShowItem = m_aryShowItem[m_iShowTopIndex];
				m_pScrollbar->getModel()->setNextStep(pShowItem->pComponent->getPreferredSize().getY() + m_iItemSpace);
			}
		}

		setDirty();
	}

	void CCtrlList::showItemState()
	{
		tagShowItem *pShowItem = NULL;
		
		for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
		{
			pShowItem = m_aryShowItem[i + m_iShowItemIndex];
			
			BOOLEAN bSelect = (m_iCurSel == i + m_iShowItemIndex ? TRUE : FALSE);	
			BOOLEAN bFocus = (m_iCurTouchFocus == i + m_iShowItemIndex ? TRUE : FALSE);	
			
			if (innerGetFocus())
			{
				if ( m_CCPSkin == NULL && m_eScrollType == CCP_FOCUS_SCROLL && m_iCurCCPFocus == (i + m_iShowItemIndex) )
				{
					bFocus = TRUE;
				}
			}
			
			pShowItem->pComponent->setSelected(bSelect);
			pShowItem->pComponent->setTouchFocus(bFocus);
			pShowItem->pComponent->refresh();
			
			m_pView->drawItem(this, pShowItem->pObject, pShowItem->pComponent, bSelect, bFocus, !pShowItem->pComponent->getEnable());
		}
		
		drawListCCPFocs();
	}

	void CCtrlList::showBubble()
	{
		if (m_pBubble == NULL)
		{
			return;
		}
		
		if (m_bControlBubbleVisible)
		{
			if (!m_bShowBubble)
			{
				return;
			}
			
			if (!m_pBubble->getVisible())
			{
				m_pBubble->setVisible(TRUE);
			}
		}
		
		UINT uiBubbleIndex = calculateShowItemIndex(m_scrollRect.getY() + m_scrollRect.getHeight() / 2);
		if (m_iCurBubble != uiBubbleIndex)
		{
			m_iCurBubble = uiBubbleIndex;
			
			if ((INT)m_aryShowItem.size() <= m_iCurBubble)
			{
				return;
			}
			
			// use bubble index to indicate the CCP focus index at present.
			if (m_eScrollType == LIST_ITEM_SCROLL)
			{
				m_iCurCCPFocus = m_iCurBubble;
			}
			
			m_pView->drawBubble(this, m_aryShowItem[m_iCurBubble]->pObject, m_pBubble);			
		}
	}

	void CCtrlList::hideBubble()
	{
		m_bShowBubble = FALSE;
		
		if (m_bControlBubbleVisible)
		{
			m_iCurBubble = -1;
			
			if (m_pBubble != NULL)
			{
				m_pBubble->setVisible(FALSE);
			}
		}
	}

	void CCtrlList::resetScrollbar(BOOLEAN bMaintainPos)
	{
		if (m_pScrollbar != NULL)
		{
			INT iMax = getTotalShowItemHeight();
			m_pScrollbar->getModel()->setScrollInfo(0, 
													iMax > 0 ? iMax - 1 : 0, 
													bMaintainPos ? m_pScrollbar->getModel()->getPosition() : 0,
													getPreferredSize().getY());
			m_pScrollbar->refresh();
			
			if ((INT)getTotalShowItemHeight() <= getPreferredSize().getY())
			{
				m_pScrollbar->setVisible(FALSE);
			}
			else
			{
				if ( m_bControlScrollbarVisible)
				{
					m_pScrollbar->setVisible(FALSE);
				}
				else
				{
					m_pScrollbar->setVisible(TRUE);
				}
			}
		}
	}

	UINT CCtrlList::getTotalShowItemHeight()
	{
		if (m_aryShowItem.size() == 0)
		{
			return 0;
		}
		
		tagShowItem* pShowItem = m_aryShowItem[m_aryShowItem.size() - 1];
		
		return pShowItem->pComponent->getY() + pShowItem->pComponent->getPreferredSize().getY() + m_iItemSpace;
	}

	BOOLEAN CCtrlList::needVerifyShowInfo(INT iPos)
	{
		INT iItemIndex = calculateShowItemIndex(iPos);
		
		if (   iItemIndex <= m_iShowItemIndex
			|| iItemIndex >= m_iShowTopIndex + 1)
		{
			m_iShowTopIndex = iItemIndex;
			return TRUE;
		}
		
		return FALSE;
	}

	void CCtrlList::verifyShowInfo()  //need further check and consider
	{
		m_iShowItemIndex = m_iShowTopIndex - 1;
		if (m_iShowItemIndex < 0)
		{
			m_iShowItemIndex = 0;
		}		
		
		m_uiShowItemCount = 0;
		if (m_aryShowItem.size() != 0)
		{
			INT iHeight = 0;
			for (INT i = m_iShowTopIndex; i < (INT)m_aryShowItem.size(); i++)
			{
				m_uiShowItemCount++;

				iHeight += m_aryShowItem[i]->pComponent->getPreferredSize().getY() + m_iItemSpace;
				
				if (iHeight >= getPreferredSize().getY())
				{
					break;
				}
			}
			
			m_uiShowItemCount += (m_iShowTopIndex - m_iShowItemIndex) + 1;
			
			if (m_iShowItemIndex + m_uiShowItemCount > m_aryShowItem.size())
			{
				m_uiShowItemCount = m_aryShowItem.size() - m_iShowItemIndex;
			}
		}
		
		{
//			for (INT j = 0; j < (INT)m_aryShowComponent.size(); )
// 			{
// 				CComponent* pComponent = m_aryShowComponent[j]->pComponent;
// 				if (pComponent != NULL)
// 				{
// 					INT m = 0;
// 					for (; m < (INT)m_uiShowItemCount; m++)
// 					{
// 						if (pComponent == m_aryShowItem[m + m_iShowItemIndex]->pComponent)
// 						{
// 							break;
// 						}
// 					}
// 					
// 					//if ( m == m_uiShowItemCount )
// 					//{
// 					//	pComponent->destroySkin();
// 					//}
// 				}
// 				
// 				m_aryShowComponent.deleteItemByIndex(j);
// 			}

			m_aryShowComponent.clear();
		}
		
		for (INT k = 0; k < (INT)m_uiShowItemCount; k++)
		{
			CComponent* pComponentShow = m_aryShowItem[k + m_iShowItemIndex]->pComponent;
			pComponentShow->setParent(this);
			pComponentShow->refresh();
			m_aryShowComponent.addItem(new tagShowComponent(pComponentShow));
		}
	}

	UINT CCtrlList::calculateShowItemIndex(INT iPos)
	{
		if (m_aryShowItem.size() == 0)
		{
			return 0;
		}			
		
		for (UINT i = 0; i < m_aryShowItem.size(); i++)
		{
			if (   m_aryShowItem[i]->pComponent->getY() <= iPos
				&& iPos < m_aryShowItem[i]->pComponent->getY() + m_aryShowItem[i]->pComponent->getPreferredSize().getY() + m_iItemSpace)
			{
				return i;
			}
		}
		
		if (iPos < 0)
		{
			return 0;
		}
		else
		{
			return m_aryShowItem.size() - 1;
		}
	}

	void CCtrlList::checkSelectChanged()
	{
		INT iOldCurSel = m_iCurSel;
		
		if (m_iCurTouchFocus != -1)
		{
			//if (!m_model.getItemDisable(m_iCurTouchFocus))
			{
				m_iCurSel = m_iCurTouchFocus;
			}
		}
		
		m_iCurTouchFocus = -1;
		showItemState();
		
		if (iOldCurSel != m_iCurSel)
		{
			dispatchEvent(CEventList::SELECT_CHANGED);
		}
	}

	INT CCtrlList::getFirstCCPItemIndex()
	{
		if (m_aryShowItem.size() == 0)
		{
			return -1;
		}

		if ( m_iCurCCPFocus == -1 )
		{
			return m_iShowTopIndex; 
		}
		
		return m_iCurCCPFocus;
	}

	INT CCtrlList::getLastCCPItemIndex()
	{
		if (m_aryShowItem.size() == 0)
		{
			return -1;
		}
		
		if (m_iCurCCPFocus != -1)
		{
			return m_iCurCCPFocus;
		}
		
		for ( INT i = (INT)m_uiShowItemCount - 1; i >= 0; i-- )
		{
			if (m_aryShowItem[m_iShowItemIndex + i]->pComponent->getY() < m_scrollRect.getY() + (INT)m_scrollRect.getHeight())
			{
				return m_iShowItemIndex + i;
			}
		}
		
		return -1;
	}

	void CCtrlList::adjustCCPFocusDisplayPos()
	{
		if ( m_iCurCCPFocus == -1 || m_aryShowItem.size() == 0)
		{
			return;
		}
		
		if ( m_eScrollType == CCP_FOCUS_SCROLL )
		{
			if (m_iCurCCPFocus >= (INT)m_aryShowItem.size())
			{
				m_iCurCCPFocus = 0;
			}
			
			if ( m_iCurCCPFocus < m_iShowTopIndex )
			{
				showTopItem(m_iCurCCPFocus);
				return;
			}
			
			INT i = 0;
			while ( m_iCurCCPFocus - i >= 0 && m_aryShowItem[m_iCurCCPFocus - i]->pComponent->getY() + DISTANCE_OFFSET > m_scrollRect.getY() + (INT)m_scrollRect.getHeight() )
			{
				i++;
			}
			
			if (i > 0)
			{
				showTopItem( m_iShowTopIndex + i );
				return;
			}
			
			littleRefresh();
		}
		else
		{
			showTopItem(m_iCurCCPFocus - m_uiCCPFocusFixPos);
		}
	}

	void CCtrlList::drawListCCPFocs()
	{
		if (   !innerGetFocus() 
			|| m_iCurCCPFocus == -1 
			|| m_iCurCCPFocus >= (INT)m_aryShowItem.size()
			|| m_CCPSkin == NULL   )
		{
			return;
		}
		
		CComponent* pComponent = m_aryShowItem[m_iCurCCPFocus]->pComponent;
		
		m_CCPSkin->setX(pComponent->getX() - m_scrollRect.getX() - ((INT)(m_CCPSkin->getWidth()) - (INT)(pComponent->getPreferredSize().getX())) / 2);
		m_CCPSkin->setY(pComponent->getY() - m_scrollRect.getY() - ((INT)(m_CCPSkin->getHeight()) - (INT)(pComponent->getPreferredSize().getY())) / 2);
	}

	CComponent* CCtrlList::getCCPFocusComponent()
	{
		if (   m_iCurCCPFocus == -1
			|| m_iCurCCPFocus >= (INT)m_aryShowItem.size())
		{
			return NULL;
		}
		else
		{
			return m_aryShowItem[m_iCurCCPFocus]->pComponent;
		}
	}

	BOOLEAN CCtrlList::moveCCPFocusIndexDecrease(ECCPOperation eCCPOperation,UINT uiStep)
	{
		if ( eCCPOperation != CComponent::CCP_MOVE_UP && eCCPOperation != CComponent::CCP_ROTATE_LEFT )
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
		
		if ( innerGetFocus() )
		{
			checkNeedMorePage();

			if (m_iCurCCPFocus <= 0)
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}
				
			if ( m_eScrollType == CCP_FOCUS_SCROLL )
			{
				m_iCurCCPFocus--;
			}
			else
			{			 
				if (m_iShowTopIndex <= 0)
				{
					return canMoveFocusOut() ? FALSE : TRUE;
				}
				
				m_iCurCCPFocus--;
			}
			
			adjustCCPFocusDisplayPos();
			dispatchEvent(CEventList::CCP_FOCUS_CHANGED);
			setDirty();
			return TRUE;
		}
		else
		{
			CComponent* pComponent = getCCPFocusComponent();
			if (pComponent == NULL)
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}
			
			if ( eCCPOperation == CComponent::CCP_MOVE_UP )
			{
				pComponent->moveFocusToUp();
			}
			else
			{
				pComponent->turnFocusToLeft(uiStep);
			}
			
			return TRUE;
		}
	}

	BOOLEAN CCtrlList::moveCCPFocusIndexIncrease(ECCPOperation eCCPOperation,UINT uiStep)
	{
		if ( eCCPOperation != CComponent::CCP_MOVE_DOWN && eCCPOperation != CComponent::CCP_ROTATE_RIGHT )
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
		
		if ( innerGetFocus() )
		{
			checkNeedMorePage();

			if (m_iCurCCPFocus >= (INT)m_aryShowItem.size() - 1)
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}
			
			if ( m_eScrollType == CCP_FOCUS_SCROLL )
			{
				m_iCurCCPFocus++;
			}
			else
			{
				// if it is in the last page, list item does not scroll
				if ( m_aryShowItem[m_aryShowItem.size()-1]->pComponent->getY() < (INT)(m_scrollRect.getY() + m_scrollRect.getHeight() - DISTANCE_OFFSET) )
				{
					return canMoveFocusOut() ? FALSE : TRUE;
				}
				m_iCurCCPFocus++;
			}
			
			adjustCCPFocusDisplayPos();
			dispatchEvent(CEventList::CCP_FOCUS_CHANGED);
			setDirty();
			return TRUE;
		}
		else
		{
			CComponent* pComponent = getCCPFocusComponent();
			if (pComponent == NULL)
			{
				return FALSE;
			}
			
			if ( eCCPOperation == CComponent::CCP_MOVE_DOWN )
			{
				pComponent->moveFocusToDown();
			}
			else
			{
				pComponent->turnFocusToRight(uiStep);
			}
			
			return TRUE;
		}
	}

	void CCtrlList::switchCCPFocusForTouch()
	{
		if (!ENV::getCCPBindTouch())
		{
			return;
		}
		
		if (m_eScrollType == LIST_ITEM_SCROLL)
		{
			ENV::switchCCPFocus(this);
		}
		else
		{
			if (m_iCurCCPFocus == m_iCurTouchFocus)
			{
				if (innerGetFocus())
				{
					// do nothing
				}
				else
				{
					ENV::switchCCPFocus(this);
				}
			}
			else
			{
				if (innerGetFocus())
				{
					m_iCurCCPFocus = m_iCurTouchFocus;
					setFocus();
				}
				else
				{
					m_iCurCCPFocus = m_iCurTouchFocus;
					ENV::switchCCPFocus(this);
				}
			}
		}
	}

	void CCtrlList::startEnterAnimation()
	{
		if ( m_pAmtEnter != NULL && m_bIsEnterAnimationOn == TRUE )
		{
			tagAmtData_Enter data;

			if (m_pAmtEnter->prepareAmtData(&data))
			{
				m_pAmtEnter->play();
				lock();
				//m_bIsEnterAnimationOn = FALSE;
			}
		}
	}

	void CCtrlList::stopEnterAnimation()
	{
		if ( m_pAmtEnter != NULL )
		{
			m_pAmtEnter->stop();
			
			unlock();
		}
	}

	void CCtrlList::startElasticityAnimation(INT iStartValue, INT iEndValue)
	{
		if ( m_pAmtElasticity != NULL && m_bIsElasticityOn == TRUE)
		{
			tagAmtData_Elasiticity data;
			data.iStartValue = iStartValue;
			data.iEndValue = iEndValue;

			if (m_pAmtElasticity->prepareAmtData(&data))
			{
				m_pAmtElasticity->play();
				lock();
			}
		}
		else
		{
			if (m_bControlScrollbarVisible)
			{
				m_pScrollbar->setVisible(FALSE);
			}

			dispatchEvent(CEventList::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlList::stopElasticityAnimation()
	{
		if ( m_pAmtElasticity != NULL )
		{
			m_pAmtElasticity->stop();

			hideBubble();
			
			unlock();

			if (m_bControlScrollbarVisible)
			{
				m_pScrollbar->setVisible(FALSE);
			}

			dispatchEvent(CEventList::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlList::startFlingAnimation(INT iStartValue, INT iSpeed)
	{
		if ( m_pAmtFling != NULL && m_bIsFlingOn == TRUE )
		{
			tagAmtData_Fling data;
			data.iStartValue = iStartValue;
			data.iSpeed = iSpeed;

			if (m_pAmtFling->prepareAmtData(&data))
			{
				m_pAmtFling->play();
				lock();
			}
		}
		else
		{
			if (m_bControlScrollbarVisible)
			{
				m_pScrollbar->setVisible(FALSE);
			}

			dispatchEvent(CEventList::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlList::stopFlingAnimation()
	{
		if ( m_pAmtFling != NULL )
		{
			m_pAmtFling->stop();

			hideBubble();
		
			unlock();

			if (m_bControlScrollbarVisible)
			{
				m_pScrollbar->setVisible(FALSE);
			}

			dispatchEvent(CEventList::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlList::startTouchAnimation(CComponent* pCom)
	{
		if( m_pAmtTouch != NULL && m_bIsTouchAnimationOn == TRUE)
		{
			tagAmtData_Touch data;
			data.pComponent = pCom;

			if (m_pAmtTouch->prepareAmtData(&data))
			{
				m_pAmtTouch->play();
			}
		}
	}

	void CCtrlList::stopTouchAnimation()
	{
		if ( m_pAmtTouch != NULL )
		{
			m_pAmtTouch->stop();
		}
	}

	void CCtrlList::onScrollbarPosChanged(CComponent *pTarget)
	{
		INT iPos = m_pScrollbar->getModel()->getPosition();

		CCtrlScrollbar::EPosChanged eType = m_pScrollbar->getPosChangedType();

// 		if (iPos == 0)
// 		{
// 			if (eType == CCtrlScrollbar::PC_LINE_UP || eType == CCtrlScrollbar::PC_CONTINUE_LINE_UP)
// 			{
// 				if (m_scrollRect.getY() == 0)
// 				{
// 					if (m_model.getReachToHead())
// 					{
// 						iPos = m_pScrollbar->getModel()->getMaxPosition();
// 						m_pScrollbar->getModel()->setPosition(iPos);
// 						m_pScrollbar->refresh();
// 					}
// 					else
// 					{
// 						m_bInAppendPrevPageMode = TRUE;
// 						dispatchEvent(CEventList::WANT_PREV_PAGE);
// 						return;
// 					}
// 				}
// 			}
// 		}
// 		else if (iPos == m_pScrollbar->getModel()->getMaxPosition())
// 		{
// 			if (eType == CCtrlScrollbar::PC_LINE_DOWN || eType == CCtrlScrollbar::PC_CONTINUE_LINE_DOWN)
// 			{
// 				if (m_scrollRect.getY() == m_pScrollbar->getModel()->getMaxPosition())
// 				{
// 					if (m_model.getReachToTail())
// 					{
// 						iPos = 0;
// 						m_pScrollbar->getModel()->setPosition(iPos);
// 						m_pScrollbar->refresh();
// 					}
// 					else
// 					{
// 						m_bInAppendNextPageMode = TRUE;
// 						dispatchEvent(CEventList::WANT_NEXT_PAGE);
// 						return;
// 					}
// 				}
// 			}
// 		}
			
		if (needVerifyShowInfo(iPos))
		{
			verifyShowInfo();
			showItemState();
		}
			
		showItems(iPos);
		
		if (   eType == CCtrlScrollbar::PC_SLIDER
			|| eType == CCtrlScrollbar::PC_CONTINUE_LINE_UP
			|| eType == CCtrlScrollbar::PC_CONTINUE_LINE_DOWN   )
		{
			m_bShowBubble = TRUE;
		}
		else
		{
			m_bShowBubble = FALSE;
		}
		
		showBubble();

		if (eType != CCtrlScrollbar::PC_SLIDER)
		{
			checkNeedMorePage();
		}
	}

	void CCtrlList::onScrollbarPosContinueChangeEnded(CComponent *pTarget)
	{
		checkNeedMorePage();

		hideBubble();
	}
}