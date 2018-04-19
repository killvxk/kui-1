#include "OpenHMI.h"

namespace OpenHMI
{
	CResGroup::CResGroup()
		:  m_uiMaxItemID(0)
	{
		m_propertySet.setProperty(L"maxItemID", m_uiMaxItemID);
	}

	CResGroup::~CResGroup()
	{
		;
	}

	UINT CResGroup::getItemSize()
	{
		buildItemArray();
		return m_aryItem.size();

		//return m_mapItem.size();	
	}

	CResItem* CResGroup::getItemByIndex(UINT uiIndex)
	{
		buildItemArray();
		return m_aryItem[uiIndex];

		//return m_mapItem.getValueByIndex(uiIndex);
	}

	CResItem* CResGroup::getItemByID(UINT uiID)
	{
		return m_mapItem.getValueByKey(uiID);
	}

	CResItem* CResGroup::getItemByName(const String &strName)
	{
		for (UINT uiIndex = 0; uiIndex < m_mapItem.size(); ++uiIndex)
		{
			CResItem* pResItem = m_mapItem.getValueByIndex(uiIndex);
			if (strName == pResItem->getName())
			{
				return pResItem;
			}
		}

		return NULL;
	}

	CResItem* CResGroup::createItem(const String &strType)
	{
		CResItem *pItem = new CResItem;
		
		m_uiMaxItemID++;
		pItem->setID(m_uiMaxItemID);
		pItem->setType(strType);

		if (m_aryItem.size() > 0)
		{
			pItem->setZOrder(m_aryItem[m_aryItem.size() - 1]->getZOrder() + 1);
		}

		pItem->setParent(this);
		m_mapItem.addItem(pItem->getID(), pItem);

		m_aryItem.addItem(pItem);
		
		CSkinParser::getInstance()->setDirty();

		return pItem;
	}

	CResItem* CResGroup::createFuncWidget(UINT uiRefID)
	{
		CResItem *pItem = new CResItem;

		m_uiMaxItemID++;		
		pItem->setID(m_uiMaxItemID);
		pItem->setType(L"REFFUNCWIDGET");
		pItem->setRefID(uiRefID);

		if (m_aryItem.size() > 0)
		{
			pItem->setZOrder(m_aryItem[m_aryItem.size() - 1]->getZOrder() + 1);
		}

		pItem->setParent(this);
		m_mapItem.addItem(pItem->getID(), pItem);

		m_aryItem.addItem(pItem);

		CSkinParser::getInstance()->setDirty();

		return pItem;
	}

	CResItem* CResGroup::createCtrlWidget(UINT uiRefID)
	{
		CResItem *pItem = new CResItem;

		m_uiMaxItemID++;		
		pItem->setID(m_uiMaxItemID);
		pItem->setType(L"REFCTRLWIDGET");
		pItem->setRefID(uiRefID);

		if (m_aryItem.size() > 0)
		{
			pItem->setZOrder(m_aryItem[m_aryItem.size() - 1]->getZOrder() + 1);
		}

		pItem->setParent(this);
		m_mapItem.addItem(pItem->getID(), pItem);

		m_aryItem.addItem(pItem);

		CSkinParser::getInstance()->setDirty();

		return pItem;
	}

	void CResGroup::deleteItem(CResItem* pItem)
	{
		if (pItem == NULL)
		{
			return;
		}

		pItem->deleteAllTextField();
		pItem->deleteAllImageField();
		pItem->setParent(NULL);
		
		m_aryItem.deleteItem(pItem);
		m_mapItem.deleteItem(pItem->getID());

		CSkinParser::getInstance()->setDirty();
	}

	void CResGroup::removeItem(CResItem* pItem)
	{
		if (pItem == NULL)
		{
			return;
		}

		pItem->setParent(NULL);

		m_aryItem.deleteItem(pItem);
		m_mapItem.popItem(pItem->getID());

		CSkinParser::getInstance()->setDirty();
	}

