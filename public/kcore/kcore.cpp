#include "stdafx.h"
#include <kcore.h>
#include <userenv.h>
#include <DbgHelp.h>
#include <time.h>
#include <TlHelp32.h>
#include <Sddl.h>
#include <algorithm>
#include <functional>
#include <IPHlpApi.h>
#include "resource.h"


#pragma	comment(lib, "dbghelp.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Shlwapi.lib")

HMODULE		g_hModule = NULL;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	WSADATA wsaData = {0};
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		WSAStartup(0x0202, &wsaData);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		WSACleanup();
		break;
	}

	if (ul_reason_for_call == DLL_PROCESS_ATTACH ||
		ul_reason_for_call == DLL_THREAD_ATTACH)
	{
		if (g_hModule == NULL)
			g_hModule = hModule;

		// 设置语言(服务器端)
		long lLanguageid = kcore::AfxGetSysLong(TEXT("config"), TEXT("languageid"));
		if (lLanguageid != 0)
		{
			kcore::AfxSetLangId((LANGID)lLanguageid);
		}
		else
		{
			// 设置语言(客户端)
			DWORD dwLanguageId = 0;
			DWORD dwRegType = 0, dwSize = sizeof(dwLanguageId);
			if (ERROR_SUCCESS == ::SHGetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet"),
				TEXT("languageid"), &dwRegType, &dwLanguageId, &dwSize) && dwRegType == REG_DWORD)
			{
				kcore::AfxSetLangId((LANGID)dwLanguageId);
			}
		}
	}
	return TRUE;
}

START_NAMESPACE_KCORE


bool __stdcall AfxIs64BitOpertionSystem()
{
#ifndef _WIN64
	typedef BOOL(WINAPI *LPFNISWOW64PROCESS)(HANDLE, PBOOL);

	BOOL bIsWow64 = FALSE;
	LPFNISWOW64PROCESS fnIsWow64Process = (LPFNISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	if (NULL != fnIsWow64Process && fnIsWow64Process(GetCurrentProcess(), &bIsWow64) && bIsWow64)
	{
		return true;
	}
	return false;
#else
	return true;
#endif
	
}

LONG __stdcall AfxUnHandledExceptionFilter(PEXCEPTION_POINTERS pExcept)
{
	TCHAR szDir[MAX_PATH] = {0}, szExe[MAX_PATH] = {0}, szVer[MAX_PATH] = {0};
	GetModuleFileName(NULL, szDir, _countof(szDir));
	AfxGetFileVersion(szDir, szVer);

	LPTSTR pFile = PathFindFileName(szDir);
	LPTSTR pExt  = PathFindExtension(szDir);
	if (pFile != szDir && pExt != NULL)
	{
		*pExt = 0;
		lstrcpy(szExe, pFile);
	}
	else
	{
		lstrcpy(szExe, TEXT("Crash"));
	}
	PathRemoveFileSpec(szDir);
	PathAddBackslash(szDir);
	lstrcat(szDir, TEXT("Dump\\"));
	CreateDirectory(szDir, NULL);

	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	TCHAR szModuleName[MAX_PATH] = {0};
	_stprintf_s(szModuleName, _countof(szModuleName), TEXT("%s%s-%s-%04d%02d%02d-%02d%02d%02d.dmp"), szDir, szExe, szVer, 
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	HANDLE hFile = CreateFile(szModuleName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo = {0};
		ExInfo.ThreadId = GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExcept;
		ExInfo.ClientPointers = false;

		BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
		CloseHandle(hFile);
	}
	return 0;
}

LPTSTR __stdcall AfxGetAppPath(LPTSTR szPath, DWORD dwSize /*= MAX_PATH*/, LPCTSTR szAppend /*= NULL*/)
{
	GetModuleFileName(NULL, szPath, dwSize);
	if (PathRemoveFileSpec(szPath))
	{
		PathAddBackslash(szPath);
	}
	if (szAppend != NULL)
	{
		lstrcat(szPath, szAppend);
	}
	return szPath;
}

void __stdcall AfxCreateDirectory(LPCTSTR szDirectory, size_t offset /*= 0*/)
{
	tstring dir(szDirectory);
	size_t end = dir.rfind(TEXT('\\'));
	size_t pt  = dir.find(TEXT('\\'), offset);

	while(pt != tstring::npos && pt<=end)
	{		
		tstring path = dir.substr(0, pt+1);
		DWORD dwRet = GetFileAttributes(path.c_str());
		if(dwRet == INVALID_FILE_ATTRIBUTES)			
			::CreateDirectory(path.c_str(), NULL);
		pt = dir.find(TEXT('\\'), pt+1);
	}
}

void AfxDeleteDirectory(LPCTSTR szDirectory, HANDLE hExit /* = NULL*/)
{
	TCHAR szRoot[MAX_PATH] = {0}, szPath[MAX_PATH] = {0};
	lstrcpy(szRoot, szDirectory);
	PathAddBackslash(szRoot);
	_stprintf_s(szPath, _countof(szPath), TEXT("%s*"), szRoot);

	WIN32_FIND_DATA wfd = {0};
	HANDLE hFile = FindFirstFile(szPath, &wfd);
	if (hFile == INVALID_HANDLE_VALUE)
		return ;

	do 
	{
		if (hExit != NULL && WaitForSingleObject(hExit, 0) == WAIT_OBJECT_0)
			break;
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmpi(wfd.cFileName, TEXT(".")) != 0 && lstrcmpi(wfd.cFileName, TEXT("..")) != 0)
			{
				TCHAR szSubDir[MAX_PATH] = {0};
				_stprintf_s(szSubDir, _countof(szSubDir), TEXT("%s%s\\"), szRoot, wfd.cFileName);
				AfxDeleteDirectory(szSubDir, hExit);
			}
		}
		else
		{
			TCHAR szFile[MAX_PATH] = {0};
			_stprintf_s(szFile, _countof(szFile), TEXT("%s%s"), szRoot, wfd.cFileName);
			SetFileAttributes(szFile, FILE_ATTRIBUTE_NORMAL);
			DeleteFile(szFile);
		}
	} while (FindNextFile(hFile, &wfd));
	FindClose(hFile);
	RemoveDirectory(szRoot);
}

static void AfxGetAllDirectory(LPCTSTR szDirectory, std::vector<tstring>& dirlist, HANDLE hExit)
{
	WIN32_FIND_DATA wfd = {0};
	TCHAR szPath[MAX_PATH] = {0};
	_stprintf_s(szPath, _countof(szPath), TEXT("%s*"), szDirectory);
	HANDLE hFile = FindFirstFile(szPath, &wfd);
	if (hFile == INVALID_HANDLE_VALUE)
		return ;

	dirlist.push_back(szDirectory);
	do
	{
		if (hExit != NULL && WaitForSingleObject(hExit, 0) == WAIT_OBJECT_0)
			break;
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmpi(wfd.cFileName, TEXT(".")) != 0 && lstrcmpi(wfd.cFileName, TEXT("..")) != 0)
			{
				tstring subdir = tstring(szDirectory) + wfd.cFileName + TEXT("\\");
				AfxGetAllDirectory(subdir.c_str(), dirlist, hExit);
			}
		}
	} while (FindNextFile(hFile, &wfd));
	FindClose(hFile);
}

void AfxDeleteMatchFile(LPCTSTR szDirectory, LPCTSTR szFilter, HANDLE hExit /*= NULL*/)
{
	TCHAR szRoot[MAX_PATH] = {0}, szPath[MAX_PATH] = {0};
	lstrcpy(szRoot, szDirectory);
	PathAddBackslash(szRoot);

	std::vector<tstring> subdirs;
	AfxGetAllDirectory(szRoot, subdirs, hExit);
	for (std::vector<tstring>::iterator it = subdirs.begin(); it != subdirs.end(); it++)
	{
		if (hExit != NULL && WaitForSingleObject(hExit, 0) == WAIT_OBJECT_0)
			break;
		WIN32_FIND_DATA wfd = {0};
		_stprintf_s(szPath, _countof(szPath), TEXT("%s%s"), it->c_str(), szFilter);
		HANDLE hFile = FindFirstFile(szPath, &wfd);
		if (hFile == INVALID_HANDLE_VALUE)
			continue ;

		do 
		{
			if (hExit != NULL && WaitForSingleObject(hExit, 0) == WAIT_OBJECT_0)
				break;
			if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				tstring file = (*it) +wfd.cFileName;
				SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
				DeleteFile(file.c_str());
			}
		} while (FindNextFile(hFile, &wfd));
		FindClose(hFile);
	}
}

LPTSTR __stdcall AfxGetWinErrorText(LPTSTR buf, DWORD dwSize, DWORD dwError)
{
	LPTSTR lpszTemp = NULL;
	DWORD nCharNums = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
		NULL,
		dwError,
		LANG_NEUTRAL,
		(LPTSTR)&lpszTemp,
		0,
		NULL);

	if (!nCharNums || dwSize <= nCharNums)
		buf[0] = 0;
	else
	{
		if (nCharNums > 2)
		{
			nCharNums -= 2;
			lpszTemp[nCharNums] = 0;
		}
		lstrcpyn(buf, lpszTemp, dwSize - 1);
	}
	if ( lpszTemp )
		LocalFree((HLOCAL) lpszTemp );
	return buf;
}

LPTSTR __stdcall AfxGetFileVersion(LPCTSTR pFileName, LPTSTR pVersion)
{
	tstring Version(TEXT("0.0.0.0"));
	tstring FileName(pFileName);
	if (PathIsRelative(pFileName))
	{
		TCHAR szFileName[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, szFileName, _countof(szFileName));
		PathRemoveFileSpec(szFileName);
		PathAddBackslash(szFileName);
		lstrcat(szFileName, pFileName);
		FileName = szFileName;
	}
	DWORD dwSize = GetFileVersionInfoSize(FileName.c_str(), NULL);
	if (dwSize)
	{
		LPTSTR pblock = new TCHAR[dwSize + 1];
		GetFileVersionInfo(pFileName, 0, dwSize, pblock);
		UINT nQuerySize;
		//DWORD* pTransTable = NULL;
		//VerQueryValue(pblock, TEXT("\\VarFileInfo\\Translation"), (void **)&pTransTable, &nQuerySize);
		//LONG m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
		//TCHAR SubBlock[MAX_PATH] = {0};
		//_stprintf_s(SubBlock, MAX_PATH, TEXT("\\StringFileInfo\\%08lx\\FileVersion"), m_dwLangCharset);	
		VS_FIXEDFILEINFO* pFixedVer = NULL;
		if (VerQueryValue(pblock, TEXT("\\"), (void**)&pFixedVer, &nQuerySize))
		{
			TCHAR szVerStr[MAX_PATH] = { 0 };
			_stprintf_s(szVerStr, TEXT("%d.%d.%d.%d"), pFixedVer->dwFileVersionMS >> 16, pFixedVer->dwFileVersionMS & 0xffff,
				pFixedVer->dwFileVersionLS >> 16, pFixedVer->dwFileVersionLS & 0xffff);
			Version = szVerStr;
		}
		delete[] pblock;
	}

	// replace ',' to '.'
// 	std::replace_if(Version.begin(), Version.end(),
// 		std::bind2nd(std::equal_to<tstring::value_type>(), ','), '.');

	// delete [Space]
// 	tstring::iterator iter = std::remove_if(Version.begin(), Version.end(),
// 		std::bind2nd(std::equal_to<tstring::value_type>(), ' '));

	// remove redundant character
// 	if (iter != Version.end())
// 		Version.erase(iter, Version.end());

	lstrcpy(pVersion, Version.c_str());
	return pVersion;
}

