#include "OpenHMI.h"

namespace OpenHMI
{
	CManager_Language	*CManager_Language::s_pInstance	= NULL;

	CManager_Language* CManager_Language::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CManager_Language();
		}

		return s_pInstance;
	}

	void CManager_Language::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CManager_Language::CManager_Language() : m_pLanguageInfo(NULL)
	{
		;
	}

	CManager_Language::~CManager_Language()
	{
		;
	}

	void CManager_Language::parseConfig()
	{
		m_pLanguageInfo = CConfigParser::getInstance()->getLanguageInfo();
	}

	void CManager_Language::initialize()
	{
		// do nothing
	}

	void CManager_Language::unInitialize()
	{
		if( m_pLanguageInfo != NULL )
		{
			m_pLanguageInfo->aryLanguage.clear();
		}
	}

	UINT CManager_Language::getLanguageCount()
	{
		if(m_pLanguageInfo == NULL)
		{
			return 0;
		}

		return m_pLanguageInfo->aryLanguage.size();
	}

	CManager_Language::tagLanguageData CManager_Language::getLanguage(UINT uiIndex)
	{
		tagLanguageData data;

		if( m_pLanguageInfo != NULL )
		{
			CConfigParser::tagLanguage *pLanguage = m_pLanguageInfo->aryLanguage[uiIndex];
			if (pLanguage != NULL)
			{
				data.strID = pLanguage->strId;

				String *pstrName = pLanguage->mapName[m_strActiveLanguage];
				if (pstrName != NULL)
				{
					data.strName = *pstrName;
				}
			}
		}
		
		return data;
	}

	void CManager_Language::setActiveLanguage(const String &strID)
	{
		if (m_strActiveLanguage == strID)
		{
			return;
		}
		
		CConfigParser::tagLanguage *pLanguage = getLanguageByID(strID);
		if (pLanguage == NULL)
		{
			return;
		}		

		if (CLanguageParser::getInstance()->parse(CConfigParser::getInstance()->getSkinPath() + pLanguage->strPath))
		{
			m_strActiveLanguage = strID;

			CManager_Skin::getInstance()->notifyLanguageChanged();
		}
	}

	CConfigParser::tagLanguage* CManager_Language::getLanguageByID(const String &strID)
	{
		CConfigParser::tagLanguage *pLanguage = NULL;

		if( m_pLanguageInfo == NULL )
		{
			return NULL;
		}

		m_pLanguageInfo->aryLanguage.resetIterator();
		while (m_pLanguageInfo->aryLanguage.hasMore())
		{
			pLanguage = m_pLanguageInfo->aryLanguage.next();
			if( pLanguage->strId == strID )
			{
				return pLanguage;
			}
		}

		return NULL;
	}
}