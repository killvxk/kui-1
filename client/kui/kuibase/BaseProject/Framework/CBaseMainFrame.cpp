#include "OpenHMI.h"
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;

namespace OpenHMI
{
	CBaseMainFrame	*CBaseMainFrame::s_pInstance	= NULL;

	CBaseMainFrame*	CBaseMainFrame::getInstance()
	{
		return CBaseMainFrame::s_pInstance;
	}

	CBaseMainFrame::CBaseMainFrame()
		: m_pOperationCenter(NULL)
		, m_pHomeScreen(NULL)
		, m_bShowBackground(TRUE)
		, m_pScreenToEnter(NULL)
		, m_pScreenToLeave(NULL)
		, m_bDestroyLeaveScreen(FALSE)
		, m_bUseGdi(FALSE)
	{
		CBaseMainFrame::s_pInstance = this;

		setType(CONFIG::CONTAINER_TYPE_SCREEN);

		CContainer::addComponent(&m_screenContainer);
		CContainer::addComponent(&m_overlayContainer);

		m_timerScreen.setListener(this);
	}

	CBaseMainFrame::~CBaseMainFrame()
	{
		;
	}

	BEGIN_EVENT_MAP(CBaseMainFrame)
	END_EVENT_MAP(CContainer)

	void CBaseMainFrame::showBackground(UINT uiIndex)
	{ 
		m_bShowBackground = TRUE;

		if (m_skin.isNotNull())
		{
			m_skin->gotoAndStop(uiIndex);
		}

		setDirty();
	}

	void CBaseMainFrame::hideBackground()
	{ 
		m_bShowBackground = FALSE;

		setDirty();
	}
	
	void CBaseMainFrame::addScreen(CBaseScreen *pScreen)
	{
		innerStopScreenAnimation();

		if (CONFIG::getUseCCP() && ENV::getCCPBindTouch())
		{
			ENV::setTopScreen(pScreen);
		}
			
		pScreen->refresh();
		//pScreen->addCommandListener(m_pOperationCenter);

		if (m_pScreenToLeave == NULL)
		{
			if (m_screenContainer.getComponentCount() > 0)
			{
				m_pScreenToLeave = static_cast<CBaseScreen*>(m_screenContainer.getComponentAt(m_screenContainer.getComponentCount() - 1));
				m_bDestroyLeaveScreen = FALSE;
			}
		}

		m_screenContainer.addComponent(pScreen);
		m_pScreenToEnter = pScreen;
	}

	void CBaseMainFrame::removeScreen(CBaseScreen *pScreen)
	{
		innerStopScreenAnimation();
		
		pScreen->unInitialize();
		
		if (pScreen->getVisible())
		{
			m_pScreenToLeave = pScreen;
			m_bDestroyLeaveScreen = TRUE;
			
			if (m_screenContainer.getComponentPos(pScreen) == (INT)m_screenContainer.getComponentCount() - 1)
			{
				m_pScreenToEnter = static_cast<CBaseScreen*>(m_screenContainer.getComponentAt(m_screenContainer.getComponentPos(pScreen) - 1));
			}
		}
		else
		{
			if (pScreen == m_pScreenToEnter)
			{
				m_pScreenToEnter = static_cast<CBaseScreen*>(m_screenContainer.getComponentAt(m_screenContainer.getComponentPos(pScreen) - 1));
			}
			
			m_screenContainer.removeComponent(pScreen);
			pScreen->destroy();

			m_ptrRubbishScreen = pScreen;
		}
	}

	void CBaseMainFrame::removeScreens(const Array2<UINT> &aryScreenID)
	{
		innerStopScreenAnimation();
		
		for (UINT i = 0; i < aryScreenID.size(); i++)
		{
			CBaseScreen *pScreen = getScreen(aryScreenID[i]);

			if (pScreen != NULL)
			{
				removeScreen(pScreen);
			}
		}
	}

	CBaseScreen* CBaseMainFrame::getScreen(UINT uiID)
	{
		return static_cast<CBaseScreen*>(m_screenContainer.getComponentByID(uiID));
	}

	CBaseScreen* CBaseMainFrame::getTopScreen()
	{
		INT iTop = (INT)m_screenContainer.getComponentCount() - 1;
		while (iTop >= 0)
		{
			CBaseScreen *pScreen = static_cast<CBaseScreen*>(m_screenContainer.getComponentAt(iTop));
			if (pScreen->getVisible() == FALSE)
			{
				iTop--;
			}
			else
			{
				return pScreen;
			}
		}
		
		return NULL;
	}

