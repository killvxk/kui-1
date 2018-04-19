#include "RenderEngine.h"

namespace OpenHMI
{
	C2DRenderUtility::C2DRenderUtility(CRenderEntry& rOwner)
		: m_rRenderEntry(rOwner)
	{
		;
	}

	C2DRenderUtility::~C2DRenderUtility()
	{
		;
	}

	void C2DRenderUtility::initialize()
	{
		//m_pOpenArtistMesh = CMeshPool::getInstance()->getDynamicUIGPUMesh(CCacheMesh::EMESHFMT_POSITION2 | CCacheMesh::EMESHFMT_COLOR4,0,0);
	}

	void C2DRenderUtility::unInitialize()
	{
		m_pOpenArtistMesh = NULL;
	}

	void C2DRenderUtility::drawText(CRenderText *pRndText, C2DBatchRenderGroup* pCurBatchRenderGroup)
	{
		if (pRndText == NULL || pRndText->getWidth() == 0 || pRndText->getHeight() == 0 || pRndText->getContent().length() == 0)
		{
			return;
		}
		
		CRenderTextCacheData* pCacheData = (CRenderTextCacheData*)(pRndText->getCacheData());
		if (pCacheData == NULL)
		{
			pCacheData = new CRenderTextCacheData();
			pRndText->setCacheData(pCacheData);
		}

		pCacheData->flush(pRndText);

		// Render.
		CRenderText::EHorizontalLayout eHLayout = pRndText->getHorizontalLayout();
		INT iTextOffsetX = 0;
 		if (pRndText->getUseOffsetX())
 		{
			eHLayout = CRenderText::HL_LEFT;
			iTextOffsetX = pRndText->getOffsetX();
		}

		CRenderText::EVerticalLayout eVLayout = pRndText->getVerticalLayout();
		INT iTextOffsetY = 0;
 		if (pRndText->getUseOffsetY())
 		{
			eVLayout = CRenderText::VL_TOP;
			iTextOffsetY = pRndText->getOffsetY();
		}

		CDIB* pDib = pCacheData->getDIB();
		if ( pDib )
		{
			// generate the C2DBatchRender
			C2DBatchRender* pBatchRender = pCurBatchRenderGroup->getBatchRender(pRndText);
			if ( pBatchRender == NULL )
			{
				pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
				pCurBatchRenderGroup->add(pBatchRender);
			}

			CRectangle rcSrc( pRndText->getX()  , 
									pRndText->getY()  ,
									pRndText->getWidth(), pRndText->getHeight());
			CRectangle rcText = CDIB::calcTextRect( rcSrc.getX() + iTextOffsetX, 
									rcSrc.getY() + iTextOffsetY, 
									rcSrc.getWidth(), rcSrc.getHeight(),
									pDib->getWidth(), pDib->getHeight(),
									eHLayout, eVLayout);
			CRectangle rcDest = rcSrc.intersect(rcText);
			CRectangleF rcTex = CDIB::calcDestRoi(rcDest.getX(), rcDest.getY(), rcDest.getWidth(), rcDest.getHeight(),
					rcText.getX(), rcText.getY(), rcText.getWidth(), rcText.getHeight());
			if( pCacheData->isUseMergeTexture() )
			{
				calUseMergeTexTextRoi(pCacheData,rcTex);
				if( !rcTex.isEmpty() )
				{
					pBatchRender->addRenderItem(rcDest, rcTex, pCacheData->getMaterial());
				}
				//CLog::info(String(L""), String::format(L"rcTex %f %f %f %f", rcTex.getX(), rcTex.getY(), rcTex.getWidth(), rcTex.getHeight()));
			}
			else
			{
				pBatchRender->addRenderItem(rcDest, rcTex, pCacheData->getMaterial());
			}

			if (pRndText->getHoritontalDuplicateSwitch())
			{
				if (CONFIG::getOutputPerformance())
				{
					CLog::info(RENDER_ENGINE_NAME, L"scrolling Text");
				}

				if (pRndText->getBiDiMode() == CRenderText::BDM_LEFT_TO_RIGHT)
				{
					INT iLeft = rcSrc.getX() + iTextOffsetX + pDib->getWidth() + pRndText->getHoritontalDuplicateInterval();
					INT iTop = rcSrc.getY() + iTextOffsetY;

					if (rcSrc.getX() < iLeft && iLeft < rcSrc.getRight())
					{
						rcText.set( iLeft,iTop, pDib->getWidth(),pDib->getHeight() );
						rcDest = rcSrc.intersect(rcText);
						rcTex = CDIB::calcDestRoi(rcDest.getX(), rcDest.getY(), rcDest.getWidth(), rcDest.getHeight(),
												  rcText.getX(), rcText.getY(), rcText.getWidth(), rcText.getHeight());
						if( pCacheData->isUseMergeTexture() )
						{
							calUseMergeTexTextRoi(pCacheData,rcTex);
							if( rcTex.isEmpty() )
							{
								return ;
							}
						}
						pBatchRender->addRenderItem(rcDest, rcTex, pCacheData->getMaterial());
					}
				}
				else
				{
					INT iLeft = rcSrc.getX() + iTextOffsetX - pRndText->getHoritontalDuplicateInterval();
					INT iTop = rcSrc.getY() + iTextOffsetY;

					if (rcSrc.getX() < iLeft && iLeft < rcSrc.getRight())
					{
						rcText.set( iLeft - pDib->getWidth(), iTop, pDib->getWidth(), pDib->getHeight() );
						rcDest = rcSrc.intersect(rcText);
						rcTex = CDIB::calcDestRoi(rcDest.getX(), rcDest.getY(), rcDest.getWidth(), rcDest.getHeight(),
												  rcText.getX(), rcText.getY(), rcText.getWidth(), rcText.getHeight());
						if( pCacheData->isUseMergeTexture() )
						{
							calUseMergeTexTextRoi(pCacheData,rcTex);
							if( rcTex.isEmpty() )
							{
								return ;
							}
						}
						pBatchRender->addRenderItem(rcDest, rcTex, pCacheData->getMaterial());
					}
				}
			}
		}
	}