bool AfxGetAccountSid(LPCTSTR AccountName, PSID *Sid)
{
	PSID pSID = NULL;
	LPTSTR DomainName = NULL;
	SID_NAME_USE SIDNameUse;
	bool  bDone = false;

	try
	{
		DWORD cbSid = 0, cbDomainName = 0;
		if(!LookupAccountName(NULL, AccountName, pSID, &cbSid, DomainName, &cbDomainName, &SIDNameUse))
		{
			pSID = (PSID)malloc(cbSid);
			DomainName = (LPTSTR)malloc(cbDomainName * sizeof(TCHAR));
			if(!pSID || !DomainName)
				throw;

			if(!LookupAccountName(NULL, AccountName, pSID, &cbSid, DomainName, &cbDomainName, &SIDNameUse))
				throw;
			bDone = true;
		}
	}
	catch(...) { }
	if(DomainName) free(DomainName);
	if(!bDone && pSID) LocalFree(pSID);
	if(bDone) *Sid = pSID;
	return bDone;
}

LPTSTR __stdcall AfxGetCurUserHKeyRoot(LPTSTR HkeyRoot, DWORD dwSize)
{
	HANDLE token = NULL;
	LPTSTR sid = NULL;
	HANDLE ph = OpenProcess (PROCESS_ALL_ACCESS, FALSE, AfxGetProcIdByName(TEXT("explorer.exe")));
	try
	{
		if (HkeyRoot == NULL || dwSize < 2)
			throw ERROR_INVALID_PARAMETER;
		HkeyRoot[0] = 0;

		if (ph == NULL || !OpenProcessToken (ph, TOKEN_QUERY | TOKEN_DUPLICATE, &token) || !ImpersonateLoggedOnUser(token))
			throw GetLastError();

		TCHAR szBuf[MAX_PATH] = {0};
		DWORD dwRet = MAX_PATH;
		if (!GetUserName(szBuf, &dwRet) || !RevertToSelf())
			throw GetLastError();

		PSID pSid = NULL;
		if (!AfxGetAccountSid(szBuf, &pSid) ||!ConvertSidToStringSid(pSid, &sid))
			throw GetLastError();

		lstrcpyn(HkeyRoot, sid, dwSize-1);
	}
	catch (...) {}
	if (ph != NULL)
		CloseHandle(ph);
	if (token != NULL)
		CloseHandle(token);
	if (sid != NULL)
		LocalFree(sid);
	return HkeyRoot;
}

LPTSTR __stdcall AfxGetSysString(LPCTSTR Section, LPCTSTR Key, LPTSTR Value, DWORD dwSize, LPCTSTR Default)
{
	TCHAR szFile[MAX_PATH] = {0};
	AfxGetAppPath(szFile, MAX_PATH, TEXT("config\\config.ini"));
	GetPrivateProfileString(Section, Key, Default, Value, dwSize, szFile);
	return Value;
}

