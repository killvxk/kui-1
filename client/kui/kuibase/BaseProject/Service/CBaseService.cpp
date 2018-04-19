#include "OpenHMI.h"

namespace OpenHMI
{
	CBaseService::CBaseService()
		: m_pBaseServcie(NULL)
	{
		;
	}

	CBaseService::~CBaseService()
	{
		;
	}

	BEGIN_SERVICE_METHOD_MAP(CBaseService)
		SERVICE_METHOD_HANDLER(0xFFFFFFFF, _initialize)
	END_SERVICE_METHOD_MAP_2

	void CBaseService::initialize()
	{
		invoke(0xFFFFFFFF, NULL);
	}

	void CBaseService::_initialize(Object *pData)
	{
		if (m_pBaseServcie != NULL)
		{
			m_pBaseServcie->connect();
		}
	}

	void CBaseService::unInitialize()
	{
		if (m_pBaseServcie != NULL)
		{
			m_pBaseServcie->disconnect();
		}
	}

	INT CBaseService::invoke(UINT uiMethodID, Object *pData)
	{
		CServiceAdaptor::getInstance()->pushInvokeMessage(new CMsgToServiceThread(this, uiMethodID, pData));

		static INT iCallID = 0;
		iCallID++;

		return iCallID;
	}

	void CBaseService::dispatchMethodResponse(UINT uiMethodID, Object *pData)
	{
		CMain::getInstance()->pushMessage(new CMsgFromServiceThread(this, uiMethodID, pData, FALSE));
	}

	void CBaseService::dispatchSignal(UINT uiSignalID, Object *pData)
	{
		CMain::getInstance()->pushMessage(new CMsgFromServiceThread(this, uiSignalID, pData, TRUE));
	}	

	void CBaseService::dispatchEvent(UINT uiEventID, Object *pData)
	{
		CEventDispatcher::dispatchEvent(this, uiEventID, pData);
	}
}