	void C2DRenderUtility::drawImage(CRenderImage *pRndImage, C2DBatchRenderGroup* pCurBatchRenderGroup)
	{
		if (pRndImage == NULL)
		{
			return;
		}

		// generate the CCacheData
		CRenderImageCacheData* pCacheData = (CRenderImageCacheData*)pRndImage->getCacheData();
		if (pCacheData == NULL)
		{
			pCacheData = new CRenderImageCacheData(pRndImage);
			if (pCacheData == NULL)
			{
				return;
			}
			pRndImage->setCacheData(pCacheData);
		}

		pCacheData->flush();
		CCacheTexture* pTex = pCacheData->getTexture();
		if( pTex == NULL )
		{
			return;
		}

		// generate the C2DBatchRender
		C2DBatchRender* pBatchRender = pCurBatchRenderGroup->getBatchRender(pRndImage);
		if ( pBatchRender == NULL )
		{
			pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
			pCurBatchRenderGroup->add(pBatchRender);
			//pCurBatchRenderGroup->markRenderImage();
		}

		CRectangle rcSrc(pRndImage->getX(), pRndImage->getY(), pRndImage->getWidth(), pRndImage->getHeight() );
		// center and fill layout donot use NormalLayoutClipOffsetX,Y
		CRectangle rcImage = CDIB::calcImageRect(rcSrc.getX() + pRndImage->getNormalLayoutClipOffsetX(), 
					rcSrc.getY() + pRndImage->getNormalLayoutClipOffsetY(), 
					rcSrc.getWidth(), rcSrc.getHeight(),
					pTex->getWidth(),pTex->getHeight(), pRndImage->getLayout());
		CRectangle rcDest = rcSrc.intersect(rcImage);
		CRectangleF rcTex = CDIB::calcDestRoi(rcDest.getX(), rcDest.getY(), rcDest.getWidth(), rcDest.getHeight(),
					rcImage.getX(), rcImage.getY(), rcImage.getWidth(), rcImage.getHeight());

		pBatchRender->addRenderItem(rcDest, rcTex, pCacheData->getMaterial());

		//CLog::info(String(L""),String::format(L"image : %d %d  %d %d",rcDest.getX(),rcDest.getY(),rcDest.getRight() + 1,rcDest.getBottom() + 1 ));
		
	}

