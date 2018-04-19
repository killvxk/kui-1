#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlTable::CCtrlTable()
		: m_pView(NULL)
		, m_uiCellWidth(100)
		, m_uiCellHeight(100)
		, m_uiRow(0)
		, m_uiColumn(0)
		, m_pScrollbar(NULL)
		, m_bControlScrollbarVisible(FALSE)
		, m_iCurSel(-1)
		, m_iCurTouchFocus(-1)
		, m_iCurCCPFocus(-1)
		, m_iCurHoverFocus(-1)
		, m_iCurTouchFocusR(-1)
		, m_iShowTopIndex(0)
		, m_iShowItemIndex(0)
		, m_uiShowItemCount(0)
		, m_uiItemsPerPage(0)
		, m_bInAppendMode(FALSE)
		, m_iTouchBeginPosY(0)
		, m_iShowBeginPosY(0)
		, m_iShowCurrentPosY(0)
		, m_bDirectionDown(FALSE)
		, m_bOuterChangedData(FALSE)
		, m_bCanMoveFocusByUpDown(FALSE)
		, m_bCanMoveFocusByLeftRight(FALSE)
		, m_bIsEnterAnimationOn(FALSE)
		, m_bIsElasticityOn(FALSE)
		, m_bIsFlingOn(FALSE)
		, m_bIsTouchAnimationOn(FALSE)
		, m_bCanWheel(TRUE)
	{
		m_model.setComponent(this);
		setBaseView(&m_view);

		m_viewState.pObjSelectedItem = NULL;
		m_viewState.pObjCCPFocusItem = NULL;
		m_viewState.iShowTopIndex = -1;

		m_pAmtEnter = new CTableAmtEnterInterdigitation();
		m_pAmtEnter->m_pTable = this;

		m_pAmtElasticity = new CTableAmtElasticity();
		m_pAmtElasticity->m_pTable = this;

		m_pAmtFling = new CTableAmtFling();
		m_pAmtFling->m_pTable = this;

		m_pAmtTouch = new CTableAmtTouchShake();
		m_pAmtTouch->m_pTable = this;		
	}

	CCtrlTable::~CCtrlTable()
	{
		
	}

	BEGIN_EVENT_MAP(CCtrlTable)
		CONTROL_EVENT_HANDLER(m_pScrollbar, CEventScrollbar::POS_CHANGED, onScrollbarPosChanged)
		CONTROL_EVENT_HANDLER(m_pScrollbar, CEventScrollbar::POS_CONTINUE_CHANGE_ENDED, onScrollbarPosContinueChangeEnded)
	END_EVENT_MAP_2

	CModelTable* CCtrlTable::getModel()
	{
		return &m_model;
	}

	void CCtrlTable::setView(IViewTable* pView)
	{
		m_pView = pView;
	}

	void CCtrlTable::setAssociateScrollbar(CCtrlScrollbar* pScrollBar, BOOLEAN bControlScrollbarVisible, BOOLEAN bMovePageWhenClickUpDownArrow)
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

	void CCtrlTable::resetTableLayout()
	{
		m_scrollRect.setWidth(getPreferredSize().getX());
		m_scrollRect.setHeight(getPreferredSize().getY());
#if 0
		UINT uiColumn = getPreferredSize().getX() / m_uiCellWidth;

		if (uiColumn == 0)
		{
			uiColumn = 1;
		}
		
		if(uiColumn == m_uiColumn)
		{
			return;
		}
#endif
		resetShowItemPos();
		resetScrollbar();
		showTopItem(0);
	}
	
	void CCtrlTable::setCurSel(INT iValue)
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
				dispatchEvent(CEventTable::SELECT_CHANGED);
			}
		}
	}

	Object* CCtrlTable::getHoverFocusedItem()
	{
		if ( m_iCurHoverFocus == -1 )
		{
			return NULL;
		}
		else
		{
			return m_aryShowItem[m_iCurHoverFocus]->pComponent;
		}
	}

	void CCtrlTable::setCurHoverFocus(INT iValue)
	{
		if (iValue < 0)
		{
			m_iCurHoverFocus = -1;
		}			
		else if (iValue >= (INT)m_model.getItemCount())
		{
			m_iCurHoverFocus = - 1;
		}
		else
		{
			m_iCurHoverFocus = iValue;
		}
	}

	void CCtrlTable::setCurTouchFocus(INT iValue)
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

	Object* CCtrlTable::getTouchFocusedItem()
	{
		if ( m_iCurTouchFocus == -1 )
		{
			return NULL;
		}
		else
		{
			return m_aryShowItem[m_iCurTouchFocus]->pComponent;
		}
	}

	Object* CCtrlTable::getTouchFocusedItemR()
	{
		if ( m_iCurTouchFocusR == -1 )
		{
			return NULL;
		}
		else
		{
			return m_aryShowItem[m_iCurTouchFocusR]->pComponent;
		}
	}


	void CCtrlTable::setCurCCPFocus(INT iValue)
	{
		if (iValue < 0)
		{
			m_iCurCCPFocus = -1;
		}			
		else if (iValue >= (INT)m_model.getItemCount())
		{
			m_iCurCCPFocus = - 1;
		}
		else
		{
			if (m_iCurCCPFocus != iValue)
			{
				m_iCurCCPFocus = iValue;
				dispatchEvent(CEventTable::CCP_FOCUS_CHANGED);
			}
		}
	}

	INT CCtrlTable::getCurCCPFocus()
	{
		if (hasFocus())
		{
			return m_iCurCCPFocus;
		}

		return -1;
	}

	void CCtrlTable::setCCPFocusedItem(Object* pObj)
	{
		m_iCurCCPFocus = m_model.getItemIndex(pObj);
	}

	Object* CCtrlTable::getCCPFocusedItem()
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
	
	Object* CCtrlTable::getDraggedItem()
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

	INT CCtrlTable::getItemInsertToPos(CPoint *pt)
	{
		if (m_aryShowItem.size() == 0)
		{
			return -1;
		}

		for (INT i= 0; i < (INT)m_aryShowItem.size(); i++)
		{
			tagShowItem* pShowItem = m_aryShowItem[i];

			if (pShowItem->rcCell.ptInRect(*pt))
			{
				return i;
			}
		}

		return -1;
	}

	CComponent* CCtrlTable::getItemComponent(Object* pObj)
	{
		if (pObj == NULL)
		{
			return NULL;
		}

		for (INT i = 0; i < (INT)m_aryShowItem.size(); i++)
		{
			if (m_aryShowItem[i]->pObject == pObj)
			{
				return m_aryShowItem[i]->pComponent;
			}
		}
		
		return NULL;
	}

	void CCtrlTable::showTopItem(UINT uiIndex)
	{
		if (m_aryShowItem.size() == 0)
		{
			return;
		}

		uiIndex = (uiIndex / m_uiColumn) * m_uiColumn;
		
		UINT uiMaxShowTopIndex = calculateShowItemIndex(getTotalShowItemHeight() - getPreferredSize().getY() + DISTANCE_OFFSET);
		if (uiIndex > uiMaxShowTopIndex)
		{
			uiIndex = uiMaxShowTopIndex;
		}

		m_iShowTopIndex = uiIndex;
		verifyShowInfo();
		showItemState();

		INT iPos = m_aryShowItem[m_iShowTopIndex]->rcCell.getY();
		showItems(iPos);
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->getModel()->setPosition(iPos);
			m_pScrollbar->refresh();
		}
	}

	void CCtrlTable::refreshForAppendItem()
	{
		appendShowItem(m_aryShowItem.size());
		resetScrollbar(TRUE);
			
		if (m_iShowTopIndex == 0)
		{
			verifyShowInfo();
			showItemState();
			
			m_scrollRect.setWidth(getPreferredSize().getX());
			m_scrollRect.setHeight(getPreferredSize().getY());
		}			
		
		showItems(m_scrollRect.getY());
		
		m_bInAppendMode = FALSE;
	}	

	void CCtrlTable::backupViewState()
	{
		m_viewState.pObjSelectedItem = getSelectedItem();
		m_viewState.pObjCCPFocusItem = getCCPFocusedItem();
		m_viewState.iShowTopIndex = m_iShowTopIndex;
	}

	void CCtrlTable::restoreViewState()
	{
		refresh();
		
		setSelectedItem(m_viewState.pObjSelectedItem);
		setCCPFocusedItem(m_viewState.pObjCCPFocusItem);
		showTopItem(m_viewState.iShowTopIndex);

		m_viewState.pObjSelectedItem = NULL;
		m_viewState.pObjCCPFocusItem = NULL;
		m_viewState.iShowTopIndex = -1;
	}

	void CCtrlTable::playEnterAnimation()
	{
		startEnterAnimation();
	}

	void CCtrlTable::setTouchAnimationType(ETouchAnimation eValue)
	{
		switch(eValue)
		{
		case TOUCH_SHAKE:
			m_pAmtTouch = new CTableAmtTouchShake();
			m_pAmtTouch->m_pTable = this;
			break;
		}
	}

	void CCtrlTable::lock()
	{
		CComponent::lock();
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->lock();
		}
	}

	void CCtrlTable::unlock()
	{
		CComponent::unlock();
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->unlock();
		}
	}

	void CCtrlTable::refresh()
	{		
		CComponent::refresh();
		
		m_scrollRect.setWidth(getPreferredSize().getX());
		m_scrollRect.setHeight(getPreferredSize().getY());

		buildShowItemList();
		resetScrollbar();
		
		m_iCurTouchFocus = -1;
		m_iCurHoverFocus = -1;
		m_iShowTopIndex = 0;
		verifyShowInfo();
		showItemState();
		
		showItems(0);
		startEnterAnimation();
	}

	void CCtrlTable::littleRefresh()
	{
		CComponent::littleRefresh();
		
		showItemState();
	}

	void CCtrlTable::destroySkin()
	{
		destroyShowItemList();
		
		CComponent::destroySkin();
	}

	void CCtrlTable::destroy()
	{
		CComponent::destroy();
	}

	void CCtrlTable::buildRenderTree(CRenderGroup &renderGroup)
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

	BOOLEAN	CCtrlTable::canSetFocus()
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

	void CCtrlTable::setFocus(ECCPOperation eCCPOperation)
	{
		innerSetFocus(TRUE);
		
		switch(eCCPOperation)
		{
		case CComponent::CCP_MOVE_DOWN:
		case CComponent::CCP_MOVE_RIGHT:
		case CComponent::CCP_ROTATE_RIGHT:				
				m_iCurCCPFocus= getFirstCCPItemIndex();
				break;
				
		case CComponent::CCP_MOVE_LEFT:
		case CComponent::CCP_MOVE_UP:
		case CComponent::CCP_ROTATE_LEFT:
				m_iCurCCPFocus = getLastCCPItemIndex();
				break;
		}
		
		adjustCCPFocusDisplayPos();
		
		dispatchEvent(CEventTable::CCP_FOCUS_CHANGED);
	}

	void CCtrlTable::releaseFocus()
	{
		if (innerGetFocus())
		{
			//m_iCurCCPFocus = -1;
			CComponent::releaseFocus();
			dispatchEvent(CEventTable::CCP_FOCUS_CHANGED);
			return;
		}	
		
		CComponent* pComponent = getCCPFocusComponent();
		if (pComponent != NULL)
		{
			pComponent->releaseFocus();
		}									
	}

	BOOLEAN CCtrlTable::hasFocus()
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

	BOOLEAN CCtrlTable::moveFocusToLeft()
	{
		if (canMoveFocus() && m_bCanMoveFocusByLeftRight)
		{
			return moveCCPFocusIndexDecrease(CComponent::CCP_MOVE_LEFT);
		}
		else
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
	}

	BOOLEAN CCtrlTable::moveFocusToRight()
	{
		if (canMoveFocus() && m_bCanMoveFocusByLeftRight)
		{
			return moveCCPFocusIndexIncrease(CComponent::CCP_MOVE_RIGHT);
		}
		else
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
	}

	BOOLEAN CCtrlTable::moveFocusToUp()
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

	BOOLEAN CCtrlTable::moveFocusToDown()
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

	BOOLEAN CCtrlTable::turnFocusToLeft(UINT uiStep)
	{
		return moveCCPFocusIndexDecrease(CComponent::CCP_ROTATE_LEFT, uiStep);
	}

	BOOLEAN CCtrlTable::turnFocusToRight(UINT uiStep)
	{
		return moveCCPFocusIndexIncrease(CComponent::CCP_ROTATE_RIGHT, uiStep);
	}

	void CCtrlTable::doFocusEnterPress()
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

	void CCtrlTable::doFocusEnterRelease()
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

	BOOLEAN CCtrlTable::doFocusBack()
	{
		if (innerGetFocus())
		{
			BOOLEAN bRet = CComponent::doFocusBack();
			dispatchEvent(CEventTable::CCP_FOCUS_CHANGED);
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

	void CCtrlTable::onMove(INT iX, INT iY)
	{	
		BOOLEAN bInInner = hitTest(iX, iY);

		if (getEnable())
		{
			if (bInInner)
			{
				if (getHoverFocus() == FALSE)
				{
					setHoverFocus(TRUE);
				}
			}
			else
			{
				if (getHoverFocus() == TRUE)
				{
					for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
					{
						tagShowItem* pShowItem = m_aryShowItem[i + m_iShowItemIndex];

						if (pShowItem->pComponent.isNull())
						{
							continue;
						}

						pShowItem->pComponent->onLostHover();
					}

					dispatchEvent(CEventTable::HOVER_CANCELED);
					setHoverFocus(FALSE);
					m_iCurHoverFocus = -1;
				}
				return;
			}
		}
		else
		{
			return;
		}

		bInInner = FALSE;

		for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
		{
			tagShowItem* pShowItem = m_aryShowItem[i + m_iShowItemIndex];

			if (pShowItem->pComponent.isNull())
			{
				continue;
			}

			{
				pShowItem->pComponent->onMove(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}

			if (pShowItem->pComponent->hitTest(iX + m_scrollRect.getX(), iY + m_scrollRect.getY()) == FALSE)
			{
				continue;
			}

			bInInner = TRUE;

			if (m_iCurHoverFocus != i + m_iShowItemIndex)
			{	
				if(m_iCurHoverFocus >= 0)
					dispatchEvent(CEventTable::ITEM_HOVER_CANCELED);

				m_iCurHoverFocus = i + m_iShowItemIndex;
				dispatchEvent(CEventTable::ITEM_HOVER_FOCUSED);			
			}
		
			//break;
		}

		if(bInInner == FALSE)
		{
			if(m_iCurHoverFocus >= 0)
				dispatchEvent(CEventTable::ITEM_HOVER_CANCELED);
			m_iCurHoverFocus = -1;
		}
	}

	void CCtrlTable::onLostHover()
	{
		if (getHoverFocus() == TRUE)
		{
			setHoverFocus(FALSE);
			m_iCurHoverFocus = -1;
			dispatchEvent(CEventTable::HOVER_CANCELED);

			for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
			{
				tagShowItem* pShowItem = m_aryShowItem[i + m_iShowItemIndex];

				if (pShowItem->pComponent.isNull())
				{
					continue;
				}

				{
					pShowItem->pComponent->onLostHover();
				}
			}

		}
	}

	void CCtrlTable::onWheel(CCursor::EWheelType eType)
	{	
		if (getEnable() && m_bCanWheel == TRUE)
		{
			if (getHoverFocus() == TRUE)
			{
				INT iTotalShowItemHeight = (INT)getTotalShowItemHeight();

				if (iTotalShowItemHeight > getPreferredSize().getY())
				{
					INT iLen = 0;

					if(eType == CCursor::WT_DOWN)
					{
						iLen = -1 * getCellHeight();
					}
					else
					{
						iLen = getCellHeight();
					}

					if (m_scrollRect.getY()-iLen <= 0 && eType == CCursor::WT_UP)
					{
						if (m_scrollRect.getY() == 0)
						{
							startElasticityAnimation(m_scrollRect.getY(), 0);
						}
						else
						{
							startFlingAnimationExt(m_scrollRect.getY(), m_scrollRect.getY());
						}
					}
					else if (iTotalShowItemHeight - getPreferredSize().getY() < m_scrollRect.getY() - iLen && eType == CCursor::WT_DOWN)
					{
						startFlingAnimationExt(m_scrollRect.getY(), m_scrollRect.getY() - iTotalShowItemHeight + getPreferredSize().getY());
					}
					else
					{

						m_iCurTouchFocus = -1;
						showItemState();
						m_iCurHoverFocus = -1;
						dispatchEvent(CEventTable::HOVER_CANCELED);
						::SetClassLong(CScreenTouchServer::getInstance()->getScreenContext()->hWnd, GCL_HCURSOR,
							(LONG_PTR)LoadCursor(NULL,IDC_ARROW));

						startFlingAnimationExt(m_scrollRect.getY(), iLen);
					}
					dispatchEvent(CEventTable::WHEEL_START);
				}
			}
		}
	}

	void CCtrlTable::onTouchPress(INT iX, INT iY)
	{
		stopFlingAnimation();		
		
		m_iTouchBeginPosY = iY;
		m_iShowBeginPosY = m_scrollRect.getY();
		m_iShowCurrentPosY = m_scrollRect.getY();
		m_iCurTouchFocus = -1;

		for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
		{
			tagShowItem* pShowItem = m_aryShowItem[i + m_iShowItemIndex];
			
			if (pShowItem->pComponent.isNull())
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
				stopTouchAnimation();
			}

			{
				m_bOuterChangedData = FALSE;
				pShowItem->pComponent->onTouchPress(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}

			break;
		}
	}

	void CCtrlTable::onTouchPressR(INT iX, INT iY)
	{
		stopFlingAnimation();		

		m_iTouchBeginPosY = iY;
		m_iShowBeginPosY = m_scrollRect.getY();
		m_iShowCurrentPosY = m_scrollRect.getY();
		m_iCurTouchFocusR = -1;


		for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
		{
			tagShowItem* pShowItem = m_aryShowItem[i + m_iShowItemIndex];

			if (pShowItem->pComponent.isNull())
			{
				continue;
			}

			if (pShowItem->pComponent->hitTest(iX + m_scrollRect.getX(), iY + m_scrollRect.getY()) == FALSE)
			{
				continue;
			}

			if (m_iCurTouchFocusR != i + m_iShowItemIndex)
			{
				m_iCurTouchFocusR = i + m_iShowItemIndex;
			}

			{
		
				pShowItem->pComponent->onTouchPressR(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}

			break;
		}
	}

	void CCtrlTable::onTouchPressRelease(INT iX, INT iY)
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
					//startTouchAnimation(m_aryShowItem[m_iCurTouchFocus]->pComponent);
				}
			}
		}

		if (!m_bOuterChangedData)
		{
			checkSelectChanged();
		}
	}

	void CCtrlTable::onTouchPressReleaseR(INT iX, INT iY)
	{
		if (m_iCurTouchFocusR >= 0)
		{
			m_aryShowItem[m_iCurTouchFocusR]->pComponent->onTouchPressReleaseR(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
		}
	}

	void CCtrlTable::onTouchLongPress(INT iX, INT iY)
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

	void CCtrlTable::onTouchContinuePress(INT iX, INT iY)
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

	void CCtrlTable::onTouchLongPressRelease(INT iX, INT iY)
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

	void CCtrlTable::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
	{
		return;

		if ((INT)getTotalShowItemHeight() > getPreferredSize().getY())
		{
			INT iPos = 0;
			UINT uiTotalShowItemHeight = getTotalShowItemHeight();

			if (m_scrollRect.getY() <= 0)
			{
				iPos = m_iShowBeginPosY - iDeltaY * (getPreferredSize().getY() + m_scrollRect.getY()) / getPreferredSize().getY();
				
				if( !m_bIsElasticityOn && iPos < 0 )
				{
					m_bDirectionDown = FALSE;
					dispatchEvent(CEventTable::ITEM_SCROLLING);
					return;
				}
			}
			else if ((INT)uiTotalShowItemHeight - getPreferredSize().getY() <= m_scrollRect.getY())
			{
				iPos = m_iShowBeginPosY - iDeltaY * ((INT)uiTotalShowItemHeight - m_scrollRect.getY()) / getPreferredSize().getY();

				if( !m_bIsElasticityOn && iPos > (INT)uiTotalShowItemHeight - getPreferredSize().getY() )
				{
					m_bDirectionDown = TRUE;
					dispatchEvent(CEventTable::ITEM_SCROLLING);
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
				m_pScrollbar->getModel()->setPosition(iPos);
				m_pScrollbar->refresh();

				if (m_bControlScrollbarVisible)
				{
					m_pScrollbar->setVisible(TRUE);
				}
			}
				
			if (needVerifyShowInfo(iPos))
			{
				verifyShowInfo();
				showItemState();
			}
			
			showItems(iPos);
			
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

			dispatchEvent(CEventTable::ITEM_SCROLLING);
		}
	}

	void CCtrlTable::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
	{
		return;

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

	void CCtrlTable::onTouchDrag(INT iX, INT iY)
	{
		if (m_iCurTouchFocus >= 0)
		{
			m_ptCurrentCursor.setX(iX);
			m_ptCurrentCursor.setY(iY);
			dispatchEvent(CEventTable::ITEM_DRAGGING);
		}
	}

	void CCtrlTable::onTouchDragRelease(INT iX, INT iY)
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
			
			dispatchEvent(CEventTable::ITEM_DROPPED);

			m_iCurTouchFocus = -1;
			showItemState();
			m_ptCurrentCursor.setX(0);
			m_ptCurrentCursor.setY(0);
		}
	}

	BOOLEAN CCtrlTable::contains(Object *pTarget)
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

	void CCtrlTable::buildShowItemList()
	{
		destroyShowItemList();

		appendShowItem(0);
	}

	void CCtrlTable::appendShowItem(UINT uiIndex)
	{
		UINT uiItemCount = m_model.getItemCount();

		if (uiIndex >= uiItemCount)
		{
			resetShowItemPos();
			return;
		}

		for ( ; uiIndex < uiItemCount; uiIndex++)
		{
			Object* pObj = m_model.getItemAt(uiIndex);
			tagShowItem* pShowItem = new tagShowItem(pObj, pObj != NULL ? m_pView->getItemComponent(this, pObj) : NULL);
			m_aryShowItem.addItem(pShowItem);			

			if (pShowItem->pComponent.isNotNull())
			{
				if (!pShowItem->pComponent->isContainer())
				{
					pShowItem->pComponent->notAcceptFocus();
				}
			}
		}

		resetShowItemPos();
	}

	void CCtrlTable::resetShowItemPos()
	{
		UINT uiItemCount = m_model.getItemCount();		

		if (m_uiCellWidth == 0 || m_uiCellHeight == 0)
		{
			return;
		}

		m_uiColumn = getPreferredSize().getX() / m_uiCellWidth;		
		if (m_uiColumn == 0)
		{
			m_uiColumn = 1;
		}

		m_uiRow = ((INT)uiItemCount - 1) / (INT)m_uiColumn + 1;

		INT iX = 0;
		INT iY = 0;

		for (UINT i = 0; i < uiItemCount; i++)
		{
			tagShowItem* pShowItem = m_aryShowItem[i];

			iX = (INT)(i % m_uiColumn) * m_uiCellWidth;
			iY = (INT)(i / m_uiColumn) * m_uiCellHeight;

			pShowItem->rcCell.setX(iX);
			pShowItem->rcCell.setY(iY);
			pShowItem->rcCell.setWidth(m_uiCellWidth);
			pShowItem->rcCell.setHeight(m_uiCellHeight);				

			if (pShowItem->pComponent.isNotNull())
			{
				pShowItem->pComponent->setX(iX + (m_uiCellWidth - pShowItem->pComponent->getPreferredSize().getX()) / 2);
				pShowItem->pComponent->setY(iY + (m_uiCellHeight - pShowItem->pComponent->getPreferredSize().getY()) / 2);
			}
		}
	}

	void CCtrlTable::destroyShowItemList()
	{
		if (m_aryShowItem.size() == 0)
		{
			return;
		}

		for (UINT i = 0; i < m_aryShowItem.size(); i++)
		{
			tagShowItem* pShowItem = m_aryShowItem[i];

			pShowItem->pObject = NULL;

			if (pShowItem->pComponent.isNotNull())
			{
				pShowItem->pComponent->destroy();			
				pShowItem->pComponent = NULL;				
			}
		}
		m_aryShowItem.clear();

		m_bOuterChangedData = TRUE;
	}

	void CCtrlTable::showItems(INT iTop)
	{
		m_scrollRect.setY(iTop);
		drawTableCCPFocs();

		if (m_pScrollbar != NULL && m_bControlScrollbarVisible == FALSE)
		{
			m_pScrollbar->getModel()->setPreviousStep(m_uiCellHeight);
			m_pScrollbar->getModel()->setNextStep(m_uiCellHeight);
		}

		setDirty();
	}

	void CCtrlTable::showItemState()
	{
		//return;

		tagShowItem *pShowItem = NULL;
		
		for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
		{
			pShowItem = m_aryShowItem[i + m_iShowItemIndex];

			if (pShowItem->pComponent.isNull())
			{
				continue;
			}
			
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
		
		drawTableCCPFocs();
	}
	
	void CCtrlTable::resetScrollbar(BOOLEAN bMaintainPos)
	{
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->getModel()->setScrollInfo(0, 
										getTotalShowItemHeight() - 1, 
										bMaintainPos ? m_pScrollbar->getModel()->getPosition() : 0,
										getPreferredSize().getY() /*> 2 * m_uiCellHeight ? 2 * m_uiCellHeight : getPreferredSize().getY()*/);
			m_pScrollbar->refresh();
			
			if ((INT)getTotalShowItemHeight() <= getPreferredSize().getY())
			{
				m_pScrollbar->setVisible(FALSE);
			}
			else
			{
				if( m_bControlScrollbarVisible)
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

	UINT CCtrlTable::getTotalShowItemHeight()
	{
		return m_uiRow * m_uiCellHeight;
	}

	BOOLEAN CCtrlTable::needVerifyShowInfo(INT iPos)
	{
		INT iItemIndex = calculateShowItemIndex(iPos);
		
		if (   iItemIndex <= m_iShowItemIndex
			|| iItemIndex >= m_iShowTopIndex + (INT)(SHOW_ITEM_OFFSET * m_uiColumn))
		{
			m_iShowTopIndex = iItemIndex;
			return TRUE;
		}
		
		return FALSE;
	}

	void CCtrlTable::verifyShowInfo()  //need further check and consider
	{
		m_iShowItemIndex = m_iShowTopIndex - SHOW_ITEM_OFFSET * m_uiColumn;
		if (m_iShowItemIndex < 0)
		{
			m_iShowItemIndex = 0;
		}

		m_uiShowItemCount = 0;
		if (m_aryShowItem.size() != 0)
		{
			INT iHeight = 0;
			for (INT i = m_iShowTopIndex; i < (INT)m_aryShowItem.size(); i += m_uiColumn)
			{
				m_uiShowItemCount += m_uiColumn;

				iHeight += m_uiCellHeight;
				
				if (iHeight >= getPreferredSize().getY())
				{
					break;
				}				
			}
			
			m_uiShowItemCount += (m_iShowTopIndex - m_iShowItemIndex) + SHOW_ITEM_OFFSET * m_uiColumn;
			
			if (m_iShowTopIndex + m_uiShowItemCount > m_aryShowItem.size())
			{
				m_uiShowItemCount = m_aryShowItem.size() - m_iShowItemIndex;
			}
		}
		
		m_aryShowComponent.clear();

		if (m_uiShowItemCount == 0)
		{
			return;
		}

		for (INT k = 0; k < (INT)m_uiShowItemCount; k++)
		{
			CComponent* pComponentShow = m_aryShowItem[k + m_iShowItemIndex]->pComponent;
			if (pComponentShow == NULL)
			{
				continue;
			}
			
			pComponentShow->setParent(this);
			pComponentShow->refresh();
			m_aryShowComponent.addItem(new tagShowComponent(pComponentShow));
		}
		
		if (   checkCanRollToTop() == FALSE
			&& m_uiItemsPerPage > 0
			&& m_aryShowItem.size() > 0
			&& m_iShowItemIndex > (INT)m_aryShowItem.size() - (INT)m_uiItemsPerPage
			&& m_bInAppendMode == FALSE )
		{
			m_bInAppendMode = TRUE;
			dispatchEvent(CEventTable::WANT_NEXT_PAGE);
		}
	}

	UINT CCtrlTable::calculateShowItemIndex(INT iPos)
	{
		if (m_aryShowItem.size() == 0)
		{
			return 0;
		}			
		
		for (UINT i = 0; i < m_aryShowItem.size(); i += m_uiColumn)
		{	
			if (  iPos < m_aryShowItem[i]->rcCell.getBottom())
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
			return (m_uiRow - 1) * m_uiColumn;
		}
	}

	void CCtrlTable::checkSelectChanged()
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
			dispatchEvent(CEventTable::SELECT_CHANGED);
		}
	}

	BOOLEAN CCtrlTable::checkCanRollToTop()
	{
		return m_model.getRealItemCount() > m_model.getItemCount() ? FALSE : TRUE;
	}

	INT CCtrlTable::getFirstCCPItemIndex()
	{
		if (m_aryShowItem.size() == 0)
		{
			return -1;
		}

		if ( m_iCurCCPFocus == -1 )
		{

			INT iIndex = m_iShowTopIndex;
					
			while (iIndex < (INT)m_aryShowItem.size() && m_aryShowItem[iIndex]->pComponent.isNull())
			{
				iIndex++;
			}
					
			if (iIndex < (INT)m_aryShowItem.size())
			{
				return iIndex;
			}
		}

		return m_iCurCCPFocus;
	}

	INT CCtrlTable::getLastCCPItemIndex()
	{
		if (m_aryShowItem.size() == 0)
		{
			return -1;
		}

		if (m_iCurCCPFocus != -1)
		{
			return m_iCurCCPFocus;
		}
		
		for ( INT i = m_uiShowItemCount - 1; i >= 0; i-- )
		{
			tagShowItem *pShowItem = m_aryShowItem[m_iShowItemIndex + i];

			if (pShowItem->pComponent.isNull())
			{
				continue;
			}

			if (pShowItem->rcCell.getY() < m_scrollRect.getY() + (INT)m_scrollRect.getHeight())
			{
				return m_iShowItemIndex + i;
			}
		}
		
		return -1;
	}

	void CCtrlTable::adjustCCPFocusDisplayPos()
	{
		if ( m_iCurCCPFocus== -1 || m_aryShowItem.size() == 0)
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
		while (m_iCurCCPFocus - i >= 0 )
		{
			tagShowItem *pShowItem = m_aryShowItem[m_iCurCCPFocus - i];

			if (pShowItem->rcCell.getY() + DISTANCE_OFFSET > m_scrollRect.getY() + (INT)m_scrollRect.getHeight() )
			{
				i += m_uiColumn;
			}
			else
			{
				break;
			}
		}

		if (i > 0)
		{
			showTopItem( m_iShowTopIndex + i );
			return;
		}
		
		littleRefresh();
	}

	void CCtrlTable::drawTableCCPFocs()
	{
		if (   !innerGetFocus() 
			|| m_iCurCCPFocus == -1 
			|| m_iCurCCPFocus >= (INT)m_aryShowItem.size()
			|| m_CCPSkin == NULL   )
		{
			if(m_CCPSkin != NULL)
			{
				m_CCPSkin->setVisible(FALSE);
			}
			return;
		}
	
		tagShowItem *pShowItem = m_aryShowItem[m_iCurCCPFocus];
		if (pShowItem->pComponent.isNull())
		{
			m_CCPSkin->setVisible(FALSE);
			return;
		}
		
		m_CCPSkin->setVisible(TRUE);
		m_CCPSkin->setX(pShowItem->rcCell.getX() - m_scrollRect.getX() - ((INT)(m_CCPSkin->getWidth()) - (INT)(pShowItem->rcCell.getWidth())) / 2);
		m_CCPSkin->setY(pShowItem->rcCell.getY() - m_scrollRect.getY() - ((INT)(m_CCPSkin->getHeight()) - (INT)(pShowItem->rcCell.getHeight())) / 2);
	}

	CComponent* CCtrlTable::getCCPFocusComponent()
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

	BOOLEAN CCtrlTable::moveCCPFocusIndexDecrease(ECCPOperation eCCPOperation,UINT uiStep)
	{
		if (   eCCPOperation != CComponent::CCP_MOVE_UP 
			&& eCCPOperation != CComponent::CCP_MOVE_LEFT
			&& eCCPOperation != CComponent::CCP_ROTATE_LEFT )
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
		
		if ( innerGetFocus() )
		{
			if (m_iCurCCPFocus <= 0)
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}

			INT iCurCCPFocus = m_iCurCCPFocus;

			if ( eCCPOperation == CComponent::CCP_MOVE_UP )
			{
				iCurCCPFocus -= m_uiColumn;

				while (iCurCCPFocus >= 0 && m_aryShowItem[iCurCCPFocus]->pComponent.isNull())
				{
					iCurCCPFocus -= m_uiColumn;
				}
			}
			else if (eCCPOperation == CComponent::CCP_MOVE_LEFT)
			{
				if (iCurCCPFocus % m_uiColumn != 0)
				{
					iCurCCPFocus -= 1;

					while (    iCurCCPFocus >= 0 
							&& m_aryShowItem[iCurCCPFocus]->pComponent.isNull())
					{
						if (iCurCCPFocus % m_uiColumn == 0)
						{
							iCurCCPFocus = -1;
							break;
						}

						iCurCCPFocus -= 1;
					}
				}
				else
				{
					iCurCCPFocus = -1;
				}
			}
			else
			{
				iCurCCPFocus -= 1;

				while (iCurCCPFocus >= 0 && m_aryShowItem[iCurCCPFocus]->pComponent.isNull())
				{
					iCurCCPFocus -= 1;
				}
			}
				
			if (iCurCCPFocus < 0)
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}

			m_iCurCCPFocus = iCurCCPFocus;
			adjustCCPFocusDisplayPos();
			dispatchEvent(CEventTable::CCP_FOCUS_CHANGED);
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
			else if ( eCCPOperation == CComponent::CCP_MOVE_LEFT )
			{
				pComponent->moveFocusToLeft();
			}
			else
			{
				pComponent->turnFocusToLeft(uiStep);
			}
			
			return TRUE;
		}
	}

	BOOLEAN CCtrlTable::moveCCPFocusIndexIncrease(ECCPOperation eCCPOperation,UINT uiStep)
	{
		if (   eCCPOperation != CComponent::CCP_MOVE_DOWN 
			&& eCCPOperation != CComponent::CCP_MOVE_RIGHT 
			&& eCCPOperation != CComponent::CCP_ROTATE_RIGHT )
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

			if ( eCCPOperation == CComponent::CCP_MOVE_DOWN )
			{
				iCurCCPFocus += m_uiColumn;

				while (iCurCCPFocus <= (INT)m_aryShowItem.size() - 1 && m_aryShowItem[iCurCCPFocus]->pComponent.isNull())
				{
					iCurCCPFocus += m_uiColumn;
				}
			}
			else if (eCCPOperation == CComponent::CCP_MOVE_RIGHT)
			{
				if (iCurCCPFocus % m_uiColumn != m_uiColumn - 1)
				{
					iCurCCPFocus += 1;

					while (    iCurCCPFocus <= (INT)m_aryShowItem.size() - 1 
							&& m_aryShowItem[iCurCCPFocus]->pComponent.isNull())
					{
						if (iCurCCPFocus % m_uiColumn == m_uiColumn - 1)
						{
							iCurCCPFocus = (INT)m_aryShowItem.size();
							break;
						}

						iCurCCPFocus += 1;
					}
				}
				else
				{
					iCurCCPFocus = (INT)m_aryShowItem.size();
				}
			}
			else
			{
				iCurCCPFocus += 1;

				while (iCurCCPFocus <= (INT)m_aryShowItem.size() - 1 && m_aryShowItem[iCurCCPFocus]->pComponent.isNull())
				{
					iCurCCPFocus += 1;
				}
			}

			if (iCurCCPFocus > (INT)m_aryShowItem.size() - 1)
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}

			m_iCurCCPFocus = iCurCCPFocus;
			adjustCCPFocusDisplayPos();
			dispatchEvent(CEventTable::CCP_FOCUS_CHANGED);
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

			if ( eCCPOperation == CComponent::CCP_MOVE_DOWN )
			{
				pComponent->moveFocusToDown();
			}
			else if ( eCCPOperation == CComponent::CCP_MOVE_RIGHT )
			{
				pComponent->moveFocusToRight();
			}
			else
			{
				pComponent->turnFocusToRight(uiStep);
			}

			return TRUE;
		}
	}

	void CCtrlTable::switchCCPFocusForTouch()
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

	void CCtrlTable::startEnterAnimation()
	{
		if ( m_pAmtEnter != NULL && m_bIsEnterAnimationOn == TRUE )
		{
			tagAmtData_Enter data;

			if (m_pAmtEnter->prepareAmtData(&data))
			{
				m_pAmtEnter->play();
				lock();
				m_bIsEnterAnimationOn = FALSE;
			}
		}
	}

	void CCtrlTable::stopEnterAnimation()
	{
		if ( m_pAmtEnter != NULL )
		{
			m_pAmtEnter->stop();
			
			unlock();
		}
	}

	void CCtrlTable::startElasticityAnimation(INT iStartValue, INT iEndValue)
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

			dispatchEvent(CEventTable::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlTable::stopElasticityAnimation()
	{
		if ( m_pAmtElasticity != NULL )
		{
			m_pAmtElasticity->stop();
			
			unlock();

			if (m_bControlScrollbarVisible)
			{
				m_pScrollbar->setVisible(FALSE);
			}

			dispatchEvent(CEventTable::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlTable::startFlingAnimation(INT iStartValue, INT iSpeed)
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

			dispatchEvent(CEventTable::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlTable::startFlingAnimationExt(INT iStartPos, INT iDeltaPos)
	{
		if ( m_pAmtFling != NULL)
		{
			tagAmtData_FlingExt data;
			data.iStartPos = iStartPos;
			data.iDeltaPos = iDeltaPos;

			if (m_pAmtFling->prepareAmtDataExt(&data))
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

			dispatchEvent(CEventTable::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlTable::stopFlingAnimation()
	{
		if ( m_pAmtFling != NULL )
		{
			m_pAmtFling->stop();
		
			unlock();

			if (m_bControlScrollbarVisible)
			{
				m_pScrollbar->setVisible(FALSE);
			}

			dispatchEvent(CEventTable::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlTable::startTouchAnimation(CComponent* pCom)
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

	void CCtrlTable::stopTouchAnimation()
	{
		if ( m_pAmtTouch != NULL )
		{
			m_pAmtTouch->stop();
		}
	}

	void CCtrlTable::onScrollbarPosChanged(CComponent *pTarget)
	{
		INT iPos = m_pScrollbar->getModel()->getPosition();

		CCtrlScrollbar::EPosChanged eType = m_pScrollbar->getPosChangedType();

// 		if (iPos == 0)
// 		{
// 			if (eType == CCtrlScrollbar::PC_LINE_UP || eType == CCtrlScrollbar::PC_CONTINUE_LINE_UP)
// 			{
// 				if (m_scrollRect.getY() == 0)
// 				{
// 					iPos = m_pScrollbar->getModel()->getMaxPosition();
// 					m_pScrollbar->getModel()->setPosition(iPos);
// 					m_pScrollbar->refresh();
// 				}
// 			}
// 		}
// 		else if (iPos == m_pScrollbar->getModel()->getMaxPosition())
// 		{
// 			if (eType == CCtrlScrollbar::PC_LINE_DOWN || eType == CCtrlScrollbar::PC_CONTINUE_LINE_DOWN)
// 			{
// 				if (m_scrollRect.getY() == m_pScrollbar->getModel()->getMaxPosition())
// 				{
// 					if (checkCanRollToTop())
// 					{
// 						iPos = 0;
// 						m_pScrollbar->getModel()->setPosition(iPos);
// 						m_pScrollbar->refresh();
// 					}
// 					else
// 					{
// 						dispatchEvent(CEventTable::WANT_NEXT_PAGE);
// 						return;
// 					}
// 				}
// 			}
// 		}
		INT iCurY = m_scrollRect.getY();

		//if (Math::abs(iPos - iCurY) < m_uiCellHeight)
		//	return;

		if (needVerifyShowInfo(iPos))
		{
			verifyShowInfo();
			showItemState();
		}
			
		showItems(iPos);
	}

	void CCtrlTable::onScrollbarPosContinueChangeEnded(CComponent *pTarget)
	{
		;
	}
}