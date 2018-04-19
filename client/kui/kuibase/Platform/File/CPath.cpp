#include "OpenHMI.h"

#ifdef OS_WIN
	#include <atlpath.h>
#endif

namespace OpenHMI
{
#define FILE_PATH_SEPARATOR		L'/'
#define FILE_PATH_SEPARATOR_2	L'\\'
#define FILE_EXTERN_SEPARATOR	L'.'

	BOOLEAN CPath::getFileFolder(const CHAR *pcFilePath,  CHAR *pcFileFolder)
	{
		if (pcFilePath == NULL || pcFileFolder == NULL)
		{
			return FALSE;
		}		

		INT iFileNameStart = getFileNameStartIndex(pcFilePath);
		if (iFileNameStart == 0)
		{
			return FALSE;
		}

		String::strncpy(pcFileFolder, pcFilePath, (UINT)iFileNameStart);

		return TRUE;
	}

	String CPath::getFileFolder(const CHAR *pcFilePath)
	{
		if (pcFilePath == NULL)
		{
			return STRING_NULL;
		}		

		INT iFileNameStart = getFileNameStartIndex(pcFilePath);
		if (iFileNameStart == 0)
		{
			return STRING_NULL;
		}

		return String(pcFilePath, (UINT)iFileNameStart);
	}

	BOOLEAN CPath::getFileName(const CHAR *pcFilePath, CHAR *pcFileName)
	{
		if (pcFilePath == NULL || pcFileName == NULL)
		{
			return FALSE;
		}		

		INT iFileNameStart = getFileNameStartIndex(pcFilePath);

		String::strcpy(pcFileName, pcFilePath + iFileNameStart);
		
		return TRUE;
	}

	String CPath::getFileName(const CHAR *pcFilePath)
	{
		if (pcFilePath == NULL)
		{
			return STRING_NULL;
		}		

		INT iFileNameStart = getFileNameStartIndex(pcFilePath);

		return String(pcFilePath + iFileNameStart, 0xFFFF);
	}

	BOOLEAN CPath::getFileExtern(const CHAR *pcFilePath, CHAR *pcFileExtern)
	{
		if (pcFilePath == NULL || pcFileExtern == NULL)
		{
			return FALSE;
		}

		INT iFileNameStart = getFileNameStartIndex(pcFilePath);
	
		INT iDot = String::strlen(pcFilePath) - 1;
		while (iDot >= 0)
		{
			if (pcFilePath[iDot] == FILE_EXTERN_SEPARATOR)
			{
				break;
			}
			else
			{
				iDot--;
			}
		}
		iDot++;
	
		if (iDot <= iFileNameStart)
		{
			return FALSE;
		}
	
		String::strcpy(pcFileExtern, pcFilePath + iDot);
		
		return TRUE;
	}

	String CPath::getFileExtern(const CHAR *pcFilePath)
	{
		if (pcFilePath == NULL)
		{
			return STRING_NULL;
		}

		INT iFileNameStart = getFileNameStartIndex(pcFilePath);
	
		INT iDot = String::strlen(pcFilePath) - 1;
		while (iDot >= 0)
		{
			if (pcFilePath[iDot] == FILE_EXTERN_SEPARATOR)
			{
				break;
			}
			else
			{
				iDot--;
			}
		}
		iDot++;
	
		if (iDot <= iFileNameStart)
		{
			return STRING_NULL;
		}
	
		return String(pcFilePath + iDot, 0xFFFF);
	}

	String CPath::changeFileExtern(const CHAR *pcFileName, CHAR* pcNewExtern)
	{
		if (pcFileName == NULL)
		{
			return STRING_NULL;
		}

		INT iFileNameStart = getFileNameStartIndex(pcFileName);
	
		INT iDot = String::strlen(pcFileName) - 1;
		while (iDot >= 0)
		{
			if (pcFileName[iDot] == FILE_EXTERN_SEPARATOR)
			{
				break;
			}
			else
			{
				iDot--;
			}
		}
		iDot++;

		if (iDot <= iFileNameStart)
		{
			String strRet(pcFileName);
			strRet.append(L".");
			strRet.append(pcNewExtern);
			return strRet;
		}
	
		if ((pcNewExtern != NULL) || (String::strlen(pcNewExtern) > 0))
		{
			String strRet(pcFileName, iDot);
			strRet.append(pcNewExtern);
			return strRet;
		}
		else
		{
			return String(pcFileName, iDot - 1);
		}
	}

#ifdef OS_WIN
	String CPath::getApplicationFolder()
	{
		CHAR acExeFilePath[MAX_PATH] = {0};
		::GetModuleFileName(NULL, acExeFilePath, MAX_PATH);

		INT iLen = String::strlen(acExeFilePath);
		for (INT i = iLen - 1; i >= 0; i--)
		{
			if (acExeFilePath[i] == L'\\')
			{
				break;
			}

			acExeFilePath[i] = L'\0';
		}

		return acExeFilePath;
	}

