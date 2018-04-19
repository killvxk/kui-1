#include "RenderEngine.h"

#ifdef _X86_
extern "C" { int _afxForceUSRDLL; }
#else
extern "C" { int __afxForceUSRDLL; }
#endif

#ifdef OS_WIN
extern "C" __declspec(dllexport) OpenHMI::IRenderEngine* getInstance()
{
	return OpenHMI::CImpRenderEngine::getInstance();
}
#endif