#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlSelector::CCtrlSelector()
		: m_strFocusSel(STRING_NULL)
		, m_iCurCCPFocus(-1)
		, m_eLayout(LT_LANDSCRAPE)
		, m_pEventListenr(NULL)
	{
		setBaseView(&m_view);
	}

	CCtrlSelector::~CCtrlSelector()
	{
		;
	}

	void CCtrlSelector::setItemGroup(const String& strName, INT iGroupID)
	{
		if (iGroupID <= 0)
		{
			return;
		}

		CCtrlButton* pBtn = m_mapButton[strName];
		if (pBtn == NULL)
		{
			return;
		}

		CButtonGroup* pGroup = m_mapGroup[iGroupID];

		if (pGroup == NULL)
		{
			pGroup = new CButtonGroup();
			m_mapGroup.addItem(iGroupID, pGroup);
		}

		pGroup->add(pBtn);
	}
	
	void CCtrlSelector::setItemSelect(const String& strName, BOOLEAN bSelected, INT iGroupID)
	{
		CCtrlButton* pBtn = m_mapButton[strName];
		if (pBtn == NULL)
		{
			return;
		}

		if (iGroupID == 0)
		{
			pBtn->setSelected(bSelected);
			pBtn->littleRefresh();
		}
		else
		{
			CButtonGroup* pBtnGroup = m_mapGroup[iGroupID];
			if (pBtnGroup == NULL)
			{
				return;
			}

			if (bSelected)
			{
				pBtnGroup->setSelected(pBtn);
			}
			else
			{
				pBtnGroup->removeSelected();
			}
		}
	}

	void CCtrlSelector::clearSelectedItem(INT iGroupID)
	{
		if (iGroupID == 0)
		{
			CCtrlButton* pBtn = NULL;
			m_mapButton.resetIterator();
			while(m_mapButton.hasMore())
			{
				pBtn = m_mapButton.next();
				if (pBtn->getSelected())
				{
					pBtn->setSelected(FALSE);
					pBtn->littleRefresh();
				}
			}

			CButtonGroup* pBtnGroup = NULL;
			m_mapGroup.resetIterator();
			while(m_mapGroup.hasMore())
			{
				pBtnGroup = m_mapGroup.next();
				if (pBtnGroup != NULL)
				{
					pBtnGroup->removeSelected();
				}
			}
		}
		else if (iGroupID > 0)
		{
			CButtonGroup* pBtnGroup = m_mapGroup[iGroupID];
			if (pBtnGroup != NULL)
			{
				pBtnGroup->removeSelected();
			}
		}
	}

	void CCtrlSelector::setItemEnable(const String& strName, BOOLEAN bEnable)
	{	
		CCtrlButton *pBtn = m_mapButton[strName];
		if (pBtn == NULL)
		{
			return;
		}

		pBtn->setEnable(bEnable);
		pBtn->littleRefresh();
	}


	void CCtrlSelector::setItemVisible(const String& strName, BOOLEAN bVisible)
	{
		CCtrlButton *pBtn = m_mapButton[strName];
		if (pBtn == NULL)
		{
			return;
		}

		pBtn->setVisible(bVisible);
		pBtn->littleRefresh();
	}

	CCtrlButton* CCtrlSelector::getButton(const String& strName)
	{
		CCtrlButton* pBtn = m_mapButton[strName];
		if (pBtn == NULL)
		{
			pBtn = new CCtrlButton();
	
			pBtn->setParent(this);
			pBtn->notAcceptFocus();

			if (m_pEventListenr != NULL)
			{
				pBtn->addEventListener(m_pEventListenr);
			}

			m_mapButton.addItem(strName, pBtn);
			m_aryButton.addItem(pBtn);
		}

		return pBtn;
	}

	void CCtrlSelector::addEventListener(IEventListener *pListener)
	{
		CComponent::addEventListener(pListener);

		m_pEventListenr = pListener;
	}

	void CCtrlSelector::setCCPFocusItem(const String& strName, BOOLEAN bAutoRefresh)
	{
		CCtrlButton* pBtn = getButton(strName);
		if (pBtn == NULL || !pBtn->getVisible() ||!pBtn->getEnable())
		{
			return;
		}

 		releaseButtonFocus();

		m_iCurCCPFocus = m_aryButton.getItemIndex(pBtn);
 
		if (bAutoRefresh)
		{
			setUniqueFocus();
		}
	}

	const String& CCtrlSelector::getCCPFocusItem()
	{
		if (!hasFocus())
		{
			return STRING_NULL;
		}

		CCtrlButton* pBtn = getCCPFocusButton();
		if (pBtn != NULL)
		{
			return pBtn->getName();
		}

 		return STRING_NULL;
	}

	const String& CCtrlSelector::getCCPFocusLastItem()
	{
		CCtrlButton* pBtn = getCCPFocusButton();
		if (pBtn != NULL)
		{
			return pBtn->getName();
		}

		return STRING_NULL;
	}

	void CCtrlSelector::refresh()
	{ 
		CComponent::refresh();

		m_mapButton.resetIterator();
		while (m_mapButton.hasMore())
		{
			m_mapButton.next()->refresh();
		}
 
 		drawItemCCPFocs();
	}

	void CCtrlSelector::littleRefresh()
	{
		refresh();
	}

	void CCtrlSelector::destroySkin()
	{
		m_mapButton.resetIterator();
		while(m_mapButton.hasMore())
		{
			m_mapButton.next()->destroySkin();
		}

		CComponent::destroySkin();
	}

	void CCtrlSelector::destroy()
	{
		if (isDestroyed())
		{
			return;
		}
		
		m_mapGroup.resetIterator();
		while (m_mapGroup.hasMore())
		{
			m_mapGroup.next()->clear();
		}
		m_mapGroup.clear();

		CCtrlButton* pBtn = NULL;
		m_mapButton.resetIterator();
		while(m_mapButton.hasMore())
		{
			pBtn = m_mapButton.next();

			pBtn->setParent(NULL);
			pBtn->destroy();
		}
		m_mapButton.clear();
		m_aryButton.clear();

		CComponent::destroy();
	}

	void CCtrlSelector::buildRenderTree(CRenderGroup &renderGroup)
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

			m_aryButton.resetIterator();
			while (m_aryButton.hasMore())
			{
				m_aryButton.next()->buildRenderTree(m_renderGroup);
			}
 			
			if (m_skin.isNotNull())
			{
				buildRenderTreeFromCCP(m_renderGroup, 1);
			}
			else 
			{
				buildRenderTreeFromCCP(m_renderGroup);
			}
		}
	}

	BOOLEAN CCtrlSelector::canSetFocus()
	{
		if (CComponent::canSetFocus() == FALSE)
		{
			return FALSE;
		}

		CCtrlButton* pBtn = NULL;

		m_mapButton.resetIterator();
		while (m_mapButton.hasMore())
		{
			pBtn = m_mapButton.next();
			if ( pBtn->getVisible() && pBtn->getEnable() )
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	void CCtrlSelector::setFocus(ECCPOperation eCCPOperation)
	{
		innerSetFocus(TRUE);

		if ( m_iCurCCPFocus == -1 )
		{
			switch(eCCPOperation)
			{
			case CComponent::CCP_MOVE_DOWN:
			case CComponent::CCP_MOVE_RIGHT:
			case CComponent::CCP_ROTATE_RIGHT:				
				m_iCurCCPFocus = getFirstCanSetFocusItem();
				break;

			case CComponent::CCP_MOVE_LEFT:
			case CComponent::CCP_MOVE_UP:
			case CComponent::CCP_ROTATE_LEFT:
				m_iCurCCPFocus = getLastCanSetFocusItem();
				break;

			default:
				break;
			}
		}

		refresh();
		setDirty();

		dispatchEvent(CEventSelector::CCP_FOCUS_CHANGED);
	}

	void CCtrlSelector::releaseFocus()
	{
		releaseButtonFocus();

		CComponent::releaseFocus();

		dispatchEvent(CEventSelector::CCP_FOCUS_CHANGED);
	}

	BOOLEAN CCtrlSelector::moveFocusToLeft()
	{			
		if ( m_eLayout == CCtrlSelector::LT_LANDSCRAPE )
		{
			if (canMoveFocus())
			{
				return moveFocusToPrevious();
			}
			else
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	BOOLEAN CCtrlSelector::moveFocusToRight()
	{
		if ( m_eLayout == CCtrlSelector::LT_LANDSCRAPE )
		{
			if (canMoveFocus())
			{
				return moveFocusToNext();
			}
			else
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	BOOLEAN CCtrlSelector::moveFocusToUp()
	{
		if ( m_eLayout == CCtrlSelector::LT_PROTRAIT )
		{
			if (canMoveFocus())
			{
				return moveFocusToPrevious();
			}
			else
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}
		}
		else
		{
			return FALSE;
		}			
	}

	 BOOLEAN CCtrlSelector::moveFocusToDown()
	{
		if ( m_eLayout == CCtrlSelector::LT_PROTRAIT )
		{
			if (canMoveFocus())
			{
				return moveFocusToNext();
			}
			else
			{
				return canMoveFocusOut() ? FALSE : TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	 BOOLEAN CCtrlSelector::turnFocusToLeft(UINT uiStep)
	{
		return moveFocusToPrevious();
	}

	BOOLEAN CCtrlSelector::turnFocusToRight(UINT uiStep)
	{
		return moveFocusToNext();
	}

	void CCtrlSelector::doFocusEnterPress()
	{
		CCtrlButton* pBtn = getCCPFocusButton();
		if (pBtn != NULL)
		{
			pBtn->doFocusEnterPress();
		}
	}

	void CCtrlSelector::doFocusEnterRelease()
	{
		CCtrlButton* pBtn = getCCPFocusButton();
		if (pBtn != NULL)
		{
			pBtn->doFocusEnterRelease();
		}
	}

	void CCtrlSelector::onMove(INT iX, INT iY)
	{
		for (INT i = (INT)m_aryButton.size() - 1; i >= 0; i--)
		{
			CCtrlButton* pBtn = m_aryButton[i];

			pBtn->onMove(iX, iY);
		}
	}

	void CCtrlSelector::onTouchPress(INT iX, INT iY)
	{
		for (INT i = (INT)m_aryButton.size() - 1; i >= 0; i--)
		{
			CCtrlButton* pBtn = m_aryButton[i];

			pBtn->setTouchFocus(FALSE);
		}

		for (INT i = (INT)m_aryButton.size() - 1; i >= 0; i--)
		{
			CCtrlButton* pBtn = m_aryButton[i];

			if (pBtn->hitTest(iX, iY) == TRUE)
			{
				m_strFocusSel = pBtn->getName();

				pBtn->onTouchPress(iX, iY);
				break;
			}
		}
	}

	void CCtrlSelector::onTouchPressRelease(INT iX, INT iY)
	{
		if (m_strFocusSel != STRING_NULL)
		{
			switchCCPFocusForTouch();

			CCtrlButton* pBtn = m_mapButton[m_strFocusSel];

			pBtn->onTouchPressRelease(iX, iY);
				
			m_strFocusSel = STRING_NULL;
		}
	}

	void CCtrlSelector::onTouchLongPress(INT iX, INT iY)
	{
		if (m_strFocusSel != STRING_NULL)
		{
			CCtrlButton* pBtn = m_mapButton[m_strFocusSel];

			pBtn->onTouchLongPress(iX, iY);
		}
	}

	void CCtrlSelector::onTouchContinuePress(INT iX, INT iY)
	{
		if (m_strFocusSel != STRING_NULL)
		{
			CCtrlButton* pBtn = m_mapButton[m_strFocusSel];

			pBtn->onTouchContinuePress(iX, iY);
		}
	}

	void CCtrlSelector::onTouchLongPressRelease(INT iX, INT iY)
	{
		if (m_strFocusSel != STRING_NULL)
		{
			switchCCPFocusForTouch();

			CCtrlButton* pBtn = m_mapButton[m_strFocusSel];

			pBtn->onTouchLongPressRelease(iX, iY);

			m_strFocusSel = STRING_NULL;
		}
	}

	void CCtrlSelector::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
	{
		;
	}

	void CCtrlSelector::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
	{
		if (m_strFocusSel != STRING_NULL)
		{
			CCtrlButton* pBtn = m_mapButton[m_strFocusSel];

			pBtn->onTouchMoveRelease(iX, iY, iSpeedX, iSpeedY);

			m_strFocusSel = STRING_NULL;
		}
	}

	void CCtrlSelector::onTouchDrag(INT iX, INT iY)
	{
		;
	}

	void CCtrlSelector::onTouchDragRelease(INT iX, INT iY)
	{
		if (m_strFocusSel != STRING_NULL)
		{
			CCtrlButton* pBtn = m_mapButton[m_strFocusSel];

			pBtn->onTouchDragRelease(iX, iY);

			m_strFocusSel = STRING_NULL;
		}
	}

	BOOLEAN CCtrlSelector::contains(Object* pTarget)
	{
		m_mapButton.resetIterator();
		while (m_mapButton.hasMore())
		{
			if (m_mapButton.next() == pTarget)
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	void CCtrlSelector::drawItemCCPFocs()
	{
		CCtrlButton* pBtn = getCCPFocusButton();
		if ( pBtn == NULL )
		{
			return;
		}

		if (innerGetFocus())
		{
			if ( m_CCPSkin.isNotNull() )
			{
				m_CCPSkin->setX(pBtn->getX() - ((INT)m_CCPSkin->getWidth() - (INT)pBtn->getPreferredSize().getX())  / 2);
				m_CCPSkin->setY(pBtn->getY() - ((INT)m_CCPSkin->getHeight() - (INT)pBtn->getPreferredSize().getY())  / 2);
			}
			else
			{
				pBtn->setFocus();
			}

		}
		else
		{
			if (m_CCPSkin == NULL)
			{
				pBtn->releaseFocus();
			}
		}

		return;
	}

	BOOLEAN CCtrlSelector::moveFocusToPrevious()
	{
		INT iPrevious = getPreviousCanSetFocusItem(m_iCurCCPFocus);

		if (iPrevious >= 0)
		{
			releaseButtonFocus();

			m_iCurCCPFocus = iPrevious;
			refresh();

			setDirty();

			dispatchEvent(CEventSelector::CCP_FOCUS_CHANGED);

			return TRUE;
		}

		return canMoveFocusOut() ? FALSE : TRUE;
	}

	BOOLEAN	CCtrlSelector::moveFocusToNext()
	{
		INT iNext = getNextCanSetFocusItem(m_iCurCCPFocus);

		if (iNext >= 0 && iNext < (INT)m_aryButton.size() )
		{
			releaseButtonFocus();

			m_iCurCCPFocus = iNext;
			refresh();

			setDirty();

			dispatchEvent(CEventSelector::CCP_FOCUS_CHANGED);
			
			return TRUE;
		}

		return canMoveFocusOut() ? FALSE : TRUE;
	}

	CCtrlButton* CCtrlSelector::getCCPFocusButton()
	{
		if ( m_iCurCCPFocus < 0 || m_iCurCCPFocus >=  (INT)m_aryButton.size() )
		{
			return NULL;
		}

		return ( m_aryButton[m_iCurCCPFocus] );
	}

	void CCtrlSelector::releaseButtonFocus()
	{
		if (m_CCPSkin.isNull())
		{
			CCtrlButton* pBtn = getCCPFocusButton();
			if (pBtn != NULL)
			{
				pBtn->releaseFocus();
			}
		}
	}

	void CCtrlSelector::switchCCPFocusForTouch()
	{	
		if (ENV::getCCPBindTouch() == FALSE)
		{
			return;
		}

		if (isSameButton(m_iCurCCPFocus, m_strFocusSel))
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
				setCCPFocusItem(m_strFocusSel, FALSE);
				setFocus();
			}
			else
			{
				CCtrlButton* pBtn = getButton(m_strFocusSel);

				if (pBtn != NULL && pBtn->getVisible() && pBtn->getEnable())
				{
					releaseButtonFocus();
					m_iCurCCPFocus = m_aryButton.getItemIndex(pBtn);
					ENV::switchCCPFocus(this);
				}
			}
		}
	}

	BOOLEAN	CCtrlSelector::isSameButton(INT iIndex, const String& strName)
	{
		if ( iIndex == m_aryButton.getItemIndex(getButton(strName)) )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	INT	CCtrlSelector::getFirstCanSetFocusItem()
	{
		CCtrlButton* pBtn = NULL;
		for (INT i = 0; i < (INT)m_aryButton.size(); i++)
		{
			pBtn = m_aryButton[i];

			if ( pBtn->getVisible() && pBtn->getEnable() )
			{
				return i;
			}
		}

		return -1;
	}

	INT	CCtrlSelector::getLastCanSetFocusItem()
	{
		CCtrlButton* pBtn = NULL;
		for (INT i = (INT)m_aryButton.size() - 1; i >= 0; i--)
		{
			pBtn = m_aryButton[i];

			if ( pBtn->getVisible() && pBtn->getEnable() )
			{
				return i;
			}
		}

		return -1;
	}

	INT	CCtrlSelector::getPreviousCanSetFocusItem(INT iIndex)
	{
		CCtrlButton* pBtn = NULL;
		for (INT i = iIndex - 1; i >= 0; i--)
		{
			pBtn = m_aryButton[i];

			if ( pBtn->getVisible() && pBtn->getEnable() )
			{
				return i;
			}
		}

		return -1;
	}

	INT	CCtrlSelector::getNextCanSetFocusItem(INT iIndex)
	{
		CCtrlButton* pBtn = NULL;
		for (INT i = iIndex + 1; i < (INT)m_aryButton.size(); i++)
		{
			pBtn = m_aryButton[i];

			if ( pBtn->getVisible() && pBtn->getEnable() )
			{
				return i;
			}
		}

		return -1;
	}
}