	String CPath::generateRelativePath(const CHAR *pcPathParent, BOOLEAN bIsDirectory, const CHAR *pcPathChild)
	{
		if (pcPathParent == NULL || pcPathChild == NULL || *pcPathChild == NULL)
		{
			return STRING_NULL;
		}

		if (::PathIsRelative(pcPathChild))
		{
			return pcPathChild;
		}
		else
		{
			DWORD dwFrom = bIsDirectory ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;

			wchar_t wcRltPath[MAX_PATH] = {0};

			if (!::PathRelativePathTo(wcRltPath, pcPathParent, dwFrom, pcPathChild, FILE_ATTRIBUTE_NORMAL))
			{
				return pcPathChild;
			}
			else
			{
				return wcRltPath;
			}			
		}
	}

	String CPath::generateAbsolutePath(const CHAR *pcPathParent, const CHAR *pcPathChild)
	{
		if (pcPathParent == NULL || pcPathChild == NULL || *pcPathChild == NULL)
		{
			return STRING_NULL;
		}

		if (!::PathIsRelative(pcPathChild))
		{		
			return pcPathChild;
		}
		else
		{
			wchar_t wcAbsPath[MAX_PATH] = {0};

			if (::PathCombine(wcAbsPath, pcPathParent, pcPathChild) != NULL)
			{
				return wcAbsPath;
			}
			else
			{
				return pcPathChild;
			}
		}
	}
#endif

#ifdef OS_QNX
	String CPath::getApplicationFolder()
	{
		char acExeFilePath[PATH_MAX] = {0};
		getwd(acExeFilePath);
		if(strlen(acExeFilePath) > 0)
		{
			String strFolder;
			String::convertFromMBCS(acExeFilePath, strFolder);

			return strFolder + L"/";
		}
		else
		{
			return STRING_NULL;
		}
	}

	String CPath::generateRelativePath(const CHAR *pcPathParent, BOOLEAN bIsDirectory, const CHAR *pcPathChild)
	{
		if (pcPathParent == NULL || pcPathChild == NULL || pcPathChild[0] == L'\0' )
		{
			return STRING_NULL;
		}
		else
		{
			INT iParentIndex = 0;
			String strParent(pcPathParent);
			String strChild(pcPathChild);
			String strRet;
			BOOLEAN	bIsContain = FALSE;
			
			if (pcPathParent[0] == L'/')
				strParent = strParent.subString(1, strParent.length() - 1);
			if (strParent.lastIndexOf(L"/") != (strParent.length() - 1))
				strParent = strParent + L"/";
			
			if (strChild.indexOf(L"/") == 0)
			{
				strChild = strChild.subString(1, strChild.length() - 1);
			}
			
			while ((iParentIndex = strParent.indexOf(L"/")) >= 0)
			{
				String strP = strParent.subString(0, iParentIndex + 1);
				INT iChildIndex = strChild.indexOf(strP);
				if (iChildIndex == 0)
				{	
					strChild = strChild.subString(strP.length(), strChild.length() - strP.length() + 1);
					bIsContain = TRUE;
				}
				else
				{
					if (bIsContain)
					{
						strRet = strRet + L"../";
					}
					else
					{
						break;
					}
				}
				strParent = strParent.subString(iParentIndex + 1, strParent.length() - iParentIndex - 1);
			}
			if (strRet.length() == 0)
			{
				strRet = L"./";
			}
			return strRet + strChild;
		}
	}

