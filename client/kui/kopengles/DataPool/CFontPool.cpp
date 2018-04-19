#include "RenderEngine.h"

namespace OpenHMI
{
	CFontPool* CFontPool::s_pInstance	= NULL;

	CFontPool*	CFontPool::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CFontPool();
		}

		return s_pInstance;
	}

	void CFontPool::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CFontPool::CFontPool() : m_ftLib(NULL)
	{
		;
	}

	CFontPool::~CFontPool()
	{
		;
	}

	void CFontPool::initialize()
	{
		if (m_ftLib == NULL)
		{
			FT_Init_FreeType(&m_ftLib);	
		}
	}

	void CFontPool::unInitialize()
	{
		if (m_ftLib != NULL)
		{
			m_mapFace.clear();

			FT_Done_FreeType(m_ftLib);

			m_ftLib = NULL;
		}
	}

	void CFontPool::clear()
	{
		m_mapFace.clear();
	}

	CFontFace* CFontPool::getFontFace(const String &strFontName, UINT uiFontSize)
	{
		if (m_ftLib == NULL)
		{
			return NULL;
		}

		tagFaceKey faceKey;
		faceKey.strName	= strFontName;
		faceKey.uiSize	= uiFontSize;
		
		CFontFace* pFontFace = m_mapFace.getValueByKey(faceKey);
		if (pFontFace != NULL)
		{
			return pFontFace;
		}

		const CFontParser::tagFontEntry* pFontEntry = CFontParser::getInstance()->getFontByName(strFontName);
		if (pFontEntry == NULL)
		{
			return NULL;
		}

		pFontFace = new CFontFace(m_ftLib, pFontEntry->strFilePath, pFontEntry->uiFaceIndex, uiFontSize);
		if (pFontFace->getHandle() == NULL)
		{
			delete pFontFace;
			return NULL;
		}

		m_mapFace.addItem(faceKey, pFontFace);
		return pFontFace;
	}
}