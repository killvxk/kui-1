#include "OpenHMI.h"

namespace OpenHMI
{
	CMsgScreen::CMsgScreen(UINT uiStatus)
		: m_uiStatus(uiStatus)
	{
		m_eType = CBaseMessage::MT_SCREEN;
		m_uiID = m_eType;
	}

	CMsgScreen::~CMsgScreen()
	{
		;
	}

	void CMsgScreen::handleMessage()
	{
		CScreenTouchServer::getInstance()->dispatchEvent(m_uiStatus);
	}
}