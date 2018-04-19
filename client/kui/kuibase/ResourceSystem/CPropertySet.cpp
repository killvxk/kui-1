
#include "OpenHMI.h"

namespace OpenHMI
{
	// Char encode convert table
	const INT	CHAR_ENC_TABLESIZE		= 7;
	const CHAR	CHAR_ENC_CHARTABLE[]	= {CHAR('\\'), CHAR('\n'), CHAR("\r"), CHAR("\t"), CHAR('{'), CHAR('}'), CHAR(';')};
	const CHAR* CHAR_ENC_DESTTABLE[]	= {L"\\\\", L"\\n", L"\\r", L"\\t", L"\\bl", L"\\br", L"\\se"};

	CPropertySet::CPropertySet()
	{
		;
	}

	CPropertySet::~CPropertySet()
	{
		;
	}

	void CPropertySet::parse(const String &strData)
	{
		UINT uiIndex	= 0;
		BOOLEAN bFlag		= FALSE;
		BOOLEAN bSpecial	= FALSE;
		UINT uiSize		= strData.length();	
		CHAR cKey[16]	= {0};

		m_mapProperty.clear();

		for (UINT i = 0; i < uiSize; i++)
		{
			CHAR c = strData[i];

			if (c != L'\r' && c!= L'\n' && c != L'\t')
			{
				if ((c == L':') && !bFlag)
				{
					String::strncpy(cKey, strData + uiIndex, i - uiIndex);
					uiIndex = i + 1;
				}
				else if ((c == L',') && !bFlag)
				{	
					INT iSize = 0;
					if (bSpecial)
					{
						iSize = i - uiIndex;
						bSpecial = FALSE;
					}
					else
					{
						iSize = i - uiIndex + 1;
					}

					ArrayPtr<CHAR> pcValue(new CHAR[iSize], iSize);
					String::strncpy(pcValue, strData + uiIndex, iSize - 1);
					uiIndex = i + 1;
					// Decode Value
					String strValue(pcValue);
					String *pStr = new String();
					String::decodeChars(strValue, CHAR_ENC_TABLESIZE, CHAR_ENC_CHARTABLE, CHAR_ENC_DESTTABLE, *pStr);
					
					m_mapProperty.addItem(cKey, pStr);
				}
				else if (c == L'{')
				{
					bFlag = TRUE;
					uiIndex = i + 1;
				}
				else if (c == L'}')
				{
					bFlag = FALSE;
					bSpecial = TRUE;
				}
			}
		}

		if ( uiSize > 0 && uiIndex < uiSize)
		{
			INT iSize = 0;
			if (bSpecial)
			{
				iSize = uiSize - uiIndex;
				bSpecial = FALSE;
			}
			else
			{
				iSize = uiSize - uiIndex + 1;
			}

			ArrayPtr<CHAR> pcValue(new CHAR[iSize], iSize);
			String::strncpy(pcValue, strData + uiIndex, iSize - 1);
			// Decode Value
			String strValue(pcValue);
			String *pStr = new String();
			String::decodeChars(strValue, CHAR_ENC_TABLESIZE, CHAR_ENC_CHARTABLE, CHAR_ENC_DESTTABLE, *pStr);

			m_mapProperty.addItem(cKey, pStr);
		}
	}

	String CPropertySet::save()
	{
		String strContent = STRING_NULL;

		updatePropertySet();

		m_mapProperty.resetIterator();
		while (m_mapProperty.hasMore())
		{			
			MapPair<String, String> itemPair = m_mapProperty.nextKeyAndValue();

			if (   *itemPair.pKey == L"content" 
				|| *itemPair.pKey == L"src" 
				|| *itemPair.pKey == L"txtfield" 
				|| *itemPair.pKey == L"imgfield" 
				|| *itemPair.pKey == L"fonts" 
				|| String::strstart((const OpenHMI::CHAR *)(*itemPair.pKey), L"hitarea") 
				|| String::strstart((const OpenHMI::CHAR *)(*itemPair.pKey), L"filter") 
				|| m_mapPropertySet.isExist(*itemPair.pKey)
				|| (*itemPair.pKey).lastIndexOf(L"_param") > 0)
			{
				// Encode Value.
				String strEncodeData;
				String::encodeChars(*itemPair.pData, CHAR_ENC_TABLESIZE, CHAR_ENC_CHARTABLE, CHAR_ENC_DESTTABLE, strEncodeData);

				strContent = strContent + String::format(L"%ls:{%ls},", (const CHAR *)(*itemPair.pKey), strEncodeData.length() == 0 ? L"" : (const CHAR *)strEncodeData);
			}
			else
			{
				strContent = strContent + String::format(L"%ls:%ls,", (const CHAR *)(*itemPair.pKey), (const CHAR *)(*itemPair.pData) == NULL ? L"" : (const CHAR *)(*itemPair.pData));
			}
		}

		if (strContent != STRING_NULL && strContent.length() > 1)
		{
			strContent = strContent.subString(0, strContent.length() - 1);
		}

		return strContent;
	}