	void CResGroup::addItem(CResItem *pItem, BOOLEAN bNeedReset)
	{
		if (pItem == NULL)
		{
			return;
		}

		if (m_aryItem.isExist(pItem))
		{
			return;
		}

		if (bNeedReset)
		{
			m_uiMaxItemID++;		
			pItem->setID(m_uiMaxItemID);
		
			if (m_aryItem.size() > 0)
			{
				pItem->setZOrder(m_aryItem[m_aryItem.size() - 1]->getZOrder() + 1);
			}

			pItem->setName(pItem->getName() + L"_" + String::toString(pItem->getID()));
		}

		pItem->setParent(this);
		m_mapItem.addItem(pItem->getID(), pItem);
		m_aryItem.addItem(pItem);

		notifyItemZOrderChanged();

		CSkinParser::getInstance()->setDirty();
	}

	UINT CResGroup::getFilterSize()
	{
		return m_filterSet.getFilterSize();
	}

	CResFilter* CResGroup::getFilterByIndex(UINT uiIndex)
	{
		return m_filterSet.getFilterByIndex(uiIndex);
	}

	CResFilter* CResGroup::createFilter(const String &strType)
	{
		CResFilter *pFilter = m_filterSet.createFilter(strType);

		updateFilterProperty();

		CSkinParser::getInstance()->setDirty();

		return pFilter;
	}

