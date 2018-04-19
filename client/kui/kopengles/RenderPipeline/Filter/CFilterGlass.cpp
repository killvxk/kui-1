#include "RenderEngine.h"

namespace OpenHMI
{

	CFilterGlass::CFilterGlass()
		: m_pMaskMaterial(NULL)
		, m_pRndFilter(NULL)
		, m_pBatchRenderGroup(NULL)
		, m_pBatchRenderGroupTemp(NULL)
	{
		;
	}

	CFilterGlass::~CFilterGlass()
	{
		;
	}

	BOOLEAN CFilterGlass::fromRenderFilter(CRenderGroupCacheData* pRndGroupCacheData, const CRenderBaseFilter* pRndFilter)
	{
		m_pRndFilter = (CRenderFltGlass*)(pRndFilter);
		CCacheTexture* pTex = CTexturePool::getInstance()->getExternalTexture(m_pRndFilter->getMaskImage());
		if ( pTex )
		{
			m_pMaskMaterial = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_UI_DEFAULT,pTex );
		}

		m_iBlurSize = m_pRndFilter->getBlurSize();

		return TRUE;
	}

	void CFilterGlass::doFilter(C2DBatchRenderGroup* pIn, C2DBatchRenderGroup* pbg, Array2<C2DBatchRenderGroup*>& aryOut)
	{
		if ( pIn == NULL )
		{
			return;
		}
		
		C2DBatchRenderGroup* pCurBatchRenderGroup = pbg;
		
		CRenderEntry* pOwner = pbg->getOwner();
		if ( m_pBatchRenderGroupTemp == NULL )
		{
			m_pBatchRenderGroupTemp = pOwner->getFree2DBatchRenderGroup();
		}
		aryOut.addItem(m_pBatchRenderGroupTemp);
		pCurBatchRenderGroup = m_pBatchRenderGroupTemp; 

		m_pBatchRenderGroupTemp->set( pIn->getX(), pIn->getY(), pIn->getWidth() , pIn->getHeight() );
		m_pBatchRenderGroupTemp->flush(TRUE);

		CRectangleF roi0(0.0f, 1.0f, 1.0f, -1.0f);
		CRectangle rc0( pIn->getX() , pIn->getY(), pIn->getWidth(),pIn->getHeight()  );

		// first BRG
		CRenderTarget* pRT = pIn->getRenderTarget();
		CCacheTexture* pRTTex = pRT->getTexture();
		if ( pRTTex == NULL )
		{
			return;
		}

		CCacheMaterialPtr  ptrMtl (CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_GAUSSIAN_BLUR_H, pRTTex ));

		C2DBatchRender* pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
		pCurBatchRenderGroup->add(pBatchRender);
		pBatchRender->addRenderItem(rc0, roi0, ptrMtl.getRaw() );


		pCurBatchRenderGroup = pbg;
		if ( isGenerateRT() )
		{	
			CRenderEntry* pOwner = pbg->getOwner();
			if ( m_pBatchRenderGroup == NULL )
			{
				m_pBatchRenderGroup = pOwner->getFree2DBatchRenderGroup();
			}
			aryOut.addItem(m_pBatchRenderGroup);

			m_pBatchRenderGroup->set( pIn->getX(), pIn->getY(), pIn->getWidth() , pIn->getHeight() );
			m_pBatchRenderGroup->flush(TRUE);

			pCurBatchRenderGroup = m_pBatchRenderGroup;
		}

		//// second BRG
		pRT = m_pBatchRenderGroupTemp->getRenderTarget();
		pRTTex = pRT->getTexture();
		ptrMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_GAUSSIAN_BLUR_V, pRTTex );

		pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
		pCurBatchRenderGroup->add(pBatchRender);
		pBatchRender->addRenderItem(rc0, roi0, ptrMtl.getRaw());

		// the mask
		if ( m_pMaskMaterial )
		{
			CRectangleF maskroi(0.0f,0.0f,1.0f,1.0f);
			//CCacheMaterial* pMaskMtl = CMaterialPool::getInstance()->getMaterial(m_iMaskMtlID);
			C2DBatchRender* pMaskBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
			pCurBatchRenderGroup->add(pMaskBatchRender);
			pMaskBatchRender->addRenderItem(rc0, maskroi ,m_pMaskMaterial);
		}
	}
}