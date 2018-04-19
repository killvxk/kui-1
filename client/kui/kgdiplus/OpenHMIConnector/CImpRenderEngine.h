#pragma once

namespace OpenHMI
{
	class CImpRenderEngine : public IRenderEngine
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		static	CImpRenderEngine*	getInstance();	

		//---start---implement IRenderEngine
		virtual	BOOLEAN				initialize(const tagScreenContext *pScreenContext);
		virtual	void				unInitialize(const tagScreenContext *pScreenContext);
		virtual IRenderSession*		getRenderSession(UINT uiID);
		virtual void				destroyRenderSession(UINT uiID);

		virtual	void				releaseCacheData();
		virtual	void				getImageInfo(const String &strFilePath, UINT *puiWidth, UINT *puiHeight);
		virtual void				getImageInfo(const String &strFilePath, Gdiplus::Bitmap* pBitmap, UINT *puiWidth, UINT *puiHeight);
		virtual	void				getTextInfo(CRenderText *pRndText, UINT *puiWidth, UINT *puiHeight, UINT *puiLineHeight);
		//---end---implement IRenderEngine

	protected:// method		
		CImpRenderEngine();	
		virtual ~CImpRenderEngine();

	private:// method

	protected:// property

	private:// property
		static	CImpRenderEngine	*s_pInstance;

		BOOLEAN						m_bInitialized;

		Map<UINT, IRenderSession, tagNumCompare<UINT> >		m_mapRenderSession;
	};
}