#include "RenderEngine.h"

namespace OpenHMI
{
#define RIPPLEVBOMESHNAME	L"rippleVBOMesh"
#define RIPPLEMESHNAME		L"rippleMesh"

	CFilterRipple::CFilterRipple()
		: m_uiTouchRadius(0)
		, m_uiDamping(0)
		, m_uiWidth(0)
		, m_uiHeight(0)
		, m_pfRippleCoeff(NULL)
		, m_pfRippleSource(NULL)
		, m_pfRippleDest(NULL)
		, m_pRndFilter(NULL)
		, m_pBatchRenderGroup(NULL)
	{
		;
	}

	CFilterRipple::~CFilterRipple()
	{
		if (m_pfRippleDest)
		{
			free(m_pfRippleDest);
			m_pfRippleDest = NULL;
		}

		if (m_pfRippleSource)
		{
			free(m_pfRippleSource);
			m_pfRippleSource = NULL;
		}

		if (m_pfRippleCoeff)
		{
			free(m_pfRippleCoeff);
			m_pfRippleCoeff = NULL;
		}

		if ( m_pBatchRenderGroup != NULL )
		{
			CRenderEntry* pOwner = m_pBatchRenderGroup->getOwner();
			pOwner->free2DBatchRenderGroup(m_pBatchRenderGroup);
			m_pBatchRenderGroup = NULL;
		}
	}

	BOOLEAN CFilterRipple::fromRenderFilter(CRenderGroupCacheData* pRndGroupCacheData, const CRenderBaseFilter* pRndFilter)
	{
		CRenderGroup* pRndGroup = pRndGroupCacheData->getRenderGroup();
		if (pRndGroup == NULL)
		{
			return FALSE;
		}

		// width and Height
		m_uiWidth  = pRndGroup->getWidth();
		m_uiHeight = pRndGroup->getHeight();

		
		// Touch Radius & Damping
		m_pRndFilter	= (CRenderFltRipple*)(pRndFilter);
		m_uiTouchRadius = m_pRndFilter->getTouchRadius();
		m_uiDamping		= m_pRndFilter->getDamping();

		m_uiTouchRadius = 1;
		m_uiDamping		= 7;

		// TODO read from Filter
		m_uiPoolWidth  = 80;
		m_uiPoolHeight = 48;

		// Init ripple
		init();

		// Init mesh
		initMesh();

		return TRUE;
	}

	void CFilterRipple::doFilter(C2DBatchRenderGroup* pIn, C2DBatchRenderGroup* pbg, Array2<C2DBatchRenderGroup*>& aryOut)
	{
// 		if (!m_pRndFilter->isPlaying())
// 		{
// 			return;
// 		}

		if ( pIn == NULL )
		{
			return;
		}

		C2DBatchRenderGroup* pCurBatchRenderGroup = pbg;

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

		CRenderTarget* pRT = pIn->getRenderTarget();
		CCacheTexture* pRTTex = pRT->getTexture();
		CCacheMaterialPtr  ptrMtl( CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_UI_DEFAULT, pRTTex) );

		C2DBatchRender* pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
		pCurBatchRenderGroup->add(pBatchRender);

		// drop
		if (m_pRndFilter->getCursorCnt() > 0)
		{
			CCursor curosr = m_pRndFilter->getCursorByIndex(0);
			drop((FLOAT)curosr.getX(), (FLOAT)curosr.getY(), 4);
		}

		// Update Ripple -- VertexData
		updateRipple();

