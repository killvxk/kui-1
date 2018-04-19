#include "RenderEngine.h"

namespace OpenHMI
{
	CPass::CPass(CCacheMaterial* p)
		: m_pProgram(NULL)
		, m_pParent(p)
		, m_eType(CMaterialPool::EPT_INTERNAL_UI_COMMON)
		, m_pRenderDelegate(NULL)
		, m_bEnableBlend(TRUE)
		, m_eBlendSrcFactor(CRenderState::EBF_ONE)
		, m_eBlendDstFactor(CRenderState::EBF_ONE_MINUS_SRC_ALPHA)
		, m_bEnableCullFace(TRUE)
		, m_eCullFace(CRenderState::ECF_CCW)

	{
		;
	}

	CPass::~CPass()
	{
		m_aryTexState.clear();
		if( m_pRenderDelegate )
		{
			delete m_pRenderDelegate;
			m_pRenderDelegate = NULL;
		}
	}

	void CPass::enableBlend(BOOLEAN bEnable)
	{
		m_bEnableBlend = bEnable;
	}

	void CPass::applyBlend(CRenderState::EBlendFactor eSrcFactor, CRenderState::EBlendFactor eDstFactor)
	{
		m_eBlendSrcFactor = eSrcFactor;
		m_eBlendDstFactor = eDstFactor;
	}
	
	void CPass::enableCullFace(BOOLEAN bEnable)
	{
		m_bEnableCullFace = bEnable;
	}

	void CPass::applyCullFace(CRenderState::ECullFace eCullFace)
	{
		m_eCullFace = eCullFace;
	}

	void CPass::addTextureState(CMaterialPool::tagTextureState& rTexState)
	{
		m_aryTexState.addItem(rTexState);
	}

	void CPass::setTextureStates(const Array2<CMaterialPool::tagTextureState>& ary)
	{
		m_aryTexState.clear();
		for( UINT i = 0; i< ary.size(); ++i )
		{
			const CMaterialPool::tagTextureState& r = ary[i];
			m_aryTexState.addItem(r);
		}
	}

	void CPass::begin()
	{
		;
	}

	void CPass::end()
	{
		if (m_pProgram)
		{
			m_pProgram->end(this);
		}
	}

	void  CPass::apply(Object* p, CRenderState* pRndState)
	{
		if ( m_eType == CMaterialPool::EPT_INTERNAL_UI_COMMON )
		{
			pRndState->enableDepthTest(FALSE);
			applyRenderStates(pRndState);
			applyProgram(p);
		}
		else if ( m_eType == CMaterialPool::EPT_INTERNAL_MODEL_CUSTOM && m_pRenderDelegate )
		{
			//CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(m_iProgramID);
			if ( m_pProgram )
			{
				C3DBatchRender* pRender = (C3DBatchRender*)p;
				if ( pRender == NULL )
				{
					return ;
				}

				C3DBatchRenderGroup * pGroup = pRender->getParent();
				CCamera* pCamera = pGroup->getCamera();

				m_pProgram->begin();
				m_pRenderDelegate->render(m_pProgram->getUniformLoc(), pCamera, pRender->getModelMatrix(), p, pRndState);
			}
		}
	}

	void CPass::applyRenderStates(CRenderState* pRndState)
	{
		if ( pRndState )
		{
			pRndState->enableBlend(m_bEnableBlend);
			if ( m_bEnableBlend )
			{
				pRndState->blendFunc(m_eBlendSrcFactor, m_eBlendDstFactor);
			}
			
			pRndState->enableCullFace(m_bEnableCullFace);
			if ( m_bEnableCullFace )
			{
				pRndState->cullFace(m_eCullFace);
			}
		}
	}

	void CPass::applyProgram(Object* p)
	{
		//CCacheProgram* pProgram = CProgramPool::getInstance()->getProgram(m_iProgramID);
		if (m_pProgram)
		{
			m_pProgram->begin();
			m_pProgram->setUniforms(p);
			m_pProgram->setTextureUniforms(this);
		}
	}

	void CPass::applyTextureParameters(UINT uidx, CCacheTexture* pTex)
	{
		if ( uidx < m_aryTexState.size() && pTex )
		{
			const CMaterialPool::tagTextureState& rState = m_aryTexState[uidx];
			pTex->setFilter(CCacheTexture::FP_MIN_FILTER, (CCacheTexture::EFilterMode)rState.m_uiFilterMode);
			pTex->setFilter(CCacheTexture::FP_MAG_FILTER, (CCacheTexture::EFilterMode)rState.m_uiFilterMode);
			pTex->setWrap(CCacheTexture::WRAP_S, (CCacheTexture::EWrapMode)rState.m_uiWrapMode);
			pTex->setWrap(CCacheTexture::WRAP_T, (CCacheTexture::EWrapMode)rState.m_uiWrapMode);
		}
	}

	CCacheTexture* CPass::getTexture(UINT idx)
	{
		if ( idx < m_aryTexState.size() )
		{
			UINT uiTexIdx = m_aryTexState[idx].m_uiTexIdx;
			return m_pParent->getCustomTexture(uiTexIdx);
		}
		return NULL;
	}
}