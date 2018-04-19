#include "OpenHMI.h"

#if defined(WIN32)
	#ifndef OS_WIN
	#define OS_WIN
	#endif
#endif

#if defined(__QNX__)
	#ifndef OS_QNX
	#define OS_QNX
	#endif
#endif

#if defined(__linux__)
	#ifndef OS_LINUX
	#define OS_LINUX
	#endif
#endif

#define BUILD_NEDMALLOC 0

#if (BUILD_NEDMALLOC)
	#ifndef NDEBUG
		#define NDEBUG
	#endif
	#define NEDMALLOCDEPRECATED
	#include "nedmalloc.c.h"
#else
	#include "malloc.h"
#endif

namespace OpenHMI
{

#ifndef PATH_MAX
#define PATH_MAX 260
#endif

#if defined(OS_QNX)
	#define MAX_PATH PATH_MAX
#endif

#if defined(OS_LINUX)
	#define MAX_PATH PATH_MAX
#endif

#if (BUILD_NEDMALLOC)
	const char* CONFIG_FILE_NAME	= "Memory.ini";
	/*	-----Example of Memory.ini ------
		use_allocator=true;
		init_pool_size=10485760;
		alloc_granularity=1048576;
	*/

	// Memory config items.
	BOOLEAN	g_bUseAlloctor			= FALSE; // Default
	INT		g_iInitPoolSize			= DEFAULT_GRANULARITY;
	INT		g_iAllocGranularity		= DEFAULT_GRANULARITY;

	class CMemoryConfig
	{
	public:
		CMemoryConfig()
		{
			m_acPath[0] = '\0';

			buildFilePath();
			if (loadData())
			{
				parseData();
			}
		}

		virtual ~CMemoryConfig()
		{
			;
		}

	private:

		void strCopy(char *pcDest, const char *pcSrc, UINT uiLen)
		{
			UINT i = 0;
			while (*pcSrc != '\0' && i < uiLen)
			{
				*pcDest++ = *pcSrc++;
				i++;
			}
			*pcDest = '\0';
		}

		BOOLEAN strEqual(const char* pcStr1, const char* pcStr2)
		{
			while ((*pcStr1 != NULL) && (*pcStr2 != NULL))
			{
				if ( *(pcStr1++) != *(pcStr2++) )
				{
					return FALSE;
				}
			}

			return (*pcStr1 == *pcStr2);
		}

#ifdef OS_WIN
		void buildFilePath()
		{
			INT	iLen = ::GetModuleFileNameA(NULL, m_acPath, MAX_PATH);
			while (iLen > 0)
			{
				if (m_acPath[iLen - 1] == '\\')
				{
					break;
				}
				iLen--;
			}

			strCopy(&(m_acPath[iLen]), CONFIG_FILE_NAME, 0xFFFF);
		}
#endif

#ifdef OS_QNX
		void buildFilePath()
		{
			getwd(m_acPath);

			INT iLen = ::strlen(m_acPath);
			m_acPath[iLen++] = '/';
			strCopy(&(m_acPath[iLen]), CONFIG_FILE_NAME, 0xFFFF);
		}
#endif

#ifdef OS_LINUX
		void buildFilePath()
		{
			char* pcPath = get_current_dir_name();

			if(pcPath)
			{				
				INT iLen = ::strlen(pcPath);
				strCopy(m_acPath, pcPath, iLen);
				m_acPath[iLen++] = '/';
				strCopy(&(m_acPath[iLen]), CONFIG_FILE_NAME, 0xFFFF);
				::free(pcPath);
			}
		}
#endif

		BOOLEAN loadData()
		{
			FILE *pFile = fopen(m_acPath, "rb");
			if (pFile == NULL)
			{
				return FALSE;
			}

			UINT uiReadCount = ::fread(m_acData, 1, DATA_BUF_SIZE - 1, pFile);
			m_acData[uiReadCount] = '\0';

			fclose(pFile);
			return TRUE;
		}

		void parseData()
		{
			char acName[32] = {0};
			char acValue[32] = {0};
			BOOLEAN bWantValue = FALSE;

			char* pCurChar = m_acData;
			char* pCpStart = pCurChar;

			while (*pCurChar != '\0')
			{
				if (*pCurChar == '=')
				{
					strCopy(acName, pCpStart, INT(pCurChar) - INT(pCpStart));
					pCurChar++;
					pCpStart = pCurChar;
					bWantValue = TRUE;
					continue;
				}

				if ((*pCurChar == '\r') || (*pCurChar == '\n') || (*pCurChar == ' ') ||
					(*pCurChar == '\t') || (*pCurChar == ';') )
				{
					if (bWantValue)
					{
						strCopy(acValue, pCpStart, INT(pCurChar) - INT(pCpStart));
						bWantValue = FALSE;
						decodeItem(acName, acValue);
					}
					pCurChar++;
					pCpStart = pCurChar;
					continue;
				}

				pCurChar++;
			}
		}

		void decodeItem(char* pcName, char* pcValue)
		{
			if (strEqual(pcName, "use_allocator"))
			{
				g_bUseAlloctor = (strEqual(pcValue, "true") || strEqual(pcValue, "1"));
			}
			else if (strEqual(pcName, "init_pool_size"))
			{
				g_iInitPoolSize = String::toInt((BYTE*)pcValue);
			}
			else if (strEqual(pcName, "alloc_granularity"))
			{
				g_iAllocGranularity = String::toInt((BYTE*)pcValue);
			}
		}

	private:
		static const INT	DATA_BUF_SIZE = 1024;

		char				m_acPath[MAX_PATH];
		char				m_acData[DATA_BUF_SIZE];
	};
#endif

	void* CMemManager::alloc(size_t size)
	{
		static INT s_iCount = 0;
		s_iCount++;

#if (BUILD_NEDMALLOC)
		if (s_iCount == 1)
		{
			// Read configs using CMemoryConfig.
			{
				CMemoryConfig memoryConfig;
			}
		
			//TODO: set Options for nedalloc here.
			if ( g_bUseAlloctor )
			{
				if (g_iAllocGranularity != DEFAULT_GRANULARITY)
				{
					mspace_mallopt(M_GRANULARITY, g_iAllocGranularity);
				}

				if (g_iInitPoolSize != DEFAULT_GRANULARITY)
				{
					nedalloc::InitPool(&nedalloc::syspool, g_iInitPoolSize, -1);
				}
			}
		}

		if (g_bUseAlloctor)
		{
			return nedalloc::nedmalloc(size);
		}
		else
		{
			return ::malloc(size);
		}
#else
		return ::malloc(size);
#endif
	}

	void  CMemManager::free(void* p)
	{
		if (!p)
		{
			return;
		}

#if (BUILD_NEDMALLOC)
		if (g_bUseAlloctor)
		{
			nedalloc::nedfree(p);
		}
		else
		{
			::free(p);
		}
#else
		::free(p);
#endif
	}
}