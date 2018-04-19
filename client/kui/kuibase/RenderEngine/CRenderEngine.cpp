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
	typedef IRenderEngine* (*pGetInstance)();

	CRenderEngine	*CRenderEngine::s_pInstance	= NULL;

	CRenderEngine* CRenderEngine::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CRenderEngine();
		}

		return s_pInstance;
	}

	void CRenderEngine::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CRenderEngine::CRenderEngine()
		: m_pEngine(NULL)
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

	CRenderEngine::~CRenderEngine()
	{
		freeLibrary();
	}

	BOOLEAN CRenderEngine::registernRenderEngine(const String &strFileName, BOOLEAN bInitialize)
	{
#ifdef OS_WIN
		if (m_hInst != NULL)
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

	IRenderSession* CRenderEngine::getRenderSession(UINT uiID)
	{
		if (m_pEngine != NULL)
		{
			return m_pEngine->getRenderSession(uiID);
		}
		else
		{
			return NULL;
		}
	}

	void CRenderEngine::destroyRenderSession(UINT uiID)
	{
		if (m_pEngine != NULL)
		{
			m_pEngine->destroyRenderSession(uiID);
		}
	}

	void CRenderEngine::releaseCacheData()
	{
		if (m_pEngine != NULL)
		{
			m_pEngine->releaseCacheData();
		}
	}

	void CRenderEngine::getImageInfo(const String &strFilePath, UINT *puiWidth, UINT *puiHeight)
	{
		if (m_pEngine != NULL)
		{
			m_pEngine->getImageInfo(strFilePath, puiWidth, puiHeight);
		}
	}

	void CRenderEngine::getImageInfo(const String &strFilePath, Gdiplus::Bitmap* pBitmap, UINT *puiWidth, UINT *puiHeight)
	{
		if (m_pEngine != NULL)
		{
			m_pEngine->getImageInfo(strFilePath, pBitmap, puiWidth, puiHeight);
		}
	}

	void CRenderEngine::getTextInfo(CRenderText *pRndText, UINT *puiWidth, UINT *puiHeight, UINT *puiLineHeight)
	{
		if (m_pEngine != NULL)
		{
			m_pEngine->getTextInfo(pRndText, puiWidth, puiHeight, puiLineHeight);
		}
	}

#ifdef OS_WIN
	BOOLEAN CRenderEngine::loadLibrary(const String &pstrFileName)
	{
		BOOLEAN bRet = FALSE;	
		String strPath = CPath::generateAbsolutePath(CPath::getApplicationFolder(), pstrFileName);

		CLog::info(String(MODULE_NAME), String::format(L"Begin to load render engine: %ls.", (const CHAR*)strPath));

		m_hInst = ::LoadLibraryEx(strPath, NULL, 0);

		if (m_hInst != NULL)
		{
			pGetInstance func = (pGetInstance)GetProcAddress(m_hInst, "getInstance");

			if (func != NULL)
			{
				m_pEngine = (IRenderEngine*)(*func)();
				if (m_pEngine != NULL)
				{
					bRet = TRUE;
					CLog::info(MODULE_NAME, L"Successfully load render engine.");
				}
				else
				{
					CLog::error2(MODULE_NAME, L"The render engine doesn't implement.");
				}
			}
			else
			{
				CLog::error2(MODULE_NAME, L"Can't get entry point of render engine.");
			}
		}
		else
		{
			CLog::error2(MODULE_NAME, L"Can't load render engine.");
		}

		return bRet;
	}

	void CRenderEngine::freeLibrary()
	{
		if (m_hInst != NULL)
		{
			if (m_pEngine != NULL)
			{
				delete m_pEngine;
				m_pEngine = NULL;
			}

			::FreeLibrary(m_hInst);
			m_hInst = NULL;

			CLog::info(MODULE_NAME, L"Free render engine.");
		}

		m_pEngine = NULL;
	}
#endif

#ifdef OS_QNX
	BOOLEAN CRenderEngine::loadLibrary(const String &pstrFileName)
	{
		BOOLEAN bRet = FALSE;	

		String strFilePath = CPath::generateAbsolutePath(CPath::getApplicationFolder(), pstrFileName);
		char * pcFilePath = new char[strFilePath.length() * 3 + 1];
		String::convertToMBCS(strFilePath, pcFilePath, strFilePath.length() * 3 + 1);

		CLog::info(String(MODULE_NAME), String::format(L"Begin to load render engine: %ls.", (const CHAR*)strFilePath));

		m_dllHandle = dlopen(pcFilePath, RTLD_LAZY);
		if (m_dllHandle != NULL)
		{
			pGetInstance func = (pGetInstance)dlsym(m_dllHandle, "getInstance");
			if (func != NULL)
			{
				m_pEngine = (IRenderEngine*)func();
				if (m_pEngine != NULL)
				{
					bRet = TRUE;
					CLog::info(MODULE_NAME, L"Successfully load render engine.");
				}
				else
				{
					CLog::error2(MODULE_NAME, L"The render engine doesn't implement.");

					String strError;
					String::convertFromMBCS(dlerror(), strError);

					CLog::error2(MODULE_NAME, (const CHAR*)strError);
				}
			}
			else
			{
				CLog::error2(MODULE_NAME, L"Can't get entry point of render engine.");

				String strError;
				String::convertFromMBCS(dlerror(), strError);

				CLog::error2(MODULE_NAME, (const CHAR*)strError);
			}
		}
		else
		{
			CLog::error2(MODULE_NAME, L"Can't load render engine.");

			String strError;
			String::convertFromMBCS(dlerror(), strError);
			
			CLog::error2(MODULE_NAME, (const CHAR*)strError);
		}

		delete[] pcFilePath;

		return bRet;
	}

	void CRenderEngine::freeLibrary()
	{
		if (m_dllHandle != NULL)
		{
			if (m_pEngine != NULL)
			{
				delete m_pEngine;
				m_pEngine = NULL;
			}

			dlclose(m_dllHandle);
			m_dllHandle = NULL;

			CLog::info(MODULE_NAME, L"Free render engine.");
		}
	}
#endif	

#ifdef OS_LINUX
	BOOLEAN CRenderEngine::loadLibrary(const String &pstrFileName)
	{
		BOOLEAN bRet = FALSE;		

		String strFilePath = CPath::generateAbsolutePath(CPath::getApplicationFolder(), pstrFileName);
		char * pcFilePath = new char[strFilePath.length() * 3 + 1];
		String::convertToMBCS(strFilePath, pcFilePath, strFilePath.length() * 3 + 1);

		CLog::info(String(MODULE_NAME), String::format(L"Begin to load render engine: %ls.", (const CHAR*)strFilePath));

		m_dllHandle = dlopen(pcFilePath, RTLD_LAZY);
		if (m_dllHandle != NULL)
		{
			pGetInstance func = (pGetInstance)dlsym(m_dllHandle, "getInstance");
			if (func != NULL)
			{
				m_pEngine = (IRenderEngine*)func();
				if (m_pEngine != NULL)
				{
					bRet = TRUE;
					CLog::info(MODULE_NAME, L"Successfully load render engine.");
				}
				else
				{
					CLog::error2(MODULE_NAME, L"The render engine doesn't implement.");

					String strError;
					String::convertFromMBCS(dlerror(), strError);

					CLog::error2(MODULE_NAME, (const CHAR*)strError);
				}
			}
			else
			{
				CLog::error2(MODULE_NAME, L"Can't get entry point of render engine.");

				String strError;
				String::convertFromMBCS(dlerror(), strError);

				CLog::error2(MODULE_NAME, (const CHAR*)strError);
			}
		}
		else
		{
			CLog::error2(MODULE_NAME, L"Can't load render engine.");

			String strError;
			String::convertFromMBCS(dlerror(), strError);

			CLog::error2(MODULE_NAME, (const CHAR*)strError);
		}

		delete[] pcFilePath;

		return bRet;
	}

	void CRenderEngine::freeLibrary()
	{
		if (m_dllHandle != NULL)
		{
			if (m_pEngine != NULL)
			{
				delete m_pEngine;
				m_pEngine = NULL;
			}

			dlclose(m_dllHandle);
			m_dllHandle = NULL;

			CLog::info(MODULE_NAME, L"Free render engine.");
		}
	}
#endif

	BOOLEAN CRenderEngine::initialize(const tagScreenContext *pScreenContext)
	{
		if (m_pEngine != NULL)
		{
			return m_pEngine->initialize(pScreenContext);
		}
		else
		{
			return FALSE;
		}
	}

	void CRenderEngine::unInitialize(const tagScreenContext *pScreenContext)
	{
		if (m_pEngine != NULL)
		{
			m_pEngine->unInitialize(pScreenContext);
		}
	}
}