	void CResGroup::deleteFilter(CResFilter *pFilter)
	{
		if (pFilter == NULL)
		{
			return;
		}

		m_filterSet.deleteFilter(pFilter);
		updateFilterProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResGroup::removeFilter(CResFilter *pFilter)
	{
		if (pFilter == NULL)
		{
			return;
		}

		m_filterSet.removeFilter(pFilter);
		updateFilterProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResGroup::addFilter(CResFilter *pFilter)
	{
		if (pFilter == NULL)
		{
			return;
		}

		m_filterSet.addFilter(pFilter);
		updateFilterProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResGroup::deleteAllFilter()
	{
		m_filterSet.clearAll();
		updateFilterProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResGroup::deleteAllTextField()
	{
		m_mapItem.resetIterator();
		while (m_mapItem.hasMore())
		{
			m_mapItem.next()->deleteAllTextField();
		}

		CSkinParser::getInstance()->setDirty();
	}

	void CResGroup::deleteAllImageField()
	{
		m_mapItem.resetIterator();
		while (m_mapItem.hasMore())
		{
			m_mapItem.next()->deleteAllImageField();
		}

		CSkinParser::getInstance()->setDirty();
	}

	void CResGroup::notifyItemZOrderChanged()
	{
		tagCompareByZOrder compare;
		m_aryItem.sort(compare);

		CSkinParser::getInstance()->setDirty();
	}

	void CResGroup::parse(const Map<String, String, tagStringCompare> *pmapData)
	{
		String *pstrBase = pmapData->getValueByKey(L"base");

		if (pstrBase == NULL || *pstrBase == STRING_NULL)
		{
			return ;
		}

		this->parseProperty(*pstrBase);

		for (UINT uiIndex = 0; uiIndex < pmapData->size(); uiIndex++)
		{
			String *pstrValue = pmapData->getValueByKey(String::format(L"item%d", uiIndex + 1));
			if (pstrValue != NULL && *pstrValue != STRING_NULL)
			{
				CResItem *pItem = new CResItem;

				pItem->setParent(this);
				pItem->parseProperty(*pstrValue);
				m_mapItem.addItem(pItem->getPropertyInt(L"id"), pItem);
			}
			else
			{
				break;
			}
		}
	}

	void CResGroup::save(Map<String, String, tagStringCompare> *pmapData)
	{
		UINT	uiIndex = 1;

		m_mapItem.resetIterator();
		while (m_mapItem.hasMore())
		{
			CResItem * pItem = m_mapItem.next();
			String *pstrValue = new String(pItem->saveProperty());
			pmapData->addItem(String::format(L"item%d", uiIndex++), pstrValue);
		}
	}

	BOOLEAN CResGroup::merge(CResNode *pNode, BOOLEAN bFull)
	{
		CResGroup* pMergeSrc = dynamic_cast<CResGroup*>(pNode);

		if (pMergeSrc == NULL)
		{
			return FALSE;
		}

		if (!CResNode::merge(pMergeSrc, bFull))
		{
			return FALSE;
		}

		mergeChildren(pMergeSrc);
		m_uiMaxItemID = pMergeSrc->m_uiMaxItemID;

		if (bFull)
		{
			// filter set.
			pMergeSrc->m_filterSet.cloneTo(&(this->m_filterSet));
			updateFilterProperty();

			setMergeFlag();
		}
		
		return TRUE;
	}

	void CResGroup::clearMergeFlag()
	{
		CResNode::clearMergeFlag();
		
		// clear children flags.
		m_mapItem.resetIterator();
		while (m_mapItem.hasMore())
		{
			CResItem * pItem = m_mapItem.next();
			pItem->clearMergeFlag();
		}
	}

	void CResGroup::parseProperty(const String &strData)
	{
		CResNode::parseProperty(strData);

		getFilterList();

		m_uiMaxItemID = CResNode::getPropertyUInt(L"maxItemID");
	}

	String CResGroup::saveProperty()
	{
		m_propertySet.setProperty(L"maxItemID", m_uiMaxItemID);

		updateFilterProperty();

		return CResNode::saveProperty();
	}

	void CResGroup::getFilterList()
	{
		m_filterSet.parse(&m_propertySet);
	}

	void CResGroup::updateFilterProperty()
	{
		m_filterSet.save(&m_propertySet);
	}

	void CResGroup::buildItemArray()
	{
		if (m_aryItem.size() > 0)
		{
			return;			
		}

		m_mapItem.resetIterator();
		while (m_mapItem.hasMore())
		{
			m_aryItem.addItem(m_mapItem.next());
		}

		tagCompareByZOrder compare;
		m_aryItem.sort(compare);
	}

	INT CResGroup::tagCompareByZOrder::operator()(const CResItem *pItem1, const CResItem *pItem2) const
	{
		if (pItem1->getZOrder() == pItem2->getZOrder())
		{
			return 0;
		}
		else if (pItem1->getZOrder() > pItem2->getZOrder())
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}

	BOOLEAN CResGroup::mergeChildren(CResGroup* pMergeSrc)
	{
		UINT uiItemCount;

		uiItemCount = pMergeSrc->getItemSize();
		for (UINT uiIndex = 0; uiIndex < uiItemCount; ++uiIndex)
		{
			CResItem* pItemSrc = pMergeSrc->getItemByIndex(uiIndex);
			CResItem* pItemDest = this->getItemByName(pItemSrc->getName());

			if (pItemDest == NULL)
			{
				// NOTE: if ResItem is FuncWidget or CtrlWidget, 
				// the RefID of them will be merge in themself.
				pItemDest = this->createItem(pItemSrc->getType());
				pItemDest->setZOrder(0); // NOTE: We must set ZOrder to 0, otherwise we can't merge ZOrder value!

				pItemDest->merge(pItemSrc, TRUE);
				setMergeFlag();
			}
			else
			{
				pItemDest->merge(pItemSrc, FALSE);
				if (pItemDest->hasMergeFlag())
				{
					setMergeFlag();
				}
			}
		}

		uiItemCount = this->getItemSize();
		for (INT iIndex = uiItemCount - 1; iIndex >= 0 ; --iIndex)
		{
			CResItem* pItemDest = this->getItemByIndex(iIndex);
			CResItem* pItemSrc = pMergeSrc->getItemByName(pItemDest->getName());
			if (pItemSrc == NULL)
			{
				this->deleteItem(pItemSrc);
				setMergeFlag();
			}
		}

		return TRUE;
	}

}