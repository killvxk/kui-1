#include "RenderEngine.h"

namespace OpenHMI
{
	CRenderTargetManager* CRenderTargetManager::s_pInstance	= NULL;

	CRenderTargetManager*	CRenderTargetManager::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CRenderTargetManager();
		}

		return s_pInstance;
	}

	void CRenderTargetManager::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CRenderTargetManager::CRenderTargetManager()
		: m_uiArySize(10)
	{
		;
	}

	CRenderTargetManager::~CRenderTargetManager()
	{
		;
	}

	void CRenderTargetManager::initialize()
	{
		;
	}

	void CRenderTargetManager::unInitialize()
	{
		clear();
	}

	void CRenderTargetManager::clear()
	{
		m_aryRenderTarget.clear();
	}

	CRenderTarget* CRenderTargetManager::getRenderTarget(UINT uiWidth, UINT uiHeight)
	{
		String strName = String::format(L"RT-%u-%u", uiWidth,uiHeight);

		CRenderTarget* pRT = getFreeRenderTarget(uiWidth,uiHeight, String::toString(g_uiSessionID) + strName);
		pRT->setInUse(TRUE);

		return pRT;
	}

	CRenderTarget* CRenderTargetManager::getFreeRenderTarget(UINT uiWidth, UINT uiHeight, const String& strName)
	{
		CRenderTarget* pRT = NULL;

		m_aryRenderTarget.resetIterator();
		while( m_aryRenderTarget.hasMore() )
		{
			CRenderTarget* pCurRT = m_aryRenderTarget.next();
			if( pCurRT->getName() == strName && !pCurRT->isInUse() )
			{
				pRT = pCurRT;
				break;
			}
		}

		if( pRT == NULL )
		{
			pRT = new CRenderTarget(uiWidth,uiHeight);
			pRT->setName(strName);
			pRT->setRenderContext(g_pRenderContext);
			addRenderTarget(pRT);
		}

		return pRT;
	}	

	void CRenderTargetManager::addRenderTarget(CRenderTarget* pRT)
	{
		if( m_aryRenderTarget.size() > m_uiArySize )
		{
			m_aryRenderTarget.deleteItemByIndex(m_uiArySize-1);
		}

		m_aryRenderTarget.addItem(CRenderTargetPtr(pRT));
	}
}