	void CBaseMainFrame::showHomeScreen()
	{
		CManager_Application::getInstance()->closeAllApplications();
		innerStartScreenAnimation();
	}

	void CBaseMainFrame::switchToApplication(const String &strClassName, const Map2<UINT, UINT, tagNumCompare<UINT> > &mapCommand)
	{
		CManager_Application::getInstance()->closeAllApplications();
		CManager_Application::getInstance()->startApplication(strClassName, mapCommand);
		innerStartScreenAnimation();
	}
	
	void CBaseMainFrame::enterIntoApplication(const String &strClassName, const Map2<UINT, UINT, tagNumCompare<UINT> > &mapCommand)
	{
		CManager_Application::getInstance()->startApplication(strClassName, mapCommand);
	}

	void CBaseMainFrame::addOverlay(CComponent *pComponent)
	{
		m_overlayContainer.addComponent(pComponent);
		
		if (CONFIG::getUseCCP() && ENV::getCCPBindTouch())
		{
			if (pComponent->isContainer())
			{
				ENV::setTopOverlay(static_cast<CContainer*>(pComponent));
			}
			else
			{
				ENV::setTopOverlay(NULL);
			}
		}
	}

	void CBaseMainFrame::removeOverlay(CComponent *pComponent)
	{
		m_overlayContainer.removeComponent(pComponent);

		if (pComponent->isContainer())
		{
			if ((static_cast<CContainer*>(pComponent))->getType() == CONFIG::CONTAINER_TYPE_POPUP)
			{
				m_ptrRubbishPopup = static_cast<CBasePopup*>(pComponent);
			}
		}
		
		if (CONFIG::getUseCCP() && ENV::getCCPBindTouch())
		{
			Array2<CComponent*> aryEmpty;
			pComponent = getTopOverlay(aryEmpty);

			if (pComponent != NULL && pComponent->isContainer())
			{
				ENV::setTopOverlay(static_cast<CContainer*>(pComponent));
			}
			else
			{
				ENV::setTopOverlay(NULL);
			}
		}
	}

	CComponent* CBaseMainFrame::getOverlay(UINT uiID)
	{
		return m_overlayContainer.getComponentByID(uiID);
	}

	CComponent* CBaseMainFrame::getTopOverlay(const Array2<CComponent*> &aryExcludedCompont)
	{
		INT iTop = (INT)m_overlayContainer.getComponentCount() - 1;
		while (iTop >= 0)
		{
			CComponent* pComponent = m_overlayContainer.getComponentAt(iTop);
			if (pComponent->getVisible() == FALSE || aryExcludedCompont.isExist(pComponent))
			{
				iTop--;
			}
			else
			{
				return pComponent;
			}
		}
		
		return NULL;
	}

	void CBaseMainFrame::showFreeControl(CComponent *pComponent, CBaseMainFrame::EFreeControlPos ePos)
	{
		hideFreeControl(pComponent);

		if (ePos == FCP_BELOW_SCREEN_STACK)
		{
			insertComponent(pComponent, getComponentPos(&m_screenContainer));
		}
		else if (ePos == FCP_BETWEEN_SCREEN_STACK_AND_OVERLAY_STACK)
		{
			insertComponent(pComponent, getComponentPos(&m_overlayContainer));
		}
		else if (ePos == FCP_AMONG_OVERLAY_STACK)
		{
			m_overlayContainer.addComponent(pComponent);
		}
		else if (ePos == FCP_ABOVE_OVERLAY_STACK)
		{
			addComponent(pComponent);
		}
	}

	void CBaseMainFrame::hideFreeControl(CComponent *pComponent)
	{
		if (getComponentPos(pComponent) >= 0)
		{
			removeComponent(pComponent);
		}
		else
		{
			if (m_overlayContainer.getComponentPos(pComponent) >= 0)
			{
				m_overlayContainer.removeComponent(pComponent);
			}
		}
	}

