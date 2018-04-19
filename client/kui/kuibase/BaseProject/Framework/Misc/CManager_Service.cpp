#include "OpenHMI.h"

namespace OpenHMI
{
	CManager_Service	*CManager_Service::s_pInstance	= NULL;

	CManager_Service* CManager_Service::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CManager_Service();
		}

		return s_pInstance;
	}

	void CManager_Service::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CManager_Service::CManager_Service()
		: m_mapService(FALSE)
	{
		;
	}

	CManager_Service::~CManager_Service()
	{
		;
	}

	void CManager_Service::initialize()
	{
		m_mapService.resetIterator();
		while (m_mapService.hasMore())
		{
			m_mapService.next()->initialize();
		}
	}

	void CManager_Service::unInitialize()
	{
		m_mapService.resetIterator();
		while (m_mapService.hasMore())
		{
			m_mapService.next()->unInitialize();
		}
	}

	void CManager_Service::registerService(const String &strClassName, CBaseService *pService)
	{
		if ( pService != NULL )
		{
			m_mapService.addItem(strClassName, pService);
		}
	}

	CBaseService* CManager_Service::getServiceByName(const String &strClassName)
	{
		return m_mapService[strClassName];
	}
}