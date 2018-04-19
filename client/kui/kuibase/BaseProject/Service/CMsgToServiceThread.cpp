#include "OpenHMI.h"

namespace OpenHMI
{
	CMsgToServiceThread::CMsgToServiceThread(CBaseService *pService, UINT uiMethodID, Object *pData)
		: m_pService(pService)
		, m_uiMethodID(uiMethodID)
		, m_pData(pData)
	{
		m_eType = CBaseMessage::MT_TO_SERVICE;
		m_uiID = m_eType;
	}

	CMsgToServiceThread::~CMsgToServiceThread()
	{
		;
	}

	void CMsgToServiceThread::handleMessage()
	{
		m_pService->onDispatchServiceMethod(m_uiMethodID, m_pData);

		if (m_pData != NULL)
		{
			delete m_pData;
			m_pData = NULL;
		}
	}
}