	UINT CPropertySet::getPropertyCount()
	{
		return m_mapProperty.size();
	}

	BOOLEAN	CPropertySet::hasProperty(const String & strName)
	{
		return m_mapProperty.isExist(strName);
	}

	const String& CPropertySet::getPropertyString(const String& strName) const
	{
		String *pstr = m_mapProperty.getValueByKey(strName);
		if (pstr != NULL)
		{
			return *pstr;
		}
		else
		{
			return STRING_NULL;
		}
	}

	INT CPropertySet::getPropertyInt(const String& strName) const
	{
		String *pstr = m_mapProperty.getValueByKey(strName);
		if (pstr != NULL)
		{
			return String::toInt(*pstr);
		}
		else
		{
			return 0;
		}
	}

	UINT CPropertySet::getPropertyUInt(const String& strName) const
	{
		String *pstr = m_mapProperty.getValueByKey(strName);
		if (pstr != NULL)
		{
			return String::toUInt(*pstr);
		}
		else
		{
			return 0;
		}
	}

	FLOAT CPropertySet::getPropertyFloat(const String& strName) const
	{
		String *pstr = m_mapProperty.getValueByKey(strName);
		if (pstr != NULL)
		{
			return String::toFloat(*pstr);
		}
		else
		{
			return 0.0f;
		}
	}

	BOOLEAN CPropertySet::getPropertyBool(const String& strName) const
	{
		String *pstr = m_mapProperty.getValueByKey(strName);
		if (pstr != NULL)
		{
			return String::toBool(*pstr);
		}
		else
		{
			return FALSE;
		}
	}

	void CPropertySet::setProperty(const String& strName, const String& strValue)
	{
		m_mapProperty.setItem(strName, new String(strValue));
	}

	void CPropertySet::setProperty(const String& strName, INT iValue)
	{
		m_mapProperty.setItem(strName, new String(String::toString(iValue)));
	}

	void CPropertySet::setProperty(const String& strName, UINT uiValue)
	{
		m_mapProperty.setItem(strName, new String(String::toString(uiValue)));
	}

	void CPropertySet::setProperty(const String& strName, FLOAT fValue)
	{
		m_mapProperty.setItem(strName, new String(String::toString(fValue)));
	}

	void CPropertySet::setProperty(const String& strName, BOOLEAN bValue)
	{
		m_mapProperty.setItem(strName, new String(String::toString(bValue)));
	}

	void CPropertySet::deleteProperty(const String& strName)
	{
		m_mapProperty.deleteItem(strName);
		m_mapPropertySet.deleteItem(strName);
	}

	void  CPropertySet::clearAll()
	{
		m_mapProperty.clear();
		m_mapPropertySet.clear();
	}

	CPropertySet* CPropertySet::getPropertySet(const String& strName)
	{
		CPropertySet* pPropSet = m_mapPropertySet.getValueByKey(strName);

		if (pPropSet != NULL)
		{
			return pPropSet;
		}
		
		pPropSet = new CPropertySet();
		
		String *pstr = m_mapProperty.getValueByKey(strName);
		if (pstr != NULL)
		{
			pPropSet->parse(*pstr);
		}
		m_mapPropertySet.setItem(strName, pPropSet);

		return pPropSet;
	}

	CPropertySet* CPropertySet::clone()
	{
		CPropertySet* pNew = new CPropertySet();

		this->cloneTo(pNew);

		return pNew;
	}

	void CPropertySet::cloneTo(CPropertySet* pPropertySet)
	{
		updatePropertySet();

		pPropertySet->m_mapProperty.clear();

		m_mapProperty.resetIterator();
		while (m_mapProperty.hasMore())
		{
			MapPair<String,String> itemKeyValue = m_mapProperty.nextKeyAndValue();
			pPropertySet->m_mapProperty.addItem(*itemKeyValue.pKey, new String(*itemKeyValue.pData));
		}
	}

	void CPropertySet::updatePropertySet()
	{
		for (UINT uiIndex = 0; uiIndex < m_mapPropertySet.size(); ++uiIndex)
		{
			String strName = m_mapPropertySet.getKeyByIndex(uiIndex);
			CPropertySet* pPropertySet = m_mapPropertySet.getValueByIndex(uiIndex);

			pPropertySet->updatePropertySet();
			if (pPropertySet->getPropertyCount() == 0)
			{
				m_mapProperty.deleteItem(strName);
				continue;
			}

			String strContent = pPropertySet->save();
			m_mapProperty.setItem(strName, new String(strContent));
		}
	}

}