	void C2DRenderUtility::drawViewport(CRenderViewport* pRndViewport, C2DBatchRenderGroup* pCurBatchRenderGroup, UINT uiViewportSiblingCnt)
	{
		if (pRndViewport == NULL)
		{
			return;
		}

		// generate the CCacheData
		CRenderViewportCacheData* pCacheData = (CRenderViewportCacheData*)pRndViewport->getCacheData();
		if (pCacheData == NULL)
		{
			pCacheData = new CRenderViewportCacheData(pRndViewport);
			if (pCacheData == NULL)
			{
				return;
			}
			pRndViewport->setCacheData(pCacheData);
		}

		pCacheData->flush();
		// generate the C2DBatchRender
		const CRenderViewport::CBaseStyle* pStyle = pRndViewport->getStyle();
		if( pStyle == NULL )
		{
			return;
		}

		switch ( pStyle->getType() )
		{
		case CRenderViewport::ST_NORMAL:
			break;

		case CRenderViewport::ST_TRANSPARENT:
			{
				C2DBatchRender* pBatchRender = pCurBatchRenderGroup->getBatchRender(pRndViewport);
				if ( pBatchRender == NULL )
				{
					pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
					pCurBatchRenderGroup->add(pBatchRender);
				}

				CRectangle rc(pRndViewport->getX(), pRndViewport->getY(), pRndViewport->getWidth(), pRndViewport->getHeight());
				CRectangleF roi(0.0f,0.0f,1.0f,1.0f);
				pBatchRender->addRenderItem(rc, roi, pCacheData->getMaterial());
			}
			break;

		case CRenderViewport::ST_CAPTURE:
			{
				C2DBatchRenderGroup* pParentBRG = (C2DBatchRenderGroup*)pCurBatchRenderGroup->getParent();

				if ( pParentBRG )
				{
					CRectangle rc(pRndViewport->getX(), pRndViewport->getY(), pRndViewport->getWidth(), pRndViewport->getHeight()); 
					pCurBatchRenderGroup->copyfrom(pParentBRG, uiViewportSiblingCnt, rc);

					C2DBatchRender* pBatchRender = pParentBRG->getFreeBatchRender();
					pParentBRG->insert(uiViewportSiblingCnt,pBatchRender);

					//CCacheMaterial* pMtl = pCacheData->getMaterial();
					//CRectangle rc(pRndViewport->getX(), pRndViewport->getY(), pRndViewport->getWidth(), pRndViewport->getHeight());
					//CRectangleF roi(0.0f,0.0f,1.0f,1.0f);
					//pBatchRender->addRenderItem(rc,roi,pMtl);
				}
			}
			break;

		case CRenderViewport::ST_WAVER:
			{
				pCacheData->addWaver(&m_rRenderEntry); 
				C2DBatchRender* pBatchRender = pCurBatchRenderGroup->getBatchRender(pRndViewport);
				if ( pBatchRender == NULL )
				{
					pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
					pCurBatchRenderGroup->add(pBatchRender);
				}

				CRectangle rc(pRndViewport->getX(), pRndViewport->getY(), pRndViewport->getWidth(), pRndViewport->getHeight());
				CRectangleF roi(0.0f,1.0f,1.0f,-1.0f);
				pBatchRender->addRenderItem(rc, roi, pCacheData->getMaterial());
			}
			break;
		}
	}

