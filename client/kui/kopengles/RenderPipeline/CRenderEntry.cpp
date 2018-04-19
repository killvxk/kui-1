#include "RenderEngine.h"

namespace OpenHMI
{
#define CHECK_CAN_DO	\
	if (!m_bInitialized)\
	{\
	return;\
	}

	const static String STROPENARTISTLINEMTL = L"OpenArtistLineMtl";
	const static String STROPENARTISTVBOMESH = L"OpenArtistVBOMesh";
	static	UINT	g_uID = 0;

	CRenderEntry::CRenderEntry()
		: m_bInitialized(FALSE)
		, m_uiSessionID(0)
		, m_bMultiSession(FALSE)
		, m_pCurBatchRenderGroup(NULL)
		, m_pool2DBatchRenderGroup(20)
		, m_pool3DBatchRenderGroup(20)
		, m_p2DRenderUtility(NULL)
		, m_bDIBDirty(TRUE)
	{
		g_uID++;
		m_uiSessionID = g_uID;

		m_p2DRenderUtility = new C2DRenderUtility(*this);
	}

	CRenderEntry::~CRenderEntry()
	{
		unInitialize();

		if( m_p2DRenderUtility )
		{
			delete m_p2DRenderUtility;
			m_p2DRenderUtility = NULL;
		}
	}

	void CRenderEntry::beginDraw(const tagScreenContext *pScreenContext)
	{
		if (pScreenContext != NULL)
		{
			m_bMultiSession = TRUE;
		}

		if (m_bMultiSession)
		{
			//if (m_screenContext!= *pScreenContext )
			//{
			//	unInitialize();

				m_screenContext = *pScreenContext;
			//}
		}

		const tagScreenContext *pCurScreenContext = m_bMultiSession ? &m_screenContext : CScreenTouchServer::getInstance()->getScreenContext();
		tagRenderContext *pCurRenderContext = m_bMultiSession ? &m_renderContext : CRenderEngineOS::getInstance()->getRenderContext();

		initialize();
	
		if ( m_bMultiSession &&
			!eglMakeCurrent(pCurRenderContext->eglDisplay, pCurRenderContext->eglSurface, pCurRenderContext->eglSurface, pCurRenderContext->eglContext))
		{
			INT er = eglGetError();
			CLog::error2(RENDER_ENGINE_NAME, L"EGL make current error.");
			return;
		}

		g_pRenderContext = pCurRenderContext;

		m_batchRenderGroup.clear();
		m_batchRenderGroup.setX(0);
		m_batchRenderGroup.setY(0);
		m_batchRenderGroup.setWidth(pCurScreenContext->uiScreenWidth);
		m_batchRenderGroup.setHeight(pCurScreenContext->uiScreenHeight);
		m_batchRenderGroup.flush();
		m_batchRenderGroup.setOwner(this);
		setCurrentBatchRenderGroup(&m_batchRenderGroup);
	}	