	String CPath::generateAbsolutePath(const CHAR *pcPathParent, const CHAR *pcPathChild)
	{
		if (pcPathParent == NULL || pcPathChild == NULL || pcPathChild[0] == L'\0' )
		{
			return STRING_NULL;
		}
		
		String strParent(pcPathParent);
		String strChild(pcPathChild);

		if (strChild.indexOf(L"./") == 0)
		{
			return strParent + strChild.subString(2, strChild.length() - 2);
		}
		else if (strChild.indexOf(L"/") == 0)
		{
			return strChild;
		}
		else
		{
			INT	iChildIndex = 0;
			String strKey = L"../";
			
			if (pcPathParent[strParent.length() - 1] == L'/')
				strParent = strParent.subString(0, strParent.length() - 1);
				
			while ((iChildIndex = strChild.indexOf(strKey)) == 0)
			{
				INT iParentIndex = strParent.lastIndexOf(L"/");
				strParent = strParent.subString(0, iParentIndex);
				strChild = strChild.subString(strKey.length(), strChild.length() - strKey.length() + 1);
			}
		}
		return strParent + L"/" + strChild;
	}
#endif

#ifdef OS_LINUX
	String CPath::getApplicationFolder()
	{
		char * pcExeFilePath = get_current_dir_name();
		if(pcExeFilePath)
		{
			String strFolder;
			String::convertFromMBCS(pcExeFilePath, strFolder);			
			free(pcExeFilePath);

			return strFolder + L"/";
		}
		else
		{
			return STRING_NULL;
		}
	}

	String CPath::generateRelativePath(const CHAR *pcPathParent, BOOLEAN bIsDirectory, const CHAR *pcPathChild)
	{
		if (pcPathParent == NULL || pcPathChild == NULL || pcPathChild[0] == L'\0' )
		{
			return STRING_NULL;
		}
		else
		{
			INT iParentIndex = 0;
			String strParent(pcPathParent);
			String strChild(pcPathChild);
			String strRet;
			BOOLEAN	bIsContain = FALSE;
			
			if (pcPathParent[0] == L'/')
				strParent = strParent.subString(1, strParent.length() - 1);
			if (strParent.lastIndexOf(L"/") != (strParent.length() - 1))
				strParent = strParent + L"/";
			
			if (strChild.indexOf(L"/") == 0)
			{
				strChild = strChild.subString(1, strChild.length() - 1);
			}
			
			while ((iParentIndex = strParent.indexOf(L"/")) >= 0)
			{
				String strP = strParent.subString(0, iParentIndex + 1);
				INT iChildIndex = strChild.indexOf(strP);
				if (iChildIndex == 0)
				{	
					strChild = strChild.subString(strP.length(), strChild.length() - strP.length() + 1);
					bIsContain = TRUE;
				}
				else
				{
					if (bIsContain)
					{
						strRet = strRet + L"../";
					}
					else
					{
						break;
					}
				}
				strParent = strParent.subString(iParentIndex + 1, strParent.length() - iParentIndex - 1);
			}
			if (strRet.length() == 0)
			{
				strRet = L"./";
			}
			return strRet + strChild;
		}
	}

	String CPath::generateAbsolutePath(const CHAR *pcPathParent, const CHAR *pcPathChild)
	{
		if (pcPathParent == NULL || pcPathChild == NULL || pcPathChild[0] == L'\0' )
		{
			return STRING_NULL;
		}
		
		String strParent(pcPathParent);
		String strChild(pcPathChild);

		if (strChild.indexOf(L"./") == 0)
		{
			return strParent + strChild.subString(2, strChild.length() - 2);
		}
		else if (strChild.indexOf(L"/") == 0)
		{
			return strChild;
		}
		else
		{
			INT	iChildIndex = 0;
			String strKey = L"../";
			
			if (pcPathParent[strParent.length() - 1] == L'/')
				strParent = strParent.subString(0, strParent.length() - 1);
				
			while ((iChildIndex = strChild.indexOf(strKey)) == 0)
			{
				INT iParentIndex = strParent.lastIndexOf(L"/");
				strParent = strParent.subString(0, iParentIndex);
				strChild = strChild.subString(strKey.length(), strChild.length() - strKey.length() + 1);
			}
		}
		return strParent + L"/" + strChild;
	}
#endif

	INT CPath::getFileNameStartIndex(const CHAR *pcFilePath)
	{
		INT iLen = String::strlen(pcFilePath);

		INT iBackslash = iLen - 1;
		while (iBackslash >= 0)
		{
			if (   pcFilePath[iBackslash] == FILE_PATH_SEPARATOR
				|| pcFilePath[iBackslash] == FILE_PATH_SEPARATOR_2)
			{
				break;
			}
			else
			{
				iBackslash--;
			}
		}

		return iBackslash + 1;
	}
}
