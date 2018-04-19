#include "HMI.h"
extern HANDLE	m_hFileMap;
typedef struct tagFileMap
{
	HWND hWnd;
	TCHAR szCmdLine[1024];
}tagFileMap;

namespace OpenHMI
{
	CMainFrame	*CMainFrame::s_pInstance	= NULL;


	CMainFrame*	CMainFrame::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CMainFrame();
		}

		return s_pInstance;
	}

	CMainFrame::CMainFrame()
		: m_bScreenAmtSwitch(FALSE)
		, m_pPopTips(NULL)
		, m_bAutoHideEdit(FALSE)
	{
		CLogic::registerAll();
		registerHomeScreen(new CScnMain());

		// TODO: add your initialize code here

		// TODO: setup parent-child relationship with children controls here

		// TODO: add event listener to children controls here


		// TODO: initialize children controls specific properties here
	}

	CMainFrame::~CMainFrame()
	{
		// TODO: add your spec un-initialize code here

	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CMainFrame)
		POPUP_EVENT_HANDLER(m_pPopTips, CEventPopup::DESTROYED, onPopTipsDsy)
	END_EVENT_MAP(CBaseMainFrame)

	// TODO: implement your public method here
	void CMainFrame::onStartup(CConfigParser *pParser)
	{
		// TODO: Set active language and skin here
		DWORD dwLanguageId = 0;
		DWORD dwRegType = 0, dwSize = sizeof(dwLanguageId);
		::SHGetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet"),
			TEXT("languageid"), &dwRegType, &dwLanguageId, &dwSize);

		if (dwLanguageId == 0)
			dwLanguageId = ::GetSystemDefaultUILanguage();

		switch (dwLanguageId)
		{
		case 2052:
			CManager_Language::getInstance()->setActiveLanguage(L"CN"); 
			break;
		case 1033:
			CManager_Language::getInstance()->setActiveLanguage(L"CN");
			break;
		default:
			CManager_Language::getInstance()->setActiveLanguage(L"CN");
			break;
		}

		
		CManager_Skin::getInstance()->setActiveSkin(L"Normal");

		if (m_hFileMap != NULL)
		{
			tagFileMap* pFileMap = (tagFileMap*)MapViewOfFile(m_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE,
				0, 0, sizeof(tagFileMap));
			if (pFileMap != NULL)
			{
				pFileMap->hWnd = OpenHMI::CScreenTouchServer::getInstance()->getScreenContext()->hWnd;
				UnmapViewOfFile(pFileMap);
			}
		}

		// TODO: initialize the global widget here
	}

	void CMainFrame::pushDataToUI()
	{
		// TODO: initialize this skin info here
	}

	void CMainFrame::onSkinResourceReady()
	{
		// TODO: prepare to show start warning dialog or home screen here
		getHomeScreen()->setPreferredSize(ENV::getScreenWidth(), ENV::getScreenHeight());
		showHomeScreen();
	}

	void CMainFrame::startScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter, UINT uiIntervalTime, UINT uiRepeatCount)
	{
		// TODO: add your animation start code here
		if (m_bScreenAmtSwitch == TRUE)
		{
			if (pScreenToEnter != NULL)
			{
				pScreenToEnter->setOpacity(0);
				pScreenToEnter->setX(0);
			}

			CBaseMainFrame::startScreenAnimation(pScreenToLeave, pScreenToEnter, 10, 10);
 		}
 		else
 		{
			if (pScreenToEnter != NULL)
			{
				pScreenToEnter->setX(0);
			}

			CBaseMainFrame::startScreenAnimation(pScreenToLeave, pScreenToEnter, 1, 1);
		}
	
	}

	void CMainFrame::doScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter, UINT uiCurrentCount)
	{
		// TODO: add your animation code here
		if (m_bScreenAmtSwitch == TRUE)
		{
			if (pScreenToLeave != NULL)
			{
				if (uiCurrentCount <= 20)	
				{
					pScreenToLeave->setOpacity(100 - 5 * uiCurrentCount);
				}
			}

			if (pScreenToEnter != NULL)
			{
				if (uiCurrentCount >= 11 && uiCurrentCount <= 30)
				{
					pScreenToEnter->setOpacity(5 * (uiCurrentCount - 11));
				}

				if (uiCurrentCount > 20)
				{
					pScreenToEnter->unlock();
				}
			}
 		}
		else
		{
			
		}

	}

	void CMainFrame::completeScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter)
	{
		// TODO: add your animation complete code here
		if (m_bScreenAmtSwitch == TRUE)
		{
			if (pScreenToEnter != NULL)
			{
 				pScreenToEnter->setOpacity(100);
			}

			m_bScreenAmtSwitch = FALSE;
		}
		else
 		{
		
		}

		CBaseMainFrame::completeScreenAnimation(pScreenToLeave, pScreenToEnter);
	}

	void CMainFrame::showTips(const String& str, BOOLEAN bAutoHideEdit, BOOLEAN bShowBg)
	{
		if (m_pPopTips == NULL)
		{
			m_bAutoHideEdit = bAutoHideEdit;

			m_pPopTips = new CPopTips(FALSE);
			if (m_pPopTips->initialize())
			{
				m_pPopTips->setContent(str, bShowBg);
				m_pPopTips->addEventListener(this);
				m_pPopTips->doModal();
			}
			else
			{
				delete m_pPopTips;
				m_pPopTips = NULL;
			}
		};
	}

	// TODO: implement control event listener here
	void CMainFrame::onPopTipsDsy(CBasePopup *pTarget)
	{
		m_pPopTips = NULL;
	}


	// TODO: implement logic event listener here

	// TODO: implement your normal private method here
}