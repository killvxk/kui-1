#include "ResourceSystem/ZipSkin.h"
#include "OpenHMI.h"

namespace OpenHMI
{

	CNodeParser::CNodeParser()
	{
		;
	}

	CNodeParser::~CNodeParser()
	{
		;
	}

	BOOLEAN CNodeParser::parse(const String &strPath)
	{
		BYTE* buffer = NULL;
		UINT size = 0;
		if (CZipSkin::getInstance()->LoadFileFromZipEx(strPath, &buffer, &size))
		{
			String			strContent;
			String::convertFromBYTE_UTF16(buffer, size, strContent);

			m_mapSection.clear();
			parseFile((const CHAR*)strContent);

			return TRUE;
		}
		return FALSE;
	}

	BOOLEAN CNodeParser::save(const String &strPath)
	{
		BOOLEAN bRet = FALSE;
		String strSection;

		m_mapSection.resetIterator();
		while (m_mapSection.hasMore())
		{
			MapPair<String, tagSection> sectionPair = m_mapSection.nextKeyAndValue();
			tagSection *pSection = sectionPair.pData;

			String strItem;
			pSection->mapItem.resetIterator();
			while (pSection->mapItem.hasMore())
			{
				MapPair<String, String> itemPair = pSection->mapItem.nextKeyAndValue();
				strItem = strItem + String::format(L"%ls=%ls;\n", (const CHAR*)*itemPair.pKey, (const CHAR*)*itemPair.pData);
			}

			if (strItem.length() > 0)
			{
				strSection = strSection + (String::format(L"[%ls]\n", (const CHAR*)*sectionPair.pKey) + strItem + L"\n");
			}
			else
			{
				strSection = strSection + String::format(L"[%ls]\n\n", (const CHAR*)*sectionPair.pKey);
			}
		}

		CFile file;
		if (file.open((CHAR*)(const CHAR*)strPath, FALSE))
		{
			BYTE abtBOM[2];
			abtBOM[0] = 0xFF;
			abtBOM[1] = 0xFE;
			file.writeData(abtBOM, 2);

			bRet = file.writeData((BYTE*)(const CHAR *)strSection, strSection.length() * 2);
		}
		return bRet;
	}

	Map<String, String, tagStringCompare>* CNodeParser::getSection(const String &strSection)
	{
		tagSection * pSection = m_mapSection[strSection];
		if (pSection != NULL)
		{
			return &pSection->mapItem;
		}
		else
		{
			return NULL;
		}

	}

	const String& CNodeParser::getItemString(const String &strSectionName, const String &strKey)
	{
		return getItemValue(strSectionName, strKey);
	}

	INT CNodeParser::getItemInt(const String &strSectionName, const String &strKey)
	{
		return String::toInt(getItemValue(strSectionName,strKey));
	}

	UINT CNodeParser::getItemUInt(const String &strSectionName, const String &strKey)
	{
		return String::toUInt(getItemValue(strSectionName,strKey));
	}

	FLOAT CNodeParser::getItemFloat(const String &strSectionName, const String &strKey)
	{
		return String::toFloat(getItemValue(strSectionName,strKey));
	}

	BOOLEAN CNodeParser::getItemBool(const String &strSectionName, const String &strKey)
	{
		return String::toBool(getItemValue(strSectionName,strKey));
	}

	void CNodeParser::createSection(const String &strSection)
	{
		tagSection *pSection = new tagSection;
		m_mapSection.addItem(strSection, pSection);
	}

	void CNodeParser::deleteSection(const String &strSection)
	{
		m_mapSection.deleteItem(strSection);
	}

	void CNodeParser::setItem(const String &strSectionName, const String &strKey, const String &strValue)
	{
		tagSection *pSection = m_mapSection.getValueByKey(strSectionName);
		if (pSection != NULL)
		{
			String *pstrValue = new String(strValue);
			pSection->mapItem.setItem(strKey, pstrValue);
		}
	}

	void CNodeParser::setItem(const String &strSectionName, const String &strKey, INT iValue)
	{
		tagSection *pSection = m_mapSection.getValueByKey(strSectionName);
		if (pSection != NULL)
		{
			String *pstrValue = new String(String::toString(iValue));
			pSection->mapItem.setItem(strKey, pstrValue);
		}
	}

	void CNodeParser::setItem(const String &strSectionName, const String &strKey, UINT uiValue)
	{
		tagSection *pSection = m_mapSection.getValueByKey(strSectionName);
		if (pSection != NULL)
		{
			String *pstrValue = new String(String::toString(uiValue));
			pSection->mapItem.setItem(strKey, pstrValue);
		}
	}

	void CNodeParser::setItem(const String &strSectionName, const String &strKey,  FLOAT fValue)
	{
		tagSection *pSection = m_mapSection.getValueByKey(strSectionName);
		if (pSection != NULL)
		{
			String *pstrValue = new String(String::toString(fValue));
			pSection->mapItem.setItem(strKey, pstrValue);
		}
	}

	void CNodeParser::setItem(const String &strSectionName, const String &strKey, BOOLEAN bValue)
	{
		tagSection *pSection = m_mapSection.getValueByKey(strSectionName);
		if (pSection != NULL)
		{
			String *pstrValue = new String(String::toString(bValue));
			pSection->mapItem.setItem(strKey, pstrValue);
		}
	}

