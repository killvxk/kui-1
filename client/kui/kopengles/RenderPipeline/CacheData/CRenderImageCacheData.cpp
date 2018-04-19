#include "RenderEngine.h"

namespace OpenHMI
{

	CRenderImageCacheData::CRenderImageCacheData(CRenderImage* pRndImage)
		: m_pTex(NULL)
		, m_pDIB(NULL)
		, m_pRndImage(pRndImage)
		, m_bPropertyChanged(TRUE)
	{
		;
	}

	CRenderImageCacheData::~CRenderImageCacheData()
	{
		if ( m_pDIB )
		{
			delete m_pDIB;
			m_pDIB = NULL;
		}

		m_pRndImage = NULL;
	}

	void CRenderImageCacheData::notifyContentPropertyChanged()
	{
		m_bPropertyChanged = TRUE;
	}

	void CRenderImageCacheData::flush(/*CCacheImage* pImage*/)
	{
		if (m_bPropertyChanged)
		{
			if (m_pTex == NULL || m_pTex->getName() != String::format(L"s%d", g_uiSessionID) + m_pRndImage->getFilePath())
			{		
				if(m_pRndImage->getIsBitmap() == TRUE)
				{
					m_pTex = CTexturePool::getInstance()->getExternalTexture(m_pRndImage->getFilePath(), m_pRndImage->getBitmap());
				}
				else
				{
					m_pTex = CTexturePool::getInstance()->getExternalTexture(m_pRndImage->getFilePath());
				}
			
				flushMaterial();
			}
			
			m_bPropertyChanged = FALSE;
		}
	}

	CCacheTexture* CRenderImageCacheData::getTexture()
	{
		return m_pTex;
	}

	void CRenderImageCacheData::flushMaterial()
	{
		m_pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_UI_DEFAULT,m_pTex);
	}

}