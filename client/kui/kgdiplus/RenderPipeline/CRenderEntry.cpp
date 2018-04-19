#include "RenderEngine.h"

namespace OpenHMI
{
#define CHECK_CAN_DO	\
if (!m_bInitialized)\
{\
	return;\
}

	CRenderEntry::CRenderEntry()
		: m_bInitialized(FALSE)
		, m_bMultiSession(FALSE)
	{

	}

	CRenderEntry::~CRenderEntry()
	{
		unInitialize();
	}

	void CRenderEntry::beginDraw(const tagScreenContext *pScreenContext)
	{
		if (pScreenContext != NULL)
		{
			m_bMultiSession = TRUE;			
		}

		if (m_bMultiSession)
		{
			if (m_screenContext != *pScreenContext )
			{
				unInitialize();

				m_screenContext = *pScreenContext;
			}
		}

		initialize();
	}

	void CRenderEntry::drawGroup(CRenderGroup *pRndGroup, INT iOffsetX, INT iOffsetY, CDIB * pDestDib, BOOLEAN *pbHasContent)
	{
		CHECK_CAN_DO

		if (pRndGroup == NULL) 
		{
			return;
		}

		if (pDestDib == NULL)
		{
			pDestDib = &m_dib;
		}

		if (pRndGroup->isRenderViewportWrapper())
		{
			drawViewport(static_cast<CRenderViewport *>(pRndGroup->getRenderItemAt(0)), iOffsetX, iOffsetY, pDestDib, pbHasContent);
			return;
		}

		BOOLEAN	bNeedCache = pRndGroup->getNeedCache();
		BOOLEAN bNeedClip = pRndGroup->getNeedClip();
		BOOLEAN bHasScale = !Math::isZero(pRndGroup->getScaleWidth() - 1.0) || !Math::isZero(pRndGroup->getScaleHeight() - 1.0);
		BOOLEAN bHasAlpha = pRndGroup->getAlpha() != 255;				
		BOOLEAN bHasFilter = pRndGroup->getFilterCount() > 0;	
		BOOLEAN bHasViewport = pRndGroup->getHasViewport();

		if ( bNeedCache || bNeedClip || bHasScale || bHasAlpha || bHasFilter || bHasViewport)
		{
			INT iX = pRndGroup->getX() - iOffsetX;
			INT iY = pRndGroup->getY() - iOffsetY;

			CCacheData* pCacheData = (CCacheData*)(pRndGroup->getCacheData());
			if (pCacheData == NULL)
			{
				pCacheData = new CCacheData();
				pRndGroup->setCacheData(pCacheData);
			}

			CDIB* pDib = pCacheData->getDIB();
			if (pDib == NULL)
			{
				pDib = new CDIB();
				pDib->createDIB(pRndGroup->getWidth(), pRndGroup->getHeight());

				BOOLEAN bHasContent = FALSE;
				drawGroupChildren(pRndGroup, pRndGroup->getX(), pRndGroup->getY(), pDib, &bHasContent);

				if (bHasContent)
				{
					for (UINT j = 0; j < pRndGroup->getFilterCount(); j++)
					{
						CFilterCenter::doFilter(*pDib, *pDestDib, iX, iY, pRndGroup->getFilterAt(j));
					}
				}

				pCacheData->setDIB(pDib);
			}

			if ( bHasScale )
			{
				pDestDib->merge32DIB(	iX, iY, 
										(INT)(pDib->getWidth() * pRndGroup->getScaleWidth()), 
										(INT)(pDib->getHeight() * pRndGroup->getScaleHeight()),
										pDib->getPixelData(), pDib->getWidth(), pDib->getHeight(), pRndGroup->getAlpha(), 
										CDIB::SL_FILL);
			}
			else
			{
				pDestDib->merge32DIB(	iX, iY, 
										pDib->getWidth(), pDib->getHeight(),
										pDib->getPixelData(), pDib->getWidth(), pDib->getHeight(), pRndGroup->getAlpha(), 
										CDIB::HL_LEFT, CDIB::VL_TOP);
			}
		}
		else 
		{
			drawGroupChildren(pRndGroup, iOffsetX, iOffsetY, pDestDib, pbHasContent);
		}

/*
		if (pRndGroup == NULL) 
		{
			return;
		}

		if (pDestDib == NULL)
		{
			pDestDib = &m_dib;
		}

		for (UINT i = 0; i < pRndGroup->getRenderItemCount(); i++ )
		{
			CRenderItem *pItem =  pRndGroup->getRenderItemAt(i);

			if ( pItem->getType() == CRenderItem::RIT_GROUP )
			{
				CRenderGroup *pGroup = static_cast<CRenderGroup *>(pItem);					

				if (pGroup->isRenderViewportWrapper())
				{
					drawViewport(static_cast<CRenderViewport *>(pGroup->getRenderItemAt(0)), iOffsetX, iOffsetY, pDestDib);
				}
				else
				{
					BOOLEAN	bNeedCache = pGroup->getNeedCache();
					BOOLEAN bNeedClip = pGroup->getNeedClip();
					BOOLEAN bHasScale = !Math::isZero(pGroup->getScaleWidth() - 1.0) || !Math::isZero(pGroup->getScaleHeight() - 1.0);
					BOOLEAN bHasAlpha = pGroup->getAlpha() != 255;				
					BOOLEAN bHasFilter = pGroup->getFilterCount() > 0;	
					BOOLEAN bHasViewport = pGroup->getHasViewport();

					if ( bNeedCache || bNeedClip || bHasScale || bHasAlpha || bHasFilter || bHasViewport)
					{
						INT iX = pGroup->getX() - iOffsetX;
						INT iY = pGroup->getY() - iOffsetY;

						CCacheData* pCacheData = (CCacheData*)(pGroup->getCacheData());
						if (pCacheData == NULL)
						{
							pCacheData = new CCacheData();
							pGroup->setCacheData(pCacheData);
						}

						CDIB* pDib = pCacheData->getDIB();
						if (pDib == NULL)
						{
							pDib = new CDIB();
							pDib->createDIB(pGroup->getWidth(), pGroup->getHeight());

							drawGroup(pGroup, pGroup->getX(), pGroup->getY(), pDib);

							for (UINT j = 0; j < pGroup->getFilterCount(); j++)
							{
								CFilterCenter::doFilter(*pDib, *pDestDib, iX, iY, pGroup->getFilterAt(j));
							}

							pCacheData->setDIB(pDib);
						}

						if ( bHasScale )
						{
							pDestDib->merge32DIB(iX, iY, 
												 (INT)(pDib->getWidth() * pGroup->getScaleWidth()), 
												 (INT)(pDib->getHeight() * pGroup->getScaleHeight()),
												 pDib->getPixelData(), pDib->getWidth(), pDib->getHeight(), pGroup->getAlpha(), 
												 CDIB::SL_FILL);
						}
						else
						{
							pDestDib->merge32DIB(	iX, iY, 
													pDib->getWidth(), pDib->getHeight(),
													pDib->getPixelData(), pDib->getWidth(), pDib->getHeight(), pGroup->getAlpha(), 
													CDIB::HL_LEFT, CDIB::VL_TOP);
						}
					}
					else 
					{
						drawGroup(pGroup, iOffsetX, iOffsetY, pDestDib);
					}
				}
			}
			else if ( pItem->getType() == CRenderItem::RIT_IMAGE )
			{
				drawImage(static_cast<CRenderImage *>(pItem), iOffsetX, iOffsetY, pDestDib);
			}
			else if  ( pItem->getType() == CRenderItem::RIT_TEXT) 
			{
				drawText(static_cast<CRenderText *>(pItem), iOffsetX, iOffsetY, pDestDib);
			}
			else if ( pItem->getType() == CRenderItem::RIT_VIEWPORT)
			{
				drawViewport(static_cast<CRenderViewport *>(pItem), iOffsetX, iOffsetY, pDestDib);
			}
		}
*/
	}