TCHAR g_strLangNameInfo[0xffff][20];
int GetSystemDefaultLocaleNameEx(LPTSTR Value, DWORD dwSize)
{
	static bool iLagInit = false;
	if (!iLagInit)
	{
		for (int i = 0; i < 0xffff; ++i)
			lstrcpy(g_strLangNameInfo[i], TEXT("zh-CN"));

		lstrcpy(g_strLangNameInfo[0x0001], TEXT("ar"));
		lstrcpy(g_strLangNameInfo[0x0002], TEXT("bg"));
		lstrcpy(g_strLangNameInfo[0x0003], TEXT("ca"));
		lstrcpy(g_strLangNameInfo[0x0004], TEXT("zh-Hans"));
		lstrcpy(g_strLangNameInfo[0x0005], TEXT("cs"));
		lstrcpy(g_strLangNameInfo[0x0006], TEXT("da"));
		lstrcpy(g_strLangNameInfo[0x0007], TEXT("de"));
		lstrcpy(g_strLangNameInfo[0x0008], TEXT("el"));
		lstrcpy(g_strLangNameInfo[0x0009], TEXT("en"));
		lstrcpy(g_strLangNameInfo[0x000A], TEXT("es"));
		lstrcpy(g_strLangNameInfo[0x000B], TEXT("fi"));
		lstrcpy(g_strLangNameInfo[0x000C], TEXT("fr"));
		lstrcpy(g_strLangNameInfo[0x000D], TEXT("he"));
		lstrcpy(g_strLangNameInfo[0x000E], TEXT("hu"));
		lstrcpy(g_strLangNameInfo[0x000F], TEXT("is"));
		lstrcpy(g_strLangNameInfo[0x0010], TEXT("it"));
		lstrcpy(g_strLangNameInfo[0x0011], TEXT("ja"));
		lstrcpy(g_strLangNameInfo[0x0012], TEXT("ko"));
		lstrcpy(g_strLangNameInfo[0x0013], TEXT("nl"));
		lstrcpy(g_strLangNameInfo[0x0014], TEXT("no"));
		lstrcpy(g_strLangNameInfo[0x0015], TEXT("pl"));
		lstrcpy(g_strLangNameInfo[0x0016], TEXT("pt"));
		lstrcpy(g_strLangNameInfo[0x0017], TEXT("rm"));
		lstrcpy(g_strLangNameInfo[0x0018], TEXT("ro"));
		lstrcpy(g_strLangNameInfo[0x0019], TEXT("ru"));
		lstrcpy(g_strLangNameInfo[0x001A], TEXT("hr"));
		lstrcpy(g_strLangNameInfo[0x001B], TEXT("sk"));
		lstrcpy(g_strLangNameInfo[0x001C], TEXT("sq"));
		lstrcpy(g_strLangNameInfo[0x001D], TEXT("sv"));
		lstrcpy(g_strLangNameInfo[0x001E], TEXT("th"));
		lstrcpy(g_strLangNameInfo[0x001F], TEXT("tr"));
		lstrcpy(g_strLangNameInfo[0x0020], TEXT("ur"));
		lstrcpy(g_strLangNameInfo[0x0021], TEXT("id"));
		lstrcpy(g_strLangNameInfo[0x0022], TEXT("uk"));
		lstrcpy(g_strLangNameInfo[0x0023], TEXT("be"));
		lstrcpy(g_strLangNameInfo[0x0024], TEXT("sl"));
		lstrcpy(g_strLangNameInfo[0x0025], TEXT("et"));
		lstrcpy(g_strLangNameInfo[0x0026], TEXT("lv"));
		lstrcpy(g_strLangNameInfo[0x0027], TEXT("lt"));
		lstrcpy(g_strLangNameInfo[0x0028], TEXT("tg"));
		lstrcpy(g_strLangNameInfo[0x0029], TEXT("fa"));
		lstrcpy(g_strLangNameInfo[0x002A], TEXT("vi"));
		lstrcpy(g_strLangNameInfo[0x002B], TEXT("hy"));
		lstrcpy(g_strLangNameInfo[0x002C], TEXT("az"));
		lstrcpy(g_strLangNameInfo[0x002D], TEXT("eu"));
		lstrcpy(g_strLangNameInfo[0x002E], TEXT("hsb"));
		lstrcpy(g_strLangNameInfo[0x002F], TEXT("mk"));
		lstrcpy(g_strLangNameInfo[0x0032], TEXT("tn"));
		lstrcpy(g_strLangNameInfo[0x0034], TEXT("xh"));
		lstrcpy(g_strLangNameInfo[0x0035], TEXT("zu"));
		lstrcpy(g_strLangNameInfo[0x0036], TEXT("af"));
		lstrcpy(g_strLangNameInfo[0x0037], TEXT("ka"));
		lstrcpy(g_strLangNameInfo[0x0038], TEXT("fo"));
		lstrcpy(g_strLangNameInfo[0x0039], TEXT("hi"));
		lstrcpy(g_strLangNameInfo[0x003A], TEXT("mt"));
		lstrcpy(g_strLangNameInfo[0x003B], TEXT("se"));
		lstrcpy(g_strLangNameInfo[0x003C], TEXT("ga"));
		lstrcpy(g_strLangNameInfo[0x003E], TEXT("ms"));
		lstrcpy(g_strLangNameInfo[0x003F], TEXT("kk"));
		lstrcpy(g_strLangNameInfo[0x0040], TEXT("ky"));
		lstrcpy(g_strLangNameInfo[0x0041], TEXT("sw"));
		lstrcpy(g_strLangNameInfo[0x0042], TEXT("tk"));
		lstrcpy(g_strLangNameInfo[0x0043], TEXT("uz"));
		lstrcpy(g_strLangNameInfo[0x0044], TEXT("tt"));
		lstrcpy(g_strLangNameInfo[0x0045], TEXT("bn"));
		lstrcpy(g_strLangNameInfo[0x0046], TEXT("pa"));
		lstrcpy(g_strLangNameInfo[0x0047], TEXT("gu"));
		lstrcpy(g_strLangNameInfo[0x0048], TEXT("or"));
		lstrcpy(g_strLangNameInfo[0x0049], TEXT("ta"));
		lstrcpy(g_strLangNameInfo[0x004A], TEXT("te"));
		lstrcpy(g_strLangNameInfo[0x004B], TEXT("kn"));
		lstrcpy(g_strLangNameInfo[0x004C], TEXT("ml"));
		lstrcpy(g_strLangNameInfo[0x004D], TEXT("as"));
		lstrcpy(g_strLangNameInfo[0x004E], TEXT("mr"));
		lstrcpy(g_strLangNameInfo[0x004F], TEXT("sa"));
		lstrcpy(g_strLangNameInfo[0x0050], TEXT("mn"));
		lstrcpy(g_strLangNameInfo[0x0051], TEXT("bo"));
		lstrcpy(g_strLangNameInfo[0x0052], TEXT("cy"));
		lstrcpy(g_strLangNameInfo[0x0053], TEXT("km"));
		lstrcpy(g_strLangNameInfo[0x0054], TEXT("lo"));
		lstrcpy(g_strLangNameInfo[0x0056], TEXT("gl"));
		lstrcpy(g_strLangNameInfo[0x0057], TEXT("kok"));
		lstrcpy(g_strLangNameInfo[0x005A], TEXT("syr"));
		lstrcpy(g_strLangNameInfo[0x005B], TEXT("si"));
		lstrcpy(g_strLangNameInfo[0x005D], TEXT("iu"));
		lstrcpy(g_strLangNameInfo[0x005E], TEXT("am"));
		lstrcpy(g_strLangNameInfo[0x005F], TEXT("tzm"));
		lstrcpy(g_strLangNameInfo[0x0061], TEXT("ne"));
		lstrcpy(g_strLangNameInfo[0x0062], TEXT("fy"));
		lstrcpy(g_strLangNameInfo[0x0063], TEXT("ps"));
		lstrcpy(g_strLangNameInfo[0x0064], TEXT("fil"));
		lstrcpy(g_strLangNameInfo[0x0065], TEXT("dv"));
		lstrcpy(g_strLangNameInfo[0x0068], TEXT("ha"));
		lstrcpy(g_strLangNameInfo[0x006A], TEXT("yo"));
		lstrcpy(g_strLangNameInfo[0x006B], TEXT("quz"));
		lstrcpy(g_strLangNameInfo[0x006C], TEXT("nso"));
		lstrcpy(g_strLangNameInfo[0x006D], TEXT("ba"));
		lstrcpy(g_strLangNameInfo[0x006E], TEXT("lb"));
		lstrcpy(g_strLangNameInfo[0x006F], TEXT("kl"));
		lstrcpy(g_strLangNameInfo[0x0070], TEXT("ig"));
		lstrcpy(g_strLangNameInfo[0x0078], TEXT("ii"));
		lstrcpy(g_strLangNameInfo[0x007A], TEXT("arn"));
		lstrcpy(g_strLangNameInfo[0x007C], TEXT("moh"));
		lstrcpy(g_strLangNameInfo[0x007E], TEXT("br"));
		lstrcpy(g_strLangNameInfo[0x0080], TEXT("ug"));
		lstrcpy(g_strLangNameInfo[0x0081], TEXT("mi"));
		lstrcpy(g_strLangNameInfo[0x0082], TEXT("oc"));
		lstrcpy(g_strLangNameInfo[0x0083], TEXT("co"));
		lstrcpy(g_strLangNameInfo[0x0084], TEXT("gsw"));
		lstrcpy(g_strLangNameInfo[0x0085], TEXT("sah"));
		lstrcpy(g_strLangNameInfo[0x0086], TEXT("qut"));
		lstrcpy(g_strLangNameInfo[0x0087], TEXT("rw"));
		lstrcpy(g_strLangNameInfo[0x0088], TEXT("wo"));
		lstrcpy(g_strLangNameInfo[0x008C], TEXT("prs"));
		lstrcpy(g_strLangNameInfo[0x0091], TEXT("gd"));
		lstrcpy(g_strLangNameInfo[0x0401], TEXT("ar-SA"));
		lstrcpy(g_strLangNameInfo[0x0402], TEXT("bg-BG"));
		lstrcpy(g_strLangNameInfo[0x0403], TEXT("ca-ES"));
		lstrcpy(g_strLangNameInfo[0x0404], TEXT("zh-TW"));
		lstrcpy(g_strLangNameInfo[0x0405], TEXT("cs-CZ"));
		lstrcpy(g_strLangNameInfo[0x0406], TEXT("da-DK"));
		lstrcpy(g_strLangNameInfo[0x0407], TEXT("de-DE"));
		lstrcpy(g_strLangNameInfo[0x0408], TEXT("el-GR"));
		lstrcpy(g_strLangNameInfo[0x0409], TEXT("en-US"));
		lstrcpy(g_strLangNameInfo[0x040B], TEXT("fi-FI"));
		lstrcpy(g_strLangNameInfo[0x040C], TEXT("fr-FR"));
		lstrcpy(g_strLangNameInfo[0x040D], TEXT("he-IL"));
		lstrcpy(g_strLangNameInfo[0x040E], TEXT("hu-HU"));
		lstrcpy(g_strLangNameInfo[0x040F], TEXT("is-IS"));
		lstrcpy(g_strLangNameInfo[0x0410], TEXT("it-IT"));
		lstrcpy(g_strLangNameInfo[0x0411], TEXT("ja-JP"));
		lstrcpy(g_strLangNameInfo[0x0412], TEXT("ko-KR"));
		lstrcpy(g_strLangNameInfo[0x0413], TEXT("nl-NL"));
		lstrcpy(g_strLangNameInfo[0x0414], TEXT("nb-NO"));
		lstrcpy(g_strLangNameInfo[0x0415], TEXT("pl-PL"));
		lstrcpy(g_strLangNameInfo[0x0416], TEXT("pt-BR"));
		lstrcpy(g_strLangNameInfo[0x0417], TEXT("rm-CH"));
		lstrcpy(g_strLangNameInfo[0x0418], TEXT("ro-RO"));
		lstrcpy(g_strLangNameInfo[0x0419], TEXT("ru-RU"));
		lstrcpy(g_strLangNameInfo[0x041A], TEXT("hr-HR"));
		lstrcpy(g_strLangNameInfo[0x041B], TEXT("sk-SK"));
		lstrcpy(g_strLangNameInfo[0x041C], TEXT("sq-AL"));
		lstrcpy(g_strLangNameInfo[0x041D], TEXT("sv-SE"));
		lstrcpy(g_strLangNameInfo[0x041E], TEXT("th-TH"));
		lstrcpy(g_strLangNameInfo[0x041F], TEXT("tr-TR"));
		lstrcpy(g_strLangNameInfo[0x0420], TEXT("ur-PK"));
		lstrcpy(g_strLangNameInfo[0x0421], TEXT("id-ID"));
		lstrcpy(g_strLangNameInfo[0x0422], TEXT("uk-UA"));
		lstrcpy(g_strLangNameInfo[0x0423], TEXT("be-BY"));
		lstrcpy(g_strLangNameInfo[0x0424], TEXT("sl-SI"));
		lstrcpy(g_strLangNameInfo[0x0425], TEXT("et-EE"));
		lstrcpy(g_strLangNameInfo[0x0426], TEXT("lv-LV"));
		lstrcpy(g_strLangNameInfo[0x0427], TEXT("lt-LT"));
		lstrcpy(g_strLangNameInfo[0x0428], TEXT("tg-Cyrl-TJ"));
		lstrcpy(g_strLangNameInfo[0x0429], TEXT("fa-IR"));
		lstrcpy(g_strLangNameInfo[0x042A], TEXT("vi-VN"));
		lstrcpy(g_strLangNameInfo[0x042B], TEXT("hy-AM"));
		lstrcpy(g_strLangNameInfo[0x042C], TEXT("az-Latn-AZ"));
		lstrcpy(g_strLangNameInfo[0x042D], TEXT("eu-ES"));
		lstrcpy(g_strLangNameInfo[0x042E], TEXT("hsb-DE"));
		lstrcpy(g_strLangNameInfo[0x042F], TEXT("mk-MK"));
		lstrcpy(g_strLangNameInfo[0x0432], TEXT("tn-ZA"));
		lstrcpy(g_strLangNameInfo[0x0434], TEXT("xh-ZA"));
		lstrcpy(g_strLangNameInfo[0x0435], TEXT("zu-ZA"));
		lstrcpy(g_strLangNameInfo[0x0436], TEXT("af-ZA"));
		lstrcpy(g_strLangNameInfo[0x0437], TEXT("ka-GE"));
		lstrcpy(g_strLangNameInfo[0x0438], TEXT("fo-FO"));
		lstrcpy(g_strLangNameInfo[0x0439], TEXT("hi-IN"));
		lstrcpy(g_strLangNameInfo[0x043A], TEXT("mt-MT"));
		lstrcpy(g_strLangNameInfo[0x043B], TEXT("se-NO"));
		lstrcpy(g_strLangNameInfo[0x043E], TEXT("ms-MY"));
		lstrcpy(g_strLangNameInfo[0x043F], TEXT("kk-KZ"));
		lstrcpy(g_strLangNameInfo[0x0440], TEXT("ky-KG"));
		lstrcpy(g_strLangNameInfo[0x0441], TEXT("sw-KE"));
		lstrcpy(g_strLangNameInfo[0x0442], TEXT("tk-TM"));
		lstrcpy(g_strLangNameInfo[0x0443], TEXT("uz-Latn-UZ"));
		lstrcpy(g_strLangNameInfo[0x0444], TEXT("tt-RU"));
		lstrcpy(g_strLangNameInfo[0x0445], TEXT("bn-IN"));
		lstrcpy(g_strLangNameInfo[0x0446], TEXT("pa-IN"));
		lstrcpy(g_strLangNameInfo[0x0447], TEXT("gu-IN"));
		lstrcpy(g_strLangNameInfo[0x0448], TEXT("or-IN"));
		lstrcpy(g_strLangNameInfo[0x0449], TEXT("ta-IN"));
		lstrcpy(g_strLangNameInfo[0x044A], TEXT("te-IN"));
		lstrcpy(g_strLangNameInfo[0x044B], TEXT("kn-IN"));
		lstrcpy(g_strLangNameInfo[0x044C], TEXT("ml-IN"));
		lstrcpy(g_strLangNameInfo[0x044D], TEXT("as-IN"));
		lstrcpy(g_strLangNameInfo[0x044E], TEXT("mr-IN"));
		lstrcpy(g_strLangNameInfo[0x044F], TEXT("sa-IN"));
		lstrcpy(g_strLangNameInfo[0x0450], TEXT("mn-MN"));
		lstrcpy(g_strLangNameInfo[0x0451], TEXT("bo-CN"));
		lstrcpy(g_strLangNameInfo[0x0452], TEXT("cy-GB"));
		lstrcpy(g_strLangNameInfo[0x0453], TEXT("km-KH"));
		lstrcpy(g_strLangNameInfo[0x0454], TEXT("lo-LA"));
		lstrcpy(g_strLangNameInfo[0x0456], TEXT("gl-ES"));
		lstrcpy(g_strLangNameInfo[0x0457], TEXT("kok-IN"));
		lstrcpy(g_strLangNameInfo[0x045A], TEXT("syr-SY"));
		lstrcpy(g_strLangNameInfo[0x045B], TEXT("si-LK"));
		lstrcpy(g_strLangNameInfo[0x045D], TEXT("iu-Cans-CA"));
		lstrcpy(g_strLangNameInfo[0x045E], TEXT("am-ET"));
		lstrcpy(g_strLangNameInfo[0x0461], TEXT("ne-NP"));
		lstrcpy(g_strLangNameInfo[0x0462], TEXT("fy-NL"));
		lstrcpy(g_strLangNameInfo[0x0463], TEXT("ps-AF"));
		lstrcpy(g_strLangNameInfo[0x0464], TEXT("fil-PH"));
		lstrcpy(g_strLangNameInfo[0x0465], TEXT("dv-MV"));
		lstrcpy(g_strLangNameInfo[0x0468], TEXT("ha-Latn-NG"));
		lstrcpy(g_strLangNameInfo[0x046A], TEXT("yo-NG"));
		lstrcpy(g_strLangNameInfo[0x046B], TEXT("quz-BO"));
		lstrcpy(g_strLangNameInfo[0x046C], TEXT("nso-ZA"));
		lstrcpy(g_strLangNameInfo[0x046D], TEXT("ba-RU"));
		lstrcpy(g_strLangNameInfo[0x046E], TEXT("lb-LU"));
		lstrcpy(g_strLangNameInfo[0x046F], TEXT("kl-GL"));
		lstrcpy(g_strLangNameInfo[0x0470], TEXT("ig-NG"));
		lstrcpy(g_strLangNameInfo[0x0478], TEXT("ii-CN"));
		lstrcpy(g_strLangNameInfo[0x047A], TEXT("arn-CL"));
		lstrcpy(g_strLangNameInfo[0x047C], TEXT("moh-CA"));
		lstrcpy(g_strLangNameInfo[0x047E], TEXT("br-FR"));
		lstrcpy(g_strLangNameInfo[0x0480], TEXT("ug-CN"));
		lstrcpy(g_strLangNameInfo[0x0481], TEXT("mi-NZ"));
		lstrcpy(g_strLangNameInfo[0x0482], TEXT("oc-FR"));
		lstrcpy(g_strLangNameInfo[0x0483], TEXT("co-FR"));
		lstrcpy(g_strLangNameInfo[0x0484], TEXT("gsw-FR"));
		lstrcpy(g_strLangNameInfo[0x0485], TEXT("sah-RU"));
		lstrcpy(g_strLangNameInfo[0x0486], TEXT("qut-GT"));
		lstrcpy(g_strLangNameInfo[0x0487], TEXT("rw-RW"));
		lstrcpy(g_strLangNameInfo[0x0488], TEXT("wo-SN"));
		lstrcpy(g_strLangNameInfo[0x048C], TEXT("prs-AF"));
		lstrcpy(g_strLangNameInfo[0x0491], TEXT("gd-GB"));
		lstrcpy(g_strLangNameInfo[0x0801], TEXT("ar-IQ"));
		lstrcpy(g_strLangNameInfo[0x0804], TEXT("zh-CN"));
		lstrcpy(g_strLangNameInfo[0x0807], TEXT("de-CH"));
		lstrcpy(g_strLangNameInfo[0x0809], TEXT("en-GB"));
		lstrcpy(g_strLangNameInfo[0x080A], TEXT("es-MX"));
		lstrcpy(g_strLangNameInfo[0x080C], TEXT("fr-BE"));
		lstrcpy(g_strLangNameInfo[0x0810], TEXT("it-CH"));
		lstrcpy(g_strLangNameInfo[0x0813], TEXT("nl-BE"));
		lstrcpy(g_strLangNameInfo[0x0814], TEXT("nn-NO"));
		lstrcpy(g_strLangNameInfo[0x0816], TEXT("pt-PT"));
		lstrcpy(g_strLangNameInfo[0x081A], TEXT("sr-Latn-CS"));
		lstrcpy(g_strLangNameInfo[0x081D], TEXT("sv-FI"));
		lstrcpy(g_strLangNameInfo[0x082C], TEXT("az-Cyrl-AZ"));
		lstrcpy(g_strLangNameInfo[0x082E], TEXT("dsb-DE"));
		lstrcpy(g_strLangNameInfo[0x083B], TEXT("se-SE"));
		lstrcpy(g_strLangNameInfo[0x083C], TEXT("ga-IE"));
		lstrcpy(g_strLangNameInfo[0x083E], TEXT("ms-BN"));
		lstrcpy(g_strLangNameInfo[0x0843], TEXT("uz-Cyrl-UZ"));
		lstrcpy(g_strLangNameInfo[0x0845], TEXT("bn-BD"));
		lstrcpy(g_strLangNameInfo[0x0850], TEXT("mn-Mong-CN"));
		lstrcpy(g_strLangNameInfo[0x085D], TEXT("iu-Latn-CA"));
		lstrcpy(g_strLangNameInfo[0x085F], TEXT("tzm-Latn-DZ"));
		lstrcpy(g_strLangNameInfo[0x086B], TEXT("quz-EC"));
		lstrcpy(g_strLangNameInfo[0x0C01], TEXT("ar-EG"));
		lstrcpy(g_strLangNameInfo[0x0C04], TEXT("zh-HK"));
		lstrcpy(g_strLangNameInfo[0x0C07], TEXT("de-AT"));
		lstrcpy(g_strLangNameInfo[0x0C09], TEXT("en-AU"));
		lstrcpy(g_strLangNameInfo[0x0C0A], TEXT("es-ES"));
		lstrcpy(g_strLangNameInfo[0x0C0C], TEXT("fr-CA"));
		lstrcpy(g_strLangNameInfo[0x0C1A], TEXT("sr-Cyrl-CS"));
		lstrcpy(g_strLangNameInfo[0x0C3B], TEXT("se-FI"));
		lstrcpy(g_strLangNameInfo[0x0C6B], TEXT("quz-PE"));
		lstrcpy(g_strLangNameInfo[0x1001], TEXT("ar-LY"));
		lstrcpy(g_strLangNameInfo[0x1004], TEXT("zh-SG"));
		lstrcpy(g_strLangNameInfo[0x1007], TEXT("de-LU"));
		lstrcpy(g_strLangNameInfo[0x1009], TEXT("en-CA"));
		lstrcpy(g_strLangNameInfo[0x100A], TEXT("es-GT"));
		lstrcpy(g_strLangNameInfo[0x100C], TEXT("fr-CH"));
		lstrcpy(g_strLangNameInfo[0x101A], TEXT("hr-BA"));
		lstrcpy(g_strLangNameInfo[0x103B], TEXT("smj-NO"));
		lstrcpy(g_strLangNameInfo[0x1401], TEXT("ar-DZ"));
		lstrcpy(g_strLangNameInfo[0x1404], TEXT("zh-MO"));
		lstrcpy(g_strLangNameInfo[0x1407], TEXT("de-LI"));
		lstrcpy(g_strLangNameInfo[0x1409], TEXT("en-NZ"));
		lstrcpy(g_strLangNameInfo[0x140A], TEXT("es-CR"));
		lstrcpy(g_strLangNameInfo[0x140C], TEXT("fr-LU"));
		lstrcpy(g_strLangNameInfo[0x141A], TEXT("bs-Latn-BA"));
		lstrcpy(g_strLangNameInfo[0x143B], TEXT("smj-SE"));
		lstrcpy(g_strLangNameInfo[0x1801], TEXT("ar-MA"));
		lstrcpy(g_strLangNameInfo[0x1809], TEXT("en-IE"));
		lstrcpy(g_strLangNameInfo[0x180A], TEXT("es-PA"));
		lstrcpy(g_strLangNameInfo[0x180C], TEXT("fr-MC"));
		lstrcpy(g_strLangNameInfo[0x181A], TEXT("sr-Latn-BA"));
		lstrcpy(g_strLangNameInfo[0x183B], TEXT("sma-NO"));
		lstrcpy(g_strLangNameInfo[0x1C01], TEXT("ar-TN"));
		lstrcpy(g_strLangNameInfo[0x1C09], TEXT("en-ZA"));
		lstrcpy(g_strLangNameInfo[0x1C0A], TEXT("es-DO"));
		lstrcpy(g_strLangNameInfo[0x1C1A], TEXT("sr-Cyrl-BA"));
		lstrcpy(g_strLangNameInfo[0x1C3B], TEXT("sma-SE"));
		lstrcpy(g_strLangNameInfo[0x2001], TEXT("ar-OM"));
		lstrcpy(g_strLangNameInfo[0x2009], TEXT("en-JM"));
		lstrcpy(g_strLangNameInfo[0x200A], TEXT("es-VE"));
		lstrcpy(g_strLangNameInfo[0x201A], TEXT("bs-Cyrl-BA"));
		lstrcpy(g_strLangNameInfo[0x203B], TEXT("sms-FI"));
		lstrcpy(g_strLangNameInfo[0x2401], TEXT("ar-YE"));
		lstrcpy(g_strLangNameInfo[0x2409], TEXT("en-029"));
		lstrcpy(g_strLangNameInfo[0x240A], TEXT("es-CO"));
		lstrcpy(g_strLangNameInfo[0x241A], TEXT("sr-Latn-RS"));
		lstrcpy(g_strLangNameInfo[0x243B], TEXT("smn-FI"));
		lstrcpy(g_strLangNameInfo[0x2801], TEXT("ar-SY"));
		lstrcpy(g_strLangNameInfo[0x2809], TEXT("en-BZ"));
		lstrcpy(g_strLangNameInfo[0x280A], TEXT("es-PE"));
		lstrcpy(g_strLangNameInfo[0x281A], TEXT("sr-Cyrl-RS"));
		lstrcpy(g_strLangNameInfo[0x2C01], TEXT("ar-JO"));
		lstrcpy(g_strLangNameInfo[0x2C09], TEXT("en-TT"));
		lstrcpy(g_strLangNameInfo[0x2C0A], TEXT("es-AR"));
		lstrcpy(g_strLangNameInfo[0x2C1A], TEXT("sr-Latn-ME"));
		lstrcpy(g_strLangNameInfo[0x3001], TEXT("ar-LB"));
		lstrcpy(g_strLangNameInfo[0x3009], TEXT("en-ZW"));
		lstrcpy(g_strLangNameInfo[0x300A], TEXT("es-EC"));
		lstrcpy(g_strLangNameInfo[0x301A], TEXT("sr-Cyrl-ME"));
		lstrcpy(g_strLangNameInfo[0x3401], TEXT("ar-KW"));
		lstrcpy(g_strLangNameInfo[0x3409], TEXT("en-PH"));
		lstrcpy(g_strLangNameInfo[0x340A], TEXT("es-CL"));
		lstrcpy(g_strLangNameInfo[0x3801], TEXT("ar-AE"));
		lstrcpy(g_strLangNameInfo[0x380A], TEXT("es-UY"));
		lstrcpy(g_strLangNameInfo[0x3C01], TEXT("ar-BH"));
		lstrcpy(g_strLangNameInfo[0x3C0A], TEXT("es-PY"));
		lstrcpy(g_strLangNameInfo[0x4001], TEXT("ar-QA"));
		lstrcpy(g_strLangNameInfo[0x4009], TEXT("en-IN"));
		lstrcpy(g_strLangNameInfo[0x400A], TEXT("es-BO"));
		lstrcpy(g_strLangNameInfo[0x4409], TEXT("en-MY"));
		lstrcpy(g_strLangNameInfo[0x440A], TEXT("es-SV"));
		lstrcpy(g_strLangNameInfo[0x4809], TEXT("en-SG"));
		lstrcpy(g_strLangNameInfo[0x480A], TEXT("es-HN"));
		lstrcpy(g_strLangNameInfo[0x4C0A], TEXT("es-NI"));
		lstrcpy(g_strLangNameInfo[0x500A], TEXT("es-PR"));
		lstrcpy(g_strLangNameInfo[0x540A], TEXT("es-US"));
		lstrcpy(g_strLangNameInfo[0x641A], TEXT("bs-Cyrl"));
		lstrcpy(g_strLangNameInfo[0x681A], TEXT("bs-Latn"));
		lstrcpy(g_strLangNameInfo[0x6C1A], TEXT("sr-Cyrl"));
		lstrcpy(g_strLangNameInfo[0x701A], TEXT("sr-Latn"));
		lstrcpy(g_strLangNameInfo[0x703B], TEXT("smn"));
		lstrcpy(g_strLangNameInfo[0x742C], TEXT("az-Cyrl"));
		lstrcpy(g_strLangNameInfo[0x743B], TEXT("sms"));
		lstrcpy(g_strLangNameInfo[0x7804], TEXT("zh"));
		lstrcpy(g_strLangNameInfo[0x7814], TEXT("nn"));
		lstrcpy(g_strLangNameInfo[0x781A], TEXT("bs"));
		lstrcpy(g_strLangNameInfo[0x782C], TEXT("az-Latn"));
		lstrcpy(g_strLangNameInfo[0x783B], TEXT("sma"));
		lstrcpy(g_strLangNameInfo[0x7843], TEXT("uz-Cyrl"));
		lstrcpy(g_strLangNameInfo[0x7850], TEXT("mn-Cyrl"));
		lstrcpy(g_strLangNameInfo[0x785D], TEXT("iu-Cans"));
		lstrcpy(g_strLangNameInfo[0x7C04], TEXT("zh-Hant"));
		lstrcpy(g_strLangNameInfo[0x7C14], TEXT("nb"));
		lstrcpy(g_strLangNameInfo[0x7C1A], TEXT("sr"));
		lstrcpy(g_strLangNameInfo[0x7C28], TEXT("tg-Cyrl"));
		lstrcpy(g_strLangNameInfo[0x7C2E], TEXT("dsb"));
		lstrcpy(g_strLangNameInfo[0x7C3B], TEXT("smj"));
		lstrcpy(g_strLangNameInfo[0x7C43], TEXT("uz-Latn"));
		lstrcpy(g_strLangNameInfo[0x7C50], TEXT("mn-Mong"));
		lstrcpy(g_strLangNameInfo[0x7C5D], TEXT("iu-Latn"));
		lstrcpy(g_strLangNameInfo[0x7C5F], TEXT("tzm-Latn"));
		lstrcpy(g_strLangNameInfo[0x7C68], TEXT("ha-Latn"));
		iLagInit = true;
	}

	LANGID langId = ::GetUserDefaultUILanguage();
	size_t len = lstrlen(g_strLangNameInfo[langId]);
	if (dwSize <= len)
		return 0;

	lstrcpy(Value, g_strLangNameInfo[langId]);
	return (int)len;
}

