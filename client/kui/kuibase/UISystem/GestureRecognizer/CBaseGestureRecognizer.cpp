#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseGestureRecognizer::CBaseGestureRecognizer()
	{
		;
	}

	CBaseGestureRecognizer::~CBaseGestureRecognizer()
	{
		;
	}

	void CBaseGestureRecognizer::dispatchGestureResult(UINT uiGestureID, Object *pData)
	{
		CMain::getInstance()->pushMessage(new CMsgGesture(uiGestureID, pData));
	}
}