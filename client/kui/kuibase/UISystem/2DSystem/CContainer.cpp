#include "OpenHMI.h"

namespace OpenHMI
{
	CContainer::CContainer()
		: m_uiType(0)
		, m_aryChildren(FALSE)
		, m_bMoveCCPFocusInAddOrder(FALSE)
		, m_pHittedComponent(NULL)
		, m_pLastHittedComponent(NULL)
	{
		setBaseView(&m_view);
		asContainer();
	}

	CContainer::~CContainer()
	{
		;
	}

	BEGIN_EVENT_MAP(CContainer)
	END_EVENT_MAP_2

	UINT CContainer::getComponentCount()
	{
		return m_aryChildren.size();
	}

	CComponent* CContainer::getComponentAt(UINT uiIndex)
	{
		return m_aryChildren[uiIndex];
	}

	CComponent* CContainer::getComponentByID(UINT uiID)
	{
		CComponent *pComponent = NULL;

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			pComponent = m_aryChildren.next();

			if (pComponent->getID() == uiID)
			{
				return pComponent;
			}
		}

		return NULL;
	}

	INT CContainer::getComponentPos(CComponent *pComponent)
	{
		return m_aryChildren.getItemIndex(pComponent);
	}

	void CContainer::addComponent(CComponent *pComponent)
	{
		m_aryChildren.deleteItem(pComponent);
		m_aryChildren.addItem(pComponent);
		pComponent->setParent(this);

		setDirty();
	}

	void CContainer::insertComponent(CComponent *pComponent, UINT uiIndex)
	{
		m_aryChildren.deleteItem(pComponent);
		m_aryChildren.insertItem(uiIndex, pComponent);
		pComponent->setParent(this);

		setDirty();
	}

	void CContainer::removeComponentAt(UINT uiIndex)
	{
		m_aryChildren.deleteItemByIndex(uiIndex);

		setDirty();
	}

	void CContainer::removeComponentByID(UINT uiID)
	{
		CComponent *pComponent = NULL;

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			pComponent = m_aryChildren.next();

			if (pComponent->getID() == uiID)
			{
				m_aryChildren.deleteItem(pComponent);

				setDirty();
				return;
			}
		}
	}

	void CContainer::removeComponent(CComponent *pComponent)
	{
		m_aryChildren.deleteItem(pComponent);
		setDirty();
	}

	void CContainer::removeAllComponents()
	{
		m_aryChildren.clear();
		setDirty();
	}

	BOOLEAN CContainer::containsComponent(CComponent *pComponent)
	{
		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			CComponent *pExistComponet = m_aryChildren.next();

			if (pExistComponet == pComponent)
			{
				return TRUE;
			}

			if (pExistComponet->isContainer())
			{
				CContainer *pContainer = static_cast<CContainer*>(pExistComponet);

				if (pContainer->containsComponent(pComponent))
				{
					return TRUE;
				}
			}
		}

		return FALSE;
	}

	void CContainer::hideComponents()
	{
		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			m_aryChildren.next()->setVisible(FALSE);
		}
	}

	void CContainer::showComponents()
	{
		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			m_aryChildren.next()->setVisible(TRUE);
		}
	}

	void CContainer::resetLayout()
	{
		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			CComponent *pComponent = m_aryChildren.next();

			if (pComponent->isContainer())
			{
				CContainer *pContainer = static_cast<CContainer *>(pComponent);
				pContainer->resetLayout();
			}
		}

		setDirty();
	}

	void CContainer::lock()
	{
		CComponent::lock();

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			m_aryChildren.next()->lock();
		}
	}

	void CContainer::unlock()
	{
		CComponent::unlock();

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			m_aryChildren.next()->unlock();
		}
	}

	BOOLEAN CContainer::canSetFocus()
	{
		if (CComponent::canSetFocus() == FALSE)
		{
			return FALSE;
		}

		if (m_CCPSkin.isNotNull())
		{
			return TRUE;
		}

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			if (m_aryChildren.next()->canSetFocus())
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	void CContainer::setFocus(CComponent::ECCPOperation eCCPOperation)
	{
		if (m_CCPSkin.isNotNull())
		{
			CComponent::setFocus(eCCPOperation);
			return;
		}

		CComponent *pComponent = NULL;

		switch (eCCPOperation)
		{
		case CComponent::CCP_MOVE_LEFT:
		case CComponent::CCP_MOVE_UP:
			pComponent = getLastComponentByPos();
			break;

		case CComponent::CCP_MOVE_RIGHT:
		case CComponent::CCP_MOVE_DOWN:
			pComponent = getFirstComponentByPos();
			break;

		case CComponent::CCP_ROTATE_LEFT:
			if (m_bMoveCCPFocusInAddOrder)
			{
				pComponent = getLastComponentByAddOrder();
			}
			else
			{
				pComponent = getLastComponentByPos();
			}
			break;

		case CComponent::CCP_ROTATE_RIGHT:
			if (m_bMoveCCPFocusInAddOrder)
			{
				pComponent = getFirstComponentByAddOrder();
			}
			else
			{
				pComponent = getFirstComponentByPos();
			}
			break;

		default:
			break;
		}

		if (pComponent != NULL)
		{
			pComponent->setFocus(eCCPOperation);
		}
	}

	void CContainer::releaseFocus()
	{
		if (innerGetFocus())
		{
			CComponent::releaseFocus();
			return;
		}

		CComponent *pComponent = getFocusComponent();
		if (pComponent != NULL)
		{
			pComponent->releaseFocus();
		}
	}

	BOOLEAN CContainer::hasFocus()
	{
		if (CComponent::hasFocus())
		{
			return TRUE;
		}

		if (getFocusComponent() != NULL)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOLEAN CContainer::moveFocusToLeft()
	{
		if (innerGetFocus())
		{
			return FALSE;
		}

		CComponent *pFocusComponent= NULL;

		{
			pFocusComponent = getFocusComponent();

			if (pFocusComponent == NULL)
			{
				return FALSE;
			}

			if (pFocusComponent->moveFocusToLeft())
			{
				return TRUE;
			}
		}

		if (!canMoveFocus())
		{
			return (m_CCPSkin.isNotNull() ? TRUE : (canMoveFocusOut() ? FALSE : TRUE));
		}

		CComponent *pNextComponent = NULL;

		{
			Array<CComponent> aryTemp(FALSE);

			m_aryChildren.resetIterator();
			while (m_aryChildren.hasMore())
			{
				CComponent *pComponent = m_aryChildren.next();

				if (pComponent == pFocusComponent || pComponent->canSetFocus() == FALSE)
				{
					continue;
				}

				// right colume or same colume
				if (pComponent->getCenterPointX() >= pFocusComponent->getCenterPointX() - DISTANCE_OFFSET)
				{
					continue;
				}

				// up line
				if (pComponent->getCenterPointY() < pFocusComponent->getCenterPointY() - DISTANCE_OFFSET)
				{
					aryTemp.addItem(pComponent);
					continue;
				}

				// below line
				if (pComponent->getCenterPointY() > pFocusComponent->getCenterPointY() + DISTANCE_OFFSET)
				{
					aryTemp.addItem(pComponent);
					continue;
				}

				// same line
				if (pNextComponent == NULL)
				{
					pNextComponent = pComponent;
				}
				else
				{
					if (pComponent->getCenterPointX() > pNextComponent->getCenterPointX())
					{
						pNextComponent = pComponent;
					}
				}
			}

			// don't find in same line, so find the min distance component in left colume.
			if (pNextComponent == NULL)
			{
				aryTemp.resetIterator();
				while (aryTemp.hasMore())
				{
					CComponent *pTempComponent = aryTemp.next();

					if (pNextComponent == NULL)
					{
						pNextComponent = pTempComponent;
						continue;
					}

					// right colume
					if (pTempComponent->getCenterPointX() > pNextComponent->getCenterPointX() + DISTANCE_OFFSET)
					{
						pNextComponent = pTempComponent;
						continue;
					}

					// left colume
					if (pTempComponent->getCenterPointX() < pNextComponent->getCenterPointX() - DISTANCE_OFFSET)
					{
						continue;
					}

					// same colume
					if (getComponentDistance(pTempComponent, pFocusComponent) < getComponentDistance(pNextComponent, pFocusComponent))
					{
						pNextComponent = pTempComponent;
					}
				}
			}
		}

		if (pNextComponent != NULL)
		{
			pFocusComponent->releaseFocus();
			pNextComponent->setFocus(CComponent::CCP_MOVE_LEFT);
			return TRUE;
		}

		return (m_CCPSkin.isNotNull() ? TRUE : (canMoveFocusOut() ? FALSE : TRUE));
	}

	BOOLEAN CContainer::moveFocusToRight()
	{
		if (innerGetFocus())
		{
			return FALSE;
		}

		CComponent *pFocusComponent= NULL;

		{
			pFocusComponent = getFocusComponent();

			if (pFocusComponent == NULL)
			{
				return FALSE;
			}

			if (pFocusComponent->moveFocusToRight())
			{
				return TRUE;
			}
		}

		if (!canMoveFocus())
		{
			return (m_CCPSkin.isNotNull() ? TRUE : (canMoveFocusOut() ? FALSE : TRUE));
		}

		CComponent *pNextComponent = NULL;

		{
			Array<CComponent> aryTemp(FALSE);

			m_aryChildren.resetIterator();
			while (m_aryChildren.hasMore())
			{
				CComponent *pComponent = m_aryChildren.next();

				if (pComponent == pFocusComponent || pComponent->canSetFocus() == FALSE)
				{
					continue;
				}

				// left colume or same colume
				if (pComponent->getCenterPointX() <= pFocusComponent->getCenterPointX() + DISTANCE_OFFSET)
				{
					continue;
				}

				// up line
				if (pComponent->getCenterPointY() < pFocusComponent->getCenterPointY() - DISTANCE_OFFSET)
				{
					aryTemp.addItem(pComponent);
					continue;
				}

				// below line
				if (pComponent->getCenterPointY() > pFocusComponent->getCenterPointY() + DISTANCE_OFFSET)
				{
					aryTemp.addItem(pComponent);
					continue;
				}

				// same line
				if (pNextComponent == NULL)
				{
					pNextComponent = pComponent;
				}
				else
				{
					if (pComponent->getCenterPointX() < pNextComponent->getCenterPointX())
					{
						pNextComponent = pComponent;
					}
				}
			}

			// don't find in same line, so find the min distance component in right colume.
			if (pNextComponent == NULL)
			{
				aryTemp.resetIterator();
				while (aryTemp.hasMore())
				{
					CComponent *pTempComponent = aryTemp.next();

					if (pNextComponent == NULL)
					{
						pNextComponent = pTempComponent;
						continue;
					}

					// left colume
					if (pTempComponent->getCenterPointX() < pNextComponent->getCenterPointX() - DISTANCE_OFFSET)
					{
						pNextComponent = pTempComponent;
						continue;
					}

					// right colume
					if (pTempComponent->getCenterPointX() > pNextComponent->getCenterPointX() + DISTANCE_OFFSET)
					{
						continue;
					}

					// same colume
					if (getComponentDistance(pTempComponent, pFocusComponent) < getComponentDistance(pNextComponent, pFocusComponent))
					{
						pNextComponent = pTempComponent;
					}
				}
			}
		}

		if (pNextComponent != NULL)
		{
			pFocusComponent->releaseFocus();
			pNextComponent->setFocus(CComponent::CCP_MOVE_RIGHT);
			return TRUE;
		}

		return (m_CCPSkin.isNotNull() ? TRUE : (canMoveFocusOut() ? FALSE : TRUE));
	}

	BOOLEAN CContainer::moveFocusToUp()
	{
		if (innerGetFocus())
		{
			return FALSE;
		}

		CComponent *pFocusComponent= NULL;

		{
			pFocusComponent = getFocusComponent();

			if (pFocusComponent == NULL)
			{
				return FALSE;
			}

			if (pFocusComponent->moveFocusToUp())
			{
				return TRUE;
			}
		}		

		if (!canMoveFocus())
		{
			return (m_CCPSkin.isNotNull() ? TRUE : (canMoveFocusOut() ? FALSE : TRUE));
		}

		CComponent *pNextComponent = NULL;

		{
			Array<CComponent> aryTemp(FALSE);

			m_aryChildren.resetIterator();
			while (m_aryChildren.hasMore())
			{
				CComponent *pComponent = m_aryChildren.next();

				if (pComponent == pFocusComponent || pComponent->canSetFocus() == FALSE)
				{
					continue;
				}

				// below line or same line
				if (pComponent->getCenterPointY() >= pFocusComponent->getCenterPointY() - DISTANCE_OFFSET)
				{
					continue;
				}

				// left colume
				if (pComponent->getCenterPointX() < pFocusComponent->getCenterPointX() - DISTANCE_OFFSET)
				{
					aryTemp.addItem(pComponent);
					continue;
				}

				// right colume
				if (pComponent->getCenterPointX() > pFocusComponent->getCenterPointX() + DISTANCE_OFFSET)
				{
					aryTemp.addItem(pComponent);
					continue;
				}

				// same colume
				if (pNextComponent == NULL)
				{
					pNextComponent = pComponent;
				}
				else
				{
					if (pComponent->getCenterPointY() > pNextComponent->getCenterPointY())
					{
						pNextComponent = pComponent;
					}
				}
			}

			// don't find in same colume, so find the min distance component in up line.
			if (pNextComponent == NULL)
			{
				aryTemp.resetIterator();
				while (aryTemp.hasMore())
				{
					CComponent *pTempComponent = aryTemp.next();

					if (pNextComponent == NULL)
					{
						pNextComponent = pTempComponent;
						continue;
					}

					// below line
					if (pTempComponent->getCenterPointY() > pNextComponent->getCenterPointY() + DISTANCE_OFFSET)
					{
						pNextComponent = pTempComponent;
						continue;
					}

					// up line
					if (pTempComponent->getCenterPointY() < pNextComponent->getCenterPointY() - DISTANCE_OFFSET)
					{
						continue;
					}

					// same line
					if (getComponentDistance(pTempComponent, pFocusComponent) < getComponentDistance(pNextComponent, pFocusComponent))
					{
						pNextComponent = pTempComponent;
					}
				}
			}
		}

		if (pNextComponent != NULL)
		{
			pFocusComponent->releaseFocus();
			pNextComponent->setFocus(CComponent::CCP_MOVE_UP);
			return TRUE;
		}

		return (m_CCPSkin.isNotNull() ? TRUE : (canMoveFocusOut() ? FALSE : TRUE));
	}

	BOOLEAN CContainer::moveFocusToDown()
	{
		if (innerGetFocus())
		{
			return FALSE;
		}

		CComponent *pFocusComponent= NULL;

		{
			pFocusComponent = getFocusComponent();

			if (pFocusComponent == NULL)
			{
				return FALSE;
			}

			if (pFocusComponent->moveFocusToDown())
			{
				return TRUE;
			}
		}

		if (!canMoveFocus())
		{
			return (m_CCPSkin.isNotNull() ? TRUE : (canMoveFocusOut() ? FALSE : TRUE));
		}

		CComponent *pNextComponent = NULL;

		{
			Array<CComponent> aryTemp(FALSE);

			m_aryChildren.resetIterator();
			while (m_aryChildren.hasMore())
			{
				CComponent *pComponent = m_aryChildren.next();

				if (pComponent == pFocusComponent || pComponent->canSetFocus() == FALSE)
				{
					continue;
				}

				// top line or same line
				if (pComponent->getCenterPointY() <= pFocusComponent->getCenterPointY() + DISTANCE_OFFSET)
				{
					continue;
				}

				// left colume
				if (pComponent->getCenterPointX() < pFocusComponent->getCenterPointX() - DISTANCE_OFFSET)
				{
					aryTemp.addItem(pComponent);
					continue;
				}

				// right colume
				if (pComponent->getCenterPointX() > pFocusComponent->getCenterPointX() + DISTANCE_OFFSET)
				{
					aryTemp.addItem(pComponent);
					continue;
				}

				// same colume
				if (pNextComponent == NULL)
				{
					pNextComponent = pComponent;
				}
				else
				{
					if (pComponent->getCenterPointY() < pNextComponent->getCenterPointY())
					{
						pNextComponent = pComponent;
					}
				}
			}

			// don't find in same colume, so find the min distance component in below line.
			if (pNextComponent == NULL)
			{
				aryTemp.resetIterator();
				while (aryTemp.hasMore())
				{
					CComponent *pTempComponent = aryTemp.next();

					if (pNextComponent == NULL)
					{
						pNextComponent = pTempComponent;
						continue;
					}

					// up line
					if (pTempComponent->getCenterPointY() < pNextComponent->getCenterPointY() - DISTANCE_OFFSET)
					{
						pNextComponent = pTempComponent;
						continue;
					}

					// below line
					if (pTempComponent->getCenterPointY() > pNextComponent->getCenterPointY() + DISTANCE_OFFSET)
					{
						continue;
					}

					// same line
					if (getComponentDistance(pTempComponent, pFocusComponent) < getComponentDistance(pNextComponent, pFocusComponent))
					{
						pNextComponent = pTempComponent;
					}
				}
			}
		}

		if (pNextComponent != NULL)
		{
			pFocusComponent->releaseFocus();
			pNextComponent->setFocus(CComponent::CCP_MOVE_DOWN);
			return TRUE;
		}

		return (m_CCPSkin.isNotNull() ? TRUE : (canMoveFocusOut() ? FALSE : TRUE));
	}

	BOOLEAN CContainer::turnFocusToLeft(UINT uiStep)
	{
		if (innerGetFocus())
		{
			return FALSE;
		}

		CComponent *pFocusComponent= NULL;

		{
			pFocusComponent = getFocusComponent();

			if (pFocusComponent == NULL)
			{
				return FALSE;
			}

			if (pFocusComponent->turnFocusToLeft(uiStep))
			{
				return TRUE;
			}
		}

		CComponent *pNextComponent = NULL;

		{
			if (m_bMoveCCPFocusInAddOrder)
			{
				for (INT i = m_aryChildren.getItemIndex(pFocusComponent) - 1; i >= 0; i--)
				{
					pNextComponent = m_aryChildren[i];

					if (pNextComponent->canSetFocus())
					{
						break;
					}
					else
					{
						pNextComponent = NULL;
					}
				}
			}
			else
			{
				Array<CComponent> aryTemp(FALSE);

				m_aryChildren.resetIterator();
				while (m_aryChildren.hasMore())
				{
					CComponent *pComponent = m_aryChildren.next();

					if (pComponent == pFocusComponent || pComponent->canSetFocus() == FALSE)
					{
						continue;
					}

					// below line
					if (pComponent->getCenterPointY() > pFocusComponent->getCenterPointY() + DISTANCE_OFFSET)
					{
						continue;
					}

					// up line
					if (pComponent->getCenterPointY() < pFocusComponent->getCenterPointY() - DISTANCE_OFFSET)
					{
						aryTemp.addItem(pComponent);
						continue;
					}

					// same line and at right
					if (pComponent->getCenterPointX() > pFocusComponent->getCenterPointX())
					{
						continue;
					}

					// same line and at left
					if (pNextComponent == NULL)
					{
						pNextComponent = pComponent;
					}
					else
					{
						if (pComponent->getCenterPointX() > pNextComponent->getCenterPointX())
						{
							pNextComponent = pComponent;
						}
					}
				}

				// don't find in same line, so find the right compont in up line.
				if (pNextComponent == NULL)
				{
					aryTemp.resetIterator();
					while (aryTemp.hasMore())
					{
						CComponent *pTempComponent = aryTemp.next();

						if (pNextComponent == NULL)
						{
							pNextComponent = pTempComponent;
							continue;
						}

						// below line
						if (pTempComponent->getCenterPointY() > pNextComponent->getCenterPointY() + DISTANCE_OFFSET)
						{
							pNextComponent = pTempComponent;
							continue;
						}

						// up line
						if (pTempComponent->getCenterPointY() < pNextComponent->getCenterPointY() - DISTANCE_OFFSET)
						{
							continue;
						}

						// same line
						if (pTempComponent->getCenterPointX() > pNextComponent->getCenterPointX())
						{
							pNextComponent = pTempComponent;
						}
					}
				}
			}
		}

		if (pNextComponent != NULL)
		{
			pFocusComponent->releaseFocus();
			pNextComponent->setFocus(CComponent::CCP_ROTATE_LEFT);
			return TRUE;
		}

		return (m_CCPSkin.isNotNull() ? TRUE : (canMoveFocusOut() ? FALSE : TRUE));
	}

	BOOLEAN CContainer::turnFocusToRight(UINT uiStep)
	{
		if (innerGetFocus())
		{
			return FALSE;
		}

		CComponent *pFocusComponent= NULL;

		{
			pFocusComponent = getFocusComponent();

			if (pFocusComponent == NULL)
			{
				return FALSE;
			}

			if (pFocusComponent->turnFocusToRight(uiStep))
			{
				return TRUE;
			}
		}

		CComponent *pNextComponent = NULL;

		{
			if (m_bMoveCCPFocusInAddOrder)
			{
				for (UINT i = m_aryChildren.getItemIndex(pFocusComponent) + 1; i < m_aryChildren.size(); i++)
				{
					pNextComponent = m_aryChildren[i];

					if (pNextComponent->canSetFocus())
					{
						break;
					}
					else
					{
						pNextComponent = NULL;
					}
				}
			}
			else
			{
				Array<CComponent> aryTemp(FALSE);

				m_aryChildren.resetIterator();
				while (m_aryChildren.hasMore())
				{
					CComponent *pComponent = m_aryChildren.next();

					if (pComponent == pFocusComponent || pComponent->canSetFocus() == FALSE)
					{
						continue;
					}

					// up line
					if (pComponent->getCenterPointY() < pFocusComponent->getCenterPointY() - DISTANCE_OFFSET)
					{
						continue;
					}

					// below line
					if (pComponent->getCenterPointY() > pFocusComponent->getCenterPointY() + DISTANCE_OFFSET)
					{
						aryTemp.addItem(pComponent);
						continue;
					}

					// same line and at left
					if (pComponent->getCenterPointX() < pFocusComponent->getCenterPointX())
					{
						continue;
					}

					// same line and at right
					if (pNextComponent == NULL)
					{
						pNextComponent = pComponent;
					}
					else
					{
						if (pComponent->getCenterPointX() < pNextComponent->getCenterPointX())
						{
							pNextComponent = pComponent;
						}
					}
				}

				// don't find in same line, so find the left compont in below line.
				if (pNextComponent == NULL)
				{
					aryTemp.resetIterator();
					while (aryTemp.hasMore())
					{
						CComponent *pTempComponent = aryTemp.next();

						if (pNextComponent == NULL)
						{
							pNextComponent = pTempComponent;
							continue;
						}

						// up line
						if (pTempComponent->getCenterPointY() < pNextComponent->getCenterPointY() - DISTANCE_OFFSET)
						{
							pNextComponent = pTempComponent;
							continue;
						}

						// below line
						if (pTempComponent->getCenterPointY() > pNextComponent->getCenterPointY() + DISTANCE_OFFSET)
						{
							continue;
						}

						// same line
						if (pTempComponent->getCenterPointX() < pNextComponent->getCenterPointX())
						{
							pNextComponent = pTempComponent;
						}
					}
				}
			}
		}

		if (pNextComponent != NULL)
		{
			pFocusComponent->releaseFocus();
			pNextComponent->setFocus(CComponent::CCP_ROTATE_RIGHT);
			return TRUE;
		}

		return (m_CCPSkin.isNotNull() ? TRUE : (canMoveFocusOut() ? FALSE : TRUE));
	}

	void CContainer::doFocusEnterPress()
	{
		if (innerGetFocus())
		{
			return;
		}

		CComponent *pComponent = getFocusComponent();

		if (pComponent != NULL)
		{
			pComponent->doFocusEnterPress();
		}
	}

	void CContainer::doFocusEnterRelease()
	{
		CComponent *pComponent = NULL;

		if (innerGetFocus())
		{
			if (m_bMoveCCPFocusInAddOrder)
			{
				pComponent = getFirstComponentByAddOrder();
			}
			else
			{
				pComponent = getFirstComponentByPos();
			}

			if (pComponent != NULL)
			{
				innerSetFocus(FALSE);
				littleRefresh();

				pComponent->setFocus(CComponent::CCP_ROTATE_RIGHT);
			}
		}
		else
		{
			pComponent = getFocusComponent();

			if (pComponent != NULL)
			{
				pComponent->doFocusEnterRelease();
			}
		}
	}

	void CContainer::doFocusEnterIn()
	{
		if (innerGetFocus())
		{
			CComponent::doFocusEnterIn();
		}
		else
		{
			CComponent *pComponent = getFocusComponent();
			if (pComponent != NULL)
			{
				pComponent->doFocusEnterIn();
			}
		}
	}

	BOOLEAN CContainer::doFocusBack()
	{
		CComponent *pComponent = getFocusComponent();
		if (pComponent != NULL)
		{
			if (pComponent->doFocusBack())
			{
				return TRUE;
			}

			if (m_CCPSkin.isNotNull())
			{
				innerSetFocus(TRUE);
				littleRefresh();
				setDirty();
				return TRUE;
			}
		}

		return CComponent::doFocusBack();
	}

	void CContainer::setParentMoving(BOOLEAN bValue)
	{
		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			m_aryChildren.next()->setParentMoving(bValue);
		}
	}

	BOOLEAN CContainer::hitTest(INT iX, INT iY)
	{
		if (!getVisible())
		{
			return FALSE;
		}

		CPoint ptLeftTop = localToGlobal(0, 0);

		for (INT i = (INT)m_aryChildren.size() - 1; i >= 0; i--)
		{
			if (m_aryChildren[i]->hitTest(iX, iY))
			{
				return TRUE;
			}
		}

		return CComponent::hitTest(iX, iY);
	}

	BOOLEAN CContainer::contains(CComponent *pComponent)
	{
		if (containsComponent(pComponent))
		{
			return TRUE;
		}

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			if (m_aryChildren.next()->contains(pComponent))
			{
				return TRUE;
			}
		}

		return FALSE;
	}
		
	void CContainer::refresh()
	{
		CComponent::refresh();

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			m_aryChildren.next()->refresh();
		}
	}

	void CContainer::littleRefresh()
	{
		CComponent::littleRefresh();

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			m_aryChildren.next()->littleRefresh();
		}
	}

	void CContainer::destroySkin()
	{
		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			m_aryChildren.next()->destroySkin();
		}

		CComponent::destroySkin();
	}

	void CContainer::destroy()
	{
		if (isDestroyed())
		{
			return;
		}

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			m_aryChildren.next()->destroy();
		}
		m_aryChildren.clear();

		CComponent::destroy();
	}

	void CContainer::buildRenderTree(CRenderGroup &renderGroup)
	{
		if ( !getVisible() )
		{
			return ;
		}

		CPoint pt = localToGlobal(0, 0);
		m_renderGroup.setX(pt.getX());
		m_renderGroup.setY(pt.getY());
		m_renderGroup.setWidth(getPreferredSize().getX());
		m_renderGroup.setHeight(getPreferredSize().getY());
		renderGroup.addRenderItem(&m_renderGroup);

		buildRenderTreeFromBackground(m_renderGroup);

		buildRenderTreeFromChildren(m_renderGroup);		

		buildRenderTreeFromCCP(m_renderGroup);
	}

	void CContainer::hoverCheck(INT iX, INT iY)
	{
		if (!getVisible() || isLock())
		{
			return;
		}
		
		for (INT i = (INT)m_aryChildren.size() - 1; i >= 0; i--)
		{
			m_aryChildren[i]->hoverCheck(iX, iY);

			CBaseMainFrame *pBaseMainFrame = CBaseMainFrame::getInstance();

			if (pBaseMainFrame == this)
			{
				Array2<CComponent*> aryEmpty;
				CContainer* pTopOverlay = dynamic_cast<CContainer*>(CBaseMainFrame::getInstance()->getTopOverlay(aryEmpty));

				if (pTopOverlay != NULL && !pTopOverlay->isLock())
				{
					Array2<CComponent*> aryComp;
					aryComp.addItem(pTopOverlay);

					CContainer* pTopOverlay1 = dynamic_cast<CContainer*>(CBaseMainFrame::getInstance()->getTopOverlay(aryComp));

					CBaseScreen *pScreen = CBaseMainFrame::getInstance()->getTopScreen();
					pScreen->lostHover();

					if (pTopOverlay1 != NULL && !pTopOverlay1->isLock())
					{
						pTopOverlay1->lostHover();
					}
					
					return;
				}				
			}

			if (m_aryChildren[i]->isContainer())
			{				
				if ((static_cast<CContainer*>(m_aryChildren[i]))->getType() > CONFIG::CONTAINER_TYPE_SCREEN)
				{
					if (m_aryChildren[i]->hitTest(iX, iY))
					{						
						for (INT j = i - 1; j >= 0; j--)
						{
							m_aryChildren[j]->lostHover();
						}

						return;
					}
				}
			}			
		}
	}

	void CContainer::lostHover()
	{
		for (INT i = (INT)m_aryChildren.size() - 1; i >= 0; i--)
		{
			m_aryChildren[i]->lostHover();
		}
	}

	void CContainer::wheel(CCursor::EWheelType eType)
	{
		if (!getVisible() || isLock())
		{
			return;
		}

		for (INT i = (INT)m_aryChildren.size() - 1; i >= 0; i--)
		{
			m_aryChildren[i]->wheel(eType);
		}
	}

	void CContainer::move(INT iX, INT iY)
	{
		if (!getVisible())
		{
			return;
		}

		for (INT i = (INT)m_aryChildren.size() - 1; i >= 0; i--)
		{
			m_aryChildren[i]->move(iX, iY);
		}
	}

	ICursorTracker* CContainer::probe(INT iX, INT iY)
	{
		if (!getVisible())
		{
			return NULL;
		}

		ICursorTracker *pTracker = NULL;

		for (INT i = (INT)m_aryChildren.size() - 1; i >= 0; i--)
		{			
			if (m_aryChildren[i] != NULL)
			{
				pTracker = m_aryChildren[i]->probe(iX, iY);
			}

			if (pTracker != NULL)
			{
				return pTracker;
			}
		}

		if (CComponent::hitTest(iX, iY))
		{
			return this;
		}

		return NULL;
	}

	void CContainer::onTouchPress(INT iX, INT iY)
	{
		m_pHittedComponent = NULL;

		for (INT i = (INT)m_aryChildren.size() - 1; i >= 0; i--)
		{
			if (m_aryChildren[i]->hitTest(iX, iY))
			{
				m_pHittedComponent = m_aryChildren[i];				
				break;
			}
		}

		if (m_pHittedComponent != NULL)
		{
			m_pHittedComponent->onTouchPress(iX, iY);
		}
	}

	void CContainer::onTouchPressRelease(INT iX, INT iY)
	{
		if (m_pHittedComponent != NULL)
		{
			m_pHittedComponent->onTouchPressRelease(iX, iY);

			m_pLastHittedComponent = m_pHittedComponent;
			m_pHittedComponent = NULL;
		}
	}

	void CContainer::onTouchLongPress(INT iX, INT iY)
	{
		if (m_pHittedComponent != NULL)
		{
			m_pHittedComponent->onTouchLongPress(iX, iY);
		}
	}

	void CContainer::onTouchContinuePress(INT iX, INT iY)
	{
		if (m_pHittedComponent != NULL)
		{
			m_pHittedComponent->onTouchContinuePress(iX, iY);
		}
	}

	void CContainer::onTouchLongPressRelease(INT iX, INT iY)
	{
		if (m_pHittedComponent != NULL)
		{
			m_pHittedComponent->onTouchLongPressRelease(iX, iY);

			m_pHittedComponent = NULL;
		}
	}

	void CContainer::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
	{
		if (m_pHittedComponent != NULL)
		{
			m_pHittedComponent->onTouchMove(iX, iY, iDeltaX, iDeltaY);
		}
	}

	void CContainer::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
	{
		if (m_pHittedComponent != NULL)
		{
			m_pHittedComponent->onTouchMoveRelease(iX, iY, iSpeedX, iSpeedY);

			m_pHittedComponent = NULL;
		}
	}

	void CContainer::onTouchDrag(INT iX, INT iY)
	{
		if (m_pHittedComponent != NULL)
		{
			m_pHittedComponent->onTouchDrag(iX, iY);
		}
	}

	void CContainer::onTouchDragRelease(INT iX, INT iY)
	{
		if (m_pHittedComponent != NULL)
		{
			m_pHittedComponent->onTouchDragRelease(iX, iY);

			m_pHittedComponent = NULL;
		}
	}

	void CContainer::onTouchDoublePress(INT iX, INT iY)
	{
		m_pHittedComponent = NULL;

		for (INT i = (INT)m_aryChildren.size() - 1; i >= 0; i--)
		{
			if (m_aryChildren[i]->hitTest(iX, iY))
			{
				m_pHittedComponent = m_aryChildren[i];

				if (m_pLastHittedComponent == NULL || m_pHittedComponent != m_pLastHittedComponent)
				{
					m_pHittedComponent = NULL;
					m_pLastHittedComponent = NULL;
				}

				break;
			}
		}

		if (m_pHittedComponent != NULL)
		{
			m_pHittedComponent->onTouchDoublePress(iX, iY);
		}
	}

	void CContainer::onTouchDoublePressRelease(INT iX, INT iY)
	{
		if (m_pHittedComponent != NULL)
		{
			m_pHittedComponent->onTouchDoublePressRelease(iX, iY);

			m_pHittedComponent = NULL;
			m_pLastHittedComponent = NULL;
		}
	}

	void CContainer::buildRenderTreeFromChildren(CRenderGroup &renderGroup)
	{
		if (!getVisible())
		{
			return;
		}

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			m_aryChildren.next()->buildRenderTree(renderGroup);
		}
	}

	CComponent* CContainer::getFocusComponent()
	{
		CComponent  *pComponent = NULL;

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			pComponent = m_aryChildren.next();

			if (pComponent->hasFocus())
			{
				return pComponent;
			}
		}

		return NULL;
	}

	CComponent* CContainer::getFirstComponentByPos()
	{
		CComponent *pFirstComponent = NULL;

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			CComponent *pComponent = m_aryChildren.next();

			if (pComponent->canSetFocus() == FALSE)
			{
				continue;
			}

			if (pFirstComponent == NULL)
			{
				pFirstComponent = pComponent;
				continue;
			}

			// up line
			if (pComponent->getY() < pFirstComponent->getY() - DISTANCE_OFFSET)
			{
				pFirstComponent = pComponent;
				continue;
			}

			// below line
			if (pComponent->getY() > pFirstComponent->getY() + DISTANCE_OFFSET)
			{
				continue;
			}

			// same line
			if (pComponent->getX() < pFirstComponent->getX())
			{
				pFirstComponent = pComponent;
			}
		}

		return pFirstComponent;
	}

	CComponent* CContainer::getLastComponentByPos()
	{
		CComponent *pLastComponent = NULL;

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			CComponent *pComponent = m_aryChildren.next();

			if (pComponent->canSetFocus() == FALSE)
			{
				continue;
			}

			if (pLastComponent == NULL)
			{
				pLastComponent = pComponent;
				continue;
			}

			// below line
			if (pComponent->getY() + pComponent->getPreferredSize().getY() > pLastComponent->getY() + pLastComponent->getPreferredSize().getY() + DISTANCE_OFFSET)
			{
				pLastComponent = pComponent;
				continue;
			}

			// up line
			if (pComponent->getY() + pComponent->getPreferredSize().getY() < pLastComponent->getY() + pLastComponent->getPreferredSize().getY() - DISTANCE_OFFSET)
			{
				continue;
			}

			// same line
			if (pComponent->getX() + pComponent->getPreferredSize().getX() > pLastComponent->getX() + pLastComponent->getPreferredSize().getX())
			{
				pLastComponent = pComponent;
			}
		}

		return pLastComponent;
	}

	CComponent* CContainer::getFirstComponentByAddOrder()
	{
		CComponent  *pComponent = NULL;

		m_aryChildren.resetIterator();
		while (m_aryChildren.hasMore())
		{
			pComponent = m_aryChildren.next();

			if (pComponent->canSetFocus())
			{
				return pComponent;
			}
		}

		return NULL;
	}

	CComponent* CContainer::getLastComponentByAddOrder()
	{
		CComponent  *pComponent = NULL;

		for (INT i = m_aryChildren.size() - 1; i >= 0; i--)
		{
			pComponent = m_aryChildren[i];

			if (pComponent->canSetFocus())
			{
				return pComponent;
			}
		}

		return NULL;
	}

	INT CContainer::getComponentDistance(CComponent* pComponent1, CComponent* pComponent2)
	{
		return    Math::pow(pComponent1->getCenterPointX() - pComponent2->getCenterPointX(), 2)
				+ Math::pow(pComponent1->getCenterPointY() - pComponent2->getCenterPointY(), 2);
	}
}