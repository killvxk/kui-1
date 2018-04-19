#include "RenderEngine.h"

namespace OpenHMI
{
	CRenderModelCacheData::CRenderModelCacheData()
		: m_pMesh(NULL)
		, m_pTex(NULL)
	{
		m_pRenderDelegate = new CWaveRenderDelegate();
		m_pRenderDelegate->initialize();
	}

	CRenderModelCacheData::~CRenderModelCacheData()
	{
		;
	}

	void CRenderModelCacheData::updateMatrix(const C3DVector& pos, const C3DVector& rot, const C3DVector& scale)
	{
		;
	}

	void CRenderModelCacheData::setTexture(CCacheTexture* pTex)
	{
		m_matrix.identity();
		m_pTex = pTex;

		flushMaterial();
	}

	CCacheMesh*	CRenderModelCacheData::getMesh()
	{
		return m_pMesh;
	}

	void CRenderModelCacheData::setMesh(CCacheMesh* pMesh)	
	{ 
		m_pMesh = pMesh;
	}
	
	void CRenderModelCacheData::flushMaterial()
	{
		m_pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_WAVE,m_pTex);
		
		CPass* pPass = m_pMtl->getCurrentPass();
		if ( pPass && m_pRenderDelegate )
		{
			m_pRenderDelegate->setTexture(m_pTex);
			pPass->setRenderDelegate(m_pRenderDelegate);
		}
	}
}