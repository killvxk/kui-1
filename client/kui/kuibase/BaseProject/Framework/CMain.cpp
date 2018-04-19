#include "OpenHMI.h"

namespace OpenHMI
{
	CMain	*CMain::s_pInstance	= NULL;

	CMain*	CMain::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CMain();
		}

		return s_pInstance;
	}

	CMain::CMain() 
		: m_uiActiveMessageList(4)
	{
		;
	}

	CMain::~CMain()
	{
		CManager_Application::deleteInstance();
		CManager_Logic::deleteInstance();
		CManager_Service::deleteInstance();
		CManager_Skin::deleteInstance();
		CManager_Language::deleteInstance();
		CServiceAdaptor::deleteInstance();
		CGestureManager::deleteInstance();
			
		CScreenTouchServer::deleteInstance();
		CScreenTouchManager::deleteInstance();			
		CFontParser::deleteInstance();
		CLanguageParser::deleteInstance();
		CSkinParser::deleteInstance();
		CPluginParser::deleteInstance();
		CRenderEngine::deleteInstance();

		CTimerManager::deleteInstance();
		CConfigParser::deleteInstance();
	}	

	void CMain::startHMI(CBaseMainFrame	*pMainFrame)
	{
		if (m_bRunning)
		{
			return;
		}

		if (pMainFrame == NULL)
		{
			return;
		}

		CLog::info(MODULE_NAME, L"Kernel begin to initialize.");

		HWND hWnd = CScreenTouchServer::getInstance()->getScreenContext()->hWnd;
		if (CConfigParser::getInstance()->parse(L"Config.ini") == FALSE)
		{
			::MessageBox(hWnd, L"无法在皮肤包里找到Conifg.ini", _T("错误"), MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		//setPriority(CConfigParser::getInstance()->getSysInfo()->iMsgQueueThreadPri);
		start();

		m_waitQuit.wait();

		CLog::info(MODULE_NAME, L"HMI quited.");

		delete s_pInstance;
		s_pInstance = NULL;
	}

	void CMain::quitHMI()
	{
		clearMessageList();	

		prepareStop();
	}

	void CMain::paintHMI()
	{
		pushMessage(new CMsgRender());
	}

	void CMain::pushMessage(CBaseMessage *pMessage)
	{
		if (pMessage == NULL)
		{
			return;
		}

		{
			CAutoSync autoSync(m_syncObject);

			switch (pMessage->getType())
			{
			case CBaseMessage::MT_SCREEN_TOUCH:
			case CBaseMessage::MT_HARD_KEY:	
			case CBaseMessage::MT_GESTURE:
			case CBaseMessage::MT_SCREEN:
				m_listInputMessage.addLast(pMessage);
				m_uiActiveMessageList = 1;
				break;

			case CBaseMessage::MT_FROM_SERVICE:
				m_listServiceMessage.addLast(pMessage);
				m_uiActiveMessageList = 2;
				break;

			case CBaseMessage::MT_TIMER:
				m_listTimerMessage.addLast(pMessage);
				m_uiActiveMessageList = 3;
				break;

			case CBaseMessage::MT_RENDER:		
				{
					switch (m_uiActiveMessageList)
					{
					case 1:
						pushRenderMessage(m_listInputMessage, pMessage);
						break;

					case 2:
						pushRenderMessage(m_listServiceMessage, pMessage);
						break;

					case 3:
						pushRenderMessage(m_listTimerMessage, pMessage);
						break;

					case 4:
					default:
						pushRenderMessage(m_listUnknownMessage, pMessage);
						break;
					}
				}
				break;

			case CBaseMessage::MT_UNKNOWN:
			default:
				m_listUnknownMessage.addLast(pMessage);
				m_uiActiveMessageList = 4;
			}
		}

		m_waitObject.notify();
	}

	void CMain::deleteMessageByID(UINT uiID)
	{
		CAutoSync autoSync(m_syncObject);

		deleteMessageByID(m_listInputMessage, uiID);
		deleteMessageByID(m_listServiceMessage, uiID);
		deleteMessageByID(m_listTimerMessage, uiID);
		deleteMessageByID(m_listUnknownMessage, uiID);
	}

	void CMain::pushRenderMessage(List<CBaseMessage> &listMessage, CBaseMessage *pMessage)
	{
		if( m_bPrepareStop )
		{
			delete pMessage;
			return;
		}

		if (listMessage.size() > 0)
		{
			if (listMessage.getLast()->getType() == CBaseMessage::MT_RENDER)
			{
				delete pMessage;
				return;
			}
		}

		listMessage.addLast(pMessage);
	}

	void CMain::deleteMessageByID(List<CBaseMessage> &listMessage, UINT uiID)
	{
		listMessage.resetIterator();
		while (listMessage.hasMore())
		{
			CBaseMessage *pMessage = listMessage.next();

			if (pMessage->getID() == uiID)
			{
				listMessage.deleteItem(pMessage);
			}
		}
	}	

	CBaseMessage* CMain::popMessage()
	{
		CAutoSync autoSync(m_syncObject);

		CBaseMessage* pMessage = NULL;

		pMessage = m_listInputMessage.popFirst();	
		if (pMessage != NULL)
		{
			return pMessage;
		}

		pMessage = m_listTimerMessage.popFirst();
		if (pMessage != NULL)
		{
			return pMessage;
		}

		pMessage = m_listServiceMessage.popFirst();	
		if (pMessage != NULL)
		{
			return pMessage;
		}	

		pMessage = m_listUnknownMessage.popFirst();	
		if (pMessage != NULL)
		{
			return pMessage;
		}

		return pMessage;
	}	

	void CMain::clearMessageList()
	{
		CAutoSync autoSync(m_syncObject);

		m_listInputMessage.clear();
		m_listServiceMessage.clear();
		m_listTimerMessage.clear();
		m_listUnknownMessage.clear();
		m_uiActiveMessageList = 4;
	}

	void CMain::run()
	{	
		{
			if (CBaseMainFrame::getInstance()->initialize() == FALSE)
			{
				return;
			}
		}

		CTimerManager::getInstance()->setPriority(CConfigParser::getInstance()->getSysInfo()->iTimerThreadPri);
		CTimerManager::getInstance()->start();		

		CBaseMainFrame::getInstance()->onStartup(CConfigParser::getInstance());

		while (TRUE)
		{
			m_waitObject.wait();			

			if (m_bPrepareStop)
			{
				break;
			}

			CBaseMessage* pMessage = popMessage();
			while (pMessage != NULL)
			{
				pMessage->handleMessage();

				delete pMessage;
#if 0	
				if (m_listInputMessage.size() >= 50)
				{
					pMessage = m_listInputMessage.popLast();
					pMessage->handleMessage();
					delete pMessage;
					m_listInputMessage.clear();
				}
#endif
				pMessage = popMessage();
			}
	
			if (ENV::isDirty())
			{
				pushMessage(new CMsgRender());
			}
			//Sleep(10);
		}

		CTimerManager::getInstance()->stop();

		{
			CLog::info(MODULE_NAME, L"Kernel begin to unInitialize.");

			CBaseMainFrame::getInstance()->unInitialize();

			CLog::info(MODULE_NAME, L"Kernel successfully unInitialized.");
		}

		{
			CLog::info(MODULE_NAME, L"Render engine begin to unInitialize.");

			CRenderEngine::getInstance()->unInitialize(CScreenTouchServer::getInstance()->getScreenContext());

			CLog::info(MODULE_NAME, L"Render engine successfully unInitialized.");
		}

		{
			CLog::info(MODULE_NAME, L"Screen touch server begin to unInitialize.");

			CScreenTouchServer::getInstance()->unInitialize();

			CLog::info(MODULE_NAME, L"Screen touch server successfully unInitialized.");
		}

		CLog::info(MODULE_NAME, L"HMI stop running and prepare to exit.");
	}
}