LPTSTR __stdcall AfxGetLangString(LPCTSTR Section, LPCTSTR Key, LPTSTR Value, DWORD dwSize, LPCTSTR Default)
{
	TCHAR szValue[MAX_PATH] = { 0 };
	AfxGetSysString(TEXT("config"), TEXT("language"), szValue, MAX_PATH);
	if (::lstrlen(szValue) <= 0 /*&& ::GetSystemDefaultLocaleName(szValue, MAX_PATH) <= 0*//*GetSystemDefaultLocaleNameEx(szValue, MAX_PATH) <= 0*/)
	{
		lstrcpy(Value, Default);
		return Value;
	}
	TCHAR szFile[MAX_PATH] = { 0 };
	AfxGetAppPath(szFile, MAX_PATH, TEXT("language\\"));
	lstrcat(szFile, szValue);
	lstrcat(szFile, _T("\\"));
	lstrcat(szValue, TEXT(".la"));
	lstrcat(szFile, szValue);
	::GetPrivateProfileString(Section, Key, Default, Value, dwSize, szFile);
	return Value;
}

LPTSTR __stdcall AfxGetLangStringEx(HANDLE hInstance, UINT uID, LPTSTR Value,
	DWORD dwSize, LPCTSTR Default)
{
	if (0 >= ::LoadString((HINSTANCE)hInstance, uID, Value, dwSize))
		::_tcscpy_s(Value, dwSize, Default);

	return Value;
}

