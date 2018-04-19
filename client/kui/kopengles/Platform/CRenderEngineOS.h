#pragma once

namespace OpenHMI
{
	struct tagRenderContext
	{
		EGLDisplay	eglDisplay;
		EGLConfig	eglConfig;
		EGLSurface	eglSurface;
		EGLContext	eglContext;

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
		static	CRenderEngineOS* getInstance();
		static	void			 deleteInstance();

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

	protected:// property

	private:// property
		static CRenderEngineOS	   *s_pInstance;

		BOOLEAN		m_bInitialized;

		tagScreenContext	m_screenContext;
		tagRenderContext	m_renderContext;
	};
}