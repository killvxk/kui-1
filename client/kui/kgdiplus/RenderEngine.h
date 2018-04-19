#pragma once

#include "OpenHMI.h"

#include "./ThirdInclude.h"

#define RENDER_ENGINE_NAME	L"RenderEngine_Software"

// TODO: Declare class here
namespace OpenHMI
{
	// Utility
	class CDIB;

	// Platform
	class CRenderEngineOS;

	// DataPool.Image
	class CImageParser;
	class CImgPNG;	
	class CImgBMP;
	class CCacheImage;
	class CImagePool;

	// DataPool.Font
	class CCacheGlyph;
	class CFontFace;
	class CFontPool;

	// RenderPipeline.Filter
	class CFltReflection;
	class CFltMask;
	class CFilterCenter;

	// RenderPipeline
	class CCacheData;
	class CTextLayout;
	class CRenderEntry;

	// OpenHMIConnector
	class CImpRenderSession;
	class CImpRenderEngine;
}

// TODO: Include Utility
#include "./Utility/CBilinearFunc.h"
#include "./Utility/CDIB.h"

// TODO: Include Platform
#include "./Platform/CRenderEngineOS.h"

// TODO: Include DataPool.Image
#include "./DataPool/Image/CImageParser.h"
#include "./DataPool/Image/CCacheImage.h"
#include "./DataPool/CImagePool.h"

// TODO: Include DataPool.Font
#include "./DataPool/Font/CCacheGlyph.h"
#include "./DataPool/Font/CFontFace.h"
#include "./DataPool/CFontPool.h"

// TODO: Include RenderPipeline.Filter
#include "./RenderPipeline/Filter/CFltReflection.h"
#include "./RenderPipeline/Filter/CFltMask.h"
#include "./RenderPipeline/Filter/CFilterCenter.h"

// TODO: Include RenderPipeline
#include "./RenderPipeline/CCacheData.h"
#include "./RenderPipeline/CTextLayout.h"
#include "./RenderPipeline/CRenderEntry.h"

// TODO: Include OpenHMIConnector
#include "./OpenHMIConnector/CImpRenderSession.h"
#include "./OpenHMIConnector/CImpRenderEngine.h"

#ifdef OS_WIN
	extern "C" __declspec(dllexport) OpenHMI::IRenderEngine* getInstance();
#endif

#ifdef OS_QNX
	extern "C" OpenHMI::IRenderEngine* getInstance();
#endif

#ifdef OS_LINUX
	extern "C" OpenHMI::IRenderEngine* getInstance();
#endif