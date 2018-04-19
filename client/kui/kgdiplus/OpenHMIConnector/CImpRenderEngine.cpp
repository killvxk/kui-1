#include "RenderEngine.h"

namespace OpenHMI
{
	CImpRenderEngine	*CImpRenderEngine::s_pInstance	= NULL;

	CImpRenderEngine*	CImpRenderEngine::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CImpRenderEngine();
		}

		return s_pInstance;
	}

	CImpRenderEngine::CImpRenderEngine()
		: m_bInitialized(FALSE)
	{
		;
	}

	CImpRenderEngine::~CImpRenderEngine()
	{
		CFontPool::deleteInstance();	
		CImagePool::deleteInstance();
		CRenderEngineOS::deleteInstance();
	}

	BOOLEAN CImpRenderEngine::initialize(const tagScreenContext *pScreenContext)
	{
		if (m_bInitialized == FALSE)
		{
			if (CRenderEngineOS::getInstance()->initialize(pScreenContext) == FALSE)
			{
				return FALSE;
			}

			CImagePool::getInstance()->initialize();
			CFontPool::getInstance()->initialize();		

			m_bInitialized = TRUE;
		}

		return m_bInitialized;
	}

	void CImpRenderEngine::unInitialize(const tagScreenContext *pScreenContext)
	{
		if (m_bInitialized == TRUE)
		{
			m_mapRenderSession.clear();

			CFontPool::getInstance()->unInitialize();
			CImagePool::getInstance()->unInitialize();
			
			CRenderEngineOS::getInstance()->unInitialize(pScreenContext);

			m_bInitialized = FALSE;
		}
	}

	IRenderSession* CImpRenderEngine::getRenderSession(UINT uiID)
	{
		IRenderSession *pRenderSession = m_mapRenderSession[uiID];
		if (pRenderSession == NULL)
		{
			pRenderSession = new CImpRenderSession();
			m_mapRenderSession.addItem(uiID, pRenderSession);
		}

		return pRenderSession;
	}

	void CImpRenderEngine::destroyRenderSession(UINT uiID)
	{
		m_mapRenderSession.deleteItem(uiID);
	}

	void CImpRenderEngine::releaseCacheData()
	{
		CFontPool::getInstance()->clear();
		CImagePool::getInstance()->clear();	
	}

	void CImpRenderEngine::getImageInfo(const String &strFilePath, UINT *puiWidth, UINT *puiHeight)
	{
		if (puiWidth != NULL)
		{
			*puiWidth = 0;
		}
		if (puiHeight != NULL)
		{
			*puiHeight = 0;
		}

		if (strFilePath.length() == 0)
		{
			return;
		}

		CCacheImagePtr ptrImage(CImagePool::getInstance()->getImage(strFilePath));

		if (puiWidth != NULL)
		{
			*puiWidth = ptrImage != NULL ? ptrImage->getWidth() : 0;
		}

		if (puiHeight != NULL)
		{
			*puiHeight = ptrImage != NULL ? ptrImage->getHeight() : 0;
		}
	}

	void CImpRenderEngine::getImageInfo(const String &strFilePath, Gdiplus::Bitmap* pBitmap, UINT *puiWidth, UINT *puiHeight)
	{
		if (puiWidth != NULL)
		{
			*puiWidth = 0;
		}

		if (puiHeight != NULL)
		{
			*puiHeight = 0;
		}

		if (pBitmap == NULL)
		{
			return;
		}

		CCacheImagePtr ptrImage(CImagePool::getInstance()->getImage(strFilePath, pBitmap));
		if ( ptrImage.isNotNull() )
		{
			if (puiWidth != NULL)
			{
				*puiWidth = ptrImage->getWidth();;
			}

			if (puiHeight != NULL)
			{
				*puiHeight = ptrImage->getHeight();
			}
		}
	}

	void CImpRenderEngine::getTextInfo(CRenderText *pRndText, UINT *puiWidth, UINT *puiHeight, UINT *puiLineHeight)
	{
		if (puiWidth != NULL)
		{
			*puiWidth = 0;
		}

		if (puiHeight != NULL)
		{
			*puiHeight = 0;
		}

		if (puiLineHeight != NULL)
		{
			*puiLineHeight = 0;
		}

		if (pRndText == NULL)
		{
			return;
		}

		// If we have cached the DIB data, we can get width and height from it.
		CCacheData* pCacheData = (CCacheData*)(pRndText->getCacheData());
		if (pCacheData != NULL)
		{
			CDIB* pDib = pCacheData->getDIB();
			if (pDib != NULL)
			{
				if (puiWidth != NULL)
				{
					*puiWidth = pDib->getWidth();
				}

				if (puiHeight != NULL)
				{
					*puiHeight = pDib->getHeight();
				}

				if (puiLineHeight != NULL)
				{
					*puiLineHeight = pCacheData->getLineHeight();
				}

				return;
			}
		}
			
		// Get width and height from CTextLayout.
		CTextLayout textLayout;
		textLayout.getTextInfo(pRndText, puiWidth, puiHeight, puiLineHeight);
	}
}