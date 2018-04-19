#include "OpenHMI.h"

namespace OpenHMI
{
	CModelTree::CModelTree()
	{
		;
	}

	CModelTree::~CModelTree()
	{
		;
	}

	Object* CModelTree::getItemAt(UINT uiIndex)
	{
		tagItemNode *pItem = m_aryItem[uiIndex];

		return pItem != NULL ? pItem->ptrObject.getRaw() : NULL;
	}

	UINT CModelTree::getItemLevel(UINT uiIndex)
	{
		tagItemNode *pItem = m_aryItem[uiIndex];

		return pItem != NULL ? pItem->uiLevel : 0;
	}

	INT CModelTree::getItemIndex(Object* pObject)
	{
		for ( INT i = 0; i < (INT)m_aryItem.size(); i++ )
		{
			if( m_aryItem[i]->ptrObject == pObject )
			{
				return i;
			}
		}

		return -1;
	}

	void CModelTree::addItem(Object* pObject, UINT uiLevel)
	{
		if (pObject == NULL)
		{
			return;
		}

		UINT uiSize = m_aryItem.size();

// 		if (uiSize == 0)
// 		{
// 			uiLevel = 0;
// 		}
// 		else
// 		{
// 			if (uiLevel > m_aryItem[uiSize - 1]->uiLevel + 1)
// 			{
// 				uiLevel = m_aryItem[uiSize - 1]->uiLevel + 1;
// 			}
// 		}

		m_aryItem.addItem(new tagItemNode(pObject, uiLevel));
	}	

	void CModelTree::insertItem(UINT uiIndex, Object* pObject, UINT uiLevel)
	{
		if (pObject == NULL)
		{
			return;
		}

		m_aryItem.insertItem(uiIndex, new tagItemNode(pObject, uiLevel));
	}

	void CModelTree::deleteItem(Object* pObject)
	{
		m_aryItem.deleteItemByIndex(getItemIndex(pObject));
	}

	void CModelTree::deleteItemAt(UINT uiIndex)
	{
		m_aryItem.deleteItemByIndex(uiIndex);
	}

	void CModelTree::deleteAllItems()
	{
		m_aryItem.clear();
	}
}