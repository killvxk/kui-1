#include "OpenHMI.h"

namespace OpenHMI
{
	CManager_Skin	*CManager_Skin::s_pInstance	= NULL;

	CManager_Skin* CManager_Skin::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CManager_Skin();
		}

		return s_pInstance;
	}

	void CManager_Skin::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CManager_Skin::CManager_Skin() : m_pSkinInfo(NULL)
	{
		;
	}

	CManager_Skin::~CManager_Skin()
	{
		;
	}

	void CManager_Skin::parseConfig()
	{
		m_pSkinInfo = CConfigParser::getInstance()->getSkinInfo();
	}

	void CManager_Skin::initialize()
	{
		// do nothing
	}

	void CManager_Skin::unInitialize()
	{
		if( m_pSkinInfo != NULL )
		{
			m_pSkinInfo->arySkin.clear();
		}
	}	

	UINT CManager_Skin::getSkinCount()
	{
		if( m_pSkinInfo != NULL )
		{
			return m_pSkinInfo->arySkin.size();
		}
		else
		{
			return 0;
		}
	}

	CManager_Skin::tagSkinData CManager_Skin::getSkin(UINT uiIndex)
	{
		tagSkinData data;

		if( m_pSkinInfo != NULL )
		{
			CConfigParser::tagSkin *pSkin = m_pSkinInfo->arySkin[uiIndex];
			if (pSkin != NULL)
			{
				data.strID = pSkin->strId;

				String *pstrName = pSkin->mapName[CManager_Language::getInstance()->getActiveLanguage()];
				if (pstrName != NULL)
				{
					data.strName = *pstrName;
				}
			}
		}
		
		return data;
	}

	void CManager_Skin::setActiveSkin(const String &strID)
	{
		if (m_strActiveSkin == strID)
		{
			return;
		}

		CConfigParser::tagSkin *pSkin = getSkinByID(strID);
		if (pSkin == NULL)
		{
			return;
		}

		m_strNewActiveSkin = strID;

		if (CManager_Language::getInstance()->getActiveLanguage() == STRING_NULL)
		{
			// the language isn't loaded, so don't load skin
			return;
		}
		
		loadSkin(pSkin);
	}

	void CManager_Skin::notifyLanguageChanged()
	{
		if (m_strNewActiveSkin != STRING_NULL)
		{
			loadSkin(getSkinByID(m_strNewActiveSkin));
		}
		else
		{
			m_strNewActiveSkin = m_strActiveSkin;

			loadSkin(getSkinByID(m_strNewActiveSkin));
		}
	}

	CMovieClip* CManager_Skin::createMovieClip(UINT uiImageID)
	{
		CResImage *pImage = CSkinParser::getInstance()->getImageByID(uiImageID);
		if (pImage == NULL)
		{
			return NULL;
		}

		CMovieClip *pMovieClip = new CMovieClip();

		UINT uiSize = pImage->getImageSize();
		for (UINT i = 0; i < uiSize; i++)
		{
			if (pImage->isCommomImage())
			{
				pMovieClip->addImage(CSkinParser::getInstance()->getCommonImageFolder() + pImage->getImage(i));
			}
			else
			{
				pMovieClip->addImage(CSkinParser::getInstance()->getPrivateImageFolder() + pImage->getImage(i));
			}			
		}

		return pMovieClip;
	}

	CConfigParser::tagSkin* CManager_Skin::getSkinByID(const String &strID)
	{
		CConfigParser::tagSkin *pSkin = NULL;

		if( m_pSkinInfo == NULL )
		{
			return NULL;
		}

		m_pSkinInfo->arySkin.resetIterator();
		while (m_pSkinInfo->arySkin.hasMore())
		{
			pSkin = m_pSkinInfo->arySkin.next();
			if( pSkin->strId == strID )
			{
				return pSkin;
			}
		}

		return NULL;
	}

	void CManager_Skin::loadSkin(CConfigParser::tagSkin *pSkin)
	{
		if (pSkin == NULL)
		{
			return;
		}

		String *pstrPath = pSkin->mapPath[CManager_Language::getInstance()->getActiveLanguage()];
		if (pstrPath == NULL)
		{
			return;
		}		

		if (CSkinParser::getInstance()->parse(CConfigParser::getInstance()->getSkinPath() + *pstrPath))
		{
			BOOLEAN bFirstLoadSkin = m_strActiveSkin == STRING_NULL ? TRUE : FALSE;

			m_strActiveSkin = m_strNewActiveSkin;
			m_strNewActiveSkin = STRING_NULL;

			if (bFirstLoadSkin)
			{
				CBaseMainFrame::getInstance()->notifySkinFirstPrepared();
			}
			else
			{
				CBaseMainFrame::getInstance()->notifySkinChanged();
			}
		}
	}
}