	void C2DRenderUtility::drawRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine, C2DBatchRenderGroup* pCurBatchRenderGroup)
	{
		CCacheMaterial* pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_OPENARTIST_LINESOLID,NULL);

		C2DBatchRender*  pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
		pCurBatchRenderGroup->add(pBatchRender);

		CCacheMesh* pMesh = CMeshPool::getInstance()->getRectangleMesh(uiWidth, uiHeight, uiColor, bDashLine);
		
		pBatchRender->setRenderModel(iX + 1, iY, pMesh,m_pOpenArtistMesh.getRaw(), pMtl);

		//CLog::info(String(L""),String::format(L"rect : %d %d  %d %d",iX,iY, iX + uiWidth - 1,uiHeight+iY-1 ));
		
	}

	void C2DRenderUtility::drawTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine, C2DBatchRenderGroup* pCurBatchRenderGroup)
	{
		CCacheMaterial* pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_OPENARTIST_LINESOLID,NULL);

		C2DBatchRender*  pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
		pCurBatchRenderGroup->add(pBatchRender);

		CCacheMesh* pMesh = CMeshPool::getInstance()->getTriangleMesh(p1,p2, p3, uiColor, bDashLine);
		
		pBatchRender->setRenderModel(0,0,pMesh,m_pOpenArtistMesh, pMtl);
	}

	void C2DRenderUtility::drawLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine, C2DBatchRenderGroup* pCurBatchRenderGroup)
	{
		CCacheMaterial* pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_OPENARTIST_LINESOLID,NULL);

		C2DBatchRender*  pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
		pCurBatchRenderGroup->add(pBatchRender);

		CCacheMesh* pMesh = CMeshPool::getInstance()->getLineMesh(iX1, iY1, iX2, iY2,uiColor,bDashLine);
		
		pBatchRender->setRenderModel(0,0,pMesh,m_pOpenArtistMesh, pMtl);
	}

	void C2DRenderUtility::drawCircle(INT iX, INT iY, UINT uiRadius, UINT uiColor, BOOLEAN bDashLine, C2DBatchRenderGroup* pCurBatchRenderGroup )
	{
		CCacheMaterial* pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_OPENARTIST_LINESOLID,NULL);

		C2DBatchRender*  pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
		pCurBatchRenderGroup->add(pBatchRender);

		CCacheMesh* pMesh = CMeshPool::getInstance()->getCircleMesh(uiRadius, uiColor, bDashLine);
		
		pBatchRender->setRenderModel(iX, iY, pMesh,m_pOpenArtistMesh, pMtl);
	}

	void C2DRenderUtility::fillRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, C2DBatchRenderGroup* pCurBatchRenderGroup)
	{
		CCacheMaterial* pMtl = CMaterialPool::getInstance()->getInternalMaterial(CMaterialPool::EMT_OPENARTIST_LINESOLID,NULL);

		C2DBatchRender*  pBatchRender = pCurBatchRenderGroup->getFreeBatchRender();
		pCurBatchRenderGroup->add(pBatchRender);

		CCacheMesh* pMesh = CMeshPool::getInstance()->getSolidRectangleMesh(uiWidth, uiHeight, uiColor);
		
		pBatchRender->setRenderModel(iX, iY, pMesh, m_pOpenArtistMesh, pMtl);
	}
	
	void C2DRenderUtility::calUseMergeTexTextRoi(CRenderTextCacheData* pCacheData,CRectangleF& rcTex)
	{
		const CRectangleF& rcMergeTexRoi = pCacheData->getTextureROI();
		rcTex.setX( rcTex.getX() * rcMergeTexRoi.getWidth() + rcMergeTexRoi.getX() );
		rcTex.setY( rcTex.getY() * rcMergeTexRoi.getHeight() + rcMergeTexRoi.getY() );
		rcTex.setWidth( rcTex.getWidth() * rcMergeTexRoi.getWidth() );
		rcTex.setHeight( rcTex.getHeight() * rcMergeTexRoi.getHeight() );
	}
}