	void CRenderEntry::drawGroup(CRenderGroup *pRndGroup, INT iOffsetX, INT iOffsetY, CDIB * pDestDib)
	{
		CHECK_CAN_DO

		if (pRndGroup == NULL || m_pCurBatchRenderGroup == NULL ) 
		{
			return;
		}

		C2DBatchRenderGroup* pCurBRG = getCurrent2DBatchRenderGroup();
		BOOLEAN bHasFilter = ( pRndGroup->getFilterCount() > 0 );
	
		if ( isNeedCacheRenderGroup(pRndGroup) || bHasFilter )
		{
			CRenderGroupCacheData *pCacheData = (CRenderGroupCacheData *)pRndGroup->getCacheData();
			if ( pCacheData == NULL )
			{
				pCacheData = new CRenderGroupCacheData(pRndGroup);
				pRndGroup->setCacheData(pCacheData);
			}
			pCacheData->flush( pCurBRG );

			C2DBatchRenderGroup* pBRG = pCacheData->getBatchRenderGroup();
			pBRG->setParent(pCurBRG);
			
			if( pCacheData->getNeedUpdate() || pRndGroup->isRenderViewportWrapper() || !pRndGroup->getCanMakeCache() )
			{			
				
				setCurrentBatchRenderGroup(pBRG);
				drawGroupChildren(pRndGroup);
				setCurrentBatchRenderGroup(pCurBRG);
				if ( !pBRG->isBlank() )
				{
					m_aryBatchRenderGroup.addItem(CBatchRenderGroupPtr(pBRG));
				}
			}
			pCacheData->resetState();

			if( !pBRG->isBlank() )
			{
				if( pRndGroup->getFilterCount() > 0 )
				{
					process2Dfilter(pRndGroup, pCurBRG);
				}
				else
				{
					C2DBatchRender* pBatchRender = pCurBRG->getFreeBatchRender();
					pCurBRG->add(pBatchRender);
					pBatchRender->addRenderItem(pRndGroup, TRUE);
				}
			}
		}
		else
		{
			drawGroupChildren(pRndGroup);
		}

		if (CONFIG::getOutputPerformance())
		{
			if( pRndGroup->getParent() == NULL )
			{
				CLog::debug2(RENDER_ENGINE_NAME,L" ");
				CLog::debug2(RENDER_ENGINE_NAME,L"Begin print RenderTree");

				if (CONFIG::getOutputDebugLog())
				{
					printRenderTree(pRndGroup, String(L"  "));
				}

				CLog::debug2(RENDER_ENGINE_NAME,L"/End print RenderTree");
				CLog::debug2(RENDER_ENGINE_NAME,L" ");
			}
		}
	}

	void CRenderEntry::drawText(CRenderText *pRndText)
	{
		CHECK_CAN_DO

		C2DBatchRenderGroup* pCurBRG = getCurrent2DBatchRenderGroup();
		m_p2DRenderUtility->drawText(pRndText,pCurBRG);
	}

	void CRenderEntry::drawImage(CRenderImage *pRndImage)
	{
		CHECK_CAN_DO

		C2DBatchRenderGroup* pCurBRG = getCurrent2DBatchRenderGroup();
		m_p2DRenderUtility->drawImage(pRndImage,pCurBRG);
	}

	void CRenderEntry::drawViewport(CRenderViewport* pRndViewport)
	{
		CHECK_CAN_DO

		C2DBatchRenderGroup* pCurBRG = getCurrent2DBatchRenderGroup();
		m_p2DRenderUtility->drawViewport(pRndViewport,pCurBRG, 0);
	}

