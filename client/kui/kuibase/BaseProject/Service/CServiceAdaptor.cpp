#include "OpenHMI.h"

#ifdef OS_WIN

#endif

#ifdef OS_QNX
	#include <dlfcn.h>
#endif

#ifdef OS_LINUX
	#include <dlfcn.h>
#endif

namespace OpenHMI
{
	typedef IServiceAdaptor* (*pGetInstance)();

	CServiceAdaptor	*CServiceAdaptor::s_pInstance	= NULL;

	CServiceAdaptor* CServiceAdaptor::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CServiceAdaptor();
		}

		return s_pInstance;
	}

	void CServiceAdaptor::deleteInstance()		
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CServiceAdaptor::CServiceAdaptor()
		: m_pServiceAdaptor(NULL)
	{
#ifdef OS_WIN
		m_hInst = NULL;
#endif

#ifdef OS_QNX
		m_dllHandle = NULL;
#endif

#ifdef OS_LINUX
		m_dllHandle = NULL;
#endif
	}

	CServiceAdaptor::~CServiceAdaptor()
	{
		freeLibrary();
	}

	BOOLEAN CServiceAdaptor::registerServiceAdaptor(const String &strFileName, BOOLEAN bInitialize)
	{
#ifdef OS_WIN
		if (m_hInst != NULL)
		{
			return FALSE;
		}
#endif

#ifdef OS_QNX
		if (m_dllHandle != NULL)
		{
			return FALSE;
		}
#endif

#ifdef OS_LINUX
		if (m_dllHandle != NULL)
		{
			return FALSE;
		}
#endif

		BOOLEAN bRet = loadLibrary(strFileName);

		if (bRet && bInitialize)
		{
			return initialize();
		}

		return bRet;
	}

	void CServiceAdaptor::pushInvokeMessage(CBaseMessage *pMessage)
	{
		if (pMessage == NULL)
		{
			return;
		}

		{
			CAutoSync autoSync(m_syncObject);

			m_listMessage.addLast(pMessage);
		}

		m_waitObject.notify();
	}

	CBaseMessage* CServiceAdaptor::popMessage()
	{
		CAutoSync autoSync(m_syncObject);

		CBaseMessage* pMessage = m_listMessage.popFirst();

		return pMessage;
	}

	void CServiceAdaptor::clearMessageList()
	{
		CAutoSync autoSync(m_syncObject);

		m_listMessage.clear();
	}

	void CServiceAdaptor::run()
	{
		while (TRUE)
		{
			m_waitObject.wait();

			if (m_bPrepareStop)
			{
				break;
			}

			CBaseMessage* pMessage = popMessage();
			while (pMessage != NULL)
			{
				pMessage->handleMessage();

				delete pMessage;

				pMessage = popMessage();
			}
		}
	}

#ifdef OS_WIN
	BOOLEAN CServiceAdaptor::loadLibrary(const String &pstrFileName)
	{
		BOOLEAN bRet = FALSE;		
		String strPath = CPath::generateAbsolutePath(CPath::getApplicationFolder(), pstrFileName);

		CLog::info(String(MODULE_NAME), String::format(L"Begin to load service adapter: %ls.", (const CHAR*)strPath));

		m_hInst = ::LoadLibraryEx(strPath, NULL, 0);

		if (m_hInst != NULL)
		{
			pGetInstance func = (pGetInstance)GetProcAddress(m_hInst, "getInstance");

			if (func != NULL)
			{
				m_pServiceAdaptor = (IServiceAdaptor*)(*func)();
				if (m_pServiceAdaptor != NULL)
				{
					bRet = TRUE;
					CLog::info(MODULE_NAME, L"Successfully load service adapter.");
				}
				else
				{
					CLog::error2(MODULE_NAME, L"The service adapter doesn't implement.");
				}
			}
			else
			{
				CLog::error2(MODULE_NAME, L"Can't get entry point of service adapter.");
			}
		}
		else
		{
			CLog::error2(MODULE_NAME, L"Can't load service adapter.");
		}

		return bRet;
	}

	void CServiceAdaptor::freeLibrary()
	{
		if (m_hInst != NULL)
		{
			if (m_pServiceAdaptor != NULL)
			{
				delete m_pServiceAdaptor;
				m_pServiceAdaptor = NULL;
			}

			::FreeLibrary(m_hInst);
			m_hInst = NULL;

			CLog::info(MODULE_NAME, L"Free service adapter.");
		}
	}
#endif

