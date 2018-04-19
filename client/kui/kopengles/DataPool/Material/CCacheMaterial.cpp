#include "RenderEngine.h"

namespace OpenHMI
{
	CCacheMaterial::CCacheMaterial()
		: m_aryTexture(8)
		, m_aryPass(1)
		, m_pCurrentPass(NULL)
	{
		CLog::debug2(RENDER_ENGINE_NAME, L"new Material");
	}

	CCacheMaterial::~CCacheMaterial()
	{
		CLog::debug2(String(RENDER_ENGINE_NAME),String(L"delete Material") + getName());
		reset();
	}

	void CCacheMaterial::addCustomTexture(CCacheTexture* pTex)
	{
		m_aryTexture.addItem(CCacheTexturePtr(pTex));
	}

	void CCacheMaterial::setCustomTexture(UINT uIdx, CCacheTexture* pTex)
	{
		;
	}

	CCacheTexture* CCacheMaterial::getCustomTexture(UINT uIdx)
	{
		return m_aryTexture[uIdx];	
	}

	void CCacheMaterial::resetTextures()
	{
		;
	}

	void CCacheMaterial::begin()
	{
		CPass* pPass = getCurrentPass();

		if ( pPass )
		{
			pPass->begin();
		}
	}

	void CCacheMaterial::apply(Object* p, CRenderState* pRndState)
	{
		CPass* pPass = getCurrentPass();

		if ( pPass )
		{
			pPass->apply(p, pRndState);
		}
	}

	void CCacheMaterial::end()
	{
		CPass* pPass = getCurrentPass();

		if ( pPass )
		{
			pPass->end();
		}
	}

	void CCacheMaterial::addPass(CPass* pPass)
	{
		m_aryPass.addItem(pPass);
	}

	void CCacheMaterial::activePass(UINT idx)
	{
		if ( idx < m_aryPass.size() )
		{
			m_pCurrentPass = m_aryPass[idx];
		}
		else
		{
			CLog::error2(RENDER_ENGINE_NAME,L"activePass error");
		}
	}

	UINT CCacheMaterial::merge(CCacheMaterial* pMtl)
	{
		CCacheTexture* pTex = pMtl->getCustomTexture(0);
		if( pTex == NULL )
		{
			return 0;
		}

		for(UINT i = 0; i < m_aryTexture.size(); ++i )
		{
			CCacheTexture* pCurTex = m_aryTexture[i];
			if( pTex == pCurTex )
			{
				return i;
			}
		}
		m_aryTexture.addItem(CCacheTexturePtr(pTex));

		return (m_aryTexture.size() - 1);
	}

	void CCacheMaterial::reset()
	{
		m_aryPass.clear();
		m_aryTexture.clear();
	}
}