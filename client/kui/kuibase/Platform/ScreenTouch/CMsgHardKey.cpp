#include "OpenHMI.h"

namespace OpenHMI
{
	CMsgHardKey::CMsgHardKey(UINT uiKeyCode, INT iKeyStatus)
		: m_uiKeyCode(uiKeyCode)
		, m_iKeyStatus(iKeyStatus)
	{
		m_eType = CBaseMessage::MT_HARD_KEY;
		m_uiID = m_eType;
	}

	CMsgHardKey::~CMsgHardKey()
	{
		;
	}

	void CMsgHardKey::handleMessage()
	{
		CBaseMainFrame::getInstance()->getOperationCenter()->onHardkey(m_uiKeyCode, m_iKeyStatus);
	} 
}