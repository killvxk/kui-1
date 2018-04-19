#include "RenderEngine.h"

namespace OpenHMI
{

	void g_contructMaterial(CMaterialPool::EMaterialType eType, CCacheMaterial* p)
	{
		switch(eType)
		{
		case CMaterialPool::EMT_UI_DEFAULT:
			{
				CPass* pPass = new CPass(p);
				p->addPass(pPass);
				
				CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(CShaderPool::EVS_UI_DEFAULT,CShaderPool::EFS_UI_DEFAULT);
				pPass->setProgram(pProgram);
				CMaterialPool::tagTextureState texState;
				pPass->addTextureState(texState);
			}
			break;
		case CMaterialPool::EMT_UI_ALPHA:
			{
				CPass* pPass = new CPass(p);
				p->addPass(pPass);
				CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(CShaderPool::EVS_UI_ALPHA,CShaderPool::EFS_UI_ALPHA);
				pPass->setProgram(pProgram);
				CMaterialPool::tagTextureState texState;
				pPass->addTextureState(texState);
			}
			break;
		case CMaterialPool::EMT_WAVE:
			{
				CPass* pPass = new CPass(p);
				p->addPass(pPass);
				CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(CShaderPool::EVS_WAVE,CShaderPool::EFS_WAVE);
				pPass->setProgram(pProgram);
				pPass->setType(CMaterialPool::EPT_INTERNAL_MODEL_CUSTOM);
				CMaterialPool::tagTextureState texState;
				pPass->addTextureState(texState);
			}
			break;
		
		case CMaterialPool::EMT_UI_VIEWPORT:
			{
				CPass* pPass = new CPass(p);
				p->addPass(pPass);
				CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(CShaderPool::EVS_UI_VIEWPORT,CShaderPool::EFS_UI_VIEWPORT);
				pPass->setProgram(pProgram);
				pPass->applyBlend(CRenderState::EBF_ONE,CRenderState::EBF_ZERO);
			}
			break;
		case CMaterialPool::EMT_25DUI_ALPHA:
			{
				CPass* pPass = new CPass(p);
				p->addPass(pPass);
				CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(CShaderPool::EVS_25DUI_ALPHA,CShaderPool::EFS_25DUI_ALPHA);
				pPass->setProgram(pProgram);
				CMaterialPool::tagTextureState texState;
				pPass->addTextureState(texState);
			}
			break;
		case CMaterialPool::EMT_OPENARTIST_LINESOLID:
			{
				CPass* pPass = new CPass(p);
				p->addPass(pPass);
				CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(CShaderPool::EVS_OPENARTIST_LINESOLID,CShaderPool::EFS_OPENARTIST_LINESOLID);
				pPass->setProgram(pProgram);
			}

			break;
		case CMaterialPool::EMT_GAUSSIAN_BLUR_H:
			{
				CPass* pPass = new CPass(p);
				p->addPass(pPass);
				CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(CShaderPool::EVS_GAUSSIAN_BLUR_H,CShaderPool::EFS_GAUSSIAN_BLUR_H);
				pPass->setProgram(pProgram);
				CMaterialPool::tagTextureState texState;
				pPass->addTextureState(texState);
			}
			break;
		case CMaterialPool::EMT_GAUSSIAN_BLUR_V:
			{
				CPass* pPass = new CPass(p);
				pPass->enableBlend(TRUE);
				pPass->applyBlend(CRenderState::EBF_SRC_ALPHA,CRenderState::EBF_ONE_MINUS_SRC_ALPHA);
				p->addPass(pPass);
				CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(CShaderPool::EVS_GAUSSIAN_BLUR_V,CShaderPool::EFS_GAUSSIAN_BLUR_V);
				pPass->setProgram(pProgram);
				CMaterialPool::tagTextureState texState;
				pPass->addTextureState(texState);
			}
			break;
		case CMaterialPool::EMT_UI_DEFAULT_MERGE:
			{
				CPass* pPass = new CPass(p);
				p->addPass(pPass);
				CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(CShaderPool::EVS_UI_DEFAULT_MERGE,CShaderPool::EFS_UI_DEFAULT_MERGE);
				pPass->setProgram(pProgram);
				CMaterialPool::tagTextureState texState;
				pPass->addTextureState(texState);
				texState.m_uiTexIdx = 1;
				pPass->addTextureState(texState);
				texState.m_uiTexIdx = 2;
				pPass->addTextureState(texState);
				texState.m_uiTexIdx = 3;
				pPass->addTextureState(texState);
				texState.m_uiTexIdx = 4;
				pPass->addTextureState(texState);
				texState.m_uiTexIdx = 5;
				pPass->addTextureState(texState);
				texState.m_uiTexIdx = 6;
				pPass->addTextureState(texState);
				texState.m_uiTexIdx = 7;
				pPass->addTextureState(texState);
			}
			break;
		case CMaterialPool::EMT_UI_MASK:
			{
				CPass* pPass = new CPass(p);
				p->addPass(pPass);
				pPass->enableBlend(TRUE);
				pPass->applyBlend(CRenderState::EBF_ZERO,CRenderState::EBF_SRC_ALPHA);
				CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(CShaderPool::EVS_UI_MASK,CShaderPool::EFS_UI_MASK);
				pPass->setProgram(pProgram);
				CMaterialPool::tagTextureState texState;
				pPass->addTextureState(texState);
			}
			break;
		case CMaterialPool::EMT_UI_MASK2:
			{
				CPass* pPass = new CPass(p);
				p->addPass(pPass);
				pPass->enableBlend(TRUE);
				pPass->applyBlend(CRenderState::EBF_SRC_ALPHA,CRenderState::EBF_ONE_MINUS_SRC_ALPHA);
				CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(CShaderPool::EVS_UI_MASK2,CShaderPool::EFS_UI_MASK2);
				pPass->setProgram(pProgram);
				CMaterialPool::tagTextureState texState;
				pPass->addTextureState(texState);
				texState.m_uiTexIdx = 1;
				pPass->addTextureState(texState);
			}
			break;
		}

		p->activePass(0);
	}
}