#include "OpenHMI.h"

namespace OpenHMI
{
	CMsgGesture::CMsgGesture(UINT uiGestureID, Object *pData)
		: m_uiGestureID(uiGestureID)
		, m_pData(pData)
	{
		m_eType = CBaseMessage::MT_GESTURE;
		m_uiID = m_eType;
	}
	
	CMsgGesture::~CMsgGesture()
	{
		;
	}

	void CMsgGesture::handleMessage()
	{
		CGestureManager::getInstance()->dispatchEvent(m_uiGestureID, m_pData);

		if (m_pData != NULL)
		{
			delete m_pData;
			m_pData = NULL;
		}
	}
}