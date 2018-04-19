#include "OpenHMI.h"

namespace OpenHMI
{
	CButtonGroup::CButtonGroup()
		: m_pBtnSelected(NULL)
	{
		;
	}

	CButtonGroup::~CButtonGroup()
	{
		;
	}
 
 	BEGIN_EVENT_MAP(CButtonGroup)
		CONTROL_EVENT_HANDLER_CHAIN(this, CEventButton::TOUCH_RELEASED, onTouchReleased);
		CONTROL_EVENT_HANDLER_CHAIN(this, CEventButton::TOUCH_LONG_RELEASED, onTouchReleased);
 	END_EVENT_MAP_2

	void CButtonGroup::add(CCtrlButton *pCtrlButton)
	{
		if (pCtrlButton == NULL)
		{
			return;
		}

		if(m_aryCtrlButton.isExist(pCtrlButton))
		{
			return;
		}

		pCtrlButton->addEventListener(this);

		m_aryCtrlButton.addItem(pCtrlButton);

		if (pCtrlButton->getSelected())
		{
			removeSelected();
			m_pBtnSelected = pCtrlButton;
		}
	}

	void CButtonGroup::remove(CCtrlButton *pCtrlButton)
	{
		if (pCtrlButton == NULL)
		{
			return;
		}

		if (m_aryCtrlButton.isExist(pCtrlButton) == FALSE)
		{
			return;
		}

 		pCtrlButton->removeEventListener(this);

		m_aryCtrlButton.deleteItem(pCtrlButton);

		if (m_pBtnSelected == pCtrlButton)
		{
			removeSelected();
		}
	}

	void CButtonGroup::setSelected(CCtrlButton *pCtrlButton)
	{
		if (pCtrlButton != NULL && m_aryCtrlButton.isExist(pCtrlButton) && m_pBtnSelected != pCtrlButton)
		{
			removeSelected();

			m_pBtnSelected = pCtrlButton;
			m_pBtnSelected->setSelected(TRUE);
			m_pBtnSelected->littleRefresh();
		}
	}

	CCtrlButton * CButtonGroup::getSelected()
	{
		return m_pBtnSelected;
	}

	void CButtonGroup::removeSelected()
	{
		if (m_pBtnSelected != NULL)
		{
			m_pBtnSelected->setSelected(FALSE);
			m_pBtnSelected->littleRefresh();
			m_pBtnSelected = NULL;
		}
	}	

	void CButtonGroup::clear()
	{
		m_aryCtrlButton.resetIterator();
		while (m_aryCtrlButton.hasMore())
		{
			m_aryCtrlButton.next()->removeEventListener(this);
		}
		m_aryCtrlButton.clear();

		removeSelected();
	}

	BOOLEAN CButtonGroup::contains(Object *pTarget)
	{
		return m_aryCtrlButton.isExist((CCtrlButton*)pTarget);
	}

	void CButtonGroup::onTouchReleased(CComponent *pTarget)
	{
		setSelected((CCtrlButton*)pTarget);
	}
}