	void CRenderEntry::drawText(CRenderText *pRndText, INT iOffsetX, INT iOffsetY, CDIB * pDestDib, BOOLEAN *pbHasContent)
	{
		CHECK_CAN_DO

		if (pRndText == NULL || pRndText->getWidth() == 0 || pRndText->getHeight() == 0 || pRndText->getContent().length() == 0)
		{
			return;
		}

		if (pbHasContent != NULL)
		{
			*pbHasContent = TRUE;
		}

		if (pDestDib == NULL)
		{
			pDestDib = &m_dib;
		}

		CCacheData* pCacheData = (CCacheData*)(pRndText->getCacheData());
		if (pCacheData == NULL)
		{
			pCacheData = new CCacheData();
			pRndText->setCacheData(pCacheData);
		}

		CDIB* pDib = pCacheData->getDIB();
		if (pDib == NULL)
		{
			// Render DIB data.
			CTextLayout textLayout;
			pDib = textLayout.render(pRndText);
			if (pDib == NULL)
			{
				return;
			}
			
			// Buffer DIB data.
			pCacheData->setDIB(pDib);
			pCacheData->setLineHeight(textLayout.getLineHeight());
		}

		// Merge TextDIB to DestDIB.
		INT iTextOffsetX = pRndText->getOffsetX();
		if (!pRndText->getUseOffsetX())
		{
			CDIB::EHorizontalLayout eHLayout = CDIB::HL_CENTER;
			switch (pRndText->getHorizontalLayout())
			{
			case CRenderText::HL_LEFT:
				eHLayout = CDIB::HL_LEFT;
				break;

			case CRenderText::HL_CENTER:
				eHLayout = CDIB::HL_CENTER;
				break;

			case CRenderText::HL_RIGHT:
				eHLayout = CDIB::HL_RIGHT;
				break;
			}
			iTextOffsetX = CDIB::calcXByLayout(0, INT(pDib->getWidth()), INT(pRndText->getWidth()), eHLayout);
		}

		INT iTextOffsetY = pRndText->getOffsetY();
		if (!pRndText->getUseOffsetY())
		{
			CDIB::EVerticalLayout eVLayout = CDIB::VL_MIDDLE;
			switch (pRndText->getVerticalLayout())
			{
			case CRenderText::VL_TOP:
				eVLayout = CDIB::VL_TOP;
				break;

			case CRenderText::VL_MIDDLE:
				eVLayout = CDIB::VL_MIDDLE;
				break;

			case CRenderText::VL_BOTTOM:
				eVLayout = CDIB::VL_BOTTOM;
				break;
			}
			iTextOffsetY = CDIB::calcYByLayout(0, INT(pDib->getHeight()), INT(pRndText->getHeight()), eVLayout);
		}
		
		CRectangle rcTextClip(pRndText->getX() - iOffsetX, pRndText->getY() - iOffsetY, pRndText->getWidth(), pRndText->getHeight());

		pDestDib->merge32DIB(	rcTextClip.getX() + iTextOffsetX, rcTextClip.getY() + iTextOffsetY, rcTextClip,
								pDib->getPixelData(), pDib->getWidth(), pDib->getHeight(), 255);

		if (pRndText->getHoritontalDuplicateSwitch())
		{
			if (pRndText->getBiDiMode() == CRenderText::BDM_LEFT_TO_RIGHT)
			{
				INT iLeft = rcTextClip.getX() + iTextOffsetX + pDib->getWidth() + pRndText->getHoritontalDuplicateInterval();
				INT iTop = rcTextClip.getY() + iTextOffsetY;

				if (rcTextClip.getX() < iLeft && iLeft < rcTextClip.getRight())
				{
					pDestDib->merge32DIB(iLeft, iTop, rcTextClip, pDib->getPixelData(), pDib->getWidth(), pDib->getHeight(), 255);
				}
			}
			else
			{
				INT iLeft = rcTextClip.getX() + iTextOffsetX - pRndText->getHoritontalDuplicateInterval();
				INT iTop = rcTextClip.getY() + iTextOffsetY;

				if (rcTextClip.getX() < iLeft && iLeft < rcTextClip.getRight())
				{
					pDestDib->merge32DIB(iLeft - pDib->getWidth(), iTop, rcTextClip, pDib->getPixelData(), pDib->getWidth(), pDib->getHeight(), 255);
				}
			}
		}
	}