LPTSTR __stdcall AfxGetLangDll(LPTSTR pPath, DWORD dwSize)
{
	TCHAR szLanguage[MAX_PATH] = { 0 };
	AfxGetSysString(TEXT("config"), TEXT("language"), szLanguage, _countof(szLanguage));
	if (_tcslen(szLanguage) <= 0 || _tcsicmp(_T("chs"), szLanguage) == 0)
		return pPath;

	TCHAR szExe[MAX_PATH] = { 0 }, szName[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szExe, _countof(szExe));
	LPTSTR pFile = PathFindFileName(szExe);
	LPTSTR pExt = PathFindExtension(szExe);
	if (pExt != NULL)
		*pExt = 0;
	_tcscpy_s(szName, _T("language\\"));
	_tcscat_s(szName, szLanguage);
	_tcscat_s(szName, _T("\\"));
	_tcscat_s(szName, pFile);
	_tcscat_s(szName, szLanguage);
	_tcscat_s(szName, _T(".dll"));
	AfxGetAppPath(pPath, dwSize, szName);
	return pPath;
}

#define PL_ULR_FIRST_STRING					TEXT("http://file.yungengxin.com/xml/")
#define PL_ULR_SECOND_STRING				TEXT("http://file.yungengxin.com/xml/")
#define USER_LOGIN_URL_STRING				TEXT("http://interface.yungengxin.com/login/validate")	//?username=xxx&password=md5(xxx)&hardid=xxx&svrver=&mac=
#define SERVER_INFO_URL_STRING				TEXT("http://interface.yungengxin.com/hardware/uploadNetbarServer")				//nid=xxx&serverinfo=
//server:str,cpu:str,mainboard:str,netcar:str,memory:str,memory:str,disk:str,os:str,softversion:str,updatestatus:str,vdiskstatus:str.
#define CLIENT_INFO_URL_STRING				TEXT("http://interface.yungengxin.com/hardware")		//?nid=xxx&zipfile=xxxx&zip file binary
#define GAME_CLICK_URL_STRING				TEXT("http://interface.yungengxin.com/stat/resClick")	//?nid=xxx&clicks=gid_xxx|gid_xxx&ramdomNum=string.
#define GAME_INDEX_FIRST_STRING				TEXT("http://file.yungengxin.com/zip/%u-%u.zip")
#define GAME_INDEX_SECOND_STRING			TEXT("http://file.yungengxin.com/zip/%u-%u.zip")
#define GAME_DOWN_URL_STRING				TEXT("http://interface.yungengxin.com/download")		//?nid=xxx&gid=xxx
#define GET_PNPLIST_URL_STRING				TEXT("http://interface.yungengxin.com/hardware/getNetworkCard")

#define DOMAIN_ZH_CN       _T("yungengxin") 
#define DOMAIN_TR_TR	   _T("netkafem") 

typedef enum
{
	PL_URL,
	USER_LOGIN_URL,
	SERVER_INFO_URL,
	CLIENT_INFO_URL,
	GAME_CLICK_URL,
	GAME_INDEX,
	GAME_DOWN_URL,
	GET_PNPLIST_URL,
}URL_TYPE;

LPCTSTR ReplaceDomain(LPTSTR lpDstBuffer, DWORD dwSize, LPTSTR lpSrcBuffer, LPCTSTR lpSrc, LPCTSTR lpDst)
{
	tstring strReplaceSrc = lpSrc;
	tstring strReplaceDst = lpDst;
	tstring strSrc = lpSrcBuffer;
	size_t nPos = strSrc.find(strReplaceSrc);
	if (nPos != tstring::npos)
		strSrc.replace(nPos, strReplaceSrc.length(), strReplaceDst);
	_tcscpy_s(lpDstBuffer, dwSize, strSrc.c_str());
	return lpDstBuffer;
}

LPSTR __stdcall AfxGetUrlString(LPSTR lpBuffer, DWORD dwSize, int nUrl)
{
	CPkgHelper pkg(lpBuffer, dwSize, 0, 0);

	TCHAR szValue[MAX_PATH] = { 0 };
	AfxGetSysString(TEXT("config"), TEXT("url"), szValue, MAX_PATH, _T("chs"));
	if (_tcsicmp(szValue, _T("trk")) == 0)
	{
		switch (nUrl)
		{
		case PL_URL:
		{
			pkg.Push(2);
			TCHAR szBuffer[MAX_PATH] = { 0 };
			ReplaceDomain(szBuffer, MAX_PATH, PL_ULR_FIRST_STRING, DOMAIN_ZH_CN, DOMAIN_TR_TR);
			pkg.PushString(szBuffer);
			ReplaceDomain(szBuffer, MAX_PATH, PL_ULR_SECOND_STRING, DOMAIN_ZH_CN, DOMAIN_TR_TR);
			pkg.PushString(szBuffer);
		}
		break;
		case USER_LOGIN_URL:
		{
			pkg.Push(1);
			TCHAR szBuffer[MAX_PATH] = { 0 };
			ReplaceDomain(szBuffer, MAX_PATH, USER_LOGIN_URL_STRING, DOMAIN_ZH_CN, DOMAIN_TR_TR);
			pkg.PushString(szBuffer);
		}
		break;
		case SERVER_INFO_URL:
		{
			pkg.Push(1);
			TCHAR szBuffer[MAX_PATH] = { 0 };
			ReplaceDomain(szBuffer, MAX_PATH, SERVER_INFO_URL_STRING, DOMAIN_ZH_CN, DOMAIN_TR_TR);
			pkg.PushString(szBuffer);
		}
		break;
		case CLIENT_INFO_URL:
		{
			pkg.Push(1);
			TCHAR szBuffer[MAX_PATH] = { 0 };
			ReplaceDomain(szBuffer, MAX_PATH, CLIENT_INFO_URL_STRING, DOMAIN_ZH_CN, DOMAIN_TR_TR);
			pkg.PushString(szBuffer);
		}
		break;
		case GAME_CLICK_URL:
		{
			pkg.Push(1);
			TCHAR szBuffer[MAX_PATH] = { 0 };
			ReplaceDomain(szBuffer, MAX_PATH, GAME_CLICK_URL_STRING, DOMAIN_ZH_CN, DOMAIN_TR_TR);
			pkg.PushString(szBuffer);
		}
		break;
		case GAME_INDEX:
		{
			pkg.Push(2);
			TCHAR szBuffer[MAX_PATH] = { 0 };
			ReplaceDomain(szBuffer, MAX_PATH, GAME_INDEX_FIRST_STRING, DOMAIN_ZH_CN, DOMAIN_TR_TR);
			pkg.PushString(szBuffer);
			ReplaceDomain(szBuffer, MAX_PATH, GAME_INDEX_SECOND_STRING, DOMAIN_ZH_CN, DOMAIN_TR_TR);
			pkg.PushString(szBuffer);
		}
		break;
		case GAME_DOWN_URL:
		{
			pkg.Push(1);
			TCHAR szBuffer[MAX_PATH] = { 0 };
			ReplaceDomain(szBuffer, MAX_PATH, GAME_DOWN_URL_STRING, DOMAIN_ZH_CN, DOMAIN_TR_TR);
			pkg.PushString(szBuffer);
		}
		break;
		case GET_PNPLIST_URL:
		{
			pkg.Push(1);
			TCHAR szBuffer[MAX_PATH] = { 0 };
			ReplaceDomain(szBuffer, MAX_PATH, GET_PNPLIST_URL_STRING, DOMAIN_ZH_CN, DOMAIN_TR_TR);
			pkg.PushString(szBuffer);
		}
		break;
		default:
			pkg.Push(0);
			break;
		}
	}
	else
	{
		switch (nUrl)
		{
		case PL_URL:
		{
			pkg.Push(2);
			pkg.PushString(PL_ULR_FIRST_STRING);
			pkg.PushString(PL_ULR_SECOND_STRING);
		}
		break;
		case USER_LOGIN_URL:
		{
			pkg.Push(1);
			pkg.PushString(USER_LOGIN_URL_STRING);
		}
		break;
		case SERVER_INFO_URL:
		{
			pkg.Push(1);
			pkg.PushString(SERVER_INFO_URL_STRING);
		}
		break;
		case CLIENT_INFO_URL:
		{
			pkg.Push(1);
			pkg.PushString(CLIENT_INFO_URL_STRING);
		}
		break;
		case GAME_CLICK_URL:
		{
			pkg.Push(1);
			pkg.PushString(GAME_CLICK_URL_STRING);
		}
		break;
		case GAME_INDEX:
		{
			pkg.Push(2);
			pkg.PushString(GAME_INDEX_FIRST_STRING);
			pkg.PushString(GAME_INDEX_SECOND_STRING);
		}
		break;
		case GAME_DOWN_URL:
		{
			pkg.Push(1);
			pkg.PushString(GAME_DOWN_URL_STRING);
		}
		break;
		case GET_PNPLIST_URL:
		{
			pkg.Push(1);
			pkg.PushString(GET_PNPLIST_URL_STRING);
		}
		break;
		default:
			pkg.Push(0);
			break;
		}
	}
	return lpBuffer;
}

//set language id
void __stdcall AfxSetLangId(WORD wLangID)
{
	if (0 == wLangID)
		return;

	LANGID lgidUI = ::GetSystemDefaultUILanguage();
	LANGID lgid = ::GetSystemDefaultLangID();
	if (lgidUI == (LANGID)wLangID && lgid == (LANGID)wLangID)
		return;

#ifndef _WIN64
	OSVERSIONINFO ovs;
	ovs.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (::GetVersionEx(&ovs))
	{
		if (ovs.dwPlatformId == VER_PLATFORM_WIN32_NT && ovs.dwMajorVersion >= 6 &&
			ovs.dwMinorVersion >= 0)
		{
			::SetThreadUILanguage((LANGID)wLangID);
		}
		else
			::SetThreadLocale(MAKELCID((LANGID)wLangID, SORT_DEFAULT));
	}
#else
	::SetThreadUILanguage((LANGID)wLangID);
#endif
}

