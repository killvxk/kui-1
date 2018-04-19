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
	typedef IGestureRecognizer* (*pGetInstance)();

	CGestureManager	*CGestureManager::s_pInstance	= NULL;

	CGestureManager* CGestureManager::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CGestureManager();
		}

		return s_pInstance;
	}

	void CGestureManager::deleteInstance()		
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CGestureManager::CGestureManager()
		: m_pGestureRecognizer(NULL)
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
		ENV::setGestureRecognizer(this);
	}

	CGestureManager::~CGestureManager()
	{
		freeLibrary();
	}

	BOOLEAN CGestureManager::registerGestureRecognizer(const String &strFileName)
	{
		if (strFileName.length() == 0)
		{
			return TRUE;
		}

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

		return bRet;
	}

#ifdef OS_WIN
	BOOLEAN CGestureManager::loadLibrary(const String &pstrFileName)
	{
		BOOLEAN bRet = FALSE;		
		String strPath = CPath::generateAbsolutePath(CPath::getApplicationFolder(), pstrFileName);
		CLog::info(String(MODULE_NAME), String::format(L"Begin to load gesture recognizer: %ls.", (const CHAR*)strPath));

		m_hInst = ::LoadLibraryEx(strPath, NULL, 0);

		if (m_hInst != NULL)
		{
			pGetInstance func = (pGetInstance)GetProcAddress(m_hInst, "getInstance");

			if (func != NULL)
			{
				m_pGestureRecognizer = (IGestureRecognizer*)(*func)();
				if (m_pGestureRecognizer != NULL)
				{
					bRet = TRUE;
					CLog::info(MODULE_NAME, L"Successfully load gesture recognizer.");
				}
				else
				{
					CLog::error2(MODULE_NAME, L"Gesture recognizer doesn't implement.");
				}
			}
			else
			{
				CLog::error2(MODULE_NAME, L"Can't get entry point of  gesture recognizer.");
			}
		}
		else
		{
			CLog::error2(MODULE_NAME, L"Can't load  gesture recognizer.");
		}

		return bRet;
	}

	void CGestureManager::freeLibrary()
	{
		if (m_hInst != NULL)
		{
			if (m_pGestureRecognizer != NULL)
			{
				delete m_pGestureRecognizer;
				m_pGestureRecognizer = NULL;
			}

			::FreeLibrary(m_hInst);
			m_hInst = NULL;

			CLog::info(MODULE_NAME, L"Free gesture recognizer.");
		}
	}
#endif

#ifdef OS_QNX
	BOOLEAN CGestureManager::loadLibrary(const String &pstrFileName)
	{
		BOOLEAN bRet = FALSE;

		String strFilePath = CPath::generateAbsolutePath(CPath::getApplicationFolder(), pstrFileName);
		char * pcFilePath = new char[strFilePath.length() * 3 + 1];
		String::convertToMBCS(strFilePath, pcFilePath, strFilePath.length() * 3 + 1);

		CLog::info(String(MODULE_NAME), String::format(L"Begin to load gesture recognizer: %ls.", (const CHAR*)strFilePath));

		m_dllHandle = dlopen(pcFilePath, RTLD_LAZY);
		if (m_dllHandle != NULL)
		{
			pGetInstance func = (pGetInstance)dlsym(m_dllHandle, "getInstance");
			if (func != NULL)
			{
				m_pGestureRecognizer = (IGestureRecognizer*)func();
				if (m_pGestureRecognizer != NULL)
				{
					bRet = TRUE;
					CLog::info(MODULE_NAME, L"Successfully load gesture recognizer.");
				}
				else
				{
					CLog::error2(MODULE_NAME, L"The gesture recognizer doesn't implement.");

					String strError;
					String::convertFromMBCS(dlerror(), strError);

					CLog::error2(MODULE_NAME, (const CHAR*)strError);
				}
			}
			else
			{
				CLog::error2(MODULE_NAME, L"Can't get entry point of gesture recognizer.");

				String strError;
				String::convertFromMBCS(dlerror(), strError);

				CLog::error2(MODULE_NAME, (const CHAR*)strError);
			}
		}
		else
		{
			CLog::error2(MODULE_NAME, L"Can't load gesture recognizer.");

			String strError;
			String::convertFromMBCS(dlerror(), strError);

			CLog::error2(MODULE_NAME, (const CHAR*)strError);
		}

		delete[] pcFilePath;

		return bRet;
	}

	void CGestureManager::freeLibrary()
	{
		if (m_dllHandle != NULL)
		{
			if (m_pGestureRecognizer != NULL)
			{
				delete m_pGestureRecognizer;
				m_pGestureRecognizer = NULL;
			}

			dlclose(m_dllHandle);
			m_dllHandle = NULL;

			CLog::info(MODULE_NAME, L"Free gesture recognizer.");
		}
	}
