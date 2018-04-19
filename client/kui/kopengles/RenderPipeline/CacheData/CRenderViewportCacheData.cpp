#include "RenderEngine.h"

namespace OpenHMI
{

	CRenderViewportCacheData::CRenderViewportCacheData(CRenderViewport* pRndViewport)
		: m_pRndViewport(pRndViewport)
		, m_p3DBatchRenderGroup(NULL)
		, m_pRenderModelCacheData(NULL)
		, m_bPropertyChanged(TRUE)
	{
		;
	}

	CRenderViewportCacheData::~CRenderViewportCacheData()
	{
		if( m_pRenderModelCacheData != NULL )
		{
			delete m_pRenderModelCacheData;
			m_pRenderModelCacheData = NULL;
		}
	}

	void CRenderViewportCacheData::flush()
	{
		if ( m_bPropertyChanged )
		{
			flushMaterial();

			m_bPropertyChanged = FALSE;
		}
	}

	void CRenderViewportCacheData::addWaver(CRenderEntry* pOwner)
	{
		if ( m_p3DBatchRenderGroup == NULL || m_pRenderModelCacheData == NULL )
		{
			return;
		}

		m_p3DBatchRenderGroup->setOwner(pOwner);
		C3DBatchRender* pBatchRender = m_p3DBatchRenderGroup->getFreeBatchRender();
		pBatchRender->addItem(m_pRenderModelCacheData);
		m_p3DBatchRenderGroup->add(pBatchRender);
		pOwner->addBatchRenderGroup(m_p3DBatchRenderGroup);
	}

	void CRenderViewportCacheData::flushMaterial()
	{
		const CRenderViewport::CBaseStyle* pStyle = m_pRndViewport->getStyle();
		if ( pStyle == NULL )
		{
			m_pMtl = NULL;
			return;
		}

		switch ( pStyle->getType() )
		{
		case CRenderViewport::ST_TRANSPARENT:
		case CRenderViewport::ST_CAPTURE:
			{
				m_pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_UI_VIEWPORT, NULL);
			}
			break;
		case CRenderViewport::ST_WAVER:
			{
				flushWaver();
				if ( m_p3DBatchRenderGroup == NULL )
				{
					return;
				}

				CRenderTarget* pRT = m_p3DBatchRenderGroup->getRenderTarget();
				if( pRT == NULL )
				{
					return;
				}
				CCacheTexture* pTex = pRT->getTexture();
				m_pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_UI_DEFAULT,pTex);
				
			}
			break;
		}
		
	}

	void CRenderViewportCacheData::flushWaver()
	{
		if ( m_p3DBatchRenderGroup == NULL )
		{
			m_p3DBatchRenderGroup = new C3DBatchRenderGroup();
			CCamera* pCamera = new CCamera();
			pCamera->perspective(65.0f, (FLOAT)m_pRndViewport->getWidth()/m_pRndViewport->getHeight() , 2.0f , 3000.0f);
			pCamera->setPosition(0.0f,0.0f,22.0f);
			pCamera->lookAt(C3DVector(0.0f,0.0f,0.0f));
			m_p3DBatchRenderGroup->setCamera(pCamera);
			m_p3DBatchRenderGroup->set(m_pRndViewport->getX(), m_pRndViewport->getY(), m_pRndViewport->getWidth(), m_pRndViewport->getHeight() );
			m_p3DBatchRenderGroup->flush(TRUE);

			m_pRenderModelCacheData = new CRenderModelCacheData();
		}

		const CRenderViewport::CBaseStyle* pStyle = m_pRndViewport->getStyle();
		CRenderViewport::CStyleWaver* pWaver = (CRenderViewport::CStyleWaver*)pStyle;
		CCacheTexture* pTex = CTexturePool::getInstance()->getExternalTexture(pWaver->strTextureImage);
		if ( pTex )
		{
			
			UINT row = pWaver->uiRow;
			UINT col = pWaver->uiColumn;
			UINT width = pWaver->uiWidth;
			UINT height = pWaver->uiHeight;
			
			CCacheMesh* pMesh = CMeshPool::getInstance()->getWaverMesh(width,height,row,col);
			m_pRenderModelCacheData->setMesh(pMesh);
			m_pRenderModelCacheData->setTexture(pTex);
		}
	}
}