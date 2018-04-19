#include "OpenHMI.h"

namespace OpenHMI
{
	CResNode::CResNode()
		: m_uiID(0)
		, m_strName(L"")
		, m_strType(L"")
		, m_eMergeState(MS_UNKNOWN)
	{
		m_propertySet.setProperty(L"id", m_uiID);
		m_propertySet.setProperty(L"name", m_strName);
		m_propertySet.setProperty(L"type", m_strType);
	}

	CResNode::~CResNode()
	{
		;
	}

	const String& CResNode::getPropertyString(const String& strName) const
	{
		return m_propertySet.getPropertyString(strName);
	}

	INT CResNode::getPropertyInt(const String& strName) const
	{
		return m_propertySet.getPropertyInt(strName);
	}

	UINT CResNode::getPropertyUInt(const String& strName) const
	{
		return m_propertySet.getPropertyUInt(strName);
	}

	FLOAT CResNode::getPropertyFloat(const String& strName) const
	{
		return m_propertySet.getPropertyFloat(strName);
	}

	BOOLEAN CResNode::getPropertyBool(const String& strName) const
	{
		return m_propertySet.getPropertyBool(strName);
	}

	void CResNode::setProperty(const String& strName, const String& strValue)
	{
		if (   ((strName == L"type" || strName == L"name") && strValue.length() == 0)
			|| (strName == L"zorder" && String::toUInt(strValue) == 0xFFFFFFFF)   )
		{
			return;
		}

		m_propertySet.setProperty(strName, strValue);

		notifyPropertyChanged();
	}

	void CResNode::setProperty(const String& strName, INT iValue)
	{
		m_propertySet.setProperty(strName, iValue);

		notifyPropertyChanged();
	}

	void CResNode::setProperty(const String& strName, UINT uiValue)
	{
		m_propertySet.setProperty(strName, uiValue);

		notifyPropertyChanged();
	}

	void CResNode::setProperty(const String& strName, FLOAT fValue)
	{
		m_propertySet.setProperty(strName, fValue);

		notifyPropertyChanged();
	}

	void CResNode::setProperty(const String& strName, BOOLEAN bValue)
	{
		m_propertySet.setProperty(strName, bValue);

		notifyPropertyChanged();
	}

	void CResNode::deleteProperty(const String& strName)
	{
		m_propertySet.deleteProperty(strName);

		notifyPropertyChanged();
	}

	CPropertySet* CResNode::getPropertySet(const String& strName)
	{
		return m_propertySet.getPropertySet(strName);
	}

	void CResNode::setID(UINT uiValue)
	{
		m_uiID = uiValue;

		m_propertySet.setProperty(L"id", m_uiID);
	}

	void CResNode::setName(const String& strValue)
	{
		if (strValue.length() == 0)
		{
			return;
		}

		m_strName = strValue;

		m_propertySet.setProperty(L"name", m_strName);
	}

	void CResNode::setType(const String &strValue)
	{
		if (strValue.length() == 0)
		{
			return;
		}

		m_strType = strValue;

		m_propertySet.setProperty(L"type", m_strType);

		if (m_strName.length() == 0)
		{
			setName(strValue + L"_" + String::toString(m_uiID));
		}
	}
	
	BOOLEAN CResNode::merge(CResNode *pNode, BOOLEAN bFull)
	{
		if (pNode == NULL)
		{
			return FALSE;
		}

		if (bFull)
		{
			pNode->m_propertySet.cloneTo(&(this->m_propertySet));
			setMergeFlag();
		}

		if (m_uiID != pNode->m_uiID)
		{
			setID(pNode->m_uiID);
			setMergeFlag();
		}

		if (m_strName != pNode->m_strName)
		{
			setName(pNode->m_strName);
			setMergeFlag();
		}

		if (m_strType != pNode->m_strType)
		{
			setType(pNode->m_strType);
			setMergeFlag();
		}

		return TRUE;
	}

	void CResNode::parseProperty(const String &strData)
	{
		m_propertySet.parse(strData);

		m_uiID = getPropertyUInt(L"id");
		m_strName = getPropertyString(L"name");
		m_strType = getPropertyString(L"type");	
	}

	String CResNode::saveProperty()
	{
		return m_propertySet.save();
	}

	void CResNode::setMergeFlag()
	{
		if (m_eMergeState != MS_MERGED)
		{
			m_eMergeState = MS_MERGED;
			m_propertySet.setProperty(L"merged", TRUE);
		}
	}

	BOOLEAN CResNode::hasMergeFlag()
	{
		if (m_eMergeState == MS_UNKNOWN)
		{
			m_eMergeState = getPropertyBool(L"merged") ? MS_MERGED : MS_NORMAL;
		}

		if (m_eMergeState == MS_MERGED)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}		
	}

	void CResNode::clearMergeFlag()
	{
		if (m_eMergeState != MS_NORMAL)
		{
			m_eMergeState = MS_NORMAL;
			m_propertySet.deleteProperty(L"merged");

			CSkinParser::getInstance()->setDirty();
		}
	}

	void CResNode::notifyPropertyChanged()
	{
		m_uiID = getPropertyUInt(L"id");
		m_strName = getPropertyString(L"name");
		m_strType = getPropertyString(L"type");	

		CSkinParser::getInstance()->setDirty();
	}

	void CResNode::cloneTo(CResNode *pNode)
	{
		if (pNode == NULL)
		{
			return;
		}

		m_propertySet.cloneTo(&(pNode->m_propertySet));

		pNode->m_uiID = m_uiID;
		pNode->m_strName = m_strName;
		pNode->m_strType = m_strType;
	}
}