	void CRenderEntry::drawImage(CRenderImage *pRndImage, INT iOffsetX, INT iOffsetY, CDIB * pDestDib, BOOLEAN *pbHasContent)
	{
		CHECK_CAN_DO

		if (pRndImage == NULL)
		{
			return;
		}

		CCacheImagePtr ptrImage;

		if(pRndImage->getIsBitmap() == TRUE)
		{
			ptrImage = CImagePool::getInstance()->getImage(pRndImage->getFilePath(), pRndImage->getBitmap());
		}
		else
		{
			ptrImage = CImagePool::getInstance()->getImage(pRndImage->getFilePath());
		}

		if (ptrImage == NULL)
		{
			return;
		}

		if (pbHasContent != NULL)
		{
			*pbHasContent = TRUE;
		}

		if (pDestDib == NULL)
		{
			pDestDib = &m_dib;
		}

		switch (pRndImage->getLayout())
		{
		case CRenderImage::L_NORMAL:
// 			pDestDib->merge32DIB(	pRndImage->getX() - iOffsetX, pRndImage->getY() - iOffsetY, pRndImage->getWidth(), pRndImage->getHeight(),
// 									pImage->getPixelData(), pImage->getWidth(), pImage->getHeight(), 255,
// 									CDIB::HL_LEFT, CDIB::VL_TOP);
			pDestDib->merge32DIB(	pRndImage->getX() - iOffsetX - (INT)pRndImage->getNormalLayoutClipOffsetX(), 
									pRndImage->getY() - iOffsetY - (INT)pRndImage->getNormalLayoutClipOffsetY(),
									CRectangle(pRndImage->getX() - iOffsetX, pRndImage->getY() - iOffsetY, pRndImage->getWidth(), pRndImage->getHeight()),
									ptrImage->getPixelData(), ptrImage->getWidth(), ptrImage->getHeight(), 255);
			break;

		case CRenderImage::L_STRETCH_CENTER:
		case CRenderImage::L_STRETCH_FILL:
			CCacheData* pCacheData = (CCacheData*)(pRndImage->getCacheData());
			if (pCacheData == NULL)
			{
				pCacheData = new CCacheData();
				pRndImage->setCacheData(pCacheData);
			}

			CDIB* pDib = pCacheData->getDIB();
			if (pDib == NULL)
			{
				pDib = new CDIB();
				pDib->createDIB( pRndImage->getWidth(), pRndImage->getHeight() );

				pDib->merge32DIB(	0, 0, pRndImage->getWidth(), pRndImage->getHeight(),
									ptrImage->getPixelData(), ptrImage->getWidth(), ptrImage->getHeight(), 255,
									pRndImage->getLayout() == CRenderImage::L_STRETCH_CENTER ? CDIB::SL_CENTER : CDIB::SL_FILL);

				pCacheData->setDIB(pDib);
			}
			
			pDestDib->merge32DIB(	pRndImage->getX() - iOffsetX, pRndImage->getY() - iOffsetY, pRndImage->getWidth(), pRndImage->getHeight(),
									pDib->getPixelData(), pDib->getWidth(), pDib->getHeight(), 255,
									CDIB::HL_LEFT, CDIB::VL_TOP);
			break;
		}
	}

