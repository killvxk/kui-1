#include "OpenHMI.h"

namespace OpenHMI
{
	BOOLEAN CFile::isExist(const CHAR * pcFilePath)
	{
#ifdef OS_WIN

		BOOLEAN bRet = FALSE;
		WIN32_FIND_DATA findData;
		HANDLE hFile = FindFirstFile(pcFilePath, &findData);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			bRet = TRUE;
		}

		FindClose(hFile);
		hFile = NULL;

		return bRet;
#endif

#ifdef OS_QNX
		
		char acFilePath[256];
		String::convertToMBCS(pcFilePath, acFilePath, 256);
		if(access(acFilePath, F_OK) != -1)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
#endif	

#ifdef OS_LINUX

		char acFilePath[256];
		String::convertToMBCS(pcFilePath, acFilePath, 256);
		if(access(acFilePath, F_OK) != -1)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
#endif
	}

	BOOLEAN	CFile::isReadOnly(const CHAR * pcFilePath)
	{
#ifdef OS_WIN

		DWORD dwAttrs;
		dwAttrs = GetFileAttributes(pcFilePath); 

		if (dwAttrs != INVALID_FILE_ATTRIBUTES && (dwAttrs & FILE_ATTRIBUTE_READONLY)) 
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
#endif

		return FALSE;
	}

	CFile::CFile() 
		: m_pFile(NULL)
	{
		;
	}

	CFile::~CFile()
	{
		close();
	}

	BOOLEAN CFile::open(const CHAR *pcFilePath, BOOLEAN bRead)
	{
		close();	

		if (bRead)
		{
			m_pFile = innerOpen(pcFilePath, L"rb");
		}
		else
		{
			m_pFile = innerOpen(pcFilePath, L"wb");
		}
		
		return m_pFile != NULL;
	}

	FILE * CFile::innerOpen(const CHAR *pcFilePath, const CHAR *pcMode)
	{
#ifdef OS_WIN
		FILE *pFile = NULL;
		_wfopen_s(&pFile, pcFilePath, pcMode);
		return pFile;
#endif

#ifdef OS_QNX
		char acFilePath[1024];
		char acMode[32];
		String::convertToMBCS(pcFilePath, acFilePath, 1024);
		String::convertToMBCS(pcMode, acMode, 32);

		return fopen(acFilePath, acMode);
#endif

#ifdef OS_LINUX
		char acFilePath[1024];
		char acMode[32];
		String::convertToMBCS(pcFilePath, acFilePath, 1024);
		String::convertToMBCS(pcMode, acMode, 32);

		return fopen(acFilePath, acMode);
#endif
	}

	void CFile::close()
	{   
		if (m_pFile)
		{
			if(::fclose(m_pFile) == 0)
			{
				m_pFile = NULL;
			}
		}
	}

	BOOLEAN CFile::isEmpty()
	{
		return getFileSize() == 0;
	}

	UINT CFile::getFileSize()
	{
		UINT uiFileSize = 0;
		UINT uiCurrentPosition;

		if (m_pFile)
		{
			uiCurrentPosition = ftell(m_pFile);
			seekTo(0, ST_END);
			uiFileSize = ftell(m_pFile);
			seekTo(uiCurrentPosition, ST_BEGIN);
		}

		return uiFileSize;
	}

	BOOLEAN CFile::writeData(const BYTE *pbtData, UINT uiDataLen)
	{
		if (m_pFile == NULL)
		{
			return FALSE;
		}

		if (pbtData != NULL && uiDataLen > 0)
		{
			UINT uiWriteCount = ::fwrite(pbtData, 1, uiDataLen, m_pFile);
			if (uiWriteCount < uiDataLen)
			{
				INT iErrorCode = ::ferror(m_pFile);
				if (iErrorCode)
				{
					;
				}

				return FALSE;
			}
		}

		return TRUE;
	}

	BOOLEAN CFile::readData(BYTE *pbtData, UINT uiDataLen)
	{
		if (m_pFile == NULL)
		{
			return FALSE;
		}

		if (pbtData != NULL && uiDataLen > 0)
		{
			UINT uiReadCount = ::fread(pbtData, 1, uiDataLen, m_pFile);
			if (uiReadCount < uiDataLen)
			{
				INT iErrorCode = ::ferror(m_pFile);
				if (iErrorCode)
				{
					;
				}

				return FALSE;
			}
		}

		return TRUE;
	}

	void CFile::seekTo(INT iDistance, ESeekType eSeekType)
	{
		if (m_pFile)
		{
			fseek(m_pFile, iDistance, eSeekType);
		}
	}
}