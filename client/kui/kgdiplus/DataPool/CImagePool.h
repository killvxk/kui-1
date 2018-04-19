#pragma once

namespace OpenHMI
{
	class CImagePool
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		static	CImagePool*	getInstance();	
		static	void		deleteInstance();

		void		initialize();
		void		unInitialize();
		void		clear();

		CCacheImage* getImage(const String &strFilePath);
		CCacheImage* getImage(const String& strID, Gdiplus::Bitmap* pBitmap);
	protected:// method		
		CImagePool();
		virtual ~CImagePool();

	private:// method

	protected:// property

	private:// property
		static	CImagePool	*s_pInstance;

		CachePool<String,CCacheImage,  tagStringCompare>	m_cachePool;
	};
}