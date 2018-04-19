#include "RenderEngine.h"

namespace OpenHMI
{
	CImagePool	*CImagePool::s_pInstance	= NULL;

	CImagePool*	CImagePool::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CImagePool();
		}

		return s_pInstance;
	}

	void CImagePool::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CImagePool::CImagePool()
	{
		m_cachePool.setPoolSize(CONFIG::getImageCacheCount());
	}

	CImagePool::~CImagePool()
	{
		;
	}

	void CImagePool::initialize()
	{
		;
	}

	void CImagePool::unInitialize()
	{
		m_cachePool.clear();
	}

	void CImagePool::clear()
	{
		m_cachePool.clear();
	}

	CCacheImage*  CImagePool::getImage(const String &strFilePath)
	{
		CCacheImage *pCacheImage = (CCacheImage*)(CacheObject*)m_cachePool.getCacheObject(strFilePath);
		if (pCacheImage != NULL)
		{
			return pCacheImage;
		}	

		CImageParser::tagImageInfo imgInfo = CImageParser::decodeFromFile(strFilePath);
		if (imgInfo.pbtPixelData == NULL)
		{
			return NULL;
		}

		pCacheImage = new CCacheImage(imgInfo.uiWidth, imgInfo.uiHeight, imgInfo.pbtPixelData);
		m_cachePool.addCacheObject(strFilePath, pCacheImage);

		return pCacheImage;
	}

	CCacheImage*  CImagePool::getImage(const String& strID, Gdiplus::Bitmap* pBitmap)
	{
		CCacheImage *pCacheImage = (CCacheImage*)(CacheObject*)m_cachePool.getCacheObject(strID);
		if (pCacheImage != NULL)
		{
			return pCacheImage;
		}	

		CImageParser::tagImageInfo imgInfo = CImageParser::decodeFromBitmap(pBitmap);
		if (imgInfo.pbtPixelData == NULL)
		{
			return NULL;
		}

		pCacheImage = new CCacheImage(imgInfo.uiWidth, imgInfo.uiHeight, imgInfo.pbtPixelData);
		m_cachePool.addCacheObject(strID, pCacheImage);
		pCacheImage->setName(strID);

		return pCacheImage;
	}
}