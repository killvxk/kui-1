#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlTree::CCtrlTree()
		: m_pView(NULL)		
		, m_pScrollbar(NULL)
		, m_bControlScrollbarVisible(FALSE)		
		, m_bFirstBuild(TRUE)
		, m_iCurSel(-1)
		, m_iCurTouchFocus(-1)
		, m_iCurCCPFocus(-1)
		, m_iShowTopIndex(0)
		, m_iShowItemIndex(0)
		, m_uiShowItemCount(0)
		, m_iLeftSpace(0)
		, m_iShowLevel(-1)
		, m_iTouchBeginPosY(0)
		, m_iShowBeginPosY(0)
		, m_iShowCurrentPosY(0)
		, m_bDirectionDown(FALSE)
		, m_bCanScrollByTouchMove(TRUE)		
		, m_bCanMoveFocusByUpDown(FALSE)
		, m_bOuterChangedData(FALSE)
	{
		m_model.setComponent(this);
		setBaseView(&m_view);	
	}

	CCtrlTree::~CCtrlTree()
	{
		;
	}

	BEGIN_EVENT_MAP(CCtrlTree)
		CONTROL_EVENT_HANDLER(m_pScrollbar, CEventScrollbar::POS_CHANGED, onScrollbarPosChanged)
		CONTROL_EVENT_HANDLER(m_pScrollbar, CEventScrollbar::POS_CONTINUE_CHANGE_ENDED, onScrollbarPosContinueChangeEnded)
	END_EVENT_MAP_2

	CModelTree* CCtrlTree::getModel()
	{
		return &m_model;
	}

	void CCtrlTree::setView(IViewTree* pView)
	{
		m_pView = pView;
	}

	void CCtrlTree::setAssociateScrollbar(CCtrlScrollbar* pScrollBar, BOOLEAN bControlScrollbarVisible, BOOLEAN bMovePageWhenClickUpDownArrow)
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

	void CCtrlTree::setCurSel(INT iValue)
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

	void CCtrlTree::setCurTouchFocus(INT iValue)
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

	void CCtrlTree::setCurCCPFocus(INT iValue)
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

	INT CCtrlTree::getCurCCPFocus()
	{
		if (hasFocus())
		{
			return m_iCurCCPFocus;
		}
		
		return -1;
	}

	Object* CCtrlTree::getCCPFocusedItem()
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

	CComponent* CCtrlTree::getItemComponent(Object* pObj)
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

	void CCtrlTree::showTopItem(UINT uiIndex)
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
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->getModel()->setPosition(iPos);
			m_pScrollbar->refresh();
		}
	}

	void CCtrlTree::showItemToLevel(UINT uiLevel)
	{
		m_iShowLevel = (INT)uiLevel;

		if (m_bFirstBuild == FALSE)
		{
			expandShowItemToLevel(uiLevel);

			resetScrollbar();

			showTopItem(m_iShowTopIndex);
		}
	}

	void CCtrlTree::lock()
	{
		CComponent::lock();
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->lock();
		}
	}

	void CCtrlTree::unlock()
	{
		CComponent::unlock();
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->unlock();
		}
	}

	void CCtrlTree::refresh()
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

			if (m_iShowLevel != -1)
			{
				showItemToLevel(m_iShowLevel);
			}
		}
	}

	void CCtrlTree::littleRefresh()
	{
		CComponent::littleRefresh();
		
		showItemState();
	}

	void CCtrlTree::destroySkin()
	{
		destroyShowItemList();
		
		CComponent::destroySkin();
	}

	void CCtrlTree::destroy()
	{
		if (isDestroyed())
		{
			return;
		}

		CComponent::destroy();
	}

	void CCtrlTree::buildRenderTree(CRenderGroup &renderGroup)
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
			if (pComponent != NULL && pComponent->getPreferredSize().getY() > 0)
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

	BOOLEAN	CCtrlTree::canSetFocus()
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

	void CCtrlTree::setFocus(ECCPOperation eCCPOperation)
	{
		innerSetFocus(TRUE);		

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
		
		adjustCCPFocusDisplayPos();
		
		dispatchEvent(CEventList::CCP_FOCUS_CHANGED);
	}

	void CCtrlTree::releaseFocus()
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

	BOOLEAN CCtrlTree::hasFocus()
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

	BOOLEAN CCtrlTree::moveFocusToLeft()
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

	BOOLEAN CCtrlTree::moveFocusToRight()
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

	BOOLEAN CCtrlTree::moveFocusToUp()
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

	BOOLEAN CCtrlTree::moveFocusToDown()
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

	BOOLEAN CCtrlTree::turnFocusToLeft(UINT uiStep)
	{
		return moveCCPFocusIndexDecrease(CComponent::CCP_ROTATE_LEFT, uiStep);
	}

	BOOLEAN CCtrlTree::turnFocusToRight(UINT uiStep)
	{
		return moveCCPFocusIndexIncrease(CComponent::CCP_ROTATE_RIGHT, uiStep);
	}

	void CCtrlTree::doFocusEnterPress()
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

	void CCtrlTree::doFocusEnterRelease()
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
						if (m_iCurCCPFocus != -1)
						{
							if (m_aryShowItem[m_iCurCCPFocus]->bExpand)
							{
								collapseShowItem(m_iCurCCPFocus);
								resetScrollbar();
								showTopItem(m_iShowTopIndex);				
							}
							else
							{
								expandShowItem(m_iCurCCPFocus);
								resetScrollbar();
								showTopItem(m_iShowTopIndex);
							}
						}

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

	BOOLEAN CCtrlTree::doFocusBack()
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

	void CCtrlTree::onTouchPress(INT iX, INT iY)
	{		
		m_iTouchBeginPosY = iY;
		m_iShowBeginPosY = m_scrollRect.getY();
		m_iShowCurrentPosY = m_scrollRect.getY();
		
		for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
		{
			tagShowItem* pShowItem = m_aryShowItem[i + m_iShowItemIndex];

			if (pShowItem->pComponent->getPreferredSize().getY() == 0)
			{
				continue;
			}
			
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
				;
			}
			
			//if (!m_model.getItemDisable(showItem.objItem))
			{
				m_bOuterChangedData = FALSE;
				pShowItem->pComponent->onTouchPress(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}

			break;
		}
	}

	void CCtrlTree::onTouchPressRelease(INT iX, INT iY)
	{
		if (m_iCurTouchFocus >= 0)
		{
			switchCCPFocusForTouch();		
			
			//if (!m_model.getItemDisable(m_aryShowItem[m_iCurTouchFocus]->pObject))
			{
				m_bOuterChangedData = FALSE;
				m_aryShowItem[m_iCurTouchFocus]->pComponent->onTouchPressRelease(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}
		}
		
		if (!m_bOuterChangedData)
		{
			if (m_iCurTouchFocus != -1)
			{
				if (m_aryShowItem[m_iCurTouchFocus]->bExpand)
				{
					collapseShowItem(m_iCurTouchFocus);
					resetScrollbar();
					showTopItem(m_iShowTopIndex);				
				}
				else
				{
					expandShowItem(m_iCurTouchFocus);
					resetScrollbar();
					showTopItem(m_iShowTopIndex);
				}
			}

			checkSelectChanged();
		}
	}

	void CCtrlTree::onTouchLongPress(INT iX, INT iY)
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

	void CCtrlTree::onTouchContinuePress(INT iX, INT iY)
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

	void CCtrlTree::onTouchLongPressRelease(INT iX, INT iY)
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

	void CCtrlTree::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
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
				if ( iPos < 0 )
				{
					m_bDirectionDown = FALSE;
					return;
				}
			}
			else if ((INT)uiTotalShowItemHeight - getPreferredSize().getY() <= m_scrollRect.getY())
			{
				iPos = m_iShowBeginPosY - iDeltaY * ((INT)uiTotalShowItemHeight - m_scrollRect.getY()) / getPreferredSize().getY();
				
				if ( iPos > (INT)uiTotalShowItemHeight - getPreferredSize().getY() )
				{
					m_bDirectionDown = TRUE;
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
		}
	}

	void CCtrlTree::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
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
		
		UINT uiTotalShowItemHeight = getTotalShowItemHeight();
		
		if ((INT)uiTotalShowItemHeight <= getPreferredSize().getY())
		{
			return;
		}
		
		if (m_scrollRect.getY() < 0)
		{
			;
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
		}
		else
		{

		}
	}

	void CCtrlTree::onTouchDrag(INT iX, INT iY)
	{
		;
	}

	void CCtrlTree::onTouchDragRelease(INT iX, INT iY)
	{
		if (m_iCurTouchFocus >= 0)
		{
			switchCCPFocusForTouch();
			
			//if (!m_model.getItemDisable(m_aryShowItem[m_iCurTouchFocus]->pObject))
			{
				m_aryShowItem[m_iCurTouchFocus]->pComponent->onTouchDragRelease(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}			
			
			m_iCurTouchFocus = -1;
			showItemState();
		}
	}

	BOOLEAN CCtrlTree::contains(Object *pTarget)
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

	void CCtrlTree::updateViewForFirstBuild()
	{
		m_scrollRect.setWidth(getPreferredSize().getX());
		m_scrollRect.setHeight(getPreferredSize().getY());

		m_iCurTouchFocus = -1;
		m_iShowTopIndex = 0;

		verifyShowInfo();
		showItemState();

		showItems(0);
	}

	void CCtrlTree::buildShowItemList()
	{
		destroyShowItemList();
		
		UINT uiItemCount = (INT)m_model.getItemCount();
		
		INT iY = 0;
		for (UINT i = 0; i < uiItemCount; i++)
		{
			iY += insertShowItem(i, m_model.getItemNodeAt(i), iY);
		}	
	}

	INT CCtrlTree::insertShowItem(INT iIndex, CModelTree::tagItemNode *pItemNode, INT iPosY)
	{
		tagShowItem* pShowItem = new tagShowItem(pItemNode->ptrObject, m_pView->getItemComponent(this, pItemNode->ptrObject), pItemNode->uiLevel);
		m_aryShowItem.insertItem(iIndex, pShowItem);

		if (!pShowItem->pComponent->isContainer())
		{
			pShowItem->pComponent->notAcceptFocus();
		}

		pShowItem->pComponent->setX(m_iLeftSpace * pItemNode->uiLevel);
		pShowItem->pComponent->setY(iPosY);
		return pShowItem->pComponent->getPreferredSize().getY();
	}

	void CCtrlTree::destroyShowItemList()
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

	void CCtrlTree::expandShowItem(INT iIndex)
	{
		INT iCount = (INT)(INT)m_aryShowItem.size();

		if (iIndex < 0 || iIndex >= iCount)
		{
			return;
		}

		m_aryShowItem[iIndex]->bExpand = TRUE;

		UINT uiLevle = m_aryShowItem[iIndex]->uiLevel;

		tagShowItem *pShowItem = NULL;

		for (INT i = iIndex + 1; i < iCount; i++)
		{
			pShowItem = m_aryShowItem[i];

			if (pShowItem->uiLevel <= uiLevle)
			{
				break;
			}

			if (pShowItem->uiLevel == uiLevle + 1)
			{
				pShowItem->pComponent->setPreferredSize(pShowItem->uiRawWidth, pShowItem->uiRawHeight);
			}
		}

		resetShowItemPos();
	}

	void CCtrlTree::expandShowItemToLevel(UINT uiLevel)
	{
		INT iCount = (INT)(INT)m_aryShowItem.size();

		tagShowItem *pShowItem = NULL;

		for (INT i = 0; i < iCount; i++)
		{
			pShowItem = m_aryShowItem[i];

			if (pShowItem->uiLevel < uiLevel)
			{				
				pShowItem->pComponent->setPreferredSize(pShowItem->uiRawWidth, pShowItem->uiRawHeight);
				pShowItem->bExpand = TRUE;
			}
			else if (pShowItem->uiLevel == uiLevel)
			{				
				pShowItem->pComponent->setPreferredSize(pShowItem->uiRawWidth, pShowItem->uiRawHeight);
				pShowItem->bExpand = FALSE;
			}
			else
			{
				pShowItem->pComponent->setPreferredSize(pShowItem->uiRawWidth, 0);
				pShowItem->bExpand = FALSE;
			}
		}

		resetShowItemPos();
	}

	void CCtrlTree::collapseShowItem(INT iIndex)
	{
		INT iCount = (INT)(INT)m_aryShowItem.size();

		if (iIndex < 0 || iIndex >= iCount)
		{
			return;
		}

		m_aryShowItem[iIndex]->bExpand = FALSE;

		UINT uiLevle = m_aryShowItem[iIndex]->uiLevel;

		tagShowItem *pShowItem = NULL;

		for (INT i = iIndex + 1; i < iCount; i++)
		{
			pShowItem = m_aryShowItem[i];

			if (pShowItem->uiLevel <= uiLevle)
			{
				break;
			}

			pShowItem->pComponent->setPreferredSize(pShowItem->uiRawWidth, 0);
			pShowItem->bExpand = FALSE;
		}

		resetShowItemPos();
	}

	void CCtrlTree::resetShowItemPos()
	{
		INT iCount = (INT)(INT)m_aryShowItem.size();

		CComponent *pComponent = NULL;
		INT iY = 0;

		for (INT i = 0; i < iCount; i++)
		{
			pComponent = m_aryShowItem[i]->pComponent;

			pComponent->setY(iY);

			iY += pComponent->getPreferredSize().getY();
		}
	}

	void CCtrlTree::showItems(INT iTop)
	{
		m_scrollRect.setY(iTop);
		drawListCCPFocs();

		if (m_pScrollbar != NULL && m_bControlScrollbarVisible == FALSE)
		{
			tagShowItem *pShowItem = NULL;

			if (m_iShowItemIndex < (INT)m_aryShowItem.size())
			{
				pShowItem = m_aryShowItem[m_iShowItemIndex];
				m_pScrollbar->getModel()->setPreviousStep(pShowItem->pComponent->getPreferredSize().getY());
			}

			if (m_iShowTopIndex < (INT)m_aryShowItem.size())
			{
				pShowItem = m_aryShowItem[m_iShowTopIndex];
				m_pScrollbar->getModel()->setNextStep(pShowItem->pComponent->getPreferredSize().getY());
			}
		}

		setDirty();
	}

	void CCtrlTree::showItemState()
	{
		tagShowItem *pShowItem = NULL;
		
		for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
		{
			pShowItem = m_aryShowItem[i + m_iShowItemIndex];
			
			BOOLEAN bSelect = (m_iCurSel == i + m_iShowItemIndex ? TRUE : FALSE);	
			BOOLEAN bFocus = (m_iCurTouchFocus == i + m_iShowItemIndex ? TRUE : FALSE);	
			
			if (innerGetFocus())
			{
				if ( m_CCPSkin == NULL && m_iCurCCPFocus == (i + m_iShowItemIndex) )
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

	void CCtrlTree::resetScrollbar(BOOLEAN bMaintainPos)
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

	UINT CCtrlTree::getTotalShowItemHeight()
	{
		if (m_aryShowItem.size() == 0)
		{
			return 0;
		}
		
		tagShowItem* pShowItem = m_aryShowItem[m_aryShowItem.size() - 1];
		
		return pShowItem->pComponent->getY() + pShowItem->pComponent->getPreferredSize().getY();
	}

	BOOLEAN CCtrlTree::needVerifyShowInfo(INT iPos)
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

	void CCtrlTree::verifyShowInfo()  //need further check and consider
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

				iHeight += m_aryShowItem[i]->pComponent->getPreferredSize().getY();
				
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

	UINT CCtrlTree::calculateShowItemIndex(INT iPos)
	{
		if (m_aryShowItem.size() == 0)
		{
			return 0;
		}			
		
		for (UINT i = 0; i < m_aryShowItem.size(); i++)
		{
			if (   m_aryShowItem[i]->pComponent->getY() <= iPos
				&& iPos < m_aryShowItem[i]->pComponent->getY() + m_aryShowItem[i]->pComponent->getPreferredSize().getY())
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

	void CCtrlTree::checkSelectChanged()
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

	INT CCtrlTree::getFirstCCPItemIndex()
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

	INT CCtrlTree::getLastCCPItemIndex()
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

	void CCtrlTree::adjustCCPFocusDisplayPos()
	{
		if ( m_iCurCCPFocus == -1 || m_aryShowItem.size() == 0)
		{
			return;
		}
		
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

	void CCtrlTree::drawListCCPFocs()
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

	CComponent* CCtrlTree::getCCPFocusComponent()
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

	BOOLEAN CCtrlTree::moveCCPFocusIndexDecrease(ECCPOperation eCCPOperation,UINT uiStep)
	{
		if ( eCCPOperation != CComponent::CCP_MOVE_UP && eCCPOperation != CComponent::CCP_ROTATE_LEFT )
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
		
		if ( innerGetFocus() )
		{
			if (m_iCurCCPFocus <= 0)
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}
				
			m_iCurCCPFocus--;
			while (m_iCurCCPFocus > 0 && m_aryShowItem[m_iCurCCPFocus]->pComponent->getPreferredSize().getY() == 0)
			{
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

	BOOLEAN CCtrlTree::moveCCPFocusIndexIncrease(ECCPOperation eCCPOperation,UINT uiStep)
	{
		if ( eCCPOperation != CComponent::CCP_MOVE_DOWN && eCCPOperation != CComponent::CCP_ROTATE_RIGHT )
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
		
		if ( innerGetFocus() )
		{
			if (m_iCurCCPFocus >= (INT)m_aryShowItem.size() - 1)
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}
			
			INT iCurCCPFocus = m_iCurCCPFocus;

			iCurCCPFocus++;
			while (iCurCCPFocus <= (INT)m_aryShowItem.size() - 1 && m_aryShowItem[iCurCCPFocus]->pComponent->getPreferredSize().getY() == 0)
			{
				iCurCCPFocus++;
			}

			if (iCurCCPFocus >= (INT)m_aryShowItem.size())
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}
			
			m_iCurCCPFocus = iCurCCPFocus;

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

	void CCtrlTree::switchCCPFocusForTouch()
	{
		if (!ENV::getCCPBindTouch())
		{
			return;
		}
		
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

	void CCtrlTree::onScrollbarPosChanged(CComponent *pTarget)
	{
		INT iPos = m_pScrollbar->getModel()->getPosition();

		CCtrlScrollbar::EPosChanged eType = m_pScrollbar->getPosChangedType();

		if (needVerifyShowInfo(iPos))
		{
			verifyShowInfo();
			showItemState();
		}
			
		showItems(iPos);
	}

	void CCtrlTree::onScrollbarPosContinueChangeEnded(CComponent *pTarget)
	{
		;
	}
}