LPTSTR __stdcall AfxGetIdcAreaName(LPCTSTR szGuid, LPTSTR szName, DWORD dwSize)
{
	static TCHAR __IdcGuid__[][40] = { ALL_AREA_GUID, DEF_AREA_GUID };

	static UINT __IdcClassNameId__[] = { IDS_KCORE_ALL_AREA_NAME, IDS_KCORE_DEF_AREA_NAME };

	for (size_t idx = 0; idx < _countof(__IdcClassNameId__); idx++)
	{
		if (lstrcmpi(szGuid, __IdcGuid__[idx]) == 0)
		{
			AfxGetLangStringEx(g_hModule,
				__IdcClassNameId__[idx], szName, dwSize);
			break;
		}
	}
	return szName;
}
LPTSTR __stdcall AfxGetIdcClassName(LPCTSTR szGuid, LPTSTR szName, DWORD dwSize)
{
	static TCHAR __IdcGuid__[][40] = { CLASS_WL_GUID, CLASS_DJ_GUID, CLASS_QP_GUID, CLASS_DZ_GUID, CLASS_LT_GUID,
		CLASS_WY_GUID, CLASS_XX_GUID, CLASS_PL_GUID, CLASS_CY_GUID, CLASS_YY_GUID, CLASS_GP_GUID, CLASS_GQ_GUID,
		CLASS_ZX_GUID, CLASS_XC_GUID, CLASS_SJ_GUID, CLASS_BG_GUID,CLASS_DPNP_GUID};

	static UINT __IdcClassNameId__[] = { IDS_KCORE_CLASS_WL_NAME, IDS_KCORE_CLASS_DJ_NAME, IDS_KCORE_CLASS_QP_NAME, 
		IDS_KCORE_CLASS_DZ_NAME, IDS_KCORE_CLASS_LT_NAME,IDS_KCORE_CLASS_WY_NAME, IDS_KCORE_CLASS_XX_NAME, 
		IDS_KCORE_CLASS_PL_NAME, IDS_KCORE_CLASS_CY_NAME, IDS_KCORE_CLASS_YY_NAME, IDS_KCORE_CLASS_GP_NAME, 
		IDS_KCORE_CLASS_GQ_NAME,IDS_KCORE_CLASS_ZX_NAME, IDS_KCORE_CLASS_XC_NAME, IDS_KCORE_CLASS_SJ_NAME, 
		IDS_KCORE_CLASS_BG_NAME, IDS_KCORE_CLASS_DPNP_NAME};

	//网吧自定义类型单独处理
	if (lstrcmpi(szGuid, CLASS_NB_GUID) == 0)
	{
		AfxGetLangStringEx(g_hModule, IDS_KCORE_CLASS_NB_NAME, szName, dwSize);
		return szName;
	}

	for (size_t idx = 0; idx < CLASS_TOTAL_COUNT; idx++)
	{
		if (lstrcmpi(szGuid, __IdcGuid__[idx]) == 0)
		{
			AfxGetLangStringEx(g_hModule,
				__IdcClassNameId__[idx], szName, dwSize);
			break;
		}
	}
	return szName;
}

DWORD __stdcall AfxGetSysLong(LPCTSTR Section, LPCTSTR Key, long Default)
{
	TCHAR szFile[MAX_PATH] = {0};
	AfxGetAppPath(szFile, MAX_PATH, TEXT("config\\config.ini"));
	return GetPrivateProfileInt(Section, Key, Default, szFile);
}

void   __stdcall AfxSetSysString(LPCTSTR Section, LPCTSTR Key, LPCTSTR Value)
{
	TCHAR szFile[MAX_PATH] = {0};
	AfxGetAppPath(szFile, MAX_PATH, TEXT("config\\config.ini"));
	WritePrivateProfileString(Section, Key, Value, szFile);
}

void   __stdcall AfxSetSysLong(LPCTSTR Section, LPCTSTR Key, long Value)
{
	TCHAR szFile[MAX_PATH] = {0}, szValue[MAX_PATH] = {0};
	_stprintf_s(szValue, _countof(szValue), TEXT("%d"), Value);
	AfxGetAppPath(szFile, MAX_PATH, TEXT("config\\config.ini"));
	WritePrivateProfileString(Section, Key, szValue, szFile);
}

LPTSTR __stdcall AfxCreateGuidString(LPTSTR guid)
{
	bool bInit = S_OK == ::CoInitialize(NULL);
#ifdef _UNICODE
	unsigned short* pStr = NULL;
#else
	unsigned char* pStr = NULL;
#endif // _UNICODE

	GUID Guid = {0};
	UuidCreate(&Guid);
	UuidToString(&Guid, &pStr);
	tstring str((LPTSTR)pStr);
	str.insert(0, TEXT("{")); str += TEXT("}");
	RpcStringFree(&pStr);
	std::transform(str.begin(), str.end(), str.begin(), _totupper);
	if (bInit)
		CoUninitialize();
	lstrcpy(guid, str.c_str());
	return guid;
}

LPTSTR __stdcall AfxFormatTimeString(DWORD time, LPTSTR buffer, DWORD dwSize)
{
	ZeroMemory(buffer, dwSize*sizeof(TCHAR));
	if (time)
	{
		__time32_t t = time;
		struct tm ptm = {0};
		_localtime32_s(&ptm, &t);
		_stprintf_s(buffer, dwSize - 1, TEXT("%04d-%02d-%02d %02d:%02d:%02d"), ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday,
			ptm.tm_hour, ptm.tm_min, ptm.tm_sec);
	}
	return buffer;
}

LPTSTR __stdcall AfxFormatIpString(DWORD ip, LPTSTR buffer, DWORD dwSize)
{
	if (ip)
	{
		_stprintf_s(buffer, dwSize - 1, TEXT("%d.%d.%d.%d"), (ip & 0xff), ((ip >> 8) & 0xff), ((ip >> 16) & 0xff), ((ip >> 24) & 0xff));
	}
	return buffer;
}

DWORD  __stdcall AfxFileTime2UtcTime(FILETIME& ft)
{
	union {
		unsigned long long ullt;
		unsigned long ult[2];
	};

	if(ft.dwLowDateTime<0xD53E8000) {
		--ft.dwHighDateTime;
		ult[0] = ft.dwLowDateTime+0x2AC18000;
	} else {
		ult[0] = ft.dwLowDateTime-0xD53E8000;
	}
	ult[1] = ft.dwHighDateTime-0x19DB1DE;
	ullt /= 0x989680;
	return ult[0];
}

void __stdcall AfxUtcTime2FileTime(DWORD time, FILETIME& ft)
{
	union 
	{
		unsigned long long ullt;
		unsigned long ult[2];
	};
	ullt = time;
	ullt *= 0x989680;
	ullt += 0x19DB1DED53E8000LL;
	ft.dwLowDateTime = ult[0];
	ft.dwHighDateTime = ult[1];
}



DWORD __stdcall AfxGetProcIdByName(LPCTSTR szProcName)
{
	DWORD dwId = 0;
	PROCESSENTRY32	pe = {sizeof(PROCESSENTRY32)};
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(!Process32First(hSnapshot,&pe))
		return 0;
	do
	{
		if(_tcsicmp(pe.szExeFile, szProcName) == 0)
		{   
			dwId = pe.th32ProcessID;
			break;
		}
		pe.dwSize = sizeof(PROCESSENTRY32);
	}while(Process32Next(hSnapshot,&pe));   
	CloseHandle(hSnapshot);
	return dwId;   
}

DWORD __stdcall AfxGetParentProcessId()
{
	typedef struct _PROCESS_BASIC_INFORMATION {
		DWORD_PTR ExitStatus;  
		DWORD_PTR PebBaseAddress;
		DWORD_PTR AffinityMask;
		DWORD_PTR BasePriority;
		ULONG_PTR UniqueProcessId;  
		ULONG_PTR InheritedFromUniqueProcessId;  
	} PROCESS_BASIC_INFORMATION;
	typedef LONG (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);  

	DWORD dwId = 0;
	PROCNTQSIP NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(GetModuleHandle(TEXT("ntdll")), "NtQueryInformationProcess");  
	if (!NtQueryInformationProcess)  
		return 0;  

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE, GetCurrentProcessId());  
	if (NULL == hProcess)
		return 0;

	PROCESS_BASIC_INFORMATION pbi = {0};
	LONG lret = NtQueryInformationProcess(hProcess, 0/*ProcessBasicInformation*/, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
	if (0 == lret)
	{
		dwId = (DWORD)pbi.InheritedFromUniqueProcessId;
	}
	CloseHandle (hProcess);
	return dwId;
}

HANDLE __stdcall AfxGetTokenByProcName(LPCTSTR lpName)
{
	HANDLE hToken = NULL;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return NULL;

	PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			if(_wcsicmp(pe32.szExeFile, lpName) == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
				if (hProcess != NULL)
				{
					if (!OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
						hToken = NULL;
					CloseHandle(hProcess);
				}
				break;
			}
			pe32.dwSize = sizeof(PROCESSENTRY32);
		} while (Process32Next(hProcessSnap, &pe32));
	}
	CloseHandle(hProcessSnap);

	return hToken;
}

