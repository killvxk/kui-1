#include "RenderEngine.h"

OpenHMI::UINT g_uiSessionID = 0;
OpenHMI::tagRenderContext*	g_pRenderContext = NULL;

#ifdef _X86_
extern "C" { int _afxForceUSRDLL; }
#else
extern "C" { int __afxForceUSRDLL; }
#endif

extern "C" __declspec(dllexport) OpenHMI::IRenderEngine* getInstance()
{
	return OpenHMI::CImpRenderEngine::getInstance();
}