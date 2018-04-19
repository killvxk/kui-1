
#include "OpenHMI.h"

namespace OpenHMI
{
	CResItem::CResItem()
		: m_pParent(NULL)
		, m_pRawParent(NULL)
		, m_bVisible(TRUE)
		, m_uiZOrder(0)
		, m_uiRefID(0)
	{
		;
	}

	CResItem::~CResItem()
	{
		;
	}

	void CResItem::setParent(CResGroup *pGroup)
	{
		m_pParent = pGroup; 
		
		if (pGroup != NULL)
		{
			m_pRawParent = pGroup;
		}		
	}

	void CResItem::setVisible(BOOLEAN bValue)
	{
		if (m_bVisible == bValue)
		{
			return;
		}

		m_bVisible = bValue;
		m_propertySet.setProperty(L"visible", m_bVisible);

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::setZOrder(UINT uiValue)
	{
		if (uiValue == 0xFFFFFFFF)
		{
			return;
		}

		if (m_uiZOrder == uiValue)
		{
			return;
		}

		m_uiZOrder = uiValue;
		m_propertySet.setProperty(L"zorder", m_uiZOrder);

		if (m_pParent != NULL)
		{
			m_pParent->notifyItemZOrderChanged();
		}
		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::setRefID(UINT uiValue)
	{
		if (m_uiRefID == uiValue)
		{
			return;
		}

		m_uiRefID = uiValue;
		m_propertySet.setProperty(L"refID", m_uiRefID);

		CSkinParser::getInstance()->setDirty();
	}

	UINT CResItem::getTextFieldSize()
	{
		return m_aryTextField.size();
	}

	CResTextField* CResItem::getTextFieldByIndex(UINT uiIndex)
	{
		return CSkinParser::getInstance()->getTextFieldByID(m_aryTextField[uiIndex]);
	}

	CResTextField* CResItem::createTextField()
	{
		CResTextField *pTextField = CSkinParser::getInstance()->createTextField();

		m_aryTextField.addItem(pTextField->getID());

		updateTextFieldProperty();

		CSkinParser::getInstance()->setDirty();

		return pTextField;
	}

	void CResItem::deleteTextField(CResTextField *pTextField)
	{
		if (pTextField == NULL)
		{
			return;
		}

		m_aryTextField.deleteItem(pTextField->getID());

		CSkinParser::getInstance()->deleteTextField(pTextField);

		updateTextFieldProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::removeTextField(CResTextField *pTextField)
	{
		if (pTextField == NULL)
		{
			return;
		}

		m_aryTextField.deleteItem(pTextField->getID());

		updateTextFieldProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::addTextField(CResTextField *pTextField)
	{
		if (pTextField == NULL)
		{
			return;
		}

		m_aryTextField.addItem(pTextField->getID());

		updateTextFieldProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::deleteAllTextField()
	{
		m_aryTextField.resetIterator();
		while (m_aryTextField.hasMore())
		{
			CSkinParser::getInstance()->deleteTextField(CSkinParser::getInstance()->getTextFieldByID(m_aryTextField.next()));
		}
		m_aryTextField.clear();

		updateTextFieldProperty();

		CSkinParser::getInstance()->setDirty();
	}

	UINT CResItem::getImageFieldSize()
	{
		return m_aryImageField.size();
	}

	CResImageField* CResItem::getImageFieldByIndex(UINT uiIndex)
	{
		return CSkinParser::getInstance()->getImageFieldByID(m_aryImageField[uiIndex]);
	}

	CResImageField* CResItem::createImageField()
	{
		CResImageField *pImageField = CSkinParser::getInstance()->createImageField();

		m_aryImageField.addItem(pImageField->getID());

		updateImageFieldProperty();

		CSkinParser::getInstance()->setDirty();

		return pImageField;
	}

	void CResItem::deleteImageField(CResImageField *pImageField)
	{
		if (pImageField == NULL)
		{
			return;
		}

		m_aryImageField.deleteItem(pImageField->getID());

		CSkinParser::getInstance()->deleteImageField(pImageField);

		updateImageFieldProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::removeImageField(CResImageField *pImageField)
	{
		if (pImageField == NULL)
		{
			return;
		}

		m_aryImageField.deleteItem(pImageField->getID());

		updateImageFieldProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::addImageField(CResImageField *pImageField)
	{
		if (pImageField == NULL)
		{
			return;
		}

		m_aryImageField.addItem(pImageField->getID());

		updateImageFieldProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::deleteAllImageField()
	{
		m_aryImageField.resetIterator();
		while (m_aryImageField.hasMore())
		{
			CSkinParser::getInstance()->deleteImageField(CSkinParser::getInstance()->getImageFieldByID(m_aryImageField.next()));
		}
		m_aryImageField.clear();

		updateImageFieldProperty();

		CSkinParser::getInstance()->setDirty();
	}

	UINT CResItem::getHitAreaSize()
	{
		return m_aryHitArea.size();
	}

	CResHitArea* CResItem::getHitAreaByIndex(UINT uiIndex)
	{
		return m_aryHitArea[uiIndex];
	}

	CResHitArea* CResItem::createHitArea(const String &strType)
	{
		CResHitArea *pHitArea = new CResHitArea();

		pHitArea->setType(strType);
		pHitArea->deleteProperty(L"name");	// Delete "name" here, because it is unnecessary.
		pHitArea->deleteProperty(L"id");	// Delete "id" here, because it is unnecessary.

		m_aryHitArea.addItem(pHitArea);
		updateHitAreaProperty();

		CSkinParser::getInstance()->setDirty();

		return pHitArea;
	}

	void CResItem::deleteHitArea(CResHitArea *pHitArea)
	{
		if (pHitArea == NULL)
		{
			return;
		}

		m_aryHitArea.deleteItem(pHitArea);
		updateHitAreaProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::removeHitArea(CResHitArea *pHitArea)
	{
		if (pHitArea == NULL)
		{
			return;
		}

		m_aryHitArea.popItem(pHitArea);
		updateHitAreaProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::addHitArea(CResHitArea *pHitArea)
	{
		if (pHitArea == NULL)
		{
			return;
		}

		m_aryHitArea.addItem(pHitArea);
		updateHitAreaProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::deleteAllHitArea()
	{
		m_aryHitArea.clear();
		updateHitAreaProperty();

		CSkinParser::getInstance()->setDirty();
	}

	UINT CResItem::getFilterSize()
	{
		return m_filterSet.getFilterSize();
	}

	CResFilter* CResItem::getFilterByIndex(UINT uiIndex)
	{
		return m_filterSet.getFilterByIndex(uiIndex);
	}

	CResFilter* CResItem::createFilter(const String &strType)
	{
		CResFilter *pFilter = m_filterSet.createFilter(strType);

		updateFilterProperty();

		CSkinParser::getInstance()->setDirty();

		return pFilter;
	}

	void CResItem::deleteFilter(CResFilter *pFilter)
	{
		if (pFilter == NULL)
		{
			return;
		}

		m_filterSet.deleteFilter(pFilter);
		updateFilterProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::removeFilter(CResFilter *pFilter)
	{
		if (pFilter == NULL)
		{
			return;
		}

		m_filterSet.removeFilter(pFilter);
		updateFilterProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::addFilter(CResFilter *pFilter)
	{
		if (pFilter == NULL)
		{
			return;
		}

		m_filterSet.addFilter(pFilter);
		updateFilterProperty();

		CSkinParser::getInstance()->setDirty();
	}

	void CResItem::deleteAllFilter()
	{
		m_filterSet.clearAll();
		updateFilterProperty();

		CSkinParser::getInstance()->setDirty();
	}

	CResItem* CResItem::clone()
	{
		CResItem *pNew = new CResItem();
		
		cloneTo(pNew);	
		pNew->m_pParent = NULL;

		return pNew;
	}

	BOOLEAN CResItem::merge(CResNode *pNode, BOOLEAN bFull)
	{
		CResItem* pMergeSrc = dynamic_cast<CResItem*>(pNode);

		if (pMergeSrc == NULL)
		{
			return FALSE;
		}

		if (!CResNode::merge(pNode, bFull))
		{
			return FALSE;
		}

		if (m_bVisible != pMergeSrc->m_bVisible)
		{
			setVisible(pMergeSrc->m_bVisible);
			setMergeFlag();
		}
		else
		{
			// We must add property to map, because default value is TRUE.
			m_propertySet.setProperty(L"visible", m_bVisible);
		}

		if (m_uiZOrder != pMergeSrc->m_uiZOrder)
		{
			setZOrder(pMergeSrc->m_uiZOrder);
			setMergeFlag();
		}

		if (m_uiRefID != pMergeSrc->m_uiRefID)
		{
			setRefID(pMergeSrc->m_uiRefID);
			setMergeFlag();
		}

		// text fields
		if (!compareTextField(pMergeSrc))
		{
			m_aryTextField.clear();
			m_aryTextField.addItems(pMergeSrc->m_aryTextField);
			updateTextFieldProperty();
			setMergeFlag();
		}

		// image fields
		if (!compareImageField(pMergeSrc))
		{
			m_aryImageField.clear();
			m_aryImageField.addItems(pMergeSrc->m_aryImageField);
			updateImageFieldProperty();
			setMergeFlag();
		}

		if (bFull)
		{
			// HitArea
			pMergeSrc->m_aryHitArea.resetIterator();
			while (pMergeSrc->m_aryHitArea.hasMore())
			{
				m_aryHitArea.addItem(pMergeSrc->m_aryHitArea.next()->clone());
			}
			updateHitAreaProperty();

			// filter set.
			pMergeSrc->m_filterSet.cloneTo(&(this->m_filterSet));
			updateFilterProperty();

			setMergeFlag();
		}

		return TRUE;
	}

	void CResItem::clearMergeFlag()
	{
		CResNode::clearMergeFlag();

		// For TextFiels
		m_aryTextField.resetIterator();
		while (m_aryTextField.hasMore())
		{
			CResTextField* pResTextField = CSkinParser::getInstance()->getTextFieldByID(m_aryTextField.next());
			if (pResTextField)
			{
				pResTextField->clearMergeFlag();
			}
		}

		// For ImageFields
		m_aryImageField.resetIterator();
		while (m_aryImageField.hasMore())
		{
			CResImageField* pResImageField = CSkinParser::getInstance()->getImageFieldByID(m_aryImageField.next());
			if (pResImageField)
			{
				pResImageField->clearMergeFlag();
			}
		}
	}

	void CResItem::parseProperty(const String &strData)
	{
		CResNode::parseProperty(strData);

		m_bVisible = CResNode::getPropertyBool(L"visible");
		m_uiZOrder = CResNode::getPropertyUInt(L"zorder");
		m_uiRefID = CResNode::getPropertyUInt(L"refID");

		getTextFieldList();
		getImageFieldList();
		getHitAreaList();
		getFilterList();
	}

	String CResItem::saveProperty()
	{
		updateHitAreaProperty();
		updateFilterProperty();

		return CResNode::saveProperty();
	}	

	void CResItem::notifyPropertyChanged()
	{
		CResNode::notifyPropertyChanged();

		m_bVisible = CResNode::getPropertyBool(L"visible");
		m_uiZOrder = CResNode::getPropertyUInt(L"zorder");
		m_uiRefID = CResNode::getPropertyUInt(L"refID");

		getTextFieldList();
		getImageFieldList();

		if (m_pParent != NULL)
		{
			m_pParent->notifyItemZOrderChanged();
		}
	}

	void CResItem::cloneTo(CResNode *pNode)
	{
		CResNode::cloneTo(pNode);

		CResItem* pItem = (CResItem*)pNode;

		pItem->m_pParent = m_pParent;
		pItem->m_pRawParent = m_pRawParent;
		pItem->m_bVisible = m_bVisible;
		pItem->m_uiZOrder = m_uiZOrder;
		pItem->m_uiRefID = m_uiRefID;

		// Clone the text field.
		m_aryTextField.resetIterator();
		while (m_aryTextField.hasMore())
		{
			CResTextField * pTextField = CSkinParser::getInstance()->getTextFieldByID(m_aryTextField.next());
			if (pTextField != NULL)
			{
				pItem->m_aryTextField.addItem(pTextField->clone()->getID());
			}
		}
		pItem->updateTextFieldProperty();

		// Clone the image field.
		m_aryImageField.resetIterator();
		while (m_aryImageField.hasMore())
		{
			CResImageField * pImageField = CSkinParser::getInstance()->getImageFieldByID(m_aryImageField.next());
			if (pImageField != NULL)
			{
				pItem->m_aryImageField.addItem(pImageField->clone()->getID());
			}
		}
		pItem->updateImageFieldProperty();

		// Clone the hit area.
		m_aryHitArea.resetIterator();
		while (m_aryHitArea.hasMore())
		{
			pItem->addHitArea(m_aryHitArea.next()->clone());
		}
		pItem->updateHitAreaProperty();

		// Clone the filter.
		m_filterSet.cloneTo(&(pItem->m_filterSet));
		pItem->updateFilterProperty();
	}

	void CResItem::getTextFieldList()
	{
		String strText = CResNode::getPropertyString(L"txtfield");
		CNodeParser::parseUIntValue(strText, m_aryTextField);
	}

	void CResItem::updateTextFieldProperty()
	{
		if (m_aryTextField.size() == 0)
		{
			m_propertySet.deleteProperty(L"txtfield");
			return;
		}

		String strContent = CNodeParser::saveUIntValue(m_aryTextField);
		m_propertySet.setProperty(L"txtfield", strContent);
	}

	void CResItem::getImageFieldList()
	{
		String strImage = CResNode::getPropertyString(L"imgfield");
		CNodeParser::parseUIntValue(strImage, m_aryImageField);
	}
	
	void CResItem::updateImageFieldProperty()
	{	
		if (m_aryImageField.size() == 0)
		{
			m_propertySet.deleteProperty(L"imgfield");
			return;
		}

		String strContent = CNodeParser::saveUIntValue(m_aryImageField);
		m_propertySet.setProperty(L"imgfield", strContent);
	}

	void CResItem::getHitAreaList()
	{
		m_aryHitArea.clear();

		for (UINT uiIndex = 0; uiIndex < m_propertySet.getPropertyCount(); uiIndex++)
		{
			String strName = String::format(L"hitarea%d", uiIndex + 1);
			const String& strValue = m_propertySet.getPropertyString(strName);
			if (strValue != STRING_NULL)
			{
				CResHitArea* pHitArea = new CResHitArea();

				pHitArea->parseProperty(strValue);
				m_aryHitArea.addItem(pHitArea);
			}
			else
			{
				break;
			}
		}		
	}

	void CResItem::updateHitAreaProperty()
	{
		// Clear all old items.
		UINT uiMax = m_propertySet.getPropertyCount();
		for (UINT uiIndex = 0; uiIndex < uiMax; uiIndex++)
		{
			String strKey = String::format(L"hitarea%d", uiIndex + 1);
			if (m_propertySet.hasProperty(strKey))
			{
				m_propertySet.deleteProperty(strKey);
			}
			else
			{
				break;
			}
		}

		// Re-arrange new items.
		UINT uiIndex = 0;
		m_aryHitArea.resetIterator();
		while (m_aryHitArea.hasMore())
		{
			CResHitArea* pHitArea = m_aryHitArea.next();

			String strKey = String::format(L"hitarea%d", uiIndex + 1);
			String strValue = pHitArea->saveProperty();
			m_propertySet.setProperty(strKey, strValue); 

			++uiIndex;
		}
	}

	void CResItem::getFilterList()
	{
		m_filterSet.parse(&m_propertySet);
	}

	void CResItem::updateFilterProperty()
	{
		m_filterSet.save(&m_propertySet);
	}

	BOOLEAN CResItem::compareTextField(CResItem* pResItem)
	{
		if (m_aryTextField.size() != pResItem->m_aryTextField.size())
		{
			return FALSE;
		}
		
		m_aryTextField.resetIterator();
		while (m_aryTextField.hasMore())
		{
			if (!pResItem->m_aryTextField.isExist(m_aryTextField.next()))
			{
				return FALSE;
			}
		}
		
		return TRUE;
	}

	BOOLEAN CResItem::compareImageField(CResItem* pResItem)
	{
		if (m_aryImageField.size() != pResItem->m_aryImageField.size())
		{
			return FALSE;
		}
		
		m_aryImageField.resetIterator();
		while (m_aryImageField.hasMore())
		{
			if (!pResItem->m_aryImageField.isExist(m_aryImageField.next()))
			{
				return FALSE;
			}
		}
		
		return TRUE;
	}

}