bool __stdcall AfxRunProcessAsAdmin(LPCTSTR lpProcess, LPCTSTR lpParameter)
{
	typedef DWORD (__stdcall * PFN_GETSESSIONID)();

	PFN_GETSESSIONID	GetSessionId = NULL;
	HMODULE				hKernel32 = NULL;
	bool				bSuccess = false;
	HANDLE				hToken = NULL;
	HANDLE				hTokenDup = NULL;
	TCHAR				szError[MAX_PATH];
	STARTUPINFO			si;
	PROCESS_INFORMATION pi;

	bool bHideWindow = false;
	TCHAR szExePath[MAX_PATH] = {0}, szWorkDir[MAX_PATH] = {0};
	_tcscpy_s(szWorkDir, lpProcess);
	if (!PathRemoveFileSpec(szWorkDir))
		szWorkDir[0] = 0;

	LPTSTR pFile = PathFindFileName(lpProcess);
	LPTSTR pExt = PathFindExtension(lpProcess);
	if (pExt != NULL && _tcsicmp(pExt, TEXT(".reg")) == 0)
	{
		bHideWindow = true;
		_stprintf_s(szExePath, _countof(szExePath), TEXT("reg import \"%s\""), lpProcess);
	}
	else if (pExt != NULL && _tcsicmp(pExt, TEXT(".vbs")) == 0)
	{
		bHideWindow = true;
		_stprintf_s(szExePath, _countof(szExePath), TEXT("wscript \"%s\""), lpProcess);
	}
	else
	{
		if (pExt != NULL && _tcsicmp(pExt, TEXT(".bat")) == 0)
			bHideWindow = true;

		if (_tcslen(lpProcess) > 3 && lpProcess[1] == TEXT(':') && lpProcess[2] == TEXT('\\'))
			_stprintf_s(szExePath, _countof(szExePath), TEXT("\"%s\" %s"), lpProcess, lpParameter != NULL ? lpParameter : TEXT(""));
		else
		{
			TCHAR szRootPath[MAX_PATH] = {0};
			AfxGetAppPath(szRootPath, MAX_PATH);
			_stprintf_s(szWorkDir, _countof(szWorkDir), _T("%s%s"),  szRootPath, lpProcess);
			if (!PathRemoveFileSpec(szWorkDir))
				szWorkDir[0] = 0;
			_stprintf_s(szExePath, _countof(szExePath), TEXT("\"%s%s\" %s"), szRootPath, lpProcess, lpParameter != NULL ? lpParameter : TEXT(""));
		}
	}

	do 
	{
		hKernel32 = GetModuleHandle(_T("kernel32.dll"));
		if (!hKernel32)
			break;

		GetSessionId = (PFN_GETSESSIONID)GetProcAddress(hKernel32, "WTSGetActiveConsoleSessionId");
		if (!GetSessionId)
			break;

		if(OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
		{
			if(DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS,NULL, SecurityIdentification, TokenPrimary, &hTokenDup))
			{
				DWORD dwSessionId = GetSessionId();
				if(!SetTokenInformation(hTokenDup, TokenSessionId, &dwSessionId,sizeof(DWORD)))
				{
					_stprintf_s(szError, _T("SetTokenInformation error:%d\n"), GetLastError());
					OutputDebugString(szError);
					break;
				}

				ZeroMemory(&si,sizeof(STARTUPINFO));
				ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
				si.cb = sizeof(STARTUPINFO);
				si.lpDesktop = _T("WinSta0\\Default");
				if (bHideWindow)
				{
					si.dwFlags = STARTF_USESHOWWINDOW;
					si.wShowWindow = SW_HIDE;
				}

				LPVOID pEnv = NULL;
				DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;
				if(!CreateEnvironmentBlock(&pEnv,hTokenDup,FALSE))
				{
					_stprintf_s(szError, _T("CreateEnvironmentBlock error:%d\n"), GetLastError());
					OutputDebugString(szError);
					break;
				}

				PVOID oldValue = 0;
				AfxDisableFsRedirection(&oldValue);

				bSuccess = TRUE == CreateProcessAsUser(hTokenDup, 
					NULL, 
					szExePath, 
					NULL, 
					NULL, 
					FALSE, 
					dwCreationFlag, 
					pEnv, 
					_tcslen(szWorkDir) > 0 ? szWorkDir : NULL, 
					&si, 
					&pi);

				AfxRevertFsRedirection(oldValue);

				if (!bSuccess)
				{
					_stprintf_s(szError, _T("CreateProcessAsUser error:%d\n"), GetLastError());
					OutputDebugString(szError);
					break;
				}

				DestroyEnvironmentBlock(pEnv);

				bSuccess = true;
			}
			else
			{
				_stprintf_s(szError, _T("DuplicateTokenEx error:%d\n"), GetLastError());
				OutputDebugString(szError);
				break;
			}
		}
		else
		{
			_stprintf_s(szError, _T("cannot get administror error:%d\n"), GetLastError());
			OutputDebugString(szError);
			break;
		}
	}while(0);

	if(hTokenDup != NULL && hTokenDup != INVALID_HANDLE_VALUE)
		CloseHandle(hTokenDup);
	if(hToken != NULL && hToken != INVALID_HANDLE_VALUE)
		CloseHandle(hToken);

	return bSuccess;
}

bool __stdcall AfxRunProcess(LPCTSTR lpProcess, LPCTSTR lpParameter)
{
	bool bHideWindow = false;
	TCHAR szExePath[MAX_PATH] = {0}, szWorkDir[MAX_PATH] = {0};
	_tcscpy_s(szWorkDir, lpProcess);
	if (!PathRemoveFileSpec(szWorkDir))
		szWorkDir[0] = 0;

	LPTSTR pFile = PathFindFileName(lpProcess);
	LPTSTR pExt = PathFindExtension(lpProcess);
	if (pExt != NULL && _tcsicmp(pExt, TEXT(".reg")) == 0)
	{
		bHideWindow = true;
		_stprintf_s(szExePath, _countof(szExePath), TEXT("reg import \"%s\""), lpProcess);
	}
	else if (pExt != NULL && _tcsicmp(pExt, TEXT(".vbs")) == 0)
	{
		bHideWindow = true;
		_stprintf_s(szExePath, _countof(szExePath), TEXT("wscript \"%s\""), lpProcess);
	}
	else
	{
		if (pExt != NULL && _tcsicmp(pExt, TEXT(".bat")) == 0)
			bHideWindow = true;

		if (_tcslen(lpProcess) > 3 && lpProcess[1] == TEXT(':') && lpProcess[2] == TEXT('\\'))
			_stprintf_s(szExePath, _countof(szExePath), TEXT("\"%s\" %s"), lpProcess, lpParameter != NULL ? lpParameter : TEXT(""));
		else
		{
			TCHAR szRootPath[MAX_PATH] = {0};
			AfxGetAppPath(szRootPath, MAX_PATH);
			_stprintf_s(szWorkDir, _countof(szWorkDir), _T("%s%s"),  szRootPath, lpProcess);
			if (!PathRemoveFileSpec(szWorkDir))
				szWorkDir[0] = 0;
			_stprintf_s(szExePath, _countof(szExePath), TEXT("\"%s%s\" %s"), szRootPath, lpProcess, lpParameter != NULL ? lpParameter : TEXT(""));
		}
	}

	STARTUPINFO si = {sizeof(STARTUPINFO)};
	PROCESS_INFORMATION pi = {0};
	if (bHideWindow)
	{
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
	}

	LPVOID lpEnv = NULL;
	BOOL bRet = FALSE;
	HANDLE hToken = AfxGetTokenByProcName(TEXT("explorer.exe"));
	if (CreateEnvironmentBlock(&lpEnv, hToken, FALSE) && lpEnv != NULL)
	{
		PVOID oldValue = 0;
		AfxDisableFsRedirection(&oldValue);

		bRet = CreateProcessAsUser(hToken,
			NULL,
			szExePath,
			NULL,
			NULL,
			FALSE,
			CREATE_UNICODE_ENVIRONMENT,
			lpEnv, 
			_tcslen(szWorkDir) > 0 ? szWorkDir : NULL,
			&si,
			&pi);
		if(bRet)
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		
		AfxRevertFsRedirection(oldValue);

		DestroyEnvironmentBlock(lpEnv);
	}
	CloseHandle(hToken);
	return (bRet == TRUE);
}

bool __stdcall AfxKillProcess(LPCTSTR lpProcess)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return false;

	BOOL bRet = false;
	PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			if(_tcsicmp(pe32.szExeFile, lpProcess) == 0 && pe32.th32ProcessID != GetCurrentProcessId())
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
				if (hProcess != NULL)
				{
					bRet = TerminateProcess(hProcess, 0);
					CloseHandle(hProcess);
				}
			}
			pe32.dwSize = sizeof(PROCESSENTRY32);
		} while (Process32Next(hProcessSnap, &pe32));
	}
	CloseHandle(hProcessSnap);

	return (bRet == TRUE);
}

void __stdcall AfxSetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	HANDLE hToken; 	
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	if ( !LookupPrivilegeValue(NULL, lpszPrivilege, &luid) )    
		return ; 

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;

	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 
}

SOCKET __stdcall AfxSocketConnect(LPCTSTR ip, WORD port, DWORD timeout)
{
	if (ip == NULL || port == 0)
		return INVALID_SOCKET;

	SOCKET sck = socket(AF_INET, SOCK_STREAM, 0);   
	if(sck == INVALID_SOCKET)
		return INVALID_SOCKET;   

	DWORD ul = 1;
	if (SOCKET_ERROR == ioctlsocket(sck, FIONBIO, &ul))
	{
		closesocket(sck);
		return INVALID_SOCKET;
	}

	sockaddr_in server = {0};
	server.sin_family = AF_INET;   
	server.sin_port	  = htons(port);
	server.sin_addr.s_addr = inet_addr(_bstr_t(ip));
	if(server.sin_addr.s_addr == INADDR_NONE)
	{
		closesocket(sck);
		return INVALID_SOCKET;
	}
	connect(sck, (PSOCKADDR)&server, sizeof(server));

	timeval to = {timeout, 0};
	fd_set  fd;
	FD_ZERO(&fd);
	FD_SET(sck, &fd);
	if (select(0, NULL, &fd, NULL, &to) <= 0)
	{
		closesocket(sck);
		return INVALID_SOCKET;   
	}
	ul = 0;
	if (SOCKET_ERROR == ioctlsocket(sck, FIONBIO, &ul))
	{
		closesocket(sck);
		return INVALID_SOCKET;
	}

	linger InternalLinger = { 1, 0 };
	setsockopt(sck, SOL_SOCKET, SO_LINGER, (const char*)&InternalLinger, sizeof(linger));

	return sck;
}

SOCKET __stdcall AfxSocketListen(WORD port, LPCTSTR ip)
{
	SOCKET sckListen = INVALID_SOCKET;
	sockaddr_in addr = {0};
	try
	{
		sckListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (sckListen == INVALID_SOCKET)
			throw WSAGetLastError();

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (ip != NULL && _tcslen(ip) != 0)
			addr.sin_addr.s_addr = inet_addr(_bstr_t(ip));

		if (SOCKET_ERROR == bind(sckListen, (PSOCKADDR)&addr, sizeof(addr)))
			throw WSAGetLastError();

		if (SOCKET_ERROR == listen(sckListen, SOMAXCONN))
			throw WSAGetLastError();
	}
	catch (...)
	{
		if (sckListen != INVALID_SOCKET)
			closesocket(sckListen);
		sckListen = INVALID_SOCKET;
	}
	return sckListen;
}

DWORD  __stdcall AfxSocketExecute(SOCKET sck, LPCSTR pOutData, int outLength, LPSTR pInData, int inLength)
{
	return AfxSocketExecuteEx(sck, pOutData, outLength, pInData, inLength, 10);
}

DWORD  __stdcall AfxSocketExecuteEx(SOCKET sck, LPCSTR pOutData, int outLength, LPSTR pInData, int inLength, int try_max)
{
	DWORD nResult = ERROR_SUCCESS;
	int nSendLen = 0;
	while(nSendLen < outLength)
	{
		int len = send(sck, pOutData+nSendLen, outLength-nSendLen, 0);
		if(len <= 0)
		{
			if(SOCKET_ERROR == len)
			{
				nResult = WSAGetLastError();
				return nResult;
			}
			return -1;
		}
		nSendLen += len;
	}

	PkgHeader* header = reinterpret_cast<PkgHeader*>(pInData);
	int nRecvLen = 0;
	int nTryTimes = 0;
	while (1)
	{
		FD_SET fdset;
		timeval tv = {1, 0};
		FD_ZERO(&fdset);
		FD_SET(sck, &fdset);
		int ret = select(0, &fdset, NULL, NULL, &tv);
		if (ret <= 0)
		{
			if(SOCKET_ERROR == ret)
			{
				nResult = WSAGetLastError();
				return nResult;
			}
			if (ret == 0 && ++nTryTimes < try_max)
				continue ;
			return -1;
		}
		nTryTimes = 0;
		if (FD_ISSET(sck, &fdset))
		{
			int len = recv(sck, pInData + nRecvLen, inLength - nRecvLen, 0);
			if (len <= 0)
			{
				if(len == SOCKET_ERROR)
				{
					nResult = WSAGetLastError();
					return nResult;
				}
				return -1;
			}
			nRecvLen += len;
			if (nRecvLen >= sizeof(PkgHeader))
			{
				if ((int)header->length > inLength)
					return -1;
				inLength = header->length;
			}
			if (nRecvLen >= (int)header->length)
				break;
		}
	}
	return ERROR_SUCCESS;
}

void __stdcall AfxSocketClose(SOCKET& sck)
{
	if (sck != INVALID_SOCKET)
	{
		shutdown(sck, SD_BOTH);
		closesocket(sck);
		sck = INVALID_SOCKET;
	}
}