	void CRenderEntry::drawRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine)
	{
		CHECK_CAN_DO

		C2DBatchRenderGroup* pCurBRG = getCurrent2DBatchRenderGroup();
		m_p2DRenderUtility->drawRectangle(iX, iY, uiWidth, uiHeight, uiColor, bDashLine,pCurBRG);
		
	}

	void CRenderEntry::drawTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine)
	{
		CHECK_CAN_DO

		C2DBatchRenderGroup* pCurBRG = getCurrent2DBatchRenderGroup();
		m_p2DRenderUtility->drawTriangle(p1, p2, p3, uiColor, bDashLine,pCurBRG);
	}

	void CRenderEntry::drawLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine)
	{
		CHECK_CAN_DO

		C2DBatchRenderGroup* pCurBRG = getCurrent2DBatchRenderGroup();
		m_p2DRenderUtility->drawLine(iX1, iY1, iX2, iY2, uiColor, bDashLine,pCurBRG);
	}

	void CRenderEntry::drawCircle(INT iX, INT iY, UINT uiRadius, UINT uiColor, BOOLEAN bDashLine )
	{
		CHECK_CAN_DO

		C2DBatchRenderGroup* pCurBRG = getCurrent2DBatchRenderGroup();
		m_p2DRenderUtility->drawCircle(iX, iY, uiRadius, uiColor, bDashLine, pCurBRG);
	}

	void CRenderEntry::fillRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor)
	{
		CHECK_CAN_DO

		C2DBatchRenderGroup* pCurBRG = getCurrent2DBatchRenderGroup();
		m_p2DRenderUtility->fillRectangle(iX, iY, uiWidth, uiHeight, uiColor,pCurBRG);
	}

	void CRenderEntry::endDraw()
	{
		CHECK_CAN_DO

		const tagScreenContext *pScreenContext = m_bMultiSession ? &m_screenContext : CScreenTouchServer::getInstance()->getScreenContext();
		tagRenderContext *pRenderContext = m_bMultiSession ? &m_renderContext : CRenderEngineOS::getInstance()->getRenderContext();

		// traverse batchrendergroup array
		UINT cnt = m_aryBatchRenderGroup.size();
		for ( UINT i = 0; i < cnt; ++i )
		{
			CBatchRenderGroup* pBatchRenderGroup = m_aryBatchRenderGroup[i];
			CRenderTarget* pRT = pBatchRenderGroup->getRenderTarget();
			if ( pRT )
			{
				pRT->bind();
				pRT->clear(0);
			}

			pBatchRenderGroup->render();

			if ( pRT )
			{
				pRT->unBind();
			}
			pBatchRenderGroup->clear();
		}

		m_aryBatchRenderGroup.clear();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear( GL_COLOR_BUFFER_BIT );

		glViewport(0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight);

		m_batchRenderGroup.render();

		if (eglSwapBuffers(pRenderContext->eglDisplay, pRenderContext->eglSurface) != EGL_TRUE )
		{
			//::MessageBox(0, _T("很抱歉，菜单遇到渲染问题产生崩溃，请先关闭引起奔溃的程序，再重启游戏菜单!"), _T("错误"), MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
			CMain::getInstance()->quitHMI();
		}
		
		m_bDIBDirty = TRUE;
	}

	void CRenderEntry::swapBuffer()
	{
		const tagScreenContext *pScreenContext = m_bMultiSession ? &m_screenContext : CScreenTouchServer::getInstance()->getScreenContext();
		tagRenderContext *pRenderContext = m_bMultiSession ? &m_renderContext : CRenderEngineOS::getInstance()->getRenderContext();

		eglMakeCurrent(pRenderContext->eglDisplay, pRenderContext->eglSurface, pRenderContext->eglSurface, pRenderContext->eglContext);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight);
		eglSwapBuffers(pRenderContext->eglDisplay, pRenderContext->eglSurface);
	}

	void CRenderEntry::reDraw()
	{  
#ifdef OS_WIN
		tagScreenContext *pScreenContext = m_bMultiSession ? &m_screenContext : CRenderEngineOS::getInstance()->getScreenContext();
		tagRenderContext *pRenderContext = m_bMultiSession ? &m_renderContext : CRenderEngineOS::getInstance()->getRenderContext();

		if (m_bDIBDirty)
		{
	
			if ( m_bMultiSession &&
				!eglMakeCurrent(pRenderContext->eglDisplay, pRenderContext->eglSurface, pRenderContext->eglSurface, pRenderContext->eglContext))
			{
				INT er = eglGetError();
				CLog::error2(RENDER_ENGINE_NAME, L"EGL make current error.");
				return;
			}

			g_pRenderContext = pRenderContext;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(0.0f,0.0f,0.0f,0.0f);
			glClear( GL_COLOR_BUFFER_BIT );


			glViewport(0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight);

			m_batchRenderGroup.render();
			
			m_dib.createDIB(pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight);
			glReadPixels(0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight, GL_RGBA, GL_UNSIGNED_BYTE, (void*)m_dib.getPixelData());
			m_dib.convertRGBToBGR();	
			m_bDIBDirty = FALSE;
			m_batchRenderGroup.clear();
		}

		::SetDIBitsToDevice(pScreenContext->hDC, 
			0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight,
			0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight,
						m_dib.getPixelData(), 
						(LPBITMAPINFO)m_dib.getDIBInfo(),
						DIB_RGB_COLORS);
#endif
		
	}

	BOOLEAN	CRenderEntry::isNeedCacheRenderGroup(CRenderGroup* pRndGroup) const
	{
		if ( pRndGroup && (    pRndGroup->getNeedClip() 
							|| pRndGroup->getAlpha() != 255
							|| Math::isZero(pRndGroup->getScaleWidth() - 1.0000f) == FALSE 
							|| Math::isZero(pRndGroup->getScaleHeight() - 1.0000f) == FALSE
							|| Math::isZero(pRndGroup->getRotateRadian()) == FALSE )
							|| (pRndGroup->getHasViewport() && pRndGroup->getParent()) )
		{
			return TRUE;
		}

		return FALSE;
	}

	C2DBatchRenderGroup* CRenderEntry::getCurrent2DBatchRenderGroup()
	{
		return (C2DBatchRenderGroup*)(m_pCurBatchRenderGroup);
	}

	C2DBatchRenderGroup* CRenderEntry::getFree2DBatchRenderGroup()
	{
		C2DBatchRenderGroup* p = m_pool2DBatchRenderGroup.get();
		p->setOwner(this);
		return p;
	}

	void CRenderEntry::free2DBatchRenderGroup(C2DBatchRenderGroup* p)
	{
		p->reset();
		m_pool2DBatchRenderGroup.release(p);
	}

	C3DBatchRenderGroup* CRenderEntry::getFree3DBatchRenderGroup()
	{
		C3DBatchRenderGroup* p = m_pool3DBatchRenderGroup.get();
		p->setOwner(this);
		return p;
	}

	void CRenderEntry::free3DBatchRenderGroup(C3DBatchRenderGroup* p)
	{
		p->reset();
		m_pool3DBatchRenderGroup.release(p);
	}

	void CRenderEntry::addBatchRenderGroup(CBatchRenderGroup* p)
	{
		m_aryBatchRenderGroup.addItem(CBatchRenderGroupPtr(p));
	}

	void CRenderEntry::initialize()
	{
		const tagScreenContext *pScreenContext = m_bMultiSession ? &m_screenContext : CScreenTouchServer::getInstance()->getScreenContext();
		tagRenderContext *pRenderContext = m_bMultiSession ? &m_renderContext : CRenderEngineOS::getInstance()->getRenderContext();

		RECT rt;
		CScreenTouchServer *pServer = CScreenTouchServer::getInstance();
		::GetWindowRect(pServer->getScreenContext()->hWnd, &rt);

		setClipRect(CRectangle(0, 0, pScreenContext->uiScreenWidth, pScreenContext->uiScreenHeight));

		if (m_bInitialized)
		{
			g_uiSessionID = m_uiSessionID;
			return;
		}

		if (m_bMultiSession)
		{
			if (CRenderEngineOS::getInstance()->initializeSession(&m_screenContext, &m_renderContext) == FALSE)
			{
				return;
			}
		}

		g_uiSessionID = m_uiSessionID;
		g_pRenderContext = pRenderContext;

		if( m_p2DRenderUtility )
		{
			//m_p2DRenderUtility->initialize();
		}
		
		m_bInitialized = TRUE;
	}

	void CRenderEntry::unInitialize()
	{
		if (!m_bInitialized)
		{
			return;
		}

		if( m_p2DRenderUtility )
		{
			m_p2DRenderUtility->unInitialize();
		}

		m_batchRenderGroup.clear();

		if (m_bMultiSession)
		{
			CRenderEngineOS::getInstance()->unInitializeSession(&m_screenContext, &m_renderContext);
			m_screenContext.reset();
		}

		g_pRenderContext = NULL;
		m_aryBatchRenderGroup.clear();
		m_pool2DBatchRenderGroup.release();
		m_pool3DBatchRenderGroup.release();

		m_bInitialized = FALSE;
	}

	void CRenderEntry::drawGroupChildren(CRenderGroup* pRndGroup)
	{
		C2DBatchRenderGroup* pCurBRG = getCurrent2DBatchRenderGroup();
		C2DBatchRenderGroup* pParent = (C2DBatchRenderGroup*)pCurBRG->getParent();
		UINT uiViewportSiblingCnt = 0;
		if( pParent )
		{
			uiViewportSiblingCnt = pParent->getCount();
		}

		UINT uiItemCnt = pRndGroup->getRenderItemCount();
		for (UINT i = 0; i < uiItemCnt; ++i )
		{
			CRenderItem *pItem = pRndGroup->getRenderItemAt(i);

			if ( pItem->getType() == CRenderItem::RIT_GROUP )
			{
				CRenderGroup *pGroup = static_cast<CRenderGroup *>(pItem);
				drawGroup(pGroup, 0, 0, NULL);
			}
			else if ( pItem->getType() == CRenderItem::RIT_IMAGE )
			{
				m_p2DRenderUtility->drawImage(static_cast<CRenderImage *>(pItem), pCurBRG);
			}
			else if  ( pItem->getType() == CRenderItem::RIT_TEXT) 
			{
				m_p2DRenderUtility->drawText(static_cast<CRenderText *>(pItem), pCurBRG);
			}
			else if ( pItem->getType() == CRenderItem::RIT_VIEWPORT)
			{
				m_p2DRenderUtility->drawViewport(static_cast<CRenderViewport *>(pItem),pCurBRG,uiViewportSiblingCnt);
			}
		}
	}

	void CRenderEntry::process2Dfilter(CRenderGroup* pRndGroup, C2DBatchRenderGroup* pCurrentBRG)
	{
		CRenderGroupCacheData *pCacheData = (CRenderGroupCacheData *)pRndGroup->getCacheData();
		C2DBatchRenderGroup* pBRG = pCacheData->getBatchRenderGroup();

		C2DBatchRenderGroup* pIn = pBRG;
		C2DBatchRenderGroup* pOut = NULL;
		Array2<C2DBatchRenderGroup*> aryOut;

		UINT uifilterCnt = pCacheData->getFilterCount();
		for (UINT j = 0; j < uifilterCnt; j++)
		{
			CBaseFilter* pFilter = pCacheData->getFilterAt(j);
			
			pFilter->doFilter(pIn, pCurrentBRG, aryOut);

			UINT cnt = aryOut.size();
			if ( cnt > 0 )
			{
				pOut = aryOut[cnt-1];
			}

			pIn = (pOut == NULL) ? pBRG : pOut;
			pOut = NULL;
		}

		UINT cnt = aryOut.size();
		for ( UINT i = 0; i < cnt; ++i )
		{
			CBatchRenderGroup* pBatchRenderGroup = aryOut[i];
			m_aryBatchRenderGroup.addItem(CBatchRenderGroupPtr(pBatchRenderGroup));	
		}
	}

	void CRenderEntry::printRenderTree(CRenderGroup* pRndGroup, const String& str)
	{
		UINT uiItemCnt = pRndGroup->getRenderItemCount();
		for (UINT i = 0; i < uiItemCnt; ++i )
		{
			CRenderItem *pItem = pRndGroup->getRenderItemAt(i);

			if ( pItem->getType() == CRenderItem::RIT_GROUP )
			{
				CLog::debug2(String(RENDER_ENGINE_NAME),str + L"BeginGroup");
				CRenderGroup *pGroup = static_cast<CRenderGroup *>(pItem);

				if (CONFIG::getOutputDebugLog())
				{
					printRenderTree(pGroup,str + L"  ");
				}

				CLog::debug2(String(RENDER_ENGINE_NAME),str + L"/EndGroup");
			}
			else if ( pItem->getType() == CRenderItem::RIT_IMAGE )
			{
				CLog::debug2(String(RENDER_ENGINE_NAME),str + L"image");
			}
			else if  ( pItem->getType() == CRenderItem::RIT_TEXT) 
			{
				CLog::debug2(String(RENDER_ENGINE_NAME),str + L"text");
			}
			else if ( pItem->getType() == CRenderItem::RIT_VIEWPORT)
			{
				CLog::debug2(String(RENDER_ENGINE_NAME),str + L"viewport");
			}
		}
		
	}
}