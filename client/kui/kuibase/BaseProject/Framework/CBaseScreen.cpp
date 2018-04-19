#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseScreen::CBaseScreen()
		: m_bIsTransparent(FALSE)
	{
		setType(CONFIG::CONTAINER_TYPE_SCREEN);
		setPreferredSize(ENV::getScreenWidth(), ENV::getScreenHeight());
		setVisible(FALSE);
	}

	CBaseScreen::~CBaseScreen()
	{
		;
	}

	BEGIN_EVENT_MAP(CBaseScreen)
	END_EVENT_MAP(CContainer)

	BEGIN_CMD_MAP(CBaseScreen)
	END_CMD_MAP_2	

	void CBaseScreen::show()
	{
// 		if (CONFIG::getUseCCP())
// 		{
// 			if (hasFocus() == FALSE)
// 			{
// 				setFocus();
// 			}
// 		}

		setVisible(TRUE);
	}

	void CBaseScreen::hide()
	{
		setVisible(FALSE);
	}

	void CBaseScreen::enter()
	{
		CBaseMainFrame::getInstance()->addScreen(this);

		CBaseMainFrame::getInstance()->innerStartScreenAnimation();
	}		

	void CBaseScreen::leave()
	{
		CBaseMainFrame::getInstance()->removeScreen(this);

		CBaseMainFrame::getInstance()->innerStartScreenAnimation();
	}
	
	void CBaseScreen::refresh()
	{	
		pushDataToUI();

		CContainer::refresh();
	}

	void CBaseScreen::destroy()
	{
		if (isDestroyed())
		{
			return;
		}	

		CContainer::destroy();

		//delete this;
	}

	BOOLEAN CBaseScreen::doFocusBack()
	{
		if (CContainer::doFocusBack() == FALSE)
		{
			doBack();
		}
		
		return TRUE;
	}

	void CBaseScreen::doBack()
	{
		leave();
	}

	void CBaseScreen::doHome()
	{
		CBaseMainFrame::getInstance()->showHomeScreen();
	}
}