#include "OpenHMI.h"

namespace OpenHMI
{
	CManager_Logic	*CManager_Logic::s_pInstance	= NULL;

	CManager_Logic*	CManager_Logic::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CManager_Logic();
		}

		return s_pInstance;
	}

	void CManager_Logic::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CManager_Logic::CManager_Logic()
		: m_mapLogic(FALSE)
	{
		;
	}

	CManager_Logic::~CManager_Logic()
	{
		;
	}

	void CManager_Logic::initialize()
	{
		m_mapLogic.resetIterator();
		while(m_mapLogic.hasMore())
		{
			m_mapLogic.next()->initialize();
		}
	}

	void CManager_Logic::unInitialize()
	{
		m_mapLogic.resetIterator();
		while(m_mapLogic.hasMore())
		{
			m_mapLogic.next()->unInitialize();
		}
	}

	void CManager_Logic::registerLogic(const String &strClassName, CBaseLogic *pLogic)
	{
		if ( pLogic != NULL )
		{
			m_mapLogic.addItem(strClassName, pLogic);
		}
	}

	CBaseLogic* CManager_Logic::getLogicByName(const String &strClassName)
	{
		return m_mapLogic[strClassName];
	}
}