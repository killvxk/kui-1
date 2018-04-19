#include "OpenHMI.h"

namespace OpenHMI
{
	CManager_Application	*CManager_Application::s_pInstance	= NULL;

	CManager_Application* CManager_Application::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CManager_Application();
		}

		return s_pInstance;
	}

	void CManager_Application::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CManager_Application::CManager_Application()
		: m_mapApplication(FALSE)
	{
		;
	}

	CManager_Application::~CManager_Application()
	{
		;
	}

	void CManager_Application::initialize()
	{
		// do nothing
	}

	void CManager_Application::unInitialize()
	{
		closeAllApplications();
	}

	void CManager_Application::registerApplication(const String &strClassName, CBaseApplication *pApplication)
	{
		if ( pApplication != NULL )
		{
			m_mapApplication.addItem(strClassName, pApplication);
		}
	}

	CBaseApplication* CManager_Application::getApplicationByName(const String &strClassName)
	{
		return m_mapApplication[strClassName];
	}

	void CManager_Application::startApplication(const String &strClassName, const Map2<UINT, UINT, tagNumCompare<UINT> > &mapCommand)
	{
		CBaseApplication* pApplication = getApplicationByName(strClassName);
		if (pApplication != NULL)
		{
			pApplication->start(mapCommand);
		}
	}

	void CManager_Application::closeApplication(const String &strClassName)
	{
		CBaseApplication* pApplication = getApplicationByName(strClassName);
		if (pApplication != NULL)
		{
			pApplication->close();
		}
	}

	void CManager_Application::closeAllApplications()
	{
		m_mapApplication.resetIterator();
		while (m_mapApplication.hasMore())
		{
			m_mapApplication.next()->close();
		}
	}
}