	void CRenderEntry::drawViewport(CRenderViewport* pRndViewport, INT iOffsetX, INT iOffsetY, CDIB* pDestDib, BOOLEAN *pbHasContent)
	{
		CHECK_CAN_DO

		if (pRndViewport == NULL)
		{
			return;
		}		

		if (pDestDib == NULL)
		{
			pDestDib = &m_dib;
		}

		CRenderGroup* pRndGroup = pRndViewport->getParent();
		if (pRndGroup == NULL)
		{
			return;
		}

		INT iX = pRndViewport->getX() - iOffsetX;
		INT iY = pRndViewport->getY() - iOffsetY;

		if (pRndViewport->getStyle()->getType() == CRenderViewport::ST_NORMAL)
		{
			// can apply mask filter on the view port area. this feature is stronger than ST_TRANSPARENT style.
		}
		else if (pRndViewport->getStyle()->getType() == CRenderViewport::ST_TRANSPARENT)
		{
			if (pbHasContent != NULL)
			{
				*pbHasContent = TRUE;
			}

			pDestDib->setColor(	iX, iY, (INT)(pRndViewport->getWidth() * pRndGroup->getScaleWidth()), (INT)(pRndViewport->getHeight() * pRndGroup->getScaleHeight()), 0);
		}
		else if (pRndViewport->getStyle()->getType() == CRenderViewport::ST_CAPTURE)
		{
			if (pbHasContent != NULL)
			{
				*pbHasContent = TRUE;
			}

			CDIB dib;
			dib.copyDIBFrom(pDestDib,iX, iY, pRndViewport->getWidth(), pRndViewport->getHeight());			

			for (UINT i = 0; i < pRndGroup->getFilterCount(); i++)
			{
				CFilterCenter::doFilter(dib, *pDestDib, iX, iY, pRndGroup->getFilterAt(i));
			}

			//pDestDib->setColor(iX, iY, (INT)(dib.getWidth() * pRndGroup->getScaleWidth()), (INT)(dib.getHeight() * pRndGroup->getScaleHeight()), 0);

			BOOLEAN bHasScale = !Math::isZero(pRndGroup->getScaleWidth() - 1.0) || !Math::isZero(pRndGroup->getScaleHeight() - 1.0);

			//pDestDib->setMergeState(CDIB::MS_ALPHA_PAINT);

			if ( bHasScale )
			{
				pDestDib->merge32DIB(	iX, iY, 
										(INT)(dib.getWidth() * pRndGroup->getScaleWidth()), 
										(INT)(dib.getHeight() * pRndGroup->getScaleHeight()),
										dib.getPixelData(), dib.getWidth(), dib.getHeight(), pRndGroup->getAlpha(), 
										CDIB::SL_FILL);
			}
			else
			{
				pDestDib->merge32DIB(	iX, iY, 
										dib.getWidth(), dib.getHeight(),
										dib.getPixelData(), dib.getWidth(), dib.getHeight(), pRndGroup->getAlpha(),
										CDIB::HL_LEFT, CDIB::VL_TOP);
			}

			//pDestDib->setMergeState(CDIB::MS_ALPHA_BLEND);
		}
	}