	void CNodeParser::parseFile(const CHAR *pcData)
	{
		tagSection		*pSection = NULL;
		EParserState	eState = PS_SECTION_START;
		ArrayPtr<CHAR>	pcKey;
		CHAR			c = NULL;
		INT				iStart = 0;
		INT				iEnd = 0;
		INT				iIndex = 0;
		
		c = pcData[iIndex++];

		while (c != 0)
		{
			if (c != L'\t' && c != L'\r' && c != L'\n')
			{	
				switch ( eState )
				{
				case PS_SECTION_START:
					{				
						if (c == L'[')
						{
							eState = PS_SECTION_END;
							iStart = iIndex;
						}

						break;
					}
				case PS_SECTION_END:
					{				
						if (c == L']')
						{
							eState = PS_KEY_START;
							// save section;
							iEnd = iIndex - 1;
							
							INT iSize = iEnd - iStart + 1;
							ArrayPtr<CHAR> pcTemp(new CHAR[iSize], iSize);
							String::strncpy(pcTemp, (const CHAR*)(pcData+iStart), iEnd - iStart);
							pSection = new tagSection;

							m_mapSection.addItem(String(pcTemp), pSection);
						}

						break;
					}
				case PS_KEY_START:
					{
						if (c == L'[')
						{
							eState = PS_SECTION_END;
							iStart = iIndex;
						}
						else
						{
							eState = PS_KEY_END;
							iStart = iIndex - 1;
						}

						break;
					}
				case PS_KEY_END:
					{
						if (c == L'=')
						{	
							eState = PS_VALUE;
							// save key;
							iEnd = iIndex - 1;

							INT iSize = iEnd - iStart + 1;
							pcKey.attach(new CHAR[iSize], iSize);
							String::strncpy(pcKey, (const CHAR*)(pcData+iStart), iEnd - iStart);
							iStart = iIndex;
						}
						
						break;
					}
				case PS_VALUE:
					{
						if (c == L';')
						{
							eState = PS_NEXT;
							// save value;	
							iEnd = iIndex - 1;
	
							INT iSize = iEnd - iStart + 1;
							ArrayPtr<CHAR> pcValue(new CHAR[iSize], iSize);
							String::strncpy(pcValue, (const CHAR*)(pcData+iStart), iEnd - iStart);
							String * pValue = new String(pcValue);

							pSection->mapItem.addItem(String(pcKey),pValue);					
						}

						break;
					}
				case PS_NEXT:
					{
						if (c == L'[')
						{
							// start new section
							eState = PS_SECTION_END;
							iStart = iIndex;
						}
						else
						{
							// start new key
							eState = PS_KEY_END;
							iStart = iIndex - 1;
						}

						break;
					}
				}
			}
			c = pcData[iIndex++];
		}
	}

	const String& CNodeParser::getItemValue(const String &strSectionName, const String &strKey)
	{
		tagSection *pSection = m_mapSection[strSectionName];
		if (pSection != NULL)
		{
			String *pstrValue = pSection->mapItem[strKey];

			if (pstrValue != NULL)
			{
				return *pstrValue;
			}
		}
		return STRING_NULL;
	}

	void CNodeParser::parseProperty(const String &strData, CPropertySet &propertySet)
	{
		propertySet.parse(strData);
	}

	String CNodeParser::saveProperty(CPropertySet &propertySet)
	{
		return propertySet.save();
	}

	void CNodeParser::parseStrValue(const String &strData, Array<String> &aryValue)
	{
		aryValue.clear();

		if ( strData != STRING_NULL)
		{
			UINT uiSize		= strData.length();
			UINT uiIndex	= 0;

			for (UINT i = 0; i < uiSize; i++)
			{
				CHAR c = strData[i];
				if (c == L',')
				{
					INT iSize = i - uiIndex + 1;
					ArrayPtr<CHAR> pc(new CHAR[iSize], iSize);

					String::strncpy(pc, strData + uiIndex, i - uiIndex);
					String *pStr = new String(pc);
					uiIndex = i + 1;

					aryValue.addItem(pStr);
				}
			}

			if (uiSize > 0 && uiIndex < uiSize)
			{
				INT iSize = uiSize - uiIndex + 1;
				ArrayPtr<CHAR> pc(new CHAR[iSize], iSize);

				String::strncpy(pc, strData + uiIndex, uiSize - uiIndex);
				String *pStr = new String(pc);

				aryValue.addItem(pStr);
			}
		}
	}

	String CNodeParser::saveStrValue(const Array<String> &aryValue)
	{
		String strContent;

		for (UINT uiIndex = 0; uiIndex < aryValue.size(); uiIndex++)
		{
			strContent = strContent + String::format(L"%ls,", (const CHAR *)*aryValue[uiIndex]);
		}

		// Delete the last ','
		if (strContent != STRING_NULL && strContent.length() > 1)
		{
			strContent = strContent.subString(0, strContent.length() - 1);
		}

		return strContent;
	}

	void CNodeParser::parseUIntValue(const String &strData, Array2<UINT> &aryValue)
	{
		Array<String> aryStrValue;
		CNodeParser::parseStrValue(strData, aryStrValue);

		aryValue.clear();
		aryStrValue.resetIterator();
		while (aryStrValue.hasMore())
		{
			aryValue.addItem(String::toUInt((const CHAR*)(*(aryStrValue.next()))));
		}
	}

	String CNodeParser::saveUIntValue(Array2<UINT> &aryValue)
	{
		String strContent;
		for (UINT uiIndex = 0; uiIndex < aryValue.size(); uiIndex++)
		{
			strContent = strContent + String::format(L"%d,", aryValue[uiIndex]);
		}

		// Delete the last ','
		if (strContent != STRING_NULL && strContent.length() > 1)
		{
			strContent = strContent.subString(0, strContent.length() - 1);
		}

		return strContent;
	}
}