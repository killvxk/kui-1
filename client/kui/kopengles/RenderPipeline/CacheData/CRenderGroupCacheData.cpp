#include "RenderEngine.h"

namespace OpenHMI
{

	CRenderGroupCacheData::CRenderGroupCacheData(CRenderGroup* pGroup)
		: m_pRndGroup(pGroup)
		, m_bSizeChanged(TRUE)
		, m_bMergePropertyChanged(TRUE)
		, m_bContentPropertyChanged(TRUE)
		, m_fW(0.0f)
		, m_fH(0.0f)
		, m_fAnchorX(0.0f)
		, m_fAnchorY(0.0f)
		, m_pBatchRenderGroup(NULL)
	{
		m_rotateZTransMatrix.identity();
	}

	CRenderGroupCacheData::~CRenderGroupCacheData()
	{
		;
	}

	void CRenderGroupCacheData::flush( C2DBatchRenderGroup* pContainer)
	{
		BOOLEAN bSizeMergeChanged = (m_bMergePropertyChanged || m_bSizeChanged);
		if ( bSizeMergeChanged )
		{
			if ( m_bMergePropertyChanged )
			{
				flushMergeProperty(pContainer);
			}
		}

		if ( m_bSizeChanged || m_bContentPropertyChanged )
		{
			if ( m_pBatchRenderGroup == NULL ) 
			{
				CRenderEntry* pOwner = pContainer->getOwner();
				m_pBatchRenderGroup = pOwner->getFree2DBatchRenderGroup();
			}
			m_pBatchRenderGroup->setX(m_pRndGroup->getX());
			m_pBatchRenderGroup->setY(m_pRndGroup->getY());
			m_pBatchRenderGroup->setWidth(m_pRndGroup->getWidth());
			m_pBatchRenderGroup->setHeight(m_pRndGroup->getHeight());
			m_pBatchRenderGroup->flush(TRUE);
		}
		
		if( bSizeMergeChanged || m_bContentPropertyChanged )
		{
			flushMaterial();
		}
		
		if ( m_bContentPropertyChanged )
		{
			flushFilter();
		}
	}

	void CRenderGroupCacheData::resetState()
	{
		m_bContentPropertyChanged = FALSE;
		m_bMergePropertyChanged = FALSE;
		m_bSizeChanged = FALSE;
	}

	CRenderTarget* CRenderGroupCacheData::getRenderTarget()
	{
		return m_pBatchRenderGroup->getRenderTarget();
	}

	CCacheTexture* CRenderGroupCacheData::getTexture()
	{
		CRenderTarget* pRT = m_pBatchRenderGroup->getRenderTarget();
		if ( pRT )
		{
			return pRT->getTexture();
		}
		
		return NULL;
	}

	void CRenderGroupCacheData::flushMergeProperty(C2DBatchRenderGroup* pContainer)
	{
		// roate and trans
		if ( !Math::isZero(m_pRndGroup->getRotateRadian()) && pContainer )
		{
			m_fAnchorX = m_pRndGroup->getAnchorPointX()/FLOAT(m_pRndGroup->getWidth());
			m_fAnchorY = m_pRndGroup->getAnchorPointY()/FLOAT(m_pRndGroup->getHeight());

			guardAnchor(m_fAnchorX);
			guardAnchor(m_fAnchorY);

			if ( m_pRndGroup->getRotateAxis() == CRenderGroup::RA_Z )
			{
				m_fW = FLOAT(m_pRndGroup->getWidth());
				m_fH = FLOAT(m_pRndGroup->getHeight());
				m_worldPos.set((FLOAT)m_pRndGroup->getX(),(FLOAT)m_pRndGroup->getY(),2.0f);
			}
			else
			{
				const C3DMatrix& projectMatrix = pContainer->getProjectMatrix(TRUE);
				
				INT l = pContainer->getX();
				INT r = pContainer->getX() + pContainer->getWidth();
				INT t = pContainer->getY();
				INT b = pContainer->getY() + pContainer->getHeight();
				INT x = m_pRndGroup->getX();
				INT y = m_pRndGroup->getY();
				INT w = m_pRndGroup->getWidth();
				INT h = m_pRndGroup->getHeight();
				INT f = (w>h) ? w : h;
				FLOAT zw = -600.0f;
				
				FLOAT xq = (x-l) * 2.0f / (r-l) - 1.0f;
				FLOAT yq = (y-b) * 2.0f / (t-b) - 1.0f;
					
				FLOAT xw = xq * (-zw)/projectMatrix[0][0];
				FLOAT yw = yq * (-zw)/projectMatrix[1][1];

				m_fW = -(2 * w)/((r-l)*projectMatrix[0][0])*zw;
				m_fH = -(2 * h)/((t-b)*projectMatrix[1][1])*zw;
				m_worldPos.set(xw,yw,zw);
			}

			if (m_pRndGroup->getRotateAxis() == CRenderGroup::RA_Y)
			{
				m_rotateZTransMatrix.rotateY(m_pRndGroup->getRotateRadian());
			}
			else if ( m_pRndGroup->getRotateAxis() == CRenderGroup::RA_X )
			{
				m_rotateZTransMatrix.rotateX(m_pRndGroup->getRotateRadian());
			}
			else
			{
				m_rotateZTransMatrix.rotateZ(m_pRndGroup->getRotateRadian());
			}

			m_rotateZTransMatrix.setTrans(m_worldPos.getX() + m_fAnchorX * m_fW, m_worldPos.getY() + m_fAnchorY * m_fH, m_worldPos.getZ() );

			if ( Math::isZero(m_pRndGroup->getScaleWidth() - 1.0000f) == FALSE ||
				 Math::isZero(m_pRndGroup->getScaleHeight() - 1.0000f) == FALSE )
			{
				C3DMatrix trans;
				trans.makeScale(m_pRndGroup->getScaleWidth(), m_pRndGroup->getScaleHeight(), 1.0f);
				m_rotateZTransMatrix *= trans;
			}
		}
	}

	void CRenderGroupCacheData::flushMaterial()
	{
		CCacheTexture* pTex = getTexture();

		if ( pTex )
		{
			if ( Math::isZero(m_pRndGroup->getRotateRadian()) )
			{
				m_pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_UI_ALPHA,pTex);
			}
			else
			{
				m_pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_25DUI_ALPHA,pTex);
			}
		}
	}

	void CRenderGroupCacheData::guardAnchor(FLOAT& fAnchor)
	{
		if ( fAnchor < 0.0f )
		{
			fAnchor = 0.0f;
		}
		else if( fAnchor > 1.0f )
		{
			fAnchor = 1.0f;
		}
	}

	void CRenderGroupCacheData::flushFilter()
	{
		m_aryFilter.clear();
		UINT uiFilterCnt = m_pRndGroup->getFilterCount();
		for (UINT j = 0; j < uiFilterCnt; j++)
		{
			const CRenderBaseFilter* p = m_pRndGroup->getFilterAt(j);
			if (p)
			{
				CBaseFilter* pFilter = CFilterCenter::createFilter(p);
				if ( pFilter )
				{
					if( pFilter->fromRenderFilter(this,p) )
					{
						m_aryFilter.addItem(pFilter);
					}
					else
					{
						delete pFilter;
					}
				}
			}
		}

		if( m_aryFilter.size() >=2 )
		{
			CFilterCenter::refreshFilterGenerateRT(m_aryFilter);
		}
	}
}