#ifdef OS_QNX
	BOOLEAN CServiceAdaptor::loadLibrary(const String &pstrFileName)
	{
		BOOLEAN bRet = FALSE;

		String strFilePath = CPath::generateAbsolutePath(CPath::getApplicationFolder(), pstrFileName);
		char * pcFilePath = new char[strFilePath.length() * 3 + 1];
		String::convertToMBCS(strFilePath, pcFilePath, strFilePath.length() * 3 + 1);

		CLog::info(String(MODULE_NAME), String::format(L"Begin to load service adapter: %ls.", (const CHAR*)strFilePath));

		m_dllHandle = dlopen(pcFilePath, RTLD_LAZY);
		if (m_dllHandle != NULL)
		{
			pGetInstance func = (pGetInstance)dlsym(m_dllHandle, "getInstance");
			if (func != NULL)
			{
				m_pServiceAdaptor = (IServiceAdaptor*)func();
				if (m_pServiceAdaptor != NULL)
				{
					bRet = TRUE;
					CLog::info(MODULE_NAME, L"Successfully load service adapter.");
				}
				else
				{
					CLog::error2(MODULE_NAME, L"The service adapter doesn't implement.");

					String strError;
					String::convertFromMBCS(dlerror(), strError);

					CLog::error2(MODULE_NAME, (const CHAR*)strError);
				}
			}
			else
			{
				CLog::error2(MODULE_NAME, L"Can't get entry point of service adapter.");

				String strError;
				String::convertFromMBCS(dlerror(), strError);

				CLog::error2(MODULE_NAME, (const CHAR*)strError);
			}
		}
		else
		{
			CLog::error2(MODULE_NAME, L"Can't load service adapter.");

			String strError;
			String::convertFromMBCS(dlerror(), strError);

			CLog::error2(MODULE_NAME, (const CHAR*)strError);
		}

		delete[] pcFilePath;

		return bRet;
	}

	void CServiceAdaptor::freeLibrary()
	{
		if (m_dllHandle != NULL)
		{
			if (m_pServiceAdaptor != NULL)
			{
				delete m_pServiceAdaptor;
				m_pServiceAdaptor = NULL;
			}

			dlclose(m_dllHandle);
			m_dllHandle = NULL;

			CLog::info(MODULE_NAME, L"Free service adapter.");
		}
	}
#endif

#ifdef OS_LINUX
	BOOLEAN CServiceAdaptor::loadLibrary(const String &pstrFileName)
	{
		BOOLEAN bRet = FALSE;

		String strFilePath = CPath::generateAbsolutePath(CPath::getApplicationFolder(), pstrFileName);
		char * pcFilePath = new char[strFilePath.length() * 3 + 1];
		String::convertToMBCS(strFilePath, pcFilePath, strFilePath.length() * 3 + 1);

		CLog::info(String(MODULE_NAME), String::format(L"Begin to load service adapter: %ls.", (const CHAR*)strFilePath));

		m_dllHandle = dlopen(pcFilePath, RTLD_LAZY);
		if (m_dllHandle != NULL)
		{
			pGetInstance func = (pGetInstance)dlsym(m_dllHandle, "getInstance");
			if (func != NULL)
			{
				m_pServiceAdaptor = (IServiceAdaptor*)func();
				if (m_pServiceAdaptor != NULL)
				{
					bRet = TRUE;
					CLog::info(MODULE_NAME, L"Successfully load service adapter.");
				}
				else
				{
					CLog::error2(MODULE_NAME, L"The service adapter doesn't implement.");

					String strError;
					String::convertFromMBCS(dlerror(), strError);

					CLog::error2(MODULE_NAME, (const CHAR*)strError);
				}
			}
			else
			{
				CLog::error2(MODULE_NAME, L"Can't get entry point of service adapter.");

				String strError;
				String::convertFromMBCS(dlerror(), strError);

				CLog::error2(MODULE_NAME, (const CHAR*)strError);
			}
		}
		else
		{
			CLog::error2(MODULE_NAME, L"Can't load service adapter.");

			String strError;
			String::convertFromMBCS(dlerror(), strError);

			CLog::error2(MODULE_NAME, (const CHAR*)strError);
		}

		delete[] pcFilePath;

		return bRet;
	}

	void CServiceAdaptor::freeLibrary()
	{
		if (m_dllHandle != NULL)
		{
			if (m_pServiceAdaptor != NULL)
			{
				delete m_pServiceAdaptor;
				m_pServiceAdaptor = NULL;
			}

			dlclose(m_dllHandle);
			m_dllHandle = NULL;

			CLog::info(MODULE_NAME, L"Free service adapter.");
		}
	}
#endif

	BOOLEAN CServiceAdaptor::initialize()
	{
		if (m_pServiceAdaptor != NULL)
		{
			BOOLEAN bRet = m_pServiceAdaptor->initialize();

			if (bRet)
			{
				setPriority(CConfigParser::getInstance()->getSysInfo()->iServiceAdapterThreadPri);
				start();
			}

			return bRet;
		}
		else
		{
			return FALSE;
		}
	}

	void CServiceAdaptor::unInitialize()
	{
		if (m_bRunning)
		{
			clearMessageList();	
			stop();
		}		

		if (m_pServiceAdaptor != NULL)
		{
			m_pServiceAdaptor->unInitialize();
		}
	}
}