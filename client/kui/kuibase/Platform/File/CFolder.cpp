#include "OpenHMI.h"

namespace OpenHMI
{
	CFolder::CFolder()
	{
		;
	}

	CFolder::~CFolder()
	{
		;
	}

#ifdef OS_WIN
	BOOLEAN CFolder::createFolder(const CHAR *pcPath)
	{
		String strPathName(pcPath);
		UINT uiLen = strPathName.length();

		if (uiLen == 0)
		{
			return FALSE;
		}

		if (INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(strPathName))
		{// the folder already exist.
			return TRUE;
		}

		INT iPos = strPathName.indexOf(L"\\", 0);	
		while (iPos != -1 && iPos < (INT)uiLen - 1)
		{
			String strSubPathName = strPathName.subString(0, iPos);

			if (INVALID_FILE_ATTRIBUTES == ::GetFileAttributes(strSubPathName))
			{// the folder doesn't exist			
				if (::CreateDirectory(strSubPathName, NULL) == FALSE)
				{
					return FALSE;
				}
			}

			iPos = strPathName.indexOf(L"\\", iPos + 1);
		}

		if (::CreateDirectory(strPathName, NULL) == FALSE)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	BOOLEAN CFolder::open(const CHAR *pcPath)
	{
		BOOLEAN bRet = TRUE;

		WIN32_FIND_DATAW FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		hFind = FindFirstFileW(String::format(L"%s\\%s", pcPath, L"*"), &FindFileData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			bRet = FALSE;
			return bRet;
		}
		else
		{
			if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
			{
				tagEntry * ptagEntry = new tagEntry;
				ptagEntry->bIsFolder = FALSE;
				ptagEntry->strName = String::format(L"%s", FindFileData.cFileName);
				m_aryEntry.addItem(ptagEntry);
			}
			else if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY 
				&& String::strcmp(FindFileData.cFileName, L".") != 0 
				&& String::strcmp(FindFileData.cFileName, L"..") != 0)
			{
				tagEntry * ptagEntry = new tagEntry;
				ptagEntry->bIsFolder = TRUE;
				ptagEntry->strName = String::format(L"%s", FindFileData.cFileName);
				ptagEntry->strPath = String::format(L"%s\\%s", pcPath, FindFileData.cFileName);
				m_aryEntry.addItem(ptagEntry);
			}

			while (FindNextFile(hFind, &FindFileData) != 0)
			{
				if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
				{   
					tagEntry * ptagEntry = new tagEntry;
					ptagEntry->bIsFolder = FALSE;
					ptagEntry->strName = String::format(L"%s", FindFileData.cFileName);
					ptagEntry->strPath = String::format(L"%s", pcPath);
					m_aryEntry.addItem(ptagEntry);
				}
				else if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
					&& String::strcmp(FindFileData.cFileName, L".") != 0
					&& String::strcmp(FindFileData.cFileName, L"..") != 0)
				{
					tagEntry * ptagEntry = new tagEntry;
					ptagEntry->bIsFolder = TRUE;
					ptagEntry->strName = String::format(L"%s", FindFileData.cFileName);
					ptagEntry->strPath = String::format(L"%s\\%s", pcPath, FindFileData.cFileName);
					m_aryEntry.addItem(ptagEntry);
				}
			}

			FindClose(hFind);
		}

		return bRet;
	}
#endif

#ifdef OS_QNX
	BOOLEAN CFolder::createFolder(const CHAR *pcPath)
	{
		return FALSE;
	}

	BOOLEAN CFolder::open(const CHAR *pcPath)
	{
		BOOLEAN bRet = TRUE;

		struct stat statbuf;
		struct dirent * dirp;
		char acPath[PATH_MAX] = {0};
		DIR	* dp;

		String::convertToMBCS(String::format(L"%ls/", pcPath), acPath, PATH_MAX);

		if ((dp = opendir(acPath)) == NULL)
		{
			bRet = FALSE;
		}
		while ((dirp = readdir(dp)) != NULL)
		{
			String strName;
			String::convertFromMBCS(dirp->d_name, strName);
			char acKey[PATH_MAX] = {0};
			strcpy(acKey, acPath);
			strcat(acKey, dirp->d_name);
			lstat(acKey, &statbuf);

			if (S_ISDIR(statbuf.st_mode)) 
			{
				if (String::strcmp(L".", strName) == 0 || String::strcmp(L"..", strName) == 0)
				{
					continue;
				}
				else
				{
					tagEntry * ptagEntry = new tagEntry;
					ptagEntry->bIsFolder = TRUE;
					ptagEntry->strName = String::format(L"%ls", (CHAR*)(const CHAR*)strName);
					ptagEntry->strPath = String::format(L"%ls/%ls", pcPath, (CHAR*)(const CHAR*)strName);
					m_aryEntry.addItem(ptagEntry);
				}
			}
			else
			{
				tagEntry * ptagEntry = new tagEntry;
				ptagEntry->bIsFolder = FALSE;
				ptagEntry->strName = String::format(L"%ls", (CHAR*)(const CHAR*)strName);
				m_aryEntry.addItem(ptagEntry);
			}
		}

		closedir(dp);

		return bRet;
	}
#endif

#ifdef OS_LINUX
	BOOLEAN CFolder::createFolder(const CHAR *pcPath)
	{
		return FALSE;
	}

	BOOLEAN CFolder::open(const CHAR *pcPath)
	{
		BOOLEAN bRet = TRUE;

		struct stat statbuf;
		struct dirent * dirp;
		char acPath[PATH_MAX] = {0};
		DIR	* dp;

		String::convertToMBCS(String::format(L"%ls/", pcPath), acPath, PATH_MAX);

		if ((dp = opendir(acPath)) == NULL)
		{
			bRet = FALSE;
		}
		while ((dirp = readdir(dp)) != NULL)
		{
			String strName;
			String::convertFromMBCS(dirp->d_name, strName);
			char acKey[PATH_MAX] = { 0 };
			strcpy(acKey, acPath);
			strcat(acKey, dirp->d_name);
			lstat(acKey, &statbuf);

			if (S_ISDIR(statbuf.st_mode)) 
			{
				if (String::strcmp(L".", strName) == 0 || String::strcmp(L"..", strName) == 0)
				{
					continue;
				}
				else
				{
					tagEntry * ptagEntry = new tagEntry;
					ptagEntry->bIsFolder = TRUE;
					ptagEntry->strName = String::format(L"%ls", (CHAR*)(const CHAR*)strName);
					ptagEntry->strPath = String::format(L"%ls/%ls", pcPath, (CHAR*)(const CHAR*)strName);
					m_aryEntry.addItem(ptagEntry);
				}
			}
			else
			{
				tagEntry * ptagEntry = new tagEntry;
				ptagEntry->bIsFolder = FALSE;
				ptagEntry->strName = String::format(L"%ls", (CHAR*)(const CHAR*)strName);
				m_aryEntry.addItem(ptagEntry);
			}
		}

		closedir(dp);

		return bRet;
	}
#endif

	void CFolder::close()
	{
		m_aryEntry.clear();
	}

	UINT CFolder::getEntryCount()
	{
		return m_aryEntry.size();
	}

	const CFolder::tagEntry* CFolder::getEntryByIndex(UINT uiIndex) const
	{
		if ( uiIndex > m_aryEntry.size() - 1)
		{
			return NULL;
		}
		else
		{
			return m_aryEntry[uiIndex];
		}
	}
}
	