#include "RenderEngine.h"

namespace OpenHMI
{

	CRenderState::CRenderState()
		: m_bEnableBlend(FALSE)
		, m_eBlendSrcFactor(EBF_ONE)
		, m_eBlendDstFactor(EBF_ONE)
		, m_eBlendOp(EBOP_ADD)
		, m_bEnableCullFace(FALSE)
		, m_eCullFace(ECF_CCW)
		, m_bEnableDepthTest(FALSE)
	{
		;
	}

	CRenderState::~CRenderState()
	{
		;
	}

	void CRenderState::enableBlend(BOOLEAN bEnable)
	{
		if ( bEnable != m_bEnableBlend )
		{
			m_bEnableBlend = bEnable;

			if ( bEnable )
			{
				glEnable(GL_BLEND);
			}
			else
			{
				glDisable(GL_BLEND);
			}
		}
	}

	void CRenderState::blendFunc(EBlendFactor eSrcFactor, EBlendFactor eDstFactor)
	{
		BOOLEAN bNeedUpdate = FALSE;
		if ( eSrcFactor != m_eBlendSrcFactor )
		{
			m_eBlendSrcFactor = eSrcFactor;
			bNeedUpdate = TRUE;
		}

		if ( eDstFactor != m_eBlendDstFactor )
		{
			m_eBlendDstFactor = eDstFactor;
			bNeedUpdate = TRUE;
		}

		if ( bNeedUpdate )
		{
			glBlendFunc(m_eBlendSrcFactor, m_eBlendDstFactor);
		}
	}

	void CRenderState::blendOp(EBlendOperator eOp)
	{
		if ( eOp != m_eBlendOp )
		{
			m_eBlendOp = eOp;

		}
	}

	void CRenderState::enableCullFace(BOOLEAN bEnable)
	{
		if ( bEnable != m_bEnableCullFace )
		{
			m_bEnableCullFace = bEnable;

			if ( bEnable )
			{
				glEnable(GL_CULL_FACE);
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}
		}
	}

	void CRenderState::cullFace(ECullFace eCullFace)
	{
		if ( eCullFace != m_eCullFace )
		{
			m_eCullFace = eCullFace;

			glFrontFace(eCullFace);
		}
	}

	void CRenderState::enableDepthTest(BOOLEAN bEnable)
	{
		if ( bEnable != m_bEnableDepthTest )
		{
			m_bEnableDepthTest = bEnable;

			if ( bEnable )
			{
				glEnable(GL_DEPTH_TEST);
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}
		}
	}

}