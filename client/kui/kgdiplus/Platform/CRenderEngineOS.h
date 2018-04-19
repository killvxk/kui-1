#pragma once
using namespace Gdiplus; 

namespace OpenHMI
{
	struct tagRenderContext
	{
#ifdef OS_WIN

#endif

#ifdef OS_QNX

	#ifdef CHIPSET_JACINTO4
		screen_buffer_t		pBuf;
		BYTE*				pbtBufData;
	#endif

	#ifdef CHIPSET_JACINTO5
		screen_buffer_t		pBuf;
		BYTE*				pbtBufData;
	#endif

	#ifdef CHIPSET_IMX6SOLO
		screen_buffer_t		pBuf;
		BYTE*				pbtBufData;
	#endif

#endif

#ifdef OS_LINUX

	#ifdef CHIPSET_JACINTO4		
		INT			iFbfd;
		INT			iScreenSize;
		BYTE*		pbtFB;
	#endif

	#ifdef CHIPSET_IMX6SOLO
		INT			iFbfd;
		INT			iScreenSize;
		BYTE*		pbtFB;
	#endif

	#ifdef CHIPSET_NBTEVO
		XImage*		pImg;
		BYTE*		pbtBufData;
	#endif

	#ifdef CHIPSET_PCIBM
		XImage*		pImg;
		BYTE*		pbtBufData;
	#endif

#endif
		tagRenderContext();
		void	reset();
	};

	class CRenderEngineOS
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		static	CRenderEngineOS*	getInstance();
		static	void				deleteInstance();

		BOOLEAN	initialize(const tagScreenContext *pScreenContext);
		void	unInitialize(const tagScreenContext *pScreenContext);

		BOOLEAN	initializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext);
		void	unInitializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext);

		tagScreenContext* getScreenContext()	{ return &m_screenContext; }
		tagRenderContext* getRenderContext()	{ return &m_renderContext; }

	protected:// method		
		CRenderEngineOS();
		virtual ~CRenderEngineOS();

	private:// method

		ULONG_PTR           m_gdiplusToken;
	protected:// property

	private:// property
		static CRenderEngineOS	   *s_pInstance;

		BOOLEAN				m_bInitialized;

		tagScreenContext	m_screenContext;
		tagRenderContext	m_renderContext;
	};
}