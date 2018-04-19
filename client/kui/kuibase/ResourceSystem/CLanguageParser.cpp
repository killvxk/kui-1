#include "OpenHMI.h"

namespace OpenHMI
{
	const OpenHMI::INT	ENC_TABLESIZE		= 1;
	const OpenHMI::CHAR	ENC_CHARTABLE[]		= {CHAR('\n')};
	const OpenHMI::CHAR* ENC_DESTTABLE[]	= {L"\\n"};

	CLanguageParser	*CLanguageParser::s_pInstance	= NULL;

	CLanguageParser* CLanguageParser::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CLanguageParser();
		}

		return s_pInstance;
	}

	void CLanguageParser::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CLanguageParser::CLanguageParser()
	{
		;
	}

	CLanguageParser::~CLanguageParser()
	{
		;
	}

	BOOLEAN CLanguageParser::parse(const String &strPath)
	{
		CNodeParser parser;

		if (parser.parse(strPath) == FALSE)
		{
			return FALSE;
		}

		Map<String, String, tagStringCompare>* pSection = parser.getSection(L"LANGUAGE");
		if (pSection != NULL)
		{
			buildStringTable(pSection);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOLEAN CLanguageParser::save(const String &strPath)
	{
		CNodeParser parser;
		parser.createSection(L"LANGUAGE");

		m_mapStringTable.resetIterator();
		while (m_mapStringTable.hasMore())
		{
			MapPair<UINT, String> itemPair = m_mapStringTable.nextKeyAndValue();
			parser.setItem(L"LANGUAGE", String::toString(*itemPair.pKey), *itemPair.pData);
		}

		return parser.save(strPath);
	}

	const String& CLanguageParser::getString(UINT uiID)
	{
		String *pstr = m_mapStringTable.getValueByKey(uiID);

		if (pstr != NULL)
		{
			return *pstr;
		}
		else
		{
			return STRING_NULL;
		}
	}

	void  CLanguageParser::setString(UINT uiID, const String& strData)
	{
		String *pstrData = new String(strData);
		if (m_mapStringTable.isExist(uiID))
		{
			m_mapStringTable.setItem(uiID, pstrData);
		}
		else
		{
			m_mapStringTable.addItem(uiID, pstrData);
		}
	}

	void CLanguageParser::buildStringTable(Map<String, String, tagStringCompare>* pSection)
	{
		m_mapStringTable.clear();
		
		pSection->resetIterator();
		while (pSection->hasMore())
		{
			MapPair<String, String> pair = pSection->nextKeyAndValue();

			String *pstrData = new String();
			OpenHMI::String::decodeChars(*pair.pData, ENC_TABLESIZE, ENC_CHARTABLE, ENC_DESTTABLE, *pstrData);
			
			m_mapStringTable.addItem(String::toInt(*pair.pKey),pstrData);
		}
	}
}