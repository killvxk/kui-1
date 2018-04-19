#include "RenderEngine.h"

namespace OpenHMI
{

	CFilterMask::CFilterMask()
		: m_pTex(NULL)
		, m_pRndFilter(NULL)
		, m_pBatchRenderGroup(NULL)
	{
		m_bIsNeedRT = FALSE;
	}

	CFilterMask::~CFilterMask()
	{
		m_pBatchRenderGroup = NULL;
	}

	BOOLEAN CFilterMask::fromRenderFilter(CRenderGroupCacheData* pRndGroupCacheData, const CRenderBaseFilter* pRndFilter)
	{
		m_pRndFilter = (CRenderFltMask*)(pRndFilter);
		m_pTex = CTexturePool::getInstance()->getExternalTexture(m_pRndFilter->getMaskImage());

		return m_pTex.isNotNull();
	}

	void CFilterMask::doFilter(C2DBatchRenderGroup* pIn, C2DBatchRenderGroup* pbg, Array2<C2DBatchRenderGroup*>& aryOut)
	{
		if ( m_pTex.isNull() || pIn == NULL )
		{
			return;
		}	

		C2DBatchRenderGroup* pCurBatchRenderGroup = pIn;

		// 
		CRectangleF roi(0.0f,0.0f,1.0f,1.0f);
		CRectangle rc( pIn->getX() , pIn->getY(), pIn->getWidth(),pIn->getHeight()  );

		CCacheMaterialPtr pMtl(CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_UI_MASK,m_pTex));
	
		C2DBatchRender* pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
		pCurBatchRenderGroup->add(pBatchRender);
		pBatchRender->addRenderItem(rc, roi, pMtl.getRaw());

		//C2DBatchRenderGroup* pCurBatchRenderGroup = pbg;
		//if ( isGenerateRT() )
		//{	
		//	CRenderEntry* pOwner = pbg->getOwner();
		//	if ( m_pBatchRenderGroup == NULL )
		//	{
		//		m_pBatchRenderGroup = pOwner->getFree2DBatchRenderGroup();
		//	}
		//	aryOut.addItem(m_pBatchRenderGroup);

		//	m_pBatchRenderGroup->set(pIn->getX(), pIn->getY(), pIn->getWidth() , pIn->getHeight() );
		//	m_pBatchRenderGroup->flush(TRUE);

		//	pCurBatchRenderGroup = m_pBatchRenderGroup;
		//}

		//// renderGroup and the reflection of it
		//CRectangleF roi(0.0f,1.0f,1.0f,-1.0f);
		//CRectangle rc( pIn->getX() , pIn->getY(), pIn->getWidth(),pIn->getHeight()  );

		//CRenderTarget* pRT = pIn->getRenderTarget();
		//CCacheTexture* pRTTex = pRT->getTexture();

		//Array2<CCacheTexture*> aryTex;
		//aryTex.addItem(pRTTex);
		//aryTex.addItem(m_pTex.getRaw());
		//CCacheMaterialPtr pMtl(CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_UI_MASK2,aryTex));
	
		//C2DBatchRender* pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
		//pCurBatchRenderGroup->add(pBatchRender);
		//pBatchRender->addRenderItem(rc, roi, pMtl.getRaw());
	}
}