bool __stdcall AfxSendUdpNotify(LPCSTR pdata, DWORD length, LPCTSTR ip, WORD port)
{
	if (ip == NULL || port == 0)
		return false;

	SOCKET sck = socket(AF_INET, SOCK_DGRAM, 0);
	if (sck == INVALID_SOCKET)
		return false;

	sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(_bstr_t(ip));
	DWORD left = length;
	while (left)
	{
		int len = sendto(sck, &pdata[length-left], (left >= 0x10000 ? 0x10000 : left), 
			0, (PSOCKADDR)&addr, sizeof(addr));
		if (len <= 0)
			break;
		left -= len;
	}
	closesocket(sck);

	return (left == 0);
}

bool __stdcall AfxDetectTakeoverEx(int main_or_takeover, DWORD bcAddr, LPDWORD lpServerIPs, int tmo, int nMaxRetry)
{
	bool	bResult = false;
	char	szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
	SOCKET sck = INVALID_SOCKET;

	do 
	{
		if (!lpServerIPs)
			return false;

		sockaddr_in myAddr ={0};
		myAddr.sin_family = AF_INET;
		myAddr.sin_addr.S_un.S_addr = INADDR_ANY;
		myAddr.sin_port = 0;

		sck = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (INVALID_SOCKET == sck)
			break;

		if (SOCKET_ERROR == bind(sck, (PSOCKADDR)&myAddr, sizeof(myAddr)))
			break;

		int iBroadCast = 1;
		if (SOCKET_ERROR == setsockopt(sck, SOL_SOCKET, SO_BROADCAST, (char *)&iBroadCast, sizeof(iBroadCast)))
			break;

		CPkgHelper pkgSend(szSendBuffer, PKG_MAXLENGTH, main_or_takeover ? CMD_MGR_QUERYMAIN : CMD_MGR_QUERYTAKEOVER, OBJECT_UNKOWN);
		for(int i = 0; i < 256; i++)
			pkgSend.Push((BYTE)i);

		sockaddr_in dstAddr ={0};
		dstAddr.sin_family = AF_INET;
		dstAddr.sin_addr.S_un.S_addr = htonl(bcAddr);
		dstAddr.sin_port = htons(APP_BCAST_PORT);

		int len = sendto(sck, szSendBuffer, pkgSend.GetLength(), 0, (sockaddr*)&dstAddr, sizeof(sockaddr_in));
		if (len != pkgSend.GetLength())
			break;

		PkgHeader*	header = reinterpret_cast<PkgHeader*>(szRecvBuffer);
		int			nRecvLen = 0;
		int			nTryTimes = 0;
		sockaddr_in	svrAddr = {0};
		int			svrAddr_len = sizeof(sockaddr_in);
		bool		bGot = false;
		while (1)
		{
			FD_SET fdset;
			timeval tv = {tmo, 0};
			FD_ZERO(&fdset);
			FD_SET(sck, &fdset);
			int ret = select(0, &fdset, NULL, NULL, &tv);
			if (ret <= 0)
			{
				if (ret == 0 && ++nTryTimes < nMaxRetry)
					continue ;
				break;
			}
			nTryTimes = 0;
			if (FD_ISSET(sck, &fdset))
			{
				ZeroMemory(&svrAddr, sizeof(sockaddr_in));
				svrAddr_len = sizeof(sockaddr_in);
				int len = recvfrom(sck, szRecvBuffer, PKG_MAXLENGTH, 0, (sockaddr*)&svrAddr, &svrAddr_len);
				if (len <= 0)
					break;
				if (len >= sizeof(PkgHeader))
				{
					if ((int)header->length > PKG_MAXLENGTH)
						break;
				}
				if (len >= (int)header->length)
				{
					bGot = true;
					break;
				}
			}
		}

		if (bGot)
		{
			try
			{
				CPkgHelper pkgRecv(szRecvBuffer);
				if (pkgRecv.GetHeader()->command != CMD_MGR_QUERYTAKEOVER)
					break;
				DWORD	dwIP = 0;
				pkgRecv >> dwIP;
				*lpServerIPs = dwIP;

				bResult = true;
			}
			catch(...)
			{
				break;
			}
		}
	} while(0);

	if (INVALID_SOCKET != sck)
		closesocket(sck);

	return bResult;
}

bool __stdcall AfxDetectTakeover(DWORD bcAddr, LPDWORD lpServerIPs, int tmo, int nMaxRetry)
{
	bool	bResult = false;
	char	szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
	SOCKET sck = INVALID_SOCKET;

	do 
	{
		if (!lpServerIPs)
			return false;

		sockaddr_in myAddr ={0};
		myAddr.sin_family = AF_INET;
		myAddr.sin_addr.S_un.S_addr = INADDR_ANY;
		myAddr.sin_port = 0;

		sck = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (INVALID_SOCKET == sck)
			break;

		if (SOCKET_ERROR == bind(sck, (PSOCKADDR)&myAddr, sizeof(myAddr)))
			break;

		int iBroadCast = 1;
		if (SOCKET_ERROR == setsockopt(sck, SOL_SOCKET, SO_BROADCAST, (char *)&iBroadCast, sizeof(iBroadCast)))
			break;

		CPkgHelper pkgSend(szSendBuffer, PKG_MAXLENGTH, CMD_MGR_QUERYTAKEOVER, OBJECT_UNKOWN);
		for(int i = 0; i < 256; i++)
			pkgSend.Push((BYTE)i);

		sockaddr_in dstAddr ={0};
		dstAddr.sin_family = AF_INET;
		dstAddr.sin_addr.S_un.S_addr = htonl(bcAddr);
		dstAddr.sin_port = htons(APP_BCAST_PORT);

		int len = sendto(sck, szSendBuffer, pkgSend.GetLength(), 0, (sockaddr*)&dstAddr, sizeof(sockaddr_in));
		if (len != pkgSend.GetLength())
			break;

		PkgHeader*	header = reinterpret_cast<PkgHeader*>(szRecvBuffer);
		int			nRecvLen = 0;
		int			nTryTimes = 0;
		sockaddr_in	svrAddr = {0};
		int			svrAddr_len = sizeof(sockaddr_in);
		bool		bGot = false;
		while (1)
		{
			FD_SET fdset;
			timeval tv = {tmo, 0};
			FD_ZERO(&fdset);
			FD_SET(sck, &fdset);
			int ret = select(0, &fdset, NULL, NULL, &tv);
			if (ret <= 0)
			{
				if (ret == 0 && ++nTryTimes < nMaxRetry)
					continue ;
				break;
			}
			nTryTimes = 0;
			if (FD_ISSET(sck, &fdset))
			{
				ZeroMemory(&svrAddr, sizeof(sockaddr_in));
				svrAddr_len = sizeof(sockaddr_in);
				int len = recvfrom(sck, szRecvBuffer, PKG_MAXLENGTH, 0, (sockaddr*)&svrAddr, &svrAddr_len);
				if (len <= 0)
					break;
				if (len >= sizeof(PkgHeader))
				{
					if ((int)header->length > PKG_MAXLENGTH)
						break;
				}
				if (len >= (int)header->length)
				{
					bGot = true;
					break;
				}
			}
		}

		if (bGot)
		{
			try
			{
				CPkgHelper pkgRecv(szRecvBuffer);
				if (pkgRecv.GetHeader()->command != CMD_MGR_QUERYTAKEOVER)
					break;
				DWORD	dwIP = 0;
				pkgRecv >> dwIP;
				*lpServerIPs = dwIP;

				bResult = true;
			}
			catch(...)
			{
				break;
			}
		}
	} while(0);

	if (INVALID_SOCKET != sck)
		closesocket(sck);

	return bResult;
}

bool __stdcall AfxIsLocalIP(DWORD ip, bool b127IsLocalMachine)
{
	if (ip == 0)
		return false;

	if (b127IsLocalMachine && ((ip & 0xff) == 127))
		return true;

	DWORD dwSize = 0;
	if (GetAdaptersInfo(NULL, &dwSize) != ERROR_BUFFER_OVERFLOW)
		return false;
	auto pAdapterInfoPtr = std::make_shared<std::vector<char>>(dwSize);
	if (GetAdaptersInfo((PIP_ADAPTER_INFO)pAdapterInfoPtr->data(), &dwSize) != NOERROR)
		return false;

	PIP_ADAPTER_INFO pAdapter = (PIP_ADAPTER_INFO)pAdapterInfoPtr->data();
	while (pAdapter != NULL)
	{
		PIP_ADDR_STRING pAddr = &pAdapter->IpAddressList;
		while (pAddr != NULL)
		{
			DWORD dwIp = ::inet_addr(pAddr->IpAddress.String);
			if (dwIp == ip)
				return true;
			pAddr = pAddr->Next;
		}
		pAdapter = pAdapter->Next;
	}
	return false;
}

void  __cdecl AfxSendLogToConsole(LPCTSTR ip, DWORD logtype, LPCTSTR format, ...)
{
	TCHAR log[PKG_MAXLENGTH] = {0};

	va_list marker;
	va_start(marker, format);
	_vstprintf_s(log, format, marker);
	va_end(marker);

	char buf[PKG_MAXLENGTH] = {0};
	CPkgHelper pkg(buf, PKG_MAXLENGTH, CMD_CON_NOTIFYLOGINFO, OBJECT_CONSOLE);
	pkg << logtype << log;

	TCHAR szIp[MAX_PATH] = {0};
	if (ip == NULL || lstrlen(ip) == 0)
		lstrcpy(szIp, TEXT("127.0.0.1"));
	else
		lstrcpy(szIp, ip);
	AfxSendUdpNotify(buf, pkg.GetLength(), szIp, CON_UDP_PORT);
}

BOOL __stdcall AfxDisableFsRedirection(PVOID* pOldValue)
{
	typedef BOOL (WINAPI *PFNDISABLEFSREDIRECTION)(PVOID *);

	BOOL					bResult = FALSE;
	PFNDISABLEFSREDIRECTION	pfnDisable = NULL;
	HMODULE					hModule = LoadLibrary(_T("kernel32.dll"));
	
	if (hModule)
	{
		pfnDisable = (PFNDISABLEFSREDIRECTION)GetProcAddress(hModule, "Wow64DisableWow64FsRedirection");	
		if (pfnDisable)
		{
			bResult = pfnDisable(pOldValue);
		}
		FreeLibrary(hModule);
	}

	return bResult;
}

BOOL __stdcall AfxRevertFsRedirection(PVOID oldValue)
{
	typedef BOOL (WINAPI *PFNREVERTFSREDIRECTION)(PVOID);

	BOOL					bResult = FALSE;
	PFNREVERTFSREDIRECTION	pfnRevert = NULL;
	HMODULE					hModule = LoadLibrary(_T("kernel32.dll"));

	if (hModule)
	{
		pfnRevert = (PFNREVERTFSREDIRECTION)GetProcAddress(hModule, "Wow64RevertWow64FsRedirection");	
		if (pfnRevert)
		{
			bResult = pfnRevert(oldValue);
		}
		FreeLibrary(hModule);
	}

	return bResult;
}

bool __stdcall AfxOnlineBackupDb(sqlite3* pSrcDb, LPSTR pBackupName)
{
	sqlite3*	pDstFile = NULL;
	bool		bBackuped = false;

	int rc = sqlite3_open(pBackupName, &pDstFile);
	if( rc == SQLITE_OK )
	{
		sqlite3_backup* pBackup = sqlite3_backup_init(pDstFile, "main", pSrcDb, "main");
		if (pBackup)
		{
			rc = sqlite3_backup_step(pBackup, -1);
			bBackuped = rc == SQLITE_DONE;
			sqlite3_backup_finish(pBackup);
		}

		sqlite3_close(pDstFile);
	}

	return bBackuped;
}

END_NAMESPACE_KCORE