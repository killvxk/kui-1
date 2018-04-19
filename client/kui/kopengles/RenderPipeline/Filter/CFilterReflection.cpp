#include "RenderEngine.h"

namespace OpenHMI
{

	CFilterReflection::CFilterReflection()
		: m_pMaskMaterial(NULL)
		, m_pRndFilter(NULL)
		, m_pTransformMatrix(NULL)
		, m_pBatchRenderGroup(NULL)
	{
		;
	}

	CFilterReflection::~CFilterReflection()
	{
		m_pBatchRenderGroup = NULL;
	}

	BOOLEAN CFilterReflection::fromRenderFilter(CRenderGroupCacheData* pRndGroupCacheData, const CRenderBaseFilter* pRndFilter)
	{
		m_pRndFilter = (CRenderFltReflection*)(pRndFilter);
		CCacheTexturePtr ptrTex (CTexturePool::getInstance()->getExternalTexture(m_pRndFilter->getMaskImage()));
		if ( ptrTex.isNotNull() ) 
		{
			m_pMaskMaterial = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_UI_ALPHA,ptrTex.getRaw() );	
		}

		return TRUE;
	}

	void CFilterReflection::doFilter(C2DBatchRenderGroup* pIn, C2DBatchRenderGroup* pbg, Array2<C2DBatchRenderGroup*>& aryOut)
	{
		if (m_pRndFilter->getHeight() == 0 || pIn == NULL )
		{
			return;
		}	

		//CRenderGroup* pRndGroup = pRndGroupCacheData->getRenderGroup();
		C2DBatchRenderGroup* pCurBatchRenderGroup = pbg;
		if ( isGenerateRT() )
		{	
			CRenderEntry* pOwner = pbg->getOwner();
			if ( m_pBatchRenderGroup == NULL )
			{
				m_pBatchRenderGroup = pOwner->getFree2DBatchRenderGroup();
			}
			aryOut.addItem(m_pBatchRenderGroup);

			m_pBatchRenderGroup->set(pIn->getX(), pIn->getY(), pIn->getWidth() , pIn->getHeight() + m_pRndFilter->getHeight() );
			m_pBatchRenderGroup->flush(TRUE);

			pCurBatchRenderGroup = m_pBatchRenderGroup;
		}

		// renderGroup and the reflection of it
		CRectangleF roi0(0.0f,1.0f,1.0f,-1.0f);
		CRectangleF roi1(0.0f,0.0f,1.0f,1.0f);
		CRectangle rc0( pIn->getX() , pIn->getY(), pIn->getWidth(),pIn->getHeight()  );
		CRectangle rc1( pIn->getX() , pIn->getY() + pIn->getHeight(), pIn->getWidth(),m_pRndFilter->getHeight()  );
		roi1.setHeight(m_pRndFilter->getHeight()/FLOAT(pIn->getHeight()));

		CRenderTarget* pRT = pIn->getRenderTarget();
		CCacheTexture* pRTTex = pRT->getTexture();
		CCacheMaterialPtr  ptrMtl( CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_UI_ALPHA,pRTTex ) );

		C2DBatchRender* pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
		pCurBatchRenderGroup->add(pBatchRender);
		pBatchRender->addRenderItem(rc0, roi0, ptrMtl.getRaw());
		pBatchRender->addRenderItem(rc1, roi1,NULL);

		// the mask
		if ( m_pMaskMaterial != NULL )
		{
			CRectangleF maskroi(0.0f,0.0f,1.0f,1.0f);
			//CCacheMaterial* pMaskMtl = CMaterialPool::getInstance()->getMaterial(m_iMaskMtlID);
			C2DBatchRender* pMaskBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
			pCurBatchRenderGroup->add(pMaskBatchRender);
			pMaskBatchRender->addRenderItem(rc1, maskroi ,m_pMaskMaterial);
		}
	}
}