	CWgtMessageBox* CBaseMainFrame::showMessageBox(	UINT								uiID,
													CWgtMessageBox::tagMessageBoxParam	tagParam)
	{
		CWgtMessageBox* pMsg = new CWgtMessageBox();
		addOverlay(pMsg);

		INIT::initMessageBox(pMsg, uiID);

		pMsg->setParam(tagParam.strTitle,
						tagParam.strContent,
						tagParam.strOk,
						tagParam.strCancel,
						tagParam.strAlternate,
						tagParam.iBeepOk,
						tagParam.iBeepCancel,
						tagParam.iBeepAlternate);
		/*pMsg->setShowAnimationInfo( CWgtMessageBox::AS_ALFA | 
									CWgtMessageBox::AS_ROTATE |
									CWgtMessageBox::AS_SCALE,
									50,100,400,240,0,0.0f,5,TRUE);*/
		pMsg->setShowAnimationInfo(tagParam.uiAnimationStyle,
									tagParam.uiIntervalTime,
									tagParam.uiRepeatCount,
									tagParam.iStartX,
									tagParam.iStartY,
									tagParam.uiStartOpacity,
									tagParam.fStartScale,
									tagParam.uiCircleCnt,
									tagParam.bNeedDestroyAnimation);
		pMsg->doModal(tagParam.onOk, tagParam.pOkParam, tagParam.onCancel, tagParam.pCancelParam, tagParam.onAlternate, tagParam.pAlternateParam, onMessageBoxDestroy, NULL, tagParam.iAutoDisappearSecond);

		if (CONFIG::getUseCCP())
		{
			pMsg->setFocus();
		}

		return pMsg;
	}

	CRenderGroup* CBaseMainFrame::buildRenderTree()
	{
		buildRenderTree(m_renderGroup);

		m_renderGroup.checkChildrenChanged();
		m_renderGroup.updateCacheFlagStep1();
		m_renderGroup.updateCacheFlagStep2();

		return &m_renderGroup;
	}

	void CBaseMainFrame::breakUpRenderTree()
	{
		m_renderGroup.breakUp();
	}

	void CBaseMainFrame::refresh()
	{
		pushDataToUI();

		CContainer::refresh();
	}

	void CBaseMainFrame::destroy()
	{
		CScreenTouchManager::getInstance()->removeCanvas(this);
		CScreenTouchServer::getInstance()->setListener(NULL);

		m_timerScreen.setListener(NULL);
		m_timerScreen.stop();

		ENV::setMovieClipGenerator(NULL);
		ENV::setSoundGenerator(NULL);

		m_pScreenToEnter = NULL;
		m_pScreenToLeave = NULL;		

		if (m_pHomeScreen != NULL)
		{	
			m_pHomeScreen->unInitialize();
			// delete home in m_screenContainer
			m_pHomeScreen = NULL;
		}		

		if (m_pOperationCenter != NULL)
		{
			m_pOperationCenter->unInitialize();
			delete m_pOperationCenter;
			m_pOperationCenter = NULL;
		}

		m_ptrRubbishScreen = NULL;
		m_ptrRubbishPopup = NULL;

		for (UINT i = 0; i < m_screenContainer.getComponentCount(); i++)
		{
			delete m_screenContainer.getComponentAt(i);
		}
		m_screenContainer.removeAllComponents();
		m_overlayContainer.removeAllComponents();

		CContainer::destroy();

		CManager_Application::getInstance()->unInitialize();
		CManager_Logic::getInstance()->unInitialize();
		CManager_Service::getInstance()->unInitialize();
		CManager_Skin::getInstance()->unInitialize();
		CManager_Language::getInstance()->unInitialize();

		delete s_pInstance;
		s_pInstance = NULL;
	}

	void CBaseMainFrame::notifySkinFirstPrepared()
	{
		refresh();

		onSkinResourceReady();

		CMain::getInstance()->paintHMI();
	}

	void CBaseMainFrame::notifySkinChanged()
	{
		destroySkin();
		refresh();

		CMain::getInstance()->paintHMI();
	}

	void CBaseMainFrame::innerStartScreenAnimation()
	{
		startScreenAnimation(m_pScreenToLeave, m_pScreenToEnter, 0, 0);
	}

	void CBaseMainFrame::innerStopScreenAnimation()
	{
		if (m_timerScreen.isRunning())
		{
			m_timerScreen.stop();

			completeScreenAnimation(m_pScreenToLeave, m_pScreenToEnter);				
		}
	}	

	void CBaseMainFrame::onTimer(const CTimer *pTimer, UINT uiCurrentCount)
	{
		doScreenAnimation(m_pScreenToLeave, m_pScreenToEnter, uiCurrentCount);
	}

	void CBaseMainFrame::onTimerComplete(const CTimer *pTimer)
	{
		m_timerScreen.stop();

		completeScreenAnimation(m_pScreenToLeave, m_pScreenToEnter);
	}

	void CBaseMainFrame::registerOperationCenter(CBaseOperationCenter *pOperationCenter)
	{
		m_pOperationCenter = pOperationCenter;
	}