		if( m_pMesh && m_pVBOMesh )
		{
			pBatchRender->setRenderModel(0,0,m_pMesh.getRaw(), m_pVBOMesh.getRaw(), ptrMtl.getRaw() );
		}
	}

	// Private
	void CFilterRipple::init()
	{
		// malloc
		m_pfRippleCoeff  = (FLOAT*)malloc((m_uiTouchRadius*2+1)*(m_uiTouchRadius*2+1)*sizeof(FLOAT));
		if (m_pfRippleCoeff == NULL)
		{
			return;
		}

		m_pfRippleSource = (FLOAT *)malloc((m_uiPoolWidth+2)*(m_uiPoolHeight+2)*sizeof(FLOAT));
		if (m_pfRippleSource == NULL)
		{
			free(m_pfRippleCoeff);
			m_pfRippleCoeff = NULL;
			return;
		}
		memset(m_pfRippleSource, 0, (m_uiPoolWidth+2)*(m_uiPoolHeight+2)*sizeof(FLOAT));

		m_pfRippleDest   = (FLOAT *)malloc((m_uiPoolWidth+2)*(m_uiPoolHeight+2)*sizeof(FLOAT));
		if (m_pfRippleDest == NULL)
		{
			free(m_pfRippleCoeff);
			free(m_pfRippleSource);
			m_pfRippleCoeff  = NULL;
			m_pfRippleSource = NULL;
			return;
		}
		memset(m_pfRippleDest, 0, (m_uiPoolWidth+2)*(m_uiPoolHeight+2)*sizeof(FLOAT));

		// Init RippleCoeff
		for (UINT y = 0; y <=2 * m_uiTouchRadius; y++)
		{
			for (UINT x = 0; x <=2 * m_uiTouchRadius; x++)
			{        
				FLOAT fDistance =(FLOAT) Math::sqrt( (x-m_uiTouchRadius)*(x-m_uiTouchRadius) + (y-m_uiTouchRadius)*(y-m_uiTouchRadius) );

				if (fDistance <= m_uiTouchRadius)
				{
					FLOAT fFactor = (fDistance/(FLOAT)m_uiTouchRadius);
					// goes from -512 -> 0
					m_pfRippleCoeff[y*(m_uiTouchRadius*2+1) + x] = -((FLOAT)Math::cos(fFactor * 3.1415) + 1.0f) * 256.0f;
				}
				else 
				{
					m_pfRippleCoeff[y*(m_uiTouchRadius*2+1) + x] = 0.0f;   
				}
			}
		}
	}

	void CFilterRipple::initMesh()
	{
		m_pVBOMesh = CMeshPool::getInstance()->getDynamicUIGPUMesh(CCacheMesh::EMESHFMT_POSITION3 | CCacheMesh::EMESHFMT_TEXCOORD0, m_uiPoolWidth * m_uiPoolHeight,(m_uiPoolHeight-1)*(m_uiPoolWidth-1)*6);

		m_pMesh = CMeshPool::getInstance()->getRippleMesh(m_uiWidth,m_uiHeight,m_uiPoolWidth,m_uiPoolHeight);
	}

	void CFilterRipple::updateRipple()
	{
		if( m_pMesh == NULL )
		{
			return;
		}

		UINT x, y;
		BOOLEAN bQuiet  = TRUE;

		for (y = 0; y < m_uiPoolHeight; y++)
		{
			for (x = 0; x < m_uiPoolWidth; x++)
			{
				// * - denotes current pixel
				//
				//       a 
				//     c * d
				//       b 

				// +1 to both x/y values because the border is padded
				FLOAT a = m_pfRippleSource[(y)*(m_uiPoolWidth+2) + x+1];
				FLOAT b = m_pfRippleSource[(y+2)*(m_uiPoolWidth+2) + x+1];
				FLOAT c = m_pfRippleSource[(y+1)*(m_uiPoolWidth+2) + x];
				FLOAT d = m_pfRippleSource[(y+1)*(m_uiPoolWidth+2) + x+2];

				FLOAT result = (a + b + c + d)/2.0f - m_pfRippleDest[(y+1)*(m_uiPoolWidth+2) + x+1];

				result -= result/m_uiDamping;

				m_pfRippleDest[(y+1)*(m_uiPoolWidth+2) + x+1] = result;

				// if the energy < 0.01, then treat it as quiet dot.
				if (  Math::abs(m_pfRippleDest[(y+1)*(m_uiPoolWidth+2) + x+1] - 0.00f) > 0.01f   )		
				{
					bQuiet = FALSE;
				}
			}            
		}

		if (bQuiet)
		{
			m_pRndFilter->stop();
		}

		FLOAT *pfVertexData = NULL;
		m_pMesh->lock(&pfVertexData);
		// second pass for modifying texture coord
		for (y = 0; y < m_uiPoolHeight; y++) 
		{
			for (x = 0; x < m_uiPoolWidth; x++) 
			{
				// * - denotes current pixel
				//
				//       a
				//     c * d
				//       b

				// +1 to both x/y values because the border is padded
				FLOAT a = m_pfRippleDest[(y)*(m_uiPoolWidth+2) + x+1];
				FLOAT b = m_pfRippleDest[(y+2)*(m_uiPoolWidth+2) + x+1];
				FLOAT c = m_pfRippleDest[(y+1)*(m_uiPoolWidth+2) + x];
				FLOAT d = m_pfRippleDest[(y+1)*(m_uiPoolWidth+2) + x+2];

				FLOAT s_offset = ((b - a) / 2048.f);
				FLOAT t_offset = ((c - d) / 2048.f);

				// clamp
				s_offset = (s_offset < -0.5f) ? -0.5f : s_offset;
				t_offset = (t_offset < -0.5f) ? -0.5f : t_offset;
				s_offset = (s_offset > 0.5f) ? 0.5f : s_offset;
				t_offset = (t_offset > 0.5f) ? 0.5f : t_offset;

				FLOAT s_tc = ((FLOAT)(x))/(m_uiPoolWidth-1);
				FLOAT t_tc = ((FLOAT)(y))/(m_uiPoolHeight-1);

				pfVertexData[(y*m_uiPoolWidth+x)*5 + 3] = s_tc + s_offset;
				pfVertexData[(y*m_uiPoolWidth+x)*5 + 4] = 1.0f - (t_tc + t_offset);
			}
		}

		FLOAT *pTmp     = m_pfRippleDest;
		m_pfRippleDest   = m_pfRippleSource;
		m_pfRippleSource = pTmp;

		m_pMesh->unLock();
	
	}

	void CFilterRipple::drop( FLOAT x, FLOAT y, FLOAT fWeight)
	{
		UINT uiXIndex = (UINT)((x / m_uiWidth) * m_uiPoolWidth);
		UINT uiYIndex = (UINT)((y / m_uiHeight) * m_uiPoolHeight);
		//uiYIndex = m_uiPoolHeight - uiYIndex;

		for (UINT y = uiYIndex - m_uiTouchRadius; y <= uiYIndex + m_uiTouchRadius; y++)
		{
			for (UINT x = uiXIndex - m_uiTouchRadius; x <= uiXIndex + m_uiTouchRadius; x++)
			{        
				if (x >= 0 && x < m_uiPoolWidth &&
					y >= 0 && y < m_uiPoolHeight)
				{
					// +1 to both x/y values because the border is padded
					m_pfRippleSource[(m_uiPoolWidth+2)*(y+1)+x+1] += fWeight * m_pfRippleCoeff[(y-(uiYIndex-m_uiTouchRadius))*(m_uiTouchRadius*2+1)
																																+ x - (uiXIndex - m_uiTouchRadius)];   
				}
			}
		}    
	}
}