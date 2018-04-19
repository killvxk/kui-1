#include "RenderEngine.h"

namespace OpenHMI
{
	CTexturePool* CTexturePool::s_pInstance	= NULL;

	CTexturePool*	CTexturePool::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CTexturePool();
		}

		return s_pInstance;
	}

	void CTexturePool::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CTexturePool::CTexturePool()
		: m_uiIndex(0)
	{
		;
	}

	CTexturePool::~CTexturePool()
	{
		;
	}

	void CTexturePool::initialize()
	{
		m_cachePool.setPoolSize(CONFIG::getTextureCacheCount());
		m_aryTextMergetTexture.resize(100);
	}

	void CTexturePool::unInitialize()
	{
		clear();
	}

	void CTexturePool::clear()
	{
		m_cachePool.clear();
		m_aryTextMergetTexture.clear();
	}

	CCacheTexture* CTexturePool::getExternalTexture(const String& strFilePath)
	{
		CCacheTexture* pTex = m_cachePool.getCacheObject(String::format(L"s%d", g_uiSessionID) + strFilePath);
		if ( pTex == NULL )
		{
			CCacheImagePtr pImage(CImagePool::getInstance()->getImage(strFilePath));
			if( pImage.isNotNull() )
			{
				pTex = createTexture(pImage->getWidth(),pImage->getHeight(),pImage->getPixelData(),CCacheTexture::TEXFMT_RGBA8888);
				pTex->setName(String::format(L"s%d", g_uiSessionID) + strFilePath);
				m_cachePool.addCacheObject(String::format(L"s%d", g_uiSessionID) + strFilePath, pTex);
			}
		}

		return pTex;
	}

	CCacheTexture* CTexturePool::getExternalTexture(const String& strFilePath, Gdiplus::Bitmap* pBitmap)
	{
		CCacheTexture* pTex = m_cachePool.getCacheObject(String::format(L"s%d", g_uiSessionID) + strFilePath);

		if ( pTex == NULL )
		{
			CCacheImagePtr pImage(CImagePool::getInstance()->getImage(strFilePath, pBitmap));
			if( pImage.isNotNull() )
			{
				pTex = createTexture(pImage->getWidth(),pImage->getHeight(),pImage->getPixelData(),CCacheTexture::TEXFMT_RGBA8888);
				pTex->setName(String::format(L"s%d", g_uiSessionID) + strFilePath);
				m_cachePool.addCacheObject(String::format(L"s%d", g_uiSessionID) + strFilePath, pTex);
			}
		}

		return pTex;
	}

	CCacheTexture* CTexturePool::getTexture(CDIB* pDib, UINT uiTexFormat)
	{		
		String strName = generateName(L"DibTex",m_uiIndex++);

		CCacheTexture* pTex = createTexture(pDib->getWidth(),pDib->getHeight(),pDib->getPixelData(),uiTexFormat);
		pTex->setName(String::toString(g_uiSessionID) + strName);

		return pTex;
	}

	CCacheTexture* CTexturePool::getTexture(CRenderTarget* pRT, UINT uiTexFormat)
	{
		String strName = String::format(L"RTTexture-%u-%u-%u", pRT->getWidth(), pRT->getHeight(), pRT->getID(), uiTexFormat);

		CCacheTexture* pTex = createTexture(pRT->getWidth(),pRT->getHeight(),NULL,uiTexFormat);
		pTex->setName(String::toString(g_uiSessionID) + strName);

		return pTex;
	}

	CTextMergeTexture* CTexturePool::getTextMergeTexture(UINT uiTexFormat)
	{
		UINT uiSession = g_uiSessionID;
		CTextMergeTexturePtr& ptrTex = m_aryTextMergetTexture[uiSession];
		if( ptrTex.isNull() )
		{
			CConfigParser::tagTextRenderOptInfo* pTagTROptInfo = CConfigParser::getInstance()->getTextRenderOptInfo();

			ptrTex = new CTextMergeTexture(pTagTROptInfo->uiCellWidth,pTagTROptInfo->uiCellHeight);
			ptrTex->setName(L"TextMergetTex");
			ptrTex->setWidth(pTagTROptInfo->uiWidth);
			ptrTex->setHeight(pTagTROptInfo->uiHeight);
			ptrTex->setFormat(uiTexFormat);
			ptrTex->generateGPUResource(NULL);
		}

		return ptrTex;
	}

	void CTexturePool::freeTexture(CCacheTexture* pTex)
	{
		if ( pTex != NULL )
		{
			m_cachePool.deleteCacheObject(pTex->getName());
		}
	}

	CCacheTexture* CTexturePool::createTexture(INT uiWidth,INT uiHeight,const BYTE* pData,UINT uiTexFormat)
	{
		CCacheTexture* pTex = new CCacheTexture();

		pTex->setWidth(uiWidth);
		pTex->setHeight(uiHeight);
		pTex->setFormat(uiTexFormat);
		pTex->generateGPUResource(pData);
	
		return pTex;
	}

	String CTexturePool::generateName(const CHAR* sz, UINT uiAddress)
	{
		return String::format(L"%s%u",sz, uiAddress);
	}

}