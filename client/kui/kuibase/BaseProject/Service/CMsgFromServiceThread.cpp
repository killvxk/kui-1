#include "OpenHMI.h"

namespace OpenHMI
{
	CMsgFromServiceThread::CMsgFromServiceThread(CBaseService *pService, UINT uiMethodSignalID, Object *pData, BOOLEAN bIsSignal)
		: m_pService(pService)
		, m_uiMethodSignalID(uiMethodSignalID)
		, m_pData(pData)
		, m_bIsSignal(bIsSignal)
	{
		m_eType = CBaseMessage::MT_FROM_SERVICE;
		m_uiID = m_eType;
	}
	
	CMsgFromServiceThread::~CMsgFromServiceThread()
	{
		;
	}

	void CMsgFromServiceThread::handleMessage()
	{
		if ( m_bIsSignal == TRUE )
		{
			m_pService->dispatchEvent(m_uiMethodSignalID, m_pData);
		}
		else
		{
			if (m_pService->checkError(m_pData))
			{
				if (m_pData != NULL)
				{
					(static_cast<CBaseData*>(m_pData))->setMethodID(m_uiID);
				}

				m_pService->dispatchEvent(SERVICE_ERROR_EVENT_ID, m_pData);
			}
			else
			{
				m_pService->dispatchEvent(m_uiMethodSignalID, m_pData);
			}
		}	

		if (m_pData != NULL)
		{
			delete m_pData;
			m_pData = NULL;
		}
	}
}