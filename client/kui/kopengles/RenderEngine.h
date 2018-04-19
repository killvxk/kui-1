#pragma once

#include "OpenHMI.h"

#include "./ThirdInclude.h"

#define RENDER_ENGINE_NAME	L"RenderEngine_OpenGLES2"

// TODO: Declare class here
namespace OpenHMI
{
	// Utility
	class CDIB;
	class CCamera;


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

	// DataPool.Texture
	class CCacheTexture;
	class CTextMergeTexture;
	class CTexturePool;

	// DataPool.Material
	class CRenderDelegate;
	class CPass;
	class CCacheMaterial;
	class CInternalMaterialLibrary;
	class CMaterialPool;

	// DataPool.Shader
	class CCacheShader;
	class CInternalShaderLibrary;
	class CShaderPool;	

	// DataPool.Program
	class CCacheProgram;
	class CProgramPool;

	// DataPool.Mesh
	class CCacheMesh;
	class CInternalMesh;
	class CDynamicUIMesh;
	class CMeshPool;	

	// RenderPipeline
	// RenderTarget
	class CRenderTarget;
	class CRenderTargetManager;
	// CacheData
	class CMaterialRenderCacheData;
	class CRenderImageCacheData;
	class CRenderTextCacheData;
	class CRenderGroupCacheData;
	class CRenderModelCacheData;
	class CRenderViewportCacheData;
	// Text
	class CTextLayout;
	// Filter
	class CFilterCenter;
	class CBaseFilter;
	class CFilterReflection;
	class CFilterGlass;
	class CFilterMask;

	class C2DRenderUtility;
	class CWaveRenderDelegate;
	class CRenderState;
	class C2DBatchRender;
	class CBatchRenderGroup;
	class C2DBatchRenderGroup;
	class C3DBatchRender;
	class C3DBatchRenderGroup;
	class CRenderEntry;

	// OpenHMIConnector
	class CImpRenderSession;
	class CImpRenderEngine;
}

// TODO: Include Utility
#include "./Utility/CBilinearFunc.h"
#include "./Utility/CDIB.h"
#include "./Utility/CPool.h"
#include "./Utility/CCamera.h"

#include "./RenderPipeline/CRenderState.h"

// TODO: Include Platform
#include "./Platform/CRenderEngineOS.h"

// 
#include "./DataPool/CacheGPUObject.h"

// TODO: Include DataPool.Image
#include "./DataPool/Image/CImageParser.h"
#include "./DataPool/Image/CCacheImage.h"
#include "./DataPool/CImagePool.h"

// TODO: Include DataPool.Font
#include "./DataPool/Font/CCacheGlyph.h"
#include "./DataPool/Font/CFontFace.h"
#include "./DataPool/CFontPool.h"

// TODO: Include DataPool.Texture
#include "./DataPool/Texture/CCacheTexture.h"
#include "./DataPool/Texture/CTextMergeTexture.h"
#include "./DataPool/CTexturePool.h"

// TODO: Include DataPool.Shader
#include "./DataPool/Shader/CCacheShader.h"
#include "./DataPool/Shader/CCacheVertexShader.h"
#include "./DataPool/Shader/CCacheFragmentShader.h"
#include "./DataPool/CShaderPool.h"
#include "./DataPool/Shader/CInternalShaderLibrary.h"

// TODO: Include DataPool.Program
#include "./DataPool/Program/CUniform.h"
#include "./DataPool/Program/CCacheProgram.h"
#include "./DataPool/CProgramPool.h"

// TODO: Include DataPool.Material
#include "./DataPool/Material/CRenderDelegate.h"
#include "./DataPool/Material/CCacheMaterial.h"
#include "./DataPool/CMaterialPool.h"
#include "./DataPool/Material/CPass.h"
#include "./DataPool/Material/CInternalMaterialLibrary.h"

// TODO: Include DataPool.Mesh
#include "./DataPool/Mesh/CMeshFactory.h"
#include "./DataPool/Mesh/CCacheMesh.h"
#include "./DataPool/Mesh/CDynamicUIMesh.h"
#include "./DataPool/CMeshPool.h"

// TODO: Include RenderPipeline
// RenderTarget
#include "./RenderPipeline/RenderTarget/CRenderTarget.h"
#include "./RenderPipeline/RenderTarget/CRenderTargetManager.h"
// CacheData
#include "./RenderPipeline/CacheData/CMaterialRenderCacheData.h"
#include "./RenderPipeline/CacheData/CRenderTextCacheData.h"
#include "./RenderPipeline/CacheData/CRenderImageCacheData.h"
#include "./RenderPipeline/CacheData/CRenderGroupCacheData.h"
#include "./RenderPipeline/CacheData/CRenderModelCacheData.h"
#include "./RenderPipeline/CacheData/CRenderViewportCacheData.h"
// Text
#include "./RenderPipeline/Text/CTextLayout.h"
// Filter
#include "./RenderPipeline/Filter/CFilterCenter.h"
#include "./RenderPipeline/Filter/CBaseFilter.h"
#include "./RenderPipeline/Filter/CFilterReflection.h"
#include "./RenderPipeline/Filter/CFilterGlass.h"
#include "./RenderPipeline/Filter/CFilterRipple.h"
#include "./RenderPipeline/Filter/CFilterMask.h"

#include "./RenderPipeline/C2DRenderUtility.h"
#include "./RenderPipeline/C2DBatchRender.h"
#include "./RenderPipeline/CBatchRenderGroup.h"
#include "./RenderPipeline/C2DBatchRenderGroup.h"
#include "./RenderPipeline/C3DBatchRender.h"
#include "./RenderPipeline/C3DBatchRenderGroup.h"
#include "./RenderPipeline/CRenderEntry.h"
#include "./RenderPipeline/CWaveRenderDelegate.h"

// TODO: Include OpenHMIConnector
#include "./OpenHMIConnector/CImpRenderSession.h"
#include "./OpenHMIConnector/CImpRenderEngine.h"

extern OpenHMI::UINT g_uiSessionID;
extern OpenHMI::tagRenderContext*	g_pRenderContext;


#ifdef OS_WIN
	extern "C" __declspec(dllexport) OpenHMI::IRenderEngine* getInstance();
#endif

#ifdef OS_QNX
	extern "C" OpenHMI::IRenderEngine* getInstance();
#endif

#ifdef OS_LINUX
	extern "C" OpenHMI::IRenderEngine* getInstance();
#endif