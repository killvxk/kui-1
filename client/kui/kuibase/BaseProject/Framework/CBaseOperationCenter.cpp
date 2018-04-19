#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseOperationCenter::CBaseOperationCenter()
	{
		;
	}

	CBaseOperationCenter::~CBaseOperationCenter()
	{
		;
	}

	void CBaseOperationCenter::onHardkey(UINT uiKeyCode, INT iKeyStatus)
	{
		CBaseScreen* pTopScreen = CBaseMainFrame::getInstance()->getTopScreen();

		if (pTopScreen != NULL && !pTopScreen->isLock())
		{
			if (pTopScreen->onHardkey(uiKeyCode, iKeyStatus))
			{
				return;
			}
		}
	}

	BOOLEAN CBaseOperationCenter::onCommand(const String &strCommand, Object *pData)
	{
		Array2<CComponent*> aryEmpty;
		CBasePopup* pTopOverlay = dynamic_cast<CBasePopup*>(CBaseMainFrame::getInstance()->getTopOverlay(aryEmpty));

		if (pTopOverlay != NULL && !pTopOverlay->isLock())
		{
			if (pTopOverlay->onDispatchCommand(strCommand, pData))
			{
				return TRUE;
			}
		}

		CBaseScreen* pTopScreen = CBaseMainFrame::getInstance()->getTopScreen();

		if (pTopScreen != NULL && !pTopScreen->isLock())
		{
			if (pTopScreen->onDispatchCommand(strCommand, pData))
			{
				return TRUE;
			}
		}

		return FALSE;
	}
}