#include "RenderEngine.h"
#include <vector>

namespace OpenHMI
{
	CRenderEngineOS	*CRenderEngineOS::s_pInstance	= NULL;

	CRenderEngineOS* CRenderEngineOS::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CRenderEngineOS();
		}

		return s_pInstance;
	}

	void CRenderEngineOS::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	tagRenderContext::tagRenderContext() 
	{
		reset();
	}

	void tagRenderContext::reset()
	{
		eglDisplay = NULL;
		eglConfig = NULL;
		eglSurface = NULL;
		eglContext = NULL;
	}

	CRenderEngineOS::CRenderEngineOS()
		: m_bInitialized(FALSE)
	{

	}

	CRenderEngineOS::~CRenderEngineOS()
	{
		;
	}

	BOOLEAN CRenderEngineOS::initialize(const tagScreenContext *pScreenContext)
	{
		if (m_bInitialized)
		{
			return m_bInitialized;
		}

		// add initialize code here
		if (pScreenContext != NULL)
		{
			m_bInitialized = TRUE; 
			m_screenContext = *pScreenContext;
			m_bInitialized = initializeSession(pScreenContext, &m_renderContext);
			return m_bInitialized;
		}

		m_bInitialized = TRUE;
		return m_bInitialized;
	}

	void CRenderEngineOS::unInitialize(const tagScreenContext *pScreenContext)
	{
		if (!m_bInitialized)
		{
			return;
		}

		// add un-initialize code here
		if (pScreenContext != NULL)
		{			
			unInitializeSession(pScreenContext, &m_renderContext);
			m_screenContext.reset();
		}

		eglReleaseThread();
		m_bInitialized = FALSE;
	}

	struct EGLPlatformParameters
	{
		EGLint renderer;
		EGLint majorVersion;
		EGLint minorVersion;
		EGLint deviceType;
		EGLint presentPath;

		EGLPlatformParameters();
		explicit EGLPlatformParameters(EGLint renderer);
		EGLPlatformParameters(EGLint renderer, EGLint majorVersion, EGLint minorVersion, EGLint deviceType);
		EGLPlatformParameters(EGLint renderer,
			EGLint majorVersion,
			EGLint minorVersion,
			EGLint deviceType,
			EGLint presentPath);
	};

	EGLPlatformParameters::EGLPlatformParameters()
		: renderer(EGL_PLATFORM_ANGLE_TYPE_DEFAULT_ANGLE),
		majorVersion(EGL_DONT_CARE),
		minorVersion(EGL_DONT_CARE),
		deviceType(EGL_DONT_CARE),
		presentPath(EGL_DONT_CARE)
	{
	}

	EGLPlatformParameters::EGLPlatformParameters(EGLint renderer)
		: renderer(renderer),
		majorVersion(EGL_DONT_CARE),
		minorVersion(EGL_DONT_CARE),
		deviceType(EGL_DONT_CARE),
		presentPath(EGL_DONT_CARE)
	{
		if (renderer == EGL_PLATFORM_ANGLE_TYPE_D3D9_ANGLE ||
			renderer == EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE)
		{
			deviceType = EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE;
		}
	}

	EGLPlatformParameters::EGLPlatformParameters(EGLint renderer,
		EGLint majorVersion,
		EGLint minorVersion,
		EGLint useWarp)
		: renderer(renderer),
		majorVersion(majorVersion),
		minorVersion(minorVersion),
		deviceType(useWarp),
		presentPath(EGL_DONT_CARE)
	{
	}

	EGLPlatformParameters::EGLPlatformParameters(EGLint renderer,
		EGLint majorVersion,
		EGLint minorVersion,
		EGLint useWarp,
		EGLint presentPath)
		: renderer(renderer),
		majorVersion(majorVersion),
		minorVersion(minorVersion),
		deviceType(useWarp),
		presentPath(presentPath)
	{
	}

	BOOLEAN CRenderEngineOS::initializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
	{
		if (!m_bInitialized)
		{
			return FALSE;
		}

		if (pScreenContext->uiScreenWidth == 0 || pScreenContext->uiScreenHeight == 0)
		{
			return FALSE;
		}

		// add initialize code here
		if (pScreenContext->hWnd == NULL || pScreenContext->hDC == NULL)
		{
			return FALSE;
		}

		PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
		if (!eglGetPlatformDisplayEXT)
		{
			return FALSE;
		}

		std::vector<EGLint> displayAttributes;
		EGLPlatformParameters mPlatform(EGL_PLATFORM_ANGLE_TYPE_DEFAULT_ANGLE);

		displayAttributes.push_back(EGL_PLATFORM_ANGLE_TYPE_ANGLE);
		displayAttributes.push_back(mPlatform.renderer);
		displayAttributes.push_back(EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE);
		displayAttributes.push_back(mPlatform.majorVersion);
		displayAttributes.push_back(EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE);
		displayAttributes.push_back(mPlatform.minorVersion);

		if (mPlatform.deviceType != EGL_DONT_CARE)
		{
			displayAttributes.push_back(EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE);
			displayAttributes.push_back(mPlatform.deviceType);
		}

		if (mPlatform.presentPath != EGL_DONT_CARE)
		{
			const char *extensionString =
				static_cast<const char *>(eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS));
			if (strstr(extensionString, "EGL_ANGLE_experimental_present_path") == nullptr)
			{
				return FALSE;
			}

			displayAttributes.push_back(EGL_EXPERIMENTAL_PRESENT_PATH_ANGLE);
			displayAttributes.push_back(mPlatform.presentPath);
		}
		displayAttributes.push_back(EGL_NONE);


		pRenderContext->eglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE,
			reinterpret_cast<void *>(pScreenContext->hDC),
			&displayAttributes[0]);
		if (pRenderContext->eglDisplay == EGL_NO_DISPLAY)
		{
			return FALSE;
		}

		
		EGLint majorVersion, minorVersion;
		if (eglInitialize(pRenderContext->eglDisplay, &majorVersion, &minorVersion) == EGL_FALSE)
		{
			return FALSE;
		}
		const char *displayExtensions = eglQueryString(pRenderContext->eglDisplay, EGL_EXTENSIONS);

		// EGL_KHR_create_context is required to request a non-ES2 context.
		bool hasKHRCreateContext = strstr(displayExtensions, "EGL_KHR_create_context") != nullptr;
		if (majorVersion != 2 && minorVersion != 0 && !hasKHRCreateContext)
		{			
			return FALSE;
		}

		eglBindAPI(EGL_OPENGL_ES_API);
		if (eglGetError() != EGL_SUCCESS)
		{
			return FALSE;
		}
		int mRedBits = 0;
		int mGreenBits = 0;
		int mBlueBits = 0;
		int mAlphaBits = 0;
		int mDepthBits = 0;
		int mStencilBits = 0;
		bool mMultisample = 0;

		const EGLint configAttributes[] =
		{
			EGL_RED_SIZE, (mRedBits >= 0) ? mRedBits : EGL_DONT_CARE,
			EGL_GREEN_SIZE, (mGreenBits >= 0) ? mGreenBits : EGL_DONT_CARE,
			EGL_BLUE_SIZE, (mBlueBits >= 0) ? mBlueBits : EGL_DONT_CARE,
			EGL_ALPHA_SIZE, (mAlphaBits >= 0) ? mAlphaBits : EGL_DONT_CARE,
			EGL_DEPTH_SIZE, (mDepthBits >= 0) ? mDepthBits : EGL_DONT_CARE,
			EGL_STENCIL_SIZE, (mStencilBits >= 0) ? mStencilBits : EGL_DONT_CARE,
			EGL_SAMPLE_BUFFERS, mMultisample ? 1 : 0,
			EGL_NONE
		};

		EGLint configCount;
		EGLConfig mConfig;

		if (!eglChooseConfig(pRenderContext->eglDisplay, configAttributes, &mConfig, 1, &configCount) || (configCount != 1))
		{
			return FALSE;
		}

		eglGetConfigAttrib(pRenderContext->eglDisplay, mConfig, EGL_RED_SIZE, &mRedBits);
		eglGetConfigAttrib(pRenderContext->eglDisplay, mConfig, EGL_GREEN_SIZE, &mGreenBits);
		eglGetConfigAttrib(pRenderContext->eglDisplay, mConfig, EGL_BLUE_SIZE, &mBlueBits);
		eglGetConfigAttrib(pRenderContext->eglDisplay, mConfig, EGL_ALPHA_SIZE, &mAlphaBits);
		eglGetConfigAttrib(pRenderContext->eglDisplay, mConfig, EGL_DEPTH_SIZE, &mDepthBits);
		eglGetConfigAttrib(pRenderContext->eglDisplay, mConfig, EGL_STENCIL_SIZE, &mStencilBits);

		std::vector<EGLint> surfaceAttributes;
		if (strstr(displayExtensions, "EGL_NV_post_sub_buffer") != nullptr)
		{
			surfaceAttributes.push_back(EGL_POST_SUB_BUFFER_SUPPORTED_NV);
			surfaceAttributes.push_back(EGL_TRUE);
		}

		surfaceAttributes.push_back(EGL_NONE);

		pRenderContext->eglSurface = eglCreateWindowSurface(pRenderContext->eglDisplay, mConfig, pScreenContext->hWnd, &surfaceAttributes[0]);
		if (eglGetError() != EGL_SUCCESS)
		{			
			return FALSE;
		}

		std::vector<EGLint> contextAttributes;
		if (hasKHRCreateContext)
		{
			contextAttributes.push_back(EGL_CONTEXT_MAJOR_VERSION_KHR);
			contextAttributes.push_back(2);

			contextAttributes.push_back(EGL_CONTEXT_MINOR_VERSION_KHR);
			contextAttributes.push_back(0);
		}
		contextAttributes.push_back(EGL_NONE);

		pRenderContext->eglContext = eglCreateContext(pRenderContext->eglDisplay, mConfig, NULL, &contextAttributes[0]);
		if (eglGetError() != EGL_SUCCESS)
		{
			return FALSE;
		}

		eglMakeCurrent(pRenderContext->eglDisplay, pRenderContext->eglSurface, pRenderContext->eglSurface, pRenderContext->eglContext);
		if (eglGetError() != EGL_SUCCESS)
		{
			return FALSE;
		}
		eglSwapInterval(pRenderContext->eglDisplay, 0);

