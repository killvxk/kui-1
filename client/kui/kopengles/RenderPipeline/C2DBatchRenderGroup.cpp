#include "RenderEngine.h"

namespace OpenHMI
{
	CPool<C2DBatchRender>	C2DBatchRenderGroup::sm_listFreeBatchRender(200);
	C2DBatchRenderGroup::C2DBatchRenderGroup()
		: m_pMesh(NULL)
		, m_pVBOMesh(NULL)
		, m_bBlank(TRUE)
		//, m_uiRndImageIdx(0xffffffff)
	{
		;
	}

	C2DBatchRenderGroup::~C2DBatchRenderGroup()
	{
		;
	}

	void C2DBatchRenderGroup::add(C2DBatchRender* p)
	{
		m_aryUsedBatchRenderArray.addItem(p);
		m_bBlank = FALSE;
	}

	void C2DBatchRenderGroup::insert(UINT uidx, C2DBatchRender* p)
	{
		m_aryUsedBatchRenderArray.insertItem(uidx,p);
		m_bBlank = FALSE;
	}

	void C2DBatchRenderGroup::render()
	{
		CCacheMesh* pVBOMesh = m_pVBOMesh.getRaw();
		CDynamicUIMesh* pMesh = m_pMesh.getRaw();
		if( m_pVBOMesh.isNull() )
		{
			pVBOMesh = CMeshPool::getInstance()->getDynamicUIGPUMesh(CCacheMesh::EMESHFMT_POSITION3 | CCacheMesh::EMESHFMT_TEXCOORD0, 1000,1000);
			
			pVBOMesh->reset();
			m_pVBOMesh = pVBOMesh;
		}

		if( m_pMesh.isNull() )
		{
			pMesh = CMeshPool::getInstance()->getDynamicUIMesh();
			m_pMesh = pMesh;
		}

		UINT cnt = m_aryUsedBatchRenderArray.size();
		for ( UINT i = 0; i < cnt; ++i )
		{
			C2DBatchRender* pRender = m_aryUsedBatchRenderArray[i];
			pRender->render(pMesh,pVBOMesh);
			//pRender->reset();
			pVBOMesh->reset();
			pMesh->reset();
			//sm_listFreeBatchRender.release(pRender);
			
		}
		
		if (CONFIG::getOutputPerformance())
		{
			CLog::debug2(String(RENDER_ENGINE_NAME),String::format(L"BatchRender count of BRG = %u",cnt));
		}

		//m_aryUsedBatchRenderArray.clear();
	}

	void C2DBatchRenderGroup::flush(BOOLEAN bGenerateRT)
	{
		if( getPositionChanged() || getSizeChanged() )
		{

			m_projectMatrix.makeOrthognal(	(FLOAT)m_iX,FLOAT(m_iX + m_uiWidth),
											(FLOAT)m_iY,FLOAT(m_iY + m_uiHeight),
											2.0f,1000.0f);

			m_perspectiveProjectMatrix.makePerspective(	(FLOAT)m_iX,FLOAT(m_uiWidth) * 40.0f/m_uiHeight + m_iX,
														(FLOAT)m_iY,FLOAT(m_iY + m_uiHeight),
														40.0f,2000.0f);
		}

		CBatchRenderGroup::flush(bGenerateRT);

		m_bBlank = TRUE;
	}

	void C2DBatchRenderGroup::clear()
	{
		UINT cnt = m_aryUsedBatchRenderArray.size();
		for ( UINT i = 0; i < cnt; ++i )
		{
			C2DBatchRender* pRender = m_aryUsedBatchRenderArray[i];
			pRender->reset();
			sm_listFreeBatchRender.release(pRender);
		}
		
		m_aryUsedBatchRenderArray.clear();
	}

	void C2DBatchRenderGroup::reset()
	{
		m_bBlank = TRUE;
		CBatchRenderGroup::reset();
	}

	void C2DBatchRenderGroup::copyfrom(C2DBatchRenderGroup* pBRG, UINT uiCnt, const CRectangle& rc)
	{
		UINT cnt = pBRG->m_aryUsedBatchRenderArray.size();
		cnt = cnt > uiCnt ? uiCnt : cnt;
		for ( UINT i = 0; i < cnt; ++i )
		{
			C2DBatchRender* pRender = pBRG->m_aryUsedBatchRenderArray[i];
			const CRectangle& bbox = pRender->getbbox();
			if( rc.isIntersect(bbox) )
			{
				C2DBatchRender* pRenderCopy = getFreeBatchRender();
				pRenderCopy->clone(pRender);
				m_aryUsedBatchRenderArray.addItem(pRenderCopy);
			}
			
		}

		if( cnt > 0 )
		{
			m_bBlank = FALSE;
		}
	}

	C2DBatchRender* C2DBatchRenderGroup::getBatchRender(CRenderItem* pRndItem)
	{
		UINT cnt = m_aryUsedBatchRenderArray.size();
		C2DBatchRender* pMergeRender = NULL;
		UINT uiMergeRenderIdx = 0xffffffff;
		if( cnt == 0 )
		{
			return NULL;
		}

		CMaterialRenderCacheData* pCacheData = (CMaterialRenderCacheData*)pRndItem->getCacheData();
		CCacheMaterial* pMtl = pCacheData->getMaterial();

		for ( UINT i = cnt-1; i != 0 ; --i )
		{
			C2DBatchRender* pRender = m_aryUsedBatchRenderArray[i];
			if( pRender->getMaterial() == pMtl )
			{
				pMergeRender = pRender;
				uiMergeRenderIdx = i;
				break;
			}
		}

		if( pMergeRender )
		{
			cnt = m_aryUsedBatchRenderArray.size();
			BOOLEAN bCanMerge = TRUE;
			CRectangle bbox(pRndItem->getX(), pRndItem->getY(), pRndItem->getWidth(), pRndItem->getHeight());
			for ( UINT i = cnt-1; i != uiMergeRenderIdx ; --i )
			{
				C2DBatchRender* pRender = m_aryUsedBatchRenderArray[i];
				const CRectangle& curbbox = pRender->getbbox();
				if( bbox.isIntersect( curbbox ) )
				{
					bCanMerge = FALSE;
					break;
				}
			}

			if( bCanMerge )
			{
				return pMergeRender;
			}
		}

		return NULL;
	}

	C2DBatchRender* C2DBatchRenderGroup::getBatchRender(CCacheMaterial* pMtl)
	{
		UINT cnt = m_aryUsedBatchRenderArray.size();
		C2DBatchRender* pRender = NULL;

		if ( cnt > 0 )
		{
			pRender = m_aryUsedBatchRenderArray[cnt-1];
			if ( pRender->getCanMerge(pMtl) )
			{
				return pRender;
			}
		}

		return NULL;
	}

	C2DBatchRender* C2DBatchRenderGroup::getFreeBatchRender()
	{
		C2DBatchRender* p = sm_listFreeBatchRender.get();
		p->setParent(this);
		return p;
	}

	const C3DMatrix& C2DBatchRenderGroup::getProjectMatrix(BOOLEAN b25D)
	{
		if ( b25D )
		{
			return m_perspectiveProjectMatrix;
		}
		else
		{
			return m_projectMatrix;
		}
	}

	void C2DBatchRenderGroup::release()
	{
		if (m_iRef > 0)
		{
			--m_iRef;
			if (m_iRef == 0)
			{
				getOwner()->free2DBatchRenderGroup(this);
			}
		}
	}

	void C2DBatchRenderGroup::releaseBatchRender()
	{
		sm_listFreeBatchRender.release();
	}

}