	void CBaseMainFrame::registerHomeScreen(CBaseScreen *pHomeScreen)
	{
		if (pHomeScreen != NULL)
		{
			m_pHomeScreen = pHomeScreen;

			if (CONFIG::getUseCCP() && ENV::getCCPBindTouch())
			{
				ENV::setTopScreen(m_pHomeScreen);
			}

			m_screenContainer.addComponent(m_pHomeScreen);
			m_pScreenToEnter = m_pHomeScreen;
		}
	}

	void CBaseMainFrame::startScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter, UINT uiIntervalTime, UINT uiRepeatCount)
	{
		if (pScreenToLeave != NULL )
		{
			pScreenToLeave->lock();
		}
		
		if (pScreenToEnter != NULL)
		{			
			if (!pScreenToEnter->getVisible())
			{
				pScreenToEnter->setVisible(TRUE);
			}

			if (CONFIG::getUseCCP())
			{
				if (pScreenToEnter->hasFocus() == FALSE)
				{
					pScreenToEnter->setFocus();
				}
			}

			pScreenToEnter->lock();
		}
		
		if (uiIntervalTime > 0 && uiRepeatCount > 0)
		{
			m_timerScreen.stop();
			m_timerScreen.setDelay(uiIntervalTime);
			m_timerScreen.setRepeatCount(uiRepeatCount);
			m_timerScreen.start();
		}
		else
		{
			completeScreenAnimation(pScreenToLeave, pScreenToEnter);
		}
	}

	void CBaseMainFrame::doScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter, UINT uiCurrentCount)
	{
		;
	}

	void CBaseMainFrame::completeScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter)
	{
		if (pScreenToLeave != NULL)
		{
			pScreenToLeave->unlock();
			pScreenToLeave->hide();
			
			if (m_bDestroyLeaveScreen)
			{
				m_screenContainer.removeComponent(pScreenToLeave);
				pScreenToLeave->destroy();

				m_ptrRubbishScreen = pScreenToLeave;
			}
		}
		
		if (pScreenToEnter != NULL )
		{				
			pScreenToEnter->unlock();
			
			if (CONFIG::getUseCCP() && ENV::getCCPBindTouch())
			{
				ENV::setTopScreen( pScreenToEnter );
			}
			
			pScreenToEnter->show();
		}
		
		m_pScreenToEnter = NULL;
		m_pScreenToLeave = NULL;
		m_bDestroyLeaveScreen = FALSE;	
	}
	ULONG_PTR m_gdiplusToken;

	BOOLEAN CBaseMainFrame::initialize()
	{
		// Initialize GDI+.
		GdiplusStartupInput gdiplusStartupInput;

		GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

		CConfigParser::tagScreenInfo*	pTagScreenInfo = CConfigParser::getInstance()->getScreenInfo();
		CConfigParser::tagTouchInfo*	pTagTouchInfo = CConfigParser::getInstance()->getTouchInfo();
		
		//ENV::setScreenOffsetX(pTagScreenInfo->iOffsetX);
		//ENV::setScreenOffsetY(pTagScreenInfo->iOffsetY);
		ENV::setScreenWidth(pTagScreenInfo->uiWidth);
		ENV::setScreenHeight(pTagScreenInfo->uiHeight);	
		ENV::setScreenCaptionRect(CRectangle(pTagScreenInfo->iCaptionX, pTagScreenInfo->iCaptionY, pTagScreenInfo->uiCaptionW, pTagScreenInfo->uiCaptionH));	
		
		//ENV::setHitOffset(pTagTouchInfo->uiHitOffset);
		//ENV::setWaverDistance(pTagTouchInfo->uiWaverDistance);
		//ENV::setMoveWaverDistance(pTagTouchInfo->uiMoveWaverDistance);
		//ENV::setLongPressDelay(pTagTouchInfo->uiLongPressDelay);
		//ENV::setContinuePressInterval(pTagTouchInfo->uiContinuePressInterval);
		//ENV::setDoubleTapInterval(pTagTouchInfo->uiDoubleTapInterval);
		//ENV::setDoubleTapWaverDistance(pTagTouchInfo->uiDoubleTapWaverDistance);
		//ENV::setContinueTapInterval(pTagTouchInfo->uiContinueTapInterval);
		
		ENV::setMovieClipGenerator(CManager_Skin::getInstance());
		
		setPreferredSize(pTagScreenInfo->uiWidth, pTagScreenInfo->uiHeight);
		
		CScreenTouchServer::getInstance()->setScreenSize(pTagScreenInfo->uiWidth, pTagScreenInfo->uiHeight);
		CScreenTouchServer::getInstance()->setScreenName(pTagScreenInfo->strName);
		CScreenTouchServer::getInstance()->setListener(CScreenTouchManager::getInstance());	
		CScreenTouchManager::getInstance()->addCanvas(this);

		if (CScreenTouchServer::getInstance()->initialize() == FALSE)
		{
			CLog::error2(MODULE_NAME, L"Screen touch server fail to initialize.");

			return FALSE;
		}

		OSVERSIONINFOEX osvi;
		BOOL bOsVersionInfoEx;

		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);

		// 检测是否win7的系统版本为NT6.1
		if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion >= 6 && osvi.dwMinorVersion >= 1)
		{
			m_bUseGdi = FALSE;
			if (!CRenderEngine::getInstance()->registernRenderEngine(L"./kopengles.dll"))
			{
				String strInfo = String::format(L"运行失败，%s文件不存在", L"kopengles.dll");
				::MessageBox(0, (const CHAR*)strInfo, _T("错误"), MB_OK | MB_ICONEXCLAMATION);
				return FALSE;
			}

			if (CRenderEngine::getInstance()->initialize(CScreenTouchServer::getInstance()->getScreenContext()) == FALSE)
			{
				m_bUseGdi = TRUE;
				CRenderEngine::getInstance()->freeLibrary();
				if (!CRenderEngine::getInstance()->registernRenderEngine(L"./kgdiplus.dll"))
				{
					String strInfo = String::format(L"运行失败，%s文件不存在", L"kgdiplus.dll");
					::MessageBox(0, (const CHAR*)strInfo, _T("错误"), MB_OK | MB_ICONEXCLAMATION);
					return FALSE;
				}

				if (CRenderEngine::getInstance()->initialize(CScreenTouchServer::getInstance()->getScreenContext()) == FALSE)
				{
					return FALSE;
				}
			}
		}
		else
		{
			m_bUseGdi = TRUE;
			if (!CRenderEngine::getInstance()->registernRenderEngine(L"./kgdiplus.dll"))
			{
				String strInfo = String::format(L"运行失败，%s文件不存在", L"kgdiplus.dll");
				::MessageBox(0, (const CHAR*)strInfo, _T("错误"), MB_OK | MB_ICONEXCLAMATION);
				return FALSE;
			}

			if (CRenderEngine::getInstance()->initialize(CScreenTouchServer::getInstance()->getScreenContext()) == FALSE)
			{
				return FALSE;
			}
		}

		CFontParser::getInstance()->parse(CConfigParser::getInstance()->getSkinPath() + CConfigParser::getInstance()->getFontLibPath());
		if (CFontParser::getInstance()->getFontCount() == 0)
		{
			CLog::warning2(MODULE_NAME, L"There is no font file.");
			//return FALSE;
		}

		CManager_Language::getInstance()->parseConfig();
		CManager_Skin::getInstance()->parseConfig();
		
		CManager_Language::getInstance()->initialize();
		CManager_Skin::getInstance()->initialize();
		//CManager_Service::getInstance()->initialize();
		CManager_Logic::getInstance()->initialize();
		//CManager_Application::getInstance()->initialize();

		if (CManager_Skin::getInstance()->getSkinCount() == 0)
		{
			CLog::error2(MODULE_NAME, L"There is no skin resource.");
			return FALSE;
		}
		
		if (m_pOperationCenter != NULL)
		{
			m_pOperationCenter->initialize();
		}
		if (m_pHomeScreen != NULL)
		{
			m_pHomeScreen->initialize();
		}	

		return TRUE;
	}

	void CBaseMainFrame::unInitialize()
	{
		destroy();

		if (m_gdiplusToken != NULL)
		{
			GdiplusShutdown(m_gdiplusToken);
		}
	}

	void CBaseMainFrame::buildRenderTree(CRenderGroup &renderGroup)
	{
		CPoint pt = localToGlobal(0, 0);
		m_renderGroup.setX(pt.getX());
		m_renderGroup.setY(pt.getY());
		m_renderGroup.setWidth(getPreferredSize().getX());
		m_renderGroup.setHeight(getPreferredSize().getY());

		if (m_bShowBackground)
		{
			m_renderImage.setIsBackGround(TRUE);
			buildRenderTreeFromBackground(m_renderGroup);
		}

		buildRenderTreeFromChildren(m_renderGroup);
	}

	void CBaseMainFrame::onMessageBoxDestroy(Object* pParam)
	{
		CBaseMainFrame::getInstance()->removeOverlay((CWgtMessageBox*)pParam);
	}
}