#if 0
		if (mSwapInterval != -1)
		{
			eglSwapInterval(mDisplay, mSwapInterval);
		}

		if (!eglChooseConfig(pRenderContext->eglDisplay, configurationAttributes, &pRenderContext->eglConfig, 1, &iNumConfigs))
		{
			CLog::error2(RENDER_ENGINE_NAME, L"EGL choose config error.");
			//MessageBox(hWnd, _T("eglChooseConfig() failed."), _T("error"), MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}

		const char *displayExtensions = eglQueryString(pRenderContext->eglDisplay, EGL_EXTENSIONS);

		if (strstr(displayExtensions, "EGL_NV_post_sub_buffer") != nullptr)
		{
			EGLint surfaceAttribList[] =
			{
				EGL_POST_SUB_BUFFER_SUPPORTED_NV, EGL_TRUE,
				EGL_NONE, EGL_NONE
			};

			pRenderContext->eglSurface = eglCreateWindowSurface(pRenderContext->eglDisplay, pRenderContext->eglConfig, (EGLNativeWindowType)pScreenContext->hWnd, surfaceAttribList);
		}
		else
			pRenderContext->eglSurface = eglCreateWindowSurface(pRenderContext->eglDisplay, pRenderContext->eglConfig, (EGLNativeWindowType)pScreenContext->hWnd, NULL);
		if (pRenderContext->eglSurface == EGL_NO_SURFACE)
		{
			CLog::error2(RENDER_ENGINE_NAME, L"EGL create window surface error.");
			//MessageBox(hWnd, _T("EGL create window surface error."), _T("error"), MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}

		EGLint aiContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
		pRenderContext->eglContext = eglCreateContext(pRenderContext->eglDisplay, pRenderContext->eglConfig, EGL_NO_CONTEXT, aiContextAttribs);
		if (pRenderContext->eglContext == EGL_NO_CONTEXT)
		{
			CLog::error2(RENDER_ENGINE_NAME, L"EGL create context error.");
			//MessageBox(hWnd, _T("EGL create context error."), _T("error"), MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}


		if (!eglMakeCurrent(pRenderContext->eglDisplay, pRenderContext->eglSurface, pRenderContext->eglSurface, pRenderContext->eglContext))
 		{
 			CLog::error2(RENDER_ENGINE_NAME, L"EGL make current error.");
			//MessageBox(hWnd, _T("EGL make current error."), _T("error"), MB_OK | MB_ICONEXCLAMATION);
 			return FALSE;
		}
#endif
		return TRUE;
	}

	void CRenderEngineOS::unInitializeSession(const tagScreenContext *pScreenContext, tagRenderContext *pRenderContext)
	{
		if (!m_bInitialized)
		{
			return;
		}

		// add un-initialize code here	
		if (pRenderContext->eglDisplay != NULL)
		{
			eglSwapBuffers(pRenderContext->eglDisplay, (EGLNativeWindowType)pScreenContext->hWnd);

			eglMakeCurrent(pRenderContext->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);	

			if (pRenderContext->eglSurface != NULL)
			{
				eglDestroySurface(pRenderContext->eglDisplay, pRenderContext->eglSurface);
			}
		
			if (pRenderContext->eglContext != NULL)
			{
				eglDestroyContext(pRenderContext->eglDisplay, pRenderContext->eglContext);
			}

			eglTerminate(m_renderContext.eglDisplay);
		}
		pRenderContext->reset();
	}
}