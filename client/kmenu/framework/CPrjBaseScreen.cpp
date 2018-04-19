#include "HMI.h"

namespace OpenHMI
{
	CPrjBaseScreen::CPrjBaseScreen()
	{
		// TODO: setup parent-child relationship with children controls here

		// TODO: add event listener to children controls here

		// TODO: initialize children controls specific properties here
	}

	CPrjBaseScreen::~CPrjBaseScreen()
	{
		;
	}

	// TODO: finish control or logic event map here
	BEGIN_EVENT_MAP(CPrjBaseScreen)
	END_EVENT_MAP(CBaseScreen)

	// TODO: finish command map here
	BEGIN_CMD_MAP(CPrjBaseScreen)
	END_CMD_MAP(CBaseScreen)

	BOOLEAN CPrjBaseScreen::initialize()
	{
		// TODO: add your spec initialize code here
		// normally add logic event listener.

		return TRUE;
	}

	void CPrjBaseScreen::unInitialize()
	{
		// TODO: add your spec un-initialize code here	
		// normally remove logic event listener.
	}

	void CPrjBaseScreen::pushDataToUI()
	{
		// TODO: initialize this and children controls skin info here

		// TODO: set data to children controls or set children controls status here
	}

	BOOLEAN CPrjBaseScreen::onHardkey(UINT uiKeyCode, INT iKeyStatus)
	{
		// TODO: deal with each key code of this screen can identify and return true here

		return FALSE;
	}

	void CPrjBaseScreen::show()
	{
		CBaseScreen::show();

		// TODO: add your special code according to your project character while screen to show here
		if (isTransparent())
		{
			CMainFrame::getInstance()->hideBackground();
		}
	}

	void CPrjBaseScreen::hide()
	{
		CBaseScreen::hide();

		// TODO: add your special code according to your project character while screen to hide here
		if (isTransparent())
		{
			CMainFrame::getInstance()->showBackground();
		}
	}

	// TODO: implement your public method here

	// TODO: implement your protected method here
	void CPrjBaseScreen::onBtnBackTchRel(CComponent *pTarget)
	{
		doBack();
	}

	// TODO: implement control event listener here

	// TODO: implement logic event listener here

	// TODO: implement your normal private method here
}