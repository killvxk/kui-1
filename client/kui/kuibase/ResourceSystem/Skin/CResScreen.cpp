#include "OpenHMI.h"

namespace OpenHMI
{
	CResScreen::CResScreen()
	{
		;
	}

	CResScreen:: ~CResScreen()
	{
		;
	}

	BOOLEAN	CResScreen::isMainFrame()
	{
		return getPropertyBool(L"mainfrm");
	}

	BOOLEAN	CResScreen::merge(CResNode *pNode, BOOLEAN bFull)
	{
		CResScreen* pMergeSrc = dynamic_cast<CResScreen*>(pNode);

		if (pMergeSrc == NULL)
		{
			return FALSE;
		}
		
		if (!CResGroup::merge(pNode, bFull))
		{
			return FALSE;
		}

		// Sync "mainfrm" flags.
		BOOLEAN bIsMainFrame = pMergeSrc->isMainFrame();
		if (bIsMainFrame != this->isMainFrame())
		{
			this->setProperty(L"mainfrm", bIsMainFrame);
		}
		return TRUE;
	}

}