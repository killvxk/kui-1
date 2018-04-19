#pragma once

namespace OpenHMI
{
	class CTexturePool
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		static	CTexturePool*	getInstance();
		static	void			deleteInstance();

		void			initialize();
		void			unInitialize();
		void			clear();

		CCacheTexture*		getExternalTexture(const String& strFilePath);
		CCacheTexture*		getExternalTexture(const String& strFilePath, Gdiplus::Bitmap* pBitmap);
		CCacheTexture*		getTexture(CDIB* pDib, UINT uiTexFormat = CCacheTexture::TEXFMT_RGBA8888);
		CCacheTexture*		getTexture(CRenderTarget* pRT, UINT uiTexFormat = CCacheTexture::TEXFMT_RGBA8888);
		CTextMergeTexture*	getTextMergeTexture(UINT uiTexFormat = CCacheTexture::TEXFMT_RGBA8888);

		void			freeTexture(CCacheTexture* pTex);

	protected:// method	
		CTexturePool();
		virtual ~CTexturePool();

	private:// method		
		CCacheTexture*	createTexture(INT uiWidth,INT uiHeight,const BYTE* pData,UINT uiTexFormat);
		String			generateName(const CHAR* sz, UINT uiAddress);

	protected:// property

	private:// property
		static	CTexturePool	*s_pInstance;

		CachePool<String, CCacheTexture, tagStringCompare>	m_cachePool;
		UINT						m_uiIndex;
		Array2<CTextMergeTexturePtr>			m_aryTextMergetTexture;
	};
}