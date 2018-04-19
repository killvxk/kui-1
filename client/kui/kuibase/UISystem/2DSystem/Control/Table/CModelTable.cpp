#include "OpenHMI.h"

namespace OpenHMI
{
	CModelTable::CModelTable()
		: m_uiRealItemCount(0)
	{
		;
	}

	CModelTable::~CModelTable()
	{
		;
	}

	Object* CModelTable::getItemAt(UINT uiIndex)
	{
		return m_aryItem[uiIndex];
	}

	INT CModelTable::getItemIndex(Object* pObject)
	{
		if (pObject == NULL)
		{
			return -1;
		}

		return m_aryItem.getItemIndex(ObjectPtr<Object>(pObject));
	}

	void CModelTable::addItem(Object* pObject)
	{		
		m_aryItem.addItem(ObjectPtr<Object>(pObject));
	}

	void CModelTable::insertItem(UINT uiIndex, Object* pObject)
	{
		m_aryItem.insertItem(uiIndex, ObjectPtr<Object>(pObject));
	}

	void CModelTable::deleteItem(Object* pObject)
	{
		m_aryItem.deleteItem(ObjectPtr<Object>(pObject));
	}

	void CModelTable::deleteItemAt(UINT uiIndex)
	{
		m_aryItem.deleteItemByIndex(uiIndex);
	}

	void CModelTable::deleteAllItems()
	{		
		m_aryItem.clear();
	}
}