#include "OpenHMI.h"

namespace OpenHMI
{
	CModelList::CModelList()
		: m_bReachToHead(TRUE)
		, m_bReachToTail(TRUE)
	{
		;
	}

	CModelList::~CModelList()
	{
		;
	}

	Object* CModelList::getItemAt(UINT uiIndex)
	{
		return m_aryItem[uiIndex];
	}

	INT CModelList::getItemIndex(Object* pObject)
	{
		return m_aryItem.getItemIndex(ObjectPtr<Object>(pObject));
	}

	void CModelList::addItem(Object* pObject)
	{
		if (pObject == NULL)
		{
			return;
		}

		m_aryItem.addItem(ObjectPtr<Object>(pObject));
	}	

	void CModelList::insertItem(UINT uiIndex, Object* pObject)
	{
		if (pObject == NULL)
		{
			return;
		}

		m_aryItem.insertItem(uiIndex, ObjectPtr<Object>(pObject));
	}

	void CModelList::deleteItem(Object* pObject)
	{
		m_aryItem.deleteItem(ObjectPtr<Object>(pObject));
	}

	void CModelList::deleteItemAt(UINT uiIndex)
	{
		m_aryItem.deleteItemByIndex(uiIndex);
	}

	void CModelList::deleteAllItems()
	{
		m_aryItem.clear();
	}
}