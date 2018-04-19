
#include "OpenHMI.h"

namespace OpenHMI
{

	CResFilterSet::CResFilterSet()
	{
		;
	}

	CResFilterSet::~CResFilterSet()
	{
		;
	}

	UINT CResFilterSet::getFilterSize()
	{
		return m_aryFilter.size();
	}

	CResFilter* CResFilterSet::getFilterByIndex(UINT uiIndex)
	{
		return m_aryFilter[uiIndex];
	}

	CResFilter* CResFilterSet::createFilter(const String &strType)
	{
		CResFilter *pFilter = new CResFilter();

		pFilter->setType(strType);
		pFilter->deleteProperty(L"name");	// Delete "name" here, because it is unnecessary.
		pFilter->deleteProperty(L"id");		// Delete "id" here, because it is unnecessary.

		m_aryFilter.addItem(pFilter);

		return pFilter;
	}

	void CResFilterSet::deleteFilter(CResFilter *pFilter)
	{
		if (pFilter == NULL)
		{
			return;
		}

		m_aryFilter.deleteItem(pFilter);
	}

	void CResFilterSet::removeFilter(CResFilter *pFilter)
	{
		if (pFilter == NULL)
		{
			return;
		}

		m_aryFilter.popItem(pFilter);
	}

	void CResFilterSet::addFilter(CResFilter *pFilter)
	{
		if (pFilter == NULL)
		{
			return;
		}

		m_aryFilter.addItem(pFilter);
	}

	void CResFilterSet::clearAll()
	{
		m_aryFilter.clear();
	}

	void CResFilterSet::parse(CPropertySet* pPropertySet)
	{
		m_aryFilter.clear();

		for (UINT uiIndex = 0; uiIndex < pPropertySet->getPropertyCount(); uiIndex++)
		{
			String strName = String::format(L"filter%d", uiIndex + 1);
			const String& strValue = pPropertySet->getPropertyString(strName);
			if (strValue != STRING_NULL)
			{
				CResFilter* pFilter = new CResFilter();

				pFilter->parseProperty(strValue);
				m_aryFilter.addItem(pFilter);
			}
			else
			{
				break;
			}
		}		
	}

	void CResFilterSet::save(CPropertySet* pPropertySet)
	{
		// Clear all old items.
		UINT uiMax = pPropertySet->getPropertyCount();
		for (UINT uiIndex = 0; uiIndex < uiMax; uiIndex++)
		{
			String strKey = String::format(L"filter%d", uiIndex + 1);
			if (pPropertySet->hasProperty(strKey))
			{
				pPropertySet->deleteProperty(strKey);
			}
			else
			{
				break;
			}
		}

		// Re-arrange new items.
		UINT uiIndex = 0;
		m_aryFilter.resetIterator();
		while (m_aryFilter.hasMore())
		{
			CResFilter* pFilter = m_aryFilter.next();

			String strKey = String::format(L"filter%d", uiIndex + 1);
			String strValue = pFilter->saveProperty();
			pPropertySet->setProperty(strKey, strValue); 

			++uiIndex;
		}
	}

	void CResFilterSet::cloneTo(CResFilterSet* pFilterSet)
	{
		m_aryFilter.resetIterator();
		while (m_aryFilter.hasMore())
		{
			pFilterSet->addFilter(m_aryFilter.next()->clone());
		}
	}

}