#include "RenderEngine.h"

namespace OpenHMI
{
	CacheGPUObject::CacheGPUObject()
	{
		this->setRenderContext(g_pRenderContext);
	}

	CacheGPUObject::~CacheGPUObject()
	{
		;
	}

	void CacheGPUObject::changeRenderContext()
	{
		tagRenderContext* pSelf = getRenderContext();
		if ( pSelf != g_pRenderContext && pSelf )
		{
			if (!eglMakeCurrent(pSelf->eglDisplay, pSelf->eglSurface, pSelf->eglSurface, pSelf->eglContext))
			{
				INT er = eglGetError();
				CLog::error2(RENDER_ENGINE_NAME, L"EGL make current error.");
				return;
			}
		}

	}

	void CacheGPUObject::resetRenderContext()
	{
		tagRenderContext* pSelf = getRenderContext();
		if ( pSelf != g_pRenderContext && g_pRenderContext )
		{
			if (!eglMakeCurrent(g_pRenderContext->eglDisplay, g_pRenderContext->eglSurface, g_pRenderContext->eglSurface, g_pRenderContext->eglContext))
			{
				INT er = eglGetError();
				CLog::error2(RENDER_ENGINE_NAME, L"EGL make current error.");
				return;
			}
		}
	}
} 