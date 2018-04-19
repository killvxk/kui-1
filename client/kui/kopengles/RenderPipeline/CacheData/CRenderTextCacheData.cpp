#include "RenderEngine.h"

namespace OpenHMI
{

	CRenderTextCacheData::CRenderTextCacheData()
		: m_pTex(NULL)
		, m_pDIB(NULL)
		, m_uiLineHeight(0)
		, m_bPropertyChanged(TRUE)
	{
		;
	}

	CRenderTextCacheData::~CRenderTextCacheData()
	{
		// for text cache
		if ( m_pDIB )
		{
			delete m_pDIB;
			m_pDIB = NULL;
		}

		if ( m_pTex && m_pTex->isTextMergeTexture())
		{
			CTextMergeTexture* pTex = (CTextMergeTexture*)m_pTex.getRaw();
			pTex->free(m_aryTexIdx);
		}

		m_uiLineHeight = 0;
	}

	void CRenderTextCacheData::notifyContentPropertyChanged()
	{
		m_bPropertyChanged = TRUE;
		
		if ( m_pDIB )
		{
			delete m_pDIB;
			m_pDIB = NULL;
		}

		m_uiLineHeight = 0;
	}

	void CRenderTextCacheData::flush(CRenderText *pRndText)
	{
		if ( m_bPropertyChanged )
		{
			if ( !m_pDIB )
			{
				generateDIB(pRndText);
			}
			 
			if ( !m_pDIB )
			{
				return;
			}

			if( !generateMergeTexture() )
			{
				// if width height doesn't change , remain use the old texture ,else free old texture and generate new texture
				if ( m_pTex )
				{
					if ( !m_pTex->isTextMergeTexture() && 
						 m_pTex->getWidth() == m_pDIB->getWidth() && 
						 m_pTex->getHeight() == m_pDIB->getHeight() )
					{
						m_pTex->fill(m_pDIB->getPixelData());
					}
					else
					{
						m_pTex = CTexturePool::getInstance()->getTexture(m_pDIB);
						flushMaterial();
					}
				}
				else
				{
					m_pTex = CTexturePool::getInstance()->getTexture(m_pDIB);
					flushMaterial();
				}
			}

			m_bPropertyChanged = FALSE;
		}
	}

	CDIB* CRenderTextCacheData::generateDIB(CRenderText* pRndText)
	{
		CTextLayout textLayout;
		m_pDIB = textLayout.render(pRndText);

		m_uiLineHeight = textLayout.getLineHeight();

		return m_pDIB;
	}

	CRectangleF	CRenderTextCacheData::getTextureROI()
	{
		if ( m_pTex && m_pTex->isTextMergeTexture())
		{
			CTextMergeTexture* pTex = (CTextMergeTexture*)m_pTex.getRaw();
			return pTex->getROI(m_aryTexIdx[0], m_pDIB->getWidth(), m_pDIB->getHeight());
		}

		return CRectangleF(0.0f,0.0f,1.0f,1.0f);
	}

	void CRenderTextCacheData::flushMaterial()
	{
		m_pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_UI_DEFAULT,m_pTex);
	}

	BOOLEAN	CRenderTextCacheData::generateMergeTexture()
	{
		CConfigParser::tagTextRenderOptInfo* pTagTROptInfo = CConfigParser::getInstance()->getTextRenderOptInfo();

		if( m_pDIB->getWidth() <= pTagTROptInfo->uiThresholdWidth && m_pDIB->getHeight() <= pTagTROptInfo->uiThresholdHeight )
		{
			if ( m_pTex && m_pTex->isTextMergeTexture() && !m_aryTexIdx.isEmpty() )
			{
				CTextMergeTexture* pTex = (CTextMergeTexture*)m_pTex.getRaw();
				pTex->free(m_aryTexIdx);	
				m_aryTexIdx.clear();
			}

			CTextMergeTexture *pTex = CTexturePool::getInstance()->getTextMergeTexture();
			Array2<UINT> aryIdx; 
			pTex->getFreeRect(m_pDIB->getWidth(), m_pDIB->getHeight(),aryIdx );
			if( !aryIdx.isEmpty() )
			{
				m_pTex = pTex;
				m_aryTexIdx.addItems(aryIdx);
				pTex->fill(aryIdx, m_pDIB );
				flushMaterial();
				return TRUE;
			}
		}
		else if ( m_pTex && m_pTex->isTextMergeTexture())
		{
			CTextMergeTexture* pTex = (CTextMergeTexture*)m_pTex.getRaw();
			pTex->free(m_aryTexIdx);
		}

		m_aryTexIdx.clear();
		return FALSE;
	}
}