	void CRenderEntry::drawRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine)
	{
		CHECK_CAN_DO

		m_dib.drawRectangle(iX, iY, uiWidth, uiHeight, uiColor, bDashLine);
	}

	void CRenderEntry::drawTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine)
	{
		CHECK_CAN_DO

		m_dib.drawTriangle(p1, p2, p3, uiColor, bDashLine);
	}

	void CRenderEntry::drawLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine)
	{
		CHECK_CAN_DO

		m_dib.drawLine(iX1, iY1, iX2, iY2, uiColor, bDashLine);
	}

	void CRenderEntry::drawCircle(INT iX, INT iY, UINT iRadius, UINT uiColor, BOOLEAN bDashLine)
	{
		CHECK_CAN_DO

		m_dib.drawCircle(iX, iY, iRadius, uiColor, bDashLine);
	}	

	void CRenderEntry::fillRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor)
	{
		CHECK_CAN_DO

		m_dib.fillRectangle(iX, iY, uiWidth, uiHeight, uiColor);
	}

	void CRenderEntry::endDraw()
	{
		CHECK_CAN_DO

		const tagScreenContext *pScreenContext = m_bMultiSession ? &m_screenContext : CScreenTouchServer::getInstance()->getScreenContext();
		tagRenderContext *pRenderContext = m_bMultiSession ? &m_renderContext : CRenderEngineOS::getInstance()->getRenderContext();

#ifdef OS_WIN
		::SetDIBitsToDevice(pScreenContext->hDC, 
							0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight, 
							0, 0, 0, pScreenContext->uiScreenHeight, 
							m_dib.getPixelData(), 
							(LPBITMAPINFO)m_dib.getDIBInfo(),
							DIB_RGB_COLORS);
#endif

#ifdef OS_QNX
	#ifdef CHIPSET_JACINTO4
		memcpy( pRenderContext->pbtBufData, m_dib.getPixelData(), pScreenContext->uiScreenWidth * pScreenContext->uiScreenHeight * 4 );

		INT iRect[4] = { 0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight };
		screen_post_window(pScreenContext->screen_win, pRenderContext->pBuf, 1, iRect, 0);
	#endif
	
	#ifdef CHIPSET_JACINTO5
		memcpy( pRenderContext->pbtBufData, m_dib.getPixelData(), pScreenContext->uiScreenWidth * pScreenContext->uiScreenHeight * 4 );

		INT iRect[4] = { 0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight };
		screen_post_window(pScreenContext->screen_win, pRenderContext->pBuf, 1, iRect, 0);
	#endif

	#ifdef CHIPSET_IMX6SOLO
		memcpy( pRenderContext->pbtBufData, m_dib.getPixelData(), pScreenContext->uiScreenWidth * pScreenContext->uiScreenHeight * 4 );

		INT iRect[4] = { 0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight };
		screen_post_window(pScreenContext->screen_win, pRenderContext->pBuf, 1, iRect, 0);
	#endif
#endif

#ifdef OS_LINUX
	#ifdef CHIPSET_JACINTO4
		memcpy( pRenderContext->pbtFB, m_dib.getPixelData(), pRenderContext->iScreenSize);
	#endif

	#ifdef CHIPSET_IMX6SOLO
		memcpy( pRenderContext->pbtFB, m_dib.getPixelData(), pRenderContext->iScreenSize);
	#endif

	#ifdef CHIPSET_NBTEVO
		memcpy( pRenderContext->pbtBufData, m_dib.getPixelData(), pScreenContext->uiScreenWidth * pScreenContext->uiScreenHeight * 4 );

		XPutImage(	pScreenContext->pDisplay, pScreenContext->win, pScreenContext->gc, pRenderContext->pImg,
					0, 0, 0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight);
	#endif

	#ifdef CHIPSET_PCIBM
		memcpy( pRenderContext->pbtBufData, m_dib.getPixelData(), pScreenContext->uiScreenWidth * pScreenContext->uiScreenHeight * 4 );

		XPutImage(	pScreenContext->pDisplay, pScreenContext->win, pScreenContext->gc, pRenderContext->pImg,
					0, 0, 0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight);
	#endif
#endif
	}

	void CRenderEntry::swapBuffer()
	{

	}

	void CRenderEntry::reDraw()
	{
		CHECK_CAN_DO

		tagScreenContext *pScreenContext = m_bMultiSession ? &m_screenContext : CRenderEngineOS::getInstance()->getScreenContext();
		tagRenderContext *pRenderContext = m_bMultiSession ? &m_renderContext : CRenderEngineOS::getInstance()->getRenderContext();

#ifdef OS_WIN
		::SetDIBitsToDevice(pScreenContext->hDC, 
							0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight, 
							0, 0, 0, pScreenContext->uiScreenHeight, 
							m_dib.getPixelData(), 
							(LPBITMAPINFO)m_dib.getDIBInfo(),
							DIB_RGB_COLORS);
#endif
	}

	void CRenderEntry::initialize()
	{
		const tagScreenContext *pScreenContet = m_bMultiSession ? &m_screenContext : CScreenTouchServer::getInstance()->getScreenContext();
		tagRenderContext *pRenderContet = m_bMultiSession ? &m_renderContext : CRenderEngineOS::getInstance()->getRenderContext();
		
		m_dib.createDIB(pScreenContet->uiScreenWidth, pScreenContet->uiScreenHeight);

		if (m_bInitialized)
		{
			return;
		}

		if (m_bMultiSession)
		{
			if (CRenderEngineOS::getInstance()->initializeSession(&m_screenContext, &m_renderContext) == FALSE)
			{
				return;
			}
		}
		
		m_bInitialized = TRUE;
	}

	void CRenderEntry::unInitialize()
	{
		if (!m_bInitialized)
		{
			return;
		}

		if (m_bMultiSession)
		{
			CRenderEngineOS::getInstance()->unInitializeSession(&m_screenContext, &m_renderContext);
			m_screenContext.reset();
		}

		m_dib.destroyDIB();		
		
		m_bInitialized = FALSE;
	}

	void CRenderEntry::drawGroupChildren(CRenderGroup *pRndGroup, INT iOffsetX, INT iOffsetY, CDIB * pDestDib, BOOLEAN *pbHasContent)
	{
		for (UINT i = 0; i < pRndGroup->getRenderItemCount(); i++ )
		{
			CRenderItem *pItem =  pRndGroup->getRenderItemAt(i);

			switch (pItem->getType())
			{
			case CRenderItem::RIT_GROUP:
				drawGroup(static_cast<CRenderGroup *>(pItem), iOffsetX, iOffsetY, pDestDib, pbHasContent);
				break;

			case CRenderItem::RIT_IMAGE:
				drawImage(static_cast<CRenderImage *>(pItem), iOffsetX, iOffsetY, pDestDib, pbHasContent);
				break;

			case CRenderItem::RIT_TEXT:
				drawText(static_cast<CRenderText *>(pItem), iOffsetX, iOffsetY, pDestDib, pbHasContent);
				break;

			case CRenderItem::RIT_VIEWPORT:
				drawViewport(static_cast<CRenderViewport *>(pItem), iOffsetX, iOffsetY, pDestDib, pbHasContent);
				break;
			}
		}
	}
}