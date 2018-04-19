#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlList_Simple::CCtrlList_Simple()
		: m_pModel(NULL)
		, m_pView(NULL)		
		, m_pScrollbar(NULL)
		, m_bControlScrollbarVisible(FALSE)		
		, m_bFirstBuild(TRUE)
		, m_uiItemHeight(0)
		, m_iCurSel(-1)
		, m_iCurTouchFocus(-1)
		, m_iCurCCPFocus(-1)
		, m_iShowTopIndex(0)
		, m_iShowItemIndex(0)
		, m_uiShowItemCount(0)
		, m_uiItemsPerPage(0)
		, m_bReachToHead(TRUE)
		, m_bReachToTail(TRUE)
		, m_bInAppendPrevPageMode(FALSE)
		, m_bInAppendNextPageMode(FALSE)
		, m_iTouchBeginPosY(0)
		, m_iShowBeginPosY(0)
		, m_iShowCurrentPosY(0)
		, m_bDirectionDown(FALSE)
		, m_bCanScrollByTouchMove(TRUE)
		, m_bOuterChangedData(FALSE)
		, m_bCanMoveFocusByUpDown(FALSE)
		, m_bIsElasticityOn(FALSE)
		, m_bIsFlingOn(FALSE)
	{
		setBaseView(&m_view);

		m_viewState.pObjSelectedItem = NULL;
		m_viewState.pObjCCPFocusItem = NULL;
		m_viewState.iShowTopIndex = -1;

		m_pAmtElasticity = new CListAmtElasticity();
		m_pAmtElasticity->m_pListSimple = this;

		m_pAmtFling = new CListAmtFling();
		m_pAmtFling->m_pListSimple = this;
	}

	CCtrlList_Simple::~CCtrlList_Simple()
	{
		;
	}

	BEGIN_EVENT_MAP(CCtrlList_Simple)
		CONTROL_EVENT_HANDLER(m_pScrollbar, CEventScrollbar::POS_CHANGED, onScrollbarPosChanged)
		CONTROL_EVENT_HANDLER(m_pScrollbar, CEventScrollbar::POS_CONTINUE_CHANGE_ENDED, onScrollbarPosContinueChangeEnded)
	END_EVENT_MAP_2

	void CCtrlList_Simple::setModel(Array<Object> *pModel)
	{
		m_pModel = pModel;

		verifyShowInfo();
	}

	void CCtrlList_Simple::setView(IViewList_Simple* pView)
	{
		m_pView = pView;
	}

	void CCtrlList_Simple::setAssociateScrollbar(CCtrlScrollbar* pScrollBar, BOOLEAN bControlScrollbarVisible, BOOLEAN bMovePageWhenClickUpDownArrow)
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

	void CCtrlList_Simple::setCurSel(INT iValue)
	{
		if (m_pModel == NULL)
		{
			return;
		}

		if (iValue < 0)
		{
			m_iCurSel = -1;
		}			
		else if ( iValue >= (INT)m_pModel->size())
		{
			m_iCurSel = - 1;
		}
		else 
		{
			if (m_iCurSel != iValue)
			{
				dispatchEvent(CEventList::CCP_FOCUS_CHANGED);
				m_iCurSel = iValue;
				dispatchEvent(CEventList::SELECT_CHANGED);
			}
		}
	}

	void CCtrlList_Simple::setCurTouchFocus(INT iValue)
	{
		if (m_pModel == NULL)
		{
			return;
		}

		if (iValue < 0)
		{
			m_iCurTouchFocus = -1;
		}			
		else if (iValue >= (INT)m_pModel->size())
		{
			m_iCurTouchFocus = - 1;
		}
		else
		{
			m_iCurTouchFocus = iValue;
		}
	}

	void CCtrlList_Simple::setCurCCPFocus(INT iValue)
	{
		if (m_pModel == NULL)
		{
			return;
		}

		if (iValue < 0)
		{
			m_iCurCCPFocus = -1;
		}			
		else if (iValue >= (INT)m_pModel->size())
		{
			m_iCurCCPFocus = m_pModel->size() - 1;
		}
		else
		{
			m_iCurCCPFocus = iValue;
		}
	}

	INT CCtrlList_Simple::getCurCCPFocus()
	{
		if (hasFocus())
		{
			return m_iCurCCPFocus;
		}
		
		return -1;
	}

	Object* CCtrlList_Simple::getCCPFocusedItem()
	{
		if (m_pModel == NULL)
		{
			return NULL;
		}

		if (hasFocus())
		{
			if (m_iCurCCPFocus == -1)
			{
				return NULL;
			}
			else
			{
				return m_pModel->getItem(m_iCurCCPFocus);
			}
		}
		else
		{
			return NULL;
		}
	}

	Object* CCtrlList_Simple::getDraggedItem()
	{
		if (m_pModel == NULL)
		{
			return NULL;
		}

		if (getCanDrag() && m_iCurTouchFocus >= 0)
		{
			return m_pModel->getItem(m_iCurTouchFocus);
		}
		else
		{
			return NULL;
		}
	}

	INT CCtrlList_Simple::getItemInsertToPos(CPoint pt)
	{
		if (m_aryComponent.size() == 0)
		{
			return -1;
		}

		for (INT i = 0; i < (INT)m_aryComponent.size(); i++)
		{
			CComponent* pComponent = m_aryComponent[i];

			if (pComponent->getVisible() && pComponent->hitTest(pt.getX(), pt.getY()))
			{
				return i + m_iShowItemIndex;
			}
		}

		return -1;
	}

	CComponent* CCtrlList_Simple::getItemComponent(Object* pObj)
	{
		if (m_pModel == NULL)
		{
			return NULL;
		}

		INT iIndex = m_pModel->getItemIndex(pObj);
		if (iIndex < 0)
		{
			return NULL;
		}

		if (m_iShowItemIndex <= iIndex && (iIndex - m_iShowItemIndex) < (INT)m_uiShowItemCount)
		{
			return m_aryComponent[iIndex - m_iShowItemIndex];
		}
		
		return NULL;
	}

	void CCtrlList_Simple::showTopItem(UINT uiIndex)
	{
		if (m_pModel == NULL || m_pModel->size() == 0)
		{
			m_uiShowItemCount = 0;
			updateComponentVisibility();
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
		
		INT iPos = (INT)(m_iShowTopIndex * m_uiItemHeight);
		showItems(iPos);
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->getModel()->setPosition(iPos);
			m_pScrollbar->refresh();
		}
	}

	void CCtrlList_Simple::refreshForAppendPrevPageItem(INT iItemCount)
	{
		if (iItemCount <= 0 || m_pModel == NULL)
		{
			return;
		}

		resetScrollbar(TRUE);

		if (m_bFirstBuild)
		{
			m_bFirstBuild = FALSE;

			updateViewForFirstBuild();
		}
		else
		{
			INT iInsertHeight = (INT)(iItemCount * m_uiItemHeight);

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
		}		

		m_bInAppendPrevPageMode = FALSE;

		setDirty();

		m_bOuterChangedData = TRUE;
	}

	void CCtrlList_Simple::refreshForAppendNextPageItem()
	{
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
		}

		m_bInAppendNextPageMode = FALSE;

		setDirty();

		m_bOuterChangedData = TRUE;
	}	

	void CCtrlList_Simple::backupViewState()
	{
		m_viewState.pObjSelectedItem = getSelectedItem();
		m_viewState.pObjCCPFocusItem = getCCPFocusedItem();
		m_viewState.iShowTopIndex = m_iShowTopIndex;
	}

	void CCtrlList_Simple::restoreViewState()
	{
		refresh();
		
		setSelectedItem(m_viewState.pObjSelectedItem);
		setCCPFocusedItem(m_viewState.pObjCCPFocusItem);
		showTopItem(m_viewState.iShowTopIndex);

		m_viewState.pObjSelectedItem = NULL;
		m_viewState.pObjCCPFocusItem = NULL;
		m_viewState.iShowTopIndex = -1;
	}

	void CCtrlList_Simple::lock()
	{
		CComponent::lock();
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->lock();
		}
	}

	void CCtrlList_Simple::unlock()
	{
		CComponent::unlock();
		
		if (m_pScrollbar != NULL)
		{
			m_pScrollbar->unlock();
		}
	}

	void CCtrlList_Simple::refresh()
	{
		CComponent::refresh();

		if (m_pView == NULL)
		{
			return;
		}

		if (m_aryComponent.size() == 0)
		{
			CComponent *pComponent = m_pView->getItemComponent(this);
			if (pComponent == NULL)
			{
				return;
			}

			m_uiItemHeight = pComponent->getPreferredSize().getY();

			UINT uiItemCount = getPreferredSize().getY() / m_uiItemHeight;

			setPreferredSize(getPreferredSize().getX(), uiItemCount * m_uiItemHeight);

			for (UINT i = 0; i < uiItemCount + 2; i++)
			{
				pComponent->setParent(this);
				pComponent->notAcceptFocus();
				pComponent->setX(0);
				m_aryComponent.addItem(pComponent);

				if ( i < uiItemCount + 1)
				{
					pComponent = m_pView->getItemComponent(this);
				}				
			}
		}

		resetScrollbar();
		
		updateViewForFirstBuild();

		if (m_pModel == NULL || m_pModel->size() == 0)
		{
			m_bFirstBuild = TRUE;
		}
		else
		{
			m_bFirstBuild = FALSE;
		}

		m_bOuterChangedData = TRUE;
	}

	void CCtrlList_Simple::littleRefresh()
	{
		CComponent::littleRefresh();
		
		showItemState();
	}

	void CCtrlList_Simple::destroySkin()
	{
		for (UINT i = 0; i < m_aryComponent.size(); i++)
		{
			m_aryComponent[i]->destroy();
		}
		m_aryComponent.clear();		
		
		CComponent::destroySkin();
	}

	void CCtrlList_Simple::destroy()
	{
		if (isDestroyed())
		{
			return;
		}

		CComponent::destroy();
	}

	void CCtrlList_Simple::buildRenderTree(CRenderGroup &renderGroup)
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
		for (UINT j = 0; j < m_uiShowItemCount; j++)
		{
			CComponent* pComponent = m_aryComponent[j];

			pComponent->setY(m_uiItemHeight * (m_iShowItemIndex + j) - m_scrollRect.getY());

			pComponent->buildRenderTree(m_renderGroup);
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

	BOOLEAN	CCtrlList_Simple::canSetFocus()
	{
		if (CComponent::canSetFocus() == FALSE)
		{
			return FALSE;
		}
		
		if (m_pModel == NULL || m_pModel->size() == 0)
		{
			return FALSE;
		}
		
		return TRUE;
	}

	void CCtrlList_Simple::setFocus(ECCPOperation eCCPOperation)
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

	void CCtrlList_Simple::releaseFocus()
	{
		//m_iCurCCPFocus = -1;
		CComponent::releaseFocus();
		dispatchEvent(CEventList::CCP_FOCUS_CHANGED);
	}

	BOOLEAN CCtrlList_Simple::moveFocusToLeft()
	{
		return FALSE;
	}

	BOOLEAN CCtrlList_Simple::moveFocusToRight()
	{
		return FALSE;
	}

	BOOLEAN CCtrlList_Simple::moveFocusToUp()
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

	BOOLEAN CCtrlList_Simple::moveFocusToDown()
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

	BOOLEAN CCtrlList_Simple::turnFocusToLeft(UINT uiStep)
	{
		return moveCCPFocusIndexDecrease(CComponent::CCP_ROTATE_LEFT, uiStep);
	}

	BOOLEAN CCtrlList_Simple::turnFocusToRight(UINT uiStep)
	{
		return moveCCPFocusIndexIncrease(CComponent::CCP_ROTATE_RIGHT, uiStep);
	}

	void CCtrlList_Simple::doFocusEnterPress()
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

	void CCtrlList_Simple::doFocusEnterRelease()
	{
		CComponent* pComponent = getCCPFocusComponent();
		
		if ( pComponent != NULL)
		{
			if (innerGetFocus())
			{					
				m_bOuterChangedData = FALSE;
				pComponent->doFocusEnterRelease();
				
				if (!m_bOuterChangedData)
				{
					checkSelectChanged();
				}
			}
		}
	}

	BOOLEAN CCtrlList_Simple::doFocusBack()
	{
		BOOLEAN bRet = CComponent::doFocusBack();
		dispatchEvent(CEventList::CCP_FOCUS_CHANGED);
		return bRet;
	}

	void CCtrlList_Simple::onMove(INT iX, INT iY)
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
						CComponent* pComponent = m_aryComponent[i + m_iShowItemIndex];
						pComponent->onLostHover();
					}

					dispatchEvent(CEventList::HOVER_CANCELED);
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
			CComponent* pComponent = m_aryComponent[i + m_iShowItemIndex];

			{
				pComponent->onMove(iX + m_scrollRect.getX(), iY + m_scrollRect.getY());
			}

			if (pComponent->hitTest(iX + m_scrollRect.getX(), iY + m_scrollRect.getY()) == FALSE)
			{
				continue;
			}

			bInInner = TRUE;

			if (m_iCurHoverFocus != i + m_iShowItemIndex)
			{
				if (m_iCurHoverFocus >= 0)
					dispatchEvent(CEventList::ITEM_HOVER_CANCELED);

				m_iCurHoverFocus = i + m_iShowItemIndex;
				dispatchEvent(CEventList::ITEM_HOVER_FOCUSED);
			}
		}

		if (bInInner == FALSE)
		{
			if (m_iCurHoverFocus >= 0)
				dispatchEvent(CEventList::ITEM_HOVER_CANCELED);
			m_iCurHoverFocus = -1;
		}
	}
	void CCtrlList_Simple::onLostHover()
	{
		if (getHoverFocus() == TRUE)
		{			
			for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
			{
				CComponent* pComponent = m_aryComponent[i];
				pComponent->onLostHover();
			}

			dispatchEvent(CEventList::HOVER_CANCELED);
			setHoverFocus(FALSE);
			m_iCurHoverFocus = -1;
		}
	}

	void CCtrlList_Simple::onWheel(CCursor::EWheelType eType)
	{
		if (getEnable())
		{
			if (getHoverFocus() == TRUE)
			{
				INT iTotalShowItemHeight = (INT)getTotalShowItemHeight();

				if (iTotalShowItemHeight > getPreferredSize().getY())
				{
					INT iLen = 0;

					if (eType == CCursor::WT_DOWN)
					{
						iLen = -1 * m_uiItemHeight;
					}
					else
					{
						iLen = m_uiItemHeight;
					}

					if (m_scrollRect.getY() - iLen <= 0 && eType == CCursor::WT_UP)
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
						//dispatchEvent(CEventTable::HOVER_CANCELED);
						//::SetClassLong(CScreenTouchServer::getInstance()->getScreenContext()->hWnd, GCL_HCURSOR,
						//	(LONG_PTR)LoadCursor(NULL, IDC_ARROW));

						startFlingAnimationExt(m_scrollRect.getY(), iLen);
					}
					//dispatchEvent(CEventTable::WHEEL_START);
				}
			}
		}
	}

	void CCtrlList_Simple::onTouchPress(INT iX, INT iY)
	{
		stopFlingAnimation();
		
		m_iTouchBeginPosY = iY;
		m_iShowBeginPosY = m_scrollRect.getY();
		m_iShowCurrentPosY = m_scrollRect.getY();
		
		for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
		{
			CComponent* pComponent = m_aryComponent[i];
			
			if (pComponent->hitTest(iX, iY) == FALSE)
			{
				continue;
			}
			
			if (m_iCurTouchFocus != i + m_iShowItemIndex)
			{
				m_iCurTouchFocus = i + m_iShowItemIndex;
				showItemState();
			}
			
			m_bOuterChangedData = FALSE;
			pComponent->onTouchPress(iX, iY);

			break;
		}
	}

	void CCtrlList_Simple::onTouchPressRelease(INT iX, INT iY)
	{
		if (   m_iCurTouchFocus >= 0
			&& m_iCurTouchFocus >= m_iShowItemIndex
			&& m_iCurTouchFocus - m_iShowItemIndex < (INT)m_uiShowItemCount)
		{
			switchCCPFocusForTouch();		
			
			m_bOuterChangedData = FALSE;
			m_aryComponent[m_iCurTouchFocus - m_iShowItemIndex]->onTouchPressRelease(iX, iY);
		}
		
		if (!m_bOuterChangedData)
		{
			checkSelectChanged();
		}
	}

	void CCtrlList_Simple::onTouchLongPress(INT iX, INT iY)
	{
		return;
		if (   m_iCurTouchFocus >= 0
			&& m_iCurTouchFocus >= m_iShowItemIndex
			&& m_iCurTouchFocus - m_iShowItemIndex < (INT)m_uiShowItemCount)
		{
			m_bOuterChangedData = FALSE;
			m_aryComponent[m_iCurTouchFocus - m_iShowItemIndex]->onTouchLongPress(iX, iY);
		}
	}

	void CCtrlList_Simple::onTouchContinuePress(INT iX, INT iY)
	{
		return;
		if (   m_iCurTouchFocus >= 0
			&& m_iCurTouchFocus >= m_iShowItemIndex
			&& m_iCurTouchFocus - m_iShowItemIndex < (INT)m_uiShowItemCount)
		{
			m_bOuterChangedData = FALSE;
			m_aryComponent[m_iCurTouchFocus - m_iShowItemIndex]->onTouchContinuePress(iX, iY);
		}
	}

	void CCtrlList_Simple::onTouchLongPressRelease(INT iX, INT iY)
	{
		return;
		if (   m_iCurTouchFocus >= 0
			&& m_iCurTouchFocus >= m_iShowItemIndex
			&& m_iCurTouchFocus - m_iShowItemIndex < (INT)m_uiShowItemCount)
		{
			switchCCPFocusForTouch();
			
			m_bOuterChangedData = FALSE;
			m_aryComponent[m_iCurTouchFocus - m_iShowItemIndex]->onTouchLongPressRelease(iX, iY);
		}
		
		if (!m_bOuterChangedData)
		{
			checkSelectChanged();
		}
	}

	void CCtrlList_Simple::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
	{
		return;
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

	void CCtrlList_Simple::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
	{
		return;
		if (   m_iCurTouchFocus >= 0
			&& m_iCurTouchFocus >= m_iShowItemIndex
			&& m_iCurTouchFocus - m_iShowItemIndex < (INT)m_uiShowItemCount)
		{
			//switchCCPFocusForTouch();
			
			m_aryComponent[m_iCurTouchFocus - m_iShowItemIndex]->setParentMoving(TRUE);
			m_aryComponent[m_iCurTouchFocus - m_iShowItemIndex]->onTouchMoveRelease(iX, iY, iSpeedX, iSpeedY);
			m_aryComponent[m_iCurTouchFocus - m_iShowItemIndex]->setParentMoving(FALSE);
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

	void CCtrlList_Simple::onTouchDrag(INT iX, INT iY)
	{
		if (m_iCurTouchFocus >= 0)
		{
			m_ptCurrentCursor.setX(iX);
			m_ptCurrentCursor.setY(iY);
			dispatchEvent(CEventList::ITEM_DRAGGING);
		}
	}

	void CCtrlList_Simple::onTouchDragRelease(INT iX, INT iY)
	{
		if (   m_iCurTouchFocus >= 0 
			&& m_iCurTouchFocus >= m_iShowItemIndex
			&& m_iCurTouchFocus - m_iShowItemIndex < (INT)m_uiShowItemCount)
		{
			m_ptCurrentCursor.setX(iX);
			m_ptCurrentCursor.setY(iY);

			switchCCPFocusForTouch();

			CComponent *pComponent = m_aryComponent[m_iCurTouchFocus - m_iShowItemIndex];
			
			pComponent->onTouchDragRelease(iX, iY);
			
			dispatchEvent(CEventList::ITEM_DROPPED);
			
			m_iCurTouchFocus = -1;
			showItemState();
			m_ptCurrentCursor.setX(0);
			m_ptCurrentCursor.setY(0);
		}
	}

	BOOLEAN CCtrlList_Simple::contains(Object *pTarget)
	{
		for (INT j = 0; j < (INT)m_aryComponent.size(); j++)
		{
			CComponent* pComponent = m_aryComponent[j];
			if (pComponent != NULL && pComponent == pTarget)
			{
				return TRUE;
			}
		}

		return FALSE;
	}	

	void CCtrlList_Simple::updateViewForFirstBuild()
	{
		m_scrollRect.setWidth(getPreferredSize().getX());
		m_scrollRect.setHeight(getPreferredSize().getY());

		m_iCurTouchFocus = -1;
		m_iShowTopIndex = 0;

		verifyShowInfo();
		
		showItemState();

		showItems(0);
	}

	void CCtrlList_Simple::checkNeedMorePage()
	{
		if (m_pModel == NULL)
		{
			return;
		}

		if (   m_bReachToHead == FALSE
			&& m_uiItemsPerPage > 0
			&& m_iShowItemIndex < (INT)m_uiItemsPerPage - 1
			&& m_bInAppendPrevPageMode == FALSE )
		{
			m_bInAppendPrevPageMode = TRUE;
			dispatchEvent(CEventList::WANT_PREV_PAGE);
		}

		if (   m_bReachToTail == FALSE
			&& m_uiItemsPerPage > 0
			&& m_pModel->size() > 0
			&& m_iShowItemIndex >= (INT)m_pModel->size() - (INT)m_uiItemsPerPage
			&& m_bInAppendNextPageMode == FALSE )
		{
			m_bInAppendNextPageMode = TRUE;
			dispatchEvent(CEventList::WANT_NEXT_PAGE);
		}
	}

	void CCtrlList_Simple::showItems(INT iTop)
	{
		m_scrollRect.setY(iTop);
		drawListCCPFocs();

		if (m_pScrollbar != NULL && m_bControlScrollbarVisible == FALSE)
		{
			m_pScrollbar->getModel()->setPreviousStep(m_uiItemHeight);
			m_pScrollbar->getModel()->setNextStep(m_uiItemHeight);
		}

		setDirty();
	}

	void CCtrlList_Simple::showItemState()
	{		
		if (m_pModel == NULL)
		{
			return;
		}

		CComponent *pComponent = NULL;

		for (INT i = 0; i < (INT)m_uiShowItemCount; i++)
		{
			BOOLEAN bSelect = (m_iCurSel == i + m_iShowItemIndex ? TRUE : FALSE);	
			BOOLEAN bFocus = (m_iCurTouchFocus == i + m_iShowItemIndex ? TRUE : FALSE);	
			
			if (innerGetFocus())
			{
				if ( m_CCPSkin == NULL && m_iCurCCPFocus == (i + m_iShowItemIndex) )
				{
					bFocus = TRUE;
				}
			}
			
			pComponent = m_aryComponent[i];
			pComponent->setSelected(bSelect);
			pComponent->setTouchFocus(bFocus);
			pComponent->refresh();
			
			m_pView->drawItem(this, m_pModel->getItem(i + m_iShowItemIndex), pComponent, bSelect, bFocus);
		}
		
		drawListCCPFocs();
	}

	void CCtrlList_Simple::resetScrollbar(BOOLEAN bMaintainPos)
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

	UINT CCtrlList_Simple::getTotalShowItemHeight()
	{
		if (m_pModel == NULL)
		{
			return 0;
		}

		return m_uiItemHeight * m_pModel->size();
	}

	BOOLEAN CCtrlList_Simple::needVerifyShowInfo(INT iPos)
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

	void CCtrlList_Simple::verifyShowInfo()  //need further check and consider
	{
		m_iShowItemIndex = m_iShowTopIndex - 1;
		if (m_iShowItemIndex < 0)
		{
			m_iShowItemIndex = 0;
		}		
		
		m_uiShowItemCount = 0;

		if (m_pModel == NULL)
		{
			updateComponentVisibility();
			return;
		}

		if (m_pModel->size() != 0)
		{
			INT iHeight = 0;
			for (INT i = m_iShowTopIndex; i < (INT)m_pModel->size(); i++)
			{
				m_uiShowItemCount++;

				iHeight += m_uiItemHeight;
				
				if (iHeight >= getPreferredSize().getY())
				{
					break;
				}
			}
			
			m_uiShowItemCount += (m_iShowTopIndex - m_iShowItemIndex) + 1;
			
			if (m_iShowItemIndex + m_uiShowItemCount > m_pModel->size())
			{
				m_uiShowItemCount = m_pModel->size() - m_iShowItemIndex;
			}
		}

		updateComponentVisibility();
	}

	void CCtrlList_Simple::updateComponentVisibility()
	{
		for (UINT i = 0; i < m_aryComponent.size(); i++)
		{
			m_aryComponent[i]->setVisible(FALSE);
		}

		if (m_pModel == NULL)
		{
			return;
		}

		for (UINT j = 0; j < m_uiShowItemCount; j++)
		{
			m_aryComponent[j]->setVisible(TRUE);
		}
	}

	UINT CCtrlList_Simple::calculateShowItemIndex(INT iPos)
	{
		if (m_pModel == NULL)
		{
			return 0;
		}	

		UINT uiSize = m_pModel->size();
		if (uiSize == 0)
		{
			return 0;
		}		

		if (iPos < 0)
		{
			return 0;
		}

		UINT uiIndex = iPos / m_uiItemHeight;

		if (uiIndex >= uiSize)
		{
			return uiSize - 1;
		}
		else
		{
			return uiIndex;
		}		
	}

	void CCtrlList_Simple::checkSelectChanged()
	{
		INT iOldCurSel = m_iCurSel;
		
		if (m_iCurTouchFocus != -1)
		{
			dispatchEvent(CEventList::CCP_FOCUS_CHANGED);
			m_iCurSel = m_iCurTouchFocus;
		}
		
		m_iCurTouchFocus = -1;
		showItemState();
		
		if (iOldCurSel != m_iCurSel)
		{
			dispatchEvent(CEventList::SELECT_CHANGED);
		}
	}

	INT CCtrlList_Simple::getFirstCCPItemIndex()
	{
		if (m_pModel == NULL || m_pModel->size() == 0)
		{
			return -1;
		}

		if ( m_iCurCCPFocus == -1 )
		{
			return m_iShowTopIndex; 
		}
		
		return m_iCurCCPFocus;
	}

	INT CCtrlList_Simple::getLastCCPItemIndex()
	{
		if (m_pModel == NULL || m_pModel->size() == 0)
		{
			return -1;
		}
		
		if (m_iCurCCPFocus != -1)
		{
			return m_iCurCCPFocus;
		}
		
		for ( INT i = (INT)m_uiShowItemCount - 1; i >= 0; i-- )
		{
			if ((m_iShowItemIndex + i) * (INT)m_uiItemHeight < m_scrollRect.getY() + (INT)m_scrollRect.getHeight())
			{
				return m_iShowItemIndex + i;
			}
		}
		
		return -1;
	}

	void CCtrlList_Simple::adjustCCPFocusDisplayPos()
	{
		if ( m_iCurCCPFocus == -1 || m_pModel == NULL || m_pModel->size() == 0)
		{
			return;
		}
		
		if (m_iCurCCPFocus >= (INT)m_pModel->size())
		{
			m_iCurCCPFocus = 0;
		}
		
		if ( m_iCurCCPFocus < m_iShowTopIndex )
		{
			showTopItem(m_iCurCCPFocus);
			return;
		}
		
		INT i = 0;
		while ( m_iCurCCPFocus - i >= 0 && (m_iCurCCPFocus - i) * (INT)m_uiItemHeight + DISTANCE_OFFSET > m_scrollRect.getY() + (INT)m_scrollRect.getHeight() )
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

	void CCtrlList_Simple::drawListCCPFocs()
	{
		if (   !innerGetFocus() 
			|| m_pModel == NULL
			|| m_iCurCCPFocus == -1 
			|| m_iCurCCPFocus >= (INT)m_pModel->size()
			|| m_CCPSkin == NULL   )
		{
			return;
		}
		
		m_CCPSkin->setX(0 - ((INT)(m_CCPSkin->getWidth()) - (INT)(getPreferredSize().getX())) / 2);
		m_CCPSkin->setY(m_iCurCCPFocus * (INT)m_uiItemHeight - m_scrollRect.getY() - ((INT)(m_CCPSkin->getHeight()) - (INT)(m_uiItemHeight)) / 2);
	}

	CComponent* CCtrlList_Simple::getCCPFocusComponent()
	{
		if (   m_pModel == NULL 
			|| m_iCurCCPFocus == -1
			|| m_iCurCCPFocus >= (INT)m_pModel->size()
			|| m_iCurCCPFocus < m_iShowItemIndex
			|| m_iCurCCPFocus - m_iShowItemIndex >= (INT)m_uiShowItemCount)
		{
			return NULL;
		}
		else
		{
			return m_aryComponent[m_iCurCCPFocus - m_iShowItemIndex];
		}
	}

	BOOLEAN CCtrlList_Simple::moveCCPFocusIndexDecrease(ECCPOperation eCCPOperation,UINT uiStep)
	{
		if (m_pModel == NULL)
		{
			return FALSE;
		}

		if ( eCCPOperation != CComponent::CCP_MOVE_UP && eCCPOperation != CComponent::CCP_ROTATE_LEFT )
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
		
		checkNeedMorePage();

		if (m_iCurCCPFocus <= 0)
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
			
		m_iCurCCPFocus--;
		
		adjustCCPFocusDisplayPos();
		dispatchEvent(CEventList::CCP_FOCUS_CHANGED);

		setDirty();

		return TRUE;
	}

	BOOLEAN CCtrlList_Simple::moveCCPFocusIndexIncrease(ECCPOperation eCCPOperation,UINT uiStep)
	{
		if (m_pModel == NULL)
		{
			return FALSE;
		}

		if ( eCCPOperation != CComponent::CCP_MOVE_DOWN && eCCPOperation != CComponent::CCP_ROTATE_RIGHT )
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
		
		checkNeedMorePage();

		if (m_iCurCCPFocus >= (INT)m_pModel->size() - 1)
		{
			return canMoveFocusOut() ? FALSE : TRUE;
		}
		
		m_iCurCCPFocus++;
		
		adjustCCPFocusDisplayPos();
		dispatchEvent(CEventList::CCP_FOCUS_CHANGED);

		setDirty();

		return TRUE;
	}

	void CCtrlList_Simple::switchCCPFocusForTouch()
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

	void CCtrlList_Simple::startElasticityAnimation(INT iStartValue, INT iEndValue)
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

	void CCtrlList_Simple::stopElasticityAnimation()
	{
		if ( m_pAmtElasticity != NULL )
		{
			m_pAmtElasticity->stop();
			
			unlock();

			if (m_bControlScrollbarVisible)
			{
				m_pScrollbar->setVisible(FALSE);
			}

			dispatchEvent(CEventList::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlList_Simple::startFlingAnimation(INT iStartValue, INT iSpeed)
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

	void CCtrlList_Simple::startFlingAnimationExt(INT iStartPos, INT iDeltaPos)
	{
		if (m_pAmtFling != NULL)
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

			dispatchEvent(CEventList::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlList_Simple::stopFlingAnimation()
	{
		if ( m_pAmtFling != NULL )
		{
			m_pAmtFling->stop();
		
			unlock();

			if (m_bControlScrollbarVisible)
			{
				m_pScrollbar->setVisible(FALSE);
			}

			dispatchEvent(CEventList::ITEM_SCROLL_STOP);
		}
	}

	void CCtrlList_Simple::onScrollbarPosChanged(CComponent *pTarget)
	{
		INT iPos = m_pScrollbar->getModel()->getPosition();

		CCtrlScrollbar::EPosChanged eType = m_pScrollbar->getPosChangedType();

		if (needVerifyShowInfo(iPos))
		{
			verifyShowInfo();
			showItemState();
		}
			
		showItems(iPos);
		
		if (eType != CCtrlScrollbar::PC_SLIDER)
		{
			checkNeedMorePage();
		}
	}

	void CCtrlList_Simple::onScrollbarPosContinueChangeEnded(CComponent *pTarget)
	{
		checkNeedMorePage();
	}
}