#endif

#ifdef OS_LINUX
	BOOLEAN CGestureManager::loadLibrary(const String &pstrFileName)
	{
		BOOLEAN bRet = FALSE;

		String strFilePath = CPath::generateAbsolutePath(CPath::getApplicationFolder(), pstrFileName);
		char * pcFilePath = new char[strFilePath.length() * 3 + 1];
		String::convertToMBCS(strFilePath, pcFilePath, strFilePath.length() * 3 + 1);

		CLog::info(String(MODULE_NAME), String::format(L"Begin to load gesture recognizer: %ls.", (const CHAR*)strFilePath));

		m_dllHandle = dlopen(pcFilePath, RTLD_LAZY);
		if (m_dllHandle != NULL)
		{
			pGetInstance func = (pGetInstance)dlsym(m_dllHandle, "getInstance");
			if (func != NULL)
			{
				m_pGestureRecognizer = (IGestureRecognizer*)func();
				if (m_pGestureRecognizer != NULL)
				{
					bRet = TRUE;
					CLog::info(MODULE_NAME, L"Successfully load gesture recognizer.");
				}
				else
				{
					CLog::error2(MODULE_NAME, L"The gesture recognizer doesn't implement.");

					String strError;
					String::convertFromMBCS(dlerror(), strError);

					CLog::error2(MODULE_NAME, (const CHAR*)strError);
				}
			}
			else
			{
				CLog::error2(MODULE_NAME, L"Can't get entry point of gesture recognizer.");

				String strError;
				String::convertFromMBCS(dlerror(), strError);

				CLog::error2(MODULE_NAME, (const CHAR*)strError);
			}
		}
		else
		{
			CLog::error2(MODULE_NAME, L"Can't load gesture recognizer.");

			String strError;
			String::convertFromMBCS(dlerror(), strError);

			CLog::error2(MODULE_NAME, (const CHAR*)strError);
		}

		delete[] pcFilePath;

		return bRet;
	}

	void CGestureManager::freeLibrary()
	{
		if (m_dllHandle != NULL)
		{
			if (m_pGestureRecognizer != NULL)
			{
				delete m_pGestureRecognizer;
				m_pGestureRecognizer = NULL;
			}

			dlclose(m_dllHandle);
			m_dllHandle = NULL;

			CLog::info(MODULE_NAME, L"Free gesture recognizer.");
		}
	}
#endif

	void CGestureManager::dispatchEvent(UINT uiEventID, Object *pData)
	{
		CEventDispatcher::dispatchEvent(this, uiEventID, pData);
	}

	void  CGestureManager::addCursor(UINT uiGroupID, const CCursor &cursor)
	{
		if( m_pGestureRecognizer != NULL )
		{
			m_pGestureRecognizer->addCursor(uiGroupID, cursor);
		}
	}

	void  CGestureManager::updateCursor(UINT uiGroupID, const CCursor &cursor)
	{
		if( m_pGestureRecognizer != NULL )
		{
			m_pGestureRecognizer->updateCursor(uiGroupID, cursor);
		}
	}

	void  CGestureManager::removeCursor(UINT uiGroupID, const CCursor &cursor)
	{
		if( m_pGestureRecognizer != NULL )
		{
			m_pGestureRecognizer->removeCursor(uiGroupID, cursor);
		}
	}
}