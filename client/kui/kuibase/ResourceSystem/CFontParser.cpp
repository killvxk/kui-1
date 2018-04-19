#include "OpenHMI.h"

namespace OpenHMI
{
	CFontParser	*CFontParser::s_pInstance	= NULL;

	CFontParser* CFontParser::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CFontParser();
		}

		return s_pInstance;
	}

	void CFontParser::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CFontParser::CFontParser()
	{
		;
	}

	CFontParser::~CFontParser()
	{
		;
	}

	BOOLEAN CFontParser::parse(const String &strPath)
	{
		CNodeParser parser;
		if (parser.parse(strPath) == FALSE)
		{
			return FALSE;
		}

		// Get font root folder
		CHAR acFolder[1024] = {0};
		CPath::getFileFolder(strPath, acFolder);
		m_strFontFolder = acFolder;

		return buildFontFamily(parser);
	}

	void CFontParser::empty()
	{
		m_strFontFolder = STRING_NULL;
		m_mapFontEntry.clear();
	}

	UINT CFontParser::getFontCount()
	{
		return m_mapFontEntry.size();
	}

	const CFontParser::tagFontEntry* CFontParser::getFontByIndex(UINT uiIndex)
	{
		return m_mapFontEntry.getValueByIndex(uiIndex);
	}	

	const CFontParser::tagFontEntry* CFontParser::getFontByName(const String &strFontName)
	{
		return m_mapFontEntry.getValueByKey(strFontName);
	}	

	BOOLEAN CFontParser::buildFontFamily(CNodeParser &parser)
	{
		Map<String, String, tagStringCompare>* pSection;

		m_mapFontEntry.clear();
		for (UINT i = 1, j = parser.getSectionCount(); i <= j; ++i)
		{
			String strSection = String::format(L"FONT_%d", i);
			pSection = parser.getSection(strSection);
			if (pSection == NULL)
			{
				break;
			}

			String* pstrFontType = pSection->getValueByKey(L"type");
			String* pstrFontName = pSection->getValueByKey(L"name"); // MUST
			String* pstrFontFile = pSection->getValueByKey(L"file"); // MUST
			String* pstrFaceIndex = pSection->getValueByKey(L"faceindex"); // MUST
			if ((pstrFontFile == NULL) || (*pstrFontFile == L"") ||
				(pstrFontName == NULL) || (*pstrFontName == L"") ||
				(pstrFaceIndex == NULL) || (*pstrFaceIndex == L""))
			{
				continue;
			}

			// Create new fontEntry if it isn't exist
			tagFontEntry* pFontEntry = m_mapFontEntry.getValueByKey(*pstrFontName);
			if (pFontEntry != NULL)
			{
				//ASSERT(FALSE);
				continue;
			}

			pFontEntry = new tagFontEntry();
			pFontEntry->eFontType = strToFontType(pstrFontType);
			pFontEntry->strFontName = *pstrFontName; 


			UINT size = GetWindowsDirectory(NULL, 0);

			wchar_t *path = new wchar_t[size];
			if (GetWindowsDirectory(path, size) != 0) //函数调用失败将返回0
			{
				m_strFontFolder = path;
				m_strFontFolder = m_strFontFolder+ L"\\Fonts\\";
			}
			delete[] path;

			pFontEntry->strFilePath = m_strFontFolder + *pstrFontFile;

			if (CFile::isExist((const CHAR*)pFontEntry->strFilePath) == FALSE)
			{
				pFontEntry->strFilePath = CPath::changeFileExtern((const CHAR*)pFontEntry->strFilePath, L"ttc");
			}

			pFontEntry->uiFaceIndex = String::toUInt(*pstrFaceIndex);

			this->m_mapFontEntry.addItem(*pstrFontName, pFontEntry);
		}
		return TRUE;
	}

	CFontParser::EFontType CFontParser::strToFontType(const String *pstrType)
	{
		if (pstrType == NULL)
		{
			return FT_TTF; // default
		}

		if (*pstrType == L"ttf")
		{
			return FT_TTF;
		}
		else if (*pstrType == L"bmp")
		{
			return FT_BMP;
		}
		return FT_TTF; // default
	}

}