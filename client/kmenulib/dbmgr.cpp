#include "stdafx.h"
#include <shlwapi.h>
#include <process.h>
#include <WinSvc.h>
#include <winsock2.h>
#include <Iphlpapi.h>
#include <intshcut.h>
#include <atlenc.h>
#include "resource.h"
#include "MyChiToLetter.h"
#include "dbmgr.h"
#include "UI/UIX.h"
#include <winhttp.h>
#include <comdef.h>
#include "kupdate.h"
#include "iconv.h"
#include <winioctl.h>
#include <ShlObj.h>
#include <Dbghelp.h>
#include <TlHelp32.h>
#include <UserEnv.h>

#pragma comment(lib,"kupdate")
#pragma comment(lib, "Iphlpapi.lib")

HANDLE g_Logger = NULL;

//多音字字库, 第一个为多音字，加L以示UNICODE编码。后面两个字母为多音字两个拼音首字母，必须为小写。
STMultiChar   g_arMultiChar[] = 
{
	{L'降', TEXT("jx")},	{L'尔', TEXT("er")},	{L'重', TEXT("cz")},	
	{L'阿', TEXT("ae")},	{L'拗', TEXT("an")},	{L'扒', TEXT("bp")},	{L'堡', TEXT("bp")},
	{L'暴', TEXT("bp")},	{L'辟', TEXT("bp")},	{L'扁', TEXT("bp")},	{L'藏', TEXT("cz")},
	{L'禅', TEXT("cs")},	{L'颤', TEXT("cz")},	{L'朝', TEXT("cz")},	{L'车', TEXT("cj")},
	{L'澄', TEXT("cd")},	{L'匙', TEXT("cs")},	{L'臭', TEXT("cx")},	{L'畜', TEXT("cx")},
	{L'枞', TEXT("cz")},	{L'攒', TEXT("cz")},	{L'撮', TEXT("cz")},	{L'单', TEXT("ds")},
	{L'提', TEXT("dt")},	{L'囤', TEXT("dt")},	{L'革', TEXT("gj")},	{L'合', TEXT("gh")},
	{L'给', TEXT("gj")},	{L'谷', TEXT("gy")},	{L'桧', TEXT("gh")},	{L'巷', TEXT("hx")},
	{L'会', TEXT("hk")},	{L'颈', TEXT("jg")},	{L'枸', TEXT("jg")},	{L'奇', TEXT("jq")},
	{L'茄', TEXT("jq")},	{L'侥', TEXT("jy")},	{L'剿', TEXT("jc")},	{L'校', TEXT("jx")},
	{L'解', TEXT("jx")},	{L'芥', TEXT("jg")},	{L'矜', TEXT("jq")},	{L'卡', TEXT("kq")},
	{L'壳', TEXT("kq")},	{L'蔓', TEXT("mw")},	{L'秘', TEXT("mb")},	{L'泌', TEXT("mb")},
	{L'刨', TEXT("pb")},	{L'瀑', TEXT("pb")},	{L'栖', TEXT("qx")},	{L'蹊', TEXT("qx")},
	{L'稽', TEXT("qj")},	{L'趄', TEXT("qj")},	{L'厦', TEXT("sx")},	{L'识', TEXT("sz")},
	{L'调', TEXT("td")},	{L'尾', TEXT("wx")},	{L'尉', TEXT("wy")},	{L'吓', TEXT("xh")},
	{L'纤', TEXT("xq")},	{L'行', TEXT("xh")},	{L'省', TEXT("xs")},	{L'宿', TEXT("xs")},
	{L'吁', TEXT("yx")},	{L'轧', TEXT("zy")},	{L'粘', TEXT("zn")},	{L'属', TEXT("zs")},
	{L'幢', TEXT("zc")},	{L'传', TEXT("cz")},	{L'乐', TEXT("ly")},	
};

CString AfxBase64Decode(CString strBase64)
{
	CString		strDecoded;

	USES_CONVERSION;
	LPSTR		pszBase64 = T2A((LPTSTR)(LPCTSTR)strBase64);

	int nRequiredLen = ATL::Base64DecodeGetRequiredLength((int)strlen(pszBase64));

	if (nRequiredLen > 0)
	{
		int			nDecodedLen = nRequiredLen + sizeof(char);
		BYTE*		pszDecoded = new BYTE[nDecodedLen];

		memset(pszDecoded, 0, nDecodedLen);

		if (ATL::Base64Decode(pszBase64, (int)strlen(pszBase64), (BYTE*)pszDecoded, &nDecodedLen))
		{
			USES_CONVERSION;
			strDecoded = A2T((LPCSTR)pszDecoded);
		}

		delete []pszDecoded;
	}

	return strDecoded;
}

//代码转换:从一种编码转为另一种编码
int code_convert(char *from_charset, char *to_charset, const char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	iconv_t cd;
	const char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
		return -1;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
		return -1;
	iconv_close(cd);
	return 0;
}

//UNICODE码转为GB2312码
int u2g(const char *inbuf, int inlen, char *outbuf, int outlen)
{
	return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
}

//GB2312码转为UNICODE码
int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}

void AfxGetPY(CString& strChinese, CString& strPinYin)
{
	std::string sChinese = CW2A((LPTSTR)(LPCTSTR)strChinese, CP_UTF8);
	char buf[256] = { 0 };
	u2g(sChinese.c_str(), (int)sChinese.length(), buf, 256);

	std::string sResult = GetFirstLetter(buf);

	strPinYin = CA2W((LPSTR)sResult.c_str());
}

_bstr_t GetPY(TCHAR* pChinese)
{
	//std::string src = _bstr_t(pChinese);
	//std::string dst = GetFirstLetter(src.c_str());//GetPyLetter(src);
	CString strCh = pChinese;
	CString strDst;

	AfxGetPY(strCh, strDst);

	return _bstr_t(strDst);
}


std::vector<CString> GetSpliter(CString strSrc, LPCTSTR pSpliter)
{
	std::vector<CString> strList;
	if(strSrc.IsEmpty() || pSpliter == NULL) return strList;
	TCHAR *context = NULL;
	TCHAR* pToken = _tcstok_s((LPTSTR)(LPCTSTR)strSrc, pSpliter,&context);
	while(pToken != NULL)
	{
		strList.push_back(pToken);
		pToken = _tcstok_s(NULL, pSpliter,&context);
	}

	return strList;
}

void AfxGetFixedDisks(CStringArray& arDisks)
{
	TCHAR	szBuf[256] = { 0 }, *pDrive = NULL;

	GetLogicalDriveStrings(256, szBuf);

	pDrive = szBuf;
	while(pDrive[0] != _T('\0'))
	{
		DWORD dwType = GetDriveType(pDrive);
		if ((dwType == DRIVE_FIXED ||
			dwType == DRIVE_REMOVABLE) &&
			_totupper(pDrive[0]) != _T('A')&&
			_totupper(pDrive[0]) != _T('B')&&
			_totupper(pDrive[0]) != _T('C'))
			arDisks.Add(pDrive);

		pDrive += _tcslen(pDrive) + 1;
	}
}

void AfxSetLangId(WORD wLangID)
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

unsigned long __Crc32Table__[256] =
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
	0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
	0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
	0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
	0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
	0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
	0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
	0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
	0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
	0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
	0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
	0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
	0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
	0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
	0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
	0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
	0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
	0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
	0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
	0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
	0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
	0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
	0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
	0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
	0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
	0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};
DWORD AfxCrc32Encode(DWORD crc, const void* data, DWORD ByteLen)
{
	unsigned char* pdata = (unsigned char*)data;
	crc ^= 0xffffffffL;
	while (ByteLen--)
		crc = (crc >> 8) ^ __Crc32Table__[(crc & 0xFF) ^ *pdata++];
	return crc ^ 0xffffffff;
}

CClientDbMgr::CClientDbMgr()
	: m_defaultImage(NULL)
{
	g_Logger = AfxCreateLogger(TEXT("kmenu"));
	WriteLog(_T("Start========================================="));
	m_LocalIp = 0;
	//Initialize();

	DWORD dwLanguageId = 0;
	DWORD dwRegType = 0, dwSize = sizeof(dwLanguageId);
	if (ERROR_SUCCESS == ::SHGetValue(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet"),
		TEXT("languageid"), &dwRegType, &dwLanguageId, &dwSize) && dwRegType == REG_DWORD)
	{
		AfxSetLangId((LANGID)dwLanguageId);
	}
}

CClientDbMgr::~CClientDbMgr()
{
	m_mapSysOpt.clear();
	FreeGameInfo();
	FreeIconInfo();

	if (g_Logger)
	{
		WriteLog(_T("End========================================="));
		AfxCloseLogger(g_Logger);
	}
}

CClientDbMgr* CClientDbMgr::getInstance()
{
	static CClientDbMgr self;
	return &self;
}

void CClientDbMgr::Initialize()
{
	WriteLog(_T("Dbmgr Initializing"));

	LoadMultiChar();
	LoadSysOpt();
	LoadIconInfoAll();
	LoadGameInfoAll();
	LoadRate();
}

void CClientDbMgr::unInitialize()
{
	FreeIconInfo();
	FreeGameInfo();
}

void CClientDbMgr::RefreshOfflineData()
{
	LoadSysOpt();
	LoadIconInfoAll();
	LoadGameInfoAll();
	LoadRate();
}

bool CClientDbMgr::GetBoolOpt(LPCTSTR lpOptName, bool nOptDef /* = false*/)
{
	std::map<CString, CString>::iterator it = m_mapSysOpt.find(lpOptName);

	if (it == m_mapSysOpt.end() || it->second.IsEmpty())
		return nOptDef;

	return (_ttoi(it->second) != 0);
}

long CClientDbMgr::GetIntOpt(LPCTSTR lpOptName, long nOptDef /* = 0 */)
{
	std::map<CString, CString>::iterator it = m_mapSysOpt.find(lpOptName);

	if (it == m_mapSysOpt.end() || it->second.IsEmpty())
		return nOptDef;

	return _ttoi(it->second);
}

__int64 CClientDbMgr::GetInt64Opt(LPCTSTR lpOptName, __int64 nOptDef /* = 0 */)
{
	std::map<CString, CString>::iterator it = m_mapSysOpt.find(lpOptName);

	if (it == m_mapSysOpt.end() || it->second.IsEmpty())
		return nOptDef;

	return _ttoi64(it->second);
}

CString CClientDbMgr::GetStrOpt(LPCTSTR lpOptName, LPCTSTR lpOptDef /* = TEXT("") */, bool bEmptyIsDef /*= false*/)
{
	CString szValue(lpOptName);
	szValue.MakeLower();
	std::map<CString, CString>::iterator it = m_mapSysOpt.find(szValue);

	if (it == m_mapSysOpt.end() || (bEmptyIsDef && it->second.IsEmpty()))
		return lpOptDef;

	return it->second;
}
TCHAR CClientDbMgr::GetUDiskUnmoutDriver()
{
	CString drv = GetStrOpt(OPT_M_UDISKVOL, TEXT("U"), true);
	return drv[0];
}
TCHAR CClientDbMgr::GetUDiskDriver()
{
	CString drv = GetStrOpt(OPT_M_UDISKVOL, TEXT("U"), true);

	CStringArray	arDisks;
	AfxGetFixedDisks(arDisks);

	for (int i =0;i<arDisks.GetSize();i++)
	{
		CString& d = arDisks[i];
		if(_totupper(drv[0]) == _totupper(d[0]))
			return (TCHAR)0;
	}
	return drv[0];
}

CString CClientDbMgr::GetIniFile()
{
	TCHAR szIniFile[MAX_PATH] = {0};
	GetModuleFileName(NULL, szIniFile, MAX_PATH);
	PathRemoveFileSpec(szIniFile);
	PathAddBackslash(szIniFile);
	lstrcat(szIniFile, TEXT("config\\config.ini"));
	return CString(szIniFile);
}

LPTSTR AfxGetAppPath(LPTSTR szPath, DWORD dwSize /*= MAX_PATH*/, LPCTSTR szAppend /*= NULL*/)
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

LPTSTR AfxGetSysString(LPCTSTR Section, LPCTSTR Key, LPTSTR Value, DWORD dwSize, LPCTSTR Default)
{
	TCHAR szFile[MAX_PATH] = { 0 };
	AfxGetAppPath(szFile, MAX_PATH, TEXT("config\\config.ini"));
	GetPrivateProfileString(Section, Key, Default, Value, dwSize, szFile);
	return Value;
}

CString CClientDbMgr::LoadIpAddr()
{
	TCHAR szBuffer[MAX_PATH] = {0};
	tstring ip = AfxGetSysString(CLIENT_INII_SECTION_SERVER, CLIENT_INII_KEY_IP,szBuffer,MAX_PATH, TEXT("127.0.0.1"));
	return CString(ip.c_str());
}
CString CClientDbMgr::LoadLocalIpAddr()
{
	TCHAR szBuffer[MAX_PATH] = {0};
	tstring ip = AfxGetSysString(CLIENT_INII_SECTION_SERVER, _T("localip"),szBuffer,MAX_PATH, TEXT("127.0.0.1"));
	return CString(ip.c_str());
}
void CClientDbMgr::SaveIpAddr(CString str)
{
	WritePrivateProfileString(CLIENT_INII_SECTION_SERVER, CLIENT_INII_KEY_IP, str, GetIniFile());
}

LPTSTR __stdcall AfxFormatIpString(DWORD ip, LPTSTR buffer, DWORD dwSize)
{
	if (ip)
	{
		_stprintf_s(buffer, dwSize - 1, TEXT("%d.%d.%d.%d"), (ip & 0xff), ((ip >> 8) & 0xff), ((ip >> 16) & 0xff), ((ip >> 24) & 0xff));
	}
	return buffer;
}

CString	CClientDbMgr::GetUptSvrList()
{
	CString ip;
	//if (m_vtServer.size() == 0)
	//{
	//	ip = AfxGetSysString(CLIENT_INII_SECTION_SERVER, CLIENT_INII_KEY_IP, TEXT("127.0.0.1")).c_str();
	//}
	//else
	//{
	//	std::set<DWORD> iplist;
	//	for (std::vector<shared_server_ptr>::iterator it = m_vtServer.begin(); it != m_vtServer.end(); it++)
	//	{
	//		iplist.insert((*it)->vip);
	//	}
	//	DWORD firstip = 0;
	//	if (m_ServerName.GetLength())
	//	{
	//		for (std::vector<shared_server_ptr>::iterator it = m_vtServer.begin(); it != m_vtServer.end(); it++)
	//		{
	//			if (lstrcmpi((*it)->Server, m_ServerName) == 0)
	//			{
	//				firstip = (*it)->uip;
	//				break;
	//			}
	//		}
	//	}
	//	if (firstip == 0)
	//	{
	//		long N = ((m_LocalIp >> 24) % iplist.size());
	//		std::set<DWORD>::iterator it  = iplist.begin();
	//		while (N > 0 && it != iplist.end())
	//		{
	//			N--;
	//			it++;
	//		}
	//		if (it != iplist.end())
	//			firstip = *it;
	//	}

	//	ip = AfxFormatIpString(firstip).c_str();
	//	for (std::vector<shared_server_ptr>::iterator it = m_vtServer.begin(); it != m_vtServer.end(); it++)
	//	{
	//		shared_server_ptr pServer = (*it);
	//		if (pServer->uip && firstip != pServer->uip)
	//		{
	//			if (ip.GetLength())
	//				ip += TEXT("|");
	//			ip += AfxFormatIpString(pServer->uip).c_str();
	//		}
	//	}
	//}


	if (gSvrList.size() == 0)
	{
		TCHAR ipbuf[MAX_PATH] = {0};
		AfxGetSysString(TEXT("config"), TEXT("serverip"), ipbuf, _countof(ipbuf), TEXT("127.0.0.1"));
		ip = ipbuf;
	}
	else
	{
		do 
		{
			//select server.
			std::map<tstring, tstring>::iterator SvrIter = gSvrList.find(m_ServerName.GetBuffer());
			if (SvrIter == gSvrList.end())
			{
				SvrIter = gSvrList.begin();
				int nSvrMode = m_LocalIp % gSvrList.size();
				while (nSvrMode > 0)
				{
					SvrIter ++;
					nSvrMode--;
				}
			}
			tstring strSvrName = SvrIter->first;

			//select server ip address.
			std::vector<tstring> iplist;
			AfxSplitString(SvrIter->second, iplist, TEXT('|'));
			int nIpMode = (m_LocalIp/gSvrList.size()) % iplist.size();
			DWORD dwIp = _ttoi(iplist[nIpMode].c_str());

			TCHAR ipbuf[MAX_PATH] = {0};
			AfxFormatIpString(dwIp, ipbuf, _countof(ipbuf));
			ip = ipbuf;

			for (SvrIter = gSvrList.begin();SvrIter != gSvrList.end();SvrIter++)
			{
				if(strSvrName != SvrIter->first)
				{
					//select server ip address.
					std::vector<tstring> iplist;
					AfxSplitString(SvrIter->second, iplist, TEXT('|'));
					int nIpMode = m_LocalIp % iplist.size();
					DWORD dwIp = _ttoi(iplist[nIpMode].c_str());

					TCHAR ipbuf[MAX_PATH] = {0};
					AfxFormatIpString(dwIp, ipbuf, _countof(ipbuf));

					if (ip.GetLength())
						ip += TEXT("|");
					ip += ipbuf;
				}
			}

		} while (0);
	}

	return ip;
}

tagGameInfo* CClientDbMgr::FindGameInfo(DWORD gid)
{
	std::map<DWORD, tagGameInfo*>::iterator it = m_mapGameInfo.find(gid);
	if (it == m_mapGameInfo.end())
		return NULL;
	return it->second;
}


//
//tVDisk* CClientDbMgr::FindVDiskInfo(TCHAR chSvr)
//{
//	std::map<TCHAR, shared_vdisk_ptr>::iterator it = m_mapVDisk.find(chSvr);
//	if (it == m_mapVDisk.end())
//		return NULL;
//	return it->second.get();
//}


bool CClientDbMgr::IsGameClassExist(CString szClass)
{
	std::map<DWORD, tagGameInfo*>::iterator iter = m_mapGameInfo.begin();
	for (; iter != m_mapGameInfo.end(); ++iter)
	{
		if (szClass == iter->second->Class)
		{
			return true;
		}
	}
	return false;
}

// 加载本地文件数据
bool CClientDbMgr::ReadConfigDatFile(LPCTSTR lpFileName, char*& pData, DWORD& dwLength)
{
	TCHAR szBuffer[MAX_PATH] = {0};
	tstring strFile = AfxGetAppPath(szBuffer,MAX_PATH,lpFileName);
	HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		WriteLog(TEXT("open %s file fail! error code:%d."), lpFileName, GetLastError());
		return false;
	}

	DWORD dwSize = GetFileSize(hFile, NULL);
	if(dwSize == 0)
	{
		CloseHandle(hFile);
		return false;
	}
	pData = (char*)CoTaskMemAlloc(dwSize + 1);
	DWORD dwReadBytes = 0;
	if (!ReadFile(hFile, pData, dwSize, &dwReadBytes, NULL) || dwReadBytes != dwSize)
	{
		WriteLog(TEXT("read open %s file fail! error code:%d."), lpFileName, GetLastError());
		CloseHandle(hFile);
		CoTaskMemFree(pData);
		return false;
	}

	dwLength = dwReadBytes;
	pData[dwReadBytes] = 0;
	CloseHandle(hFile);
	return true;
}

void CClientDbMgr::LoadSysOpt()
{
	WriteLog(TEXT("load system option."));
	m_mapSysOpt.clear();
	char* pData = NULL;
	DWORD dwLength = 0;
	try
	{
		if(ReadConfigDatFile(CLIENT_DATA_SYSOPT, pData, dwLength))
		{
			kmenu::CPkgHelper pkg(pData);
			DWORD dwSize = 0;
			pkg.Pop(dwSize);
			for (DWORD i=0; i<dwSize; ++i)
			{
				TCHAR szKey[MAX_OPTION_LEN] = {0};
				TCHAR szVal[MAX_OPTION_LEN] = {0};
				pkg.PopString(szKey);
				pkg.PopString(szVal);
				m_mapSysOpt.insert(std::make_pair(CString(szKey), CString(szVal)));
			}
		}
	}
	catch (...)
	{
		WriteLog(TEXT("data package exeception.")); 
		CoTaskMemFree(pData);
		return;
	}

	CoTaskMemFree(pData);
	WriteLog(TEXT("load system option success."));
}


void CClientDbMgr::LoadGameInfoAll()
{
	CString	strTmp;

	WriteLog(TEXT("Load Game Info ..."));
	FreeGameInfo();
	char* pData = NULL;
	std::set<CString> ServerClass;
	DWORD dwLength = 0;
	try
	{
		if(ReadConfigDatFile(CLIENT_DATA_GAME, pData, dwLength))
		{
			kmenu::CPkgHelper pkg(pData);
			DWORD dwSize = 0;
			pkg.Pop(dwSize);
			for (DWORD i=0; i<dwSize; ++i)
			{
				tagGameInfo* pGameInfo = new tagGameInfo;
				ZeroMemory(pGameInfo, sizeof(tagGameInfo));

				TCHAR szVersizon[MAX_PATH] = { 0 };
				AfxGetFileVersion(TEXT("knbmenu.exe"), szVersizon);

				CString strVer = szVersizon;
				strVer = strVer.Right(4);


				if (strVer.CompareNoCase(L"5498") > 0)
				{
					pkg >> pGameInfo->gid >> pGameInfo->cid >> pGameInfo->Class >> pGameInfo->gname >> pGameInfo->galias >> pGameInfo->grungame >> pGameInfo->gexefile >> pGameInfo->gparameter >> pGameInfo->gsvrpath >> pGameInfo->gclipath;
					pkg >> pGameInfo->gprogress >> pGameInfo->gsize >> pGameInfo->gtoolbar >> pGameInfo->gpush >> pGameInfo->desklnk >> pGameInfo->toolbar >> pGameInfo->SvrVer >> pGameInfo->svrclick >> pGameInfo->invalue;
					pkg >> pGameInfo->runtype >> pGameInfo->hottag >> pGameInfo->ideclick >> pGameInfo->gConfig >> pGameInfo->idcdate >> pGameInfo->iWait;
				}
				else if (strVer.CompareNoCase(L"4976") >= 0)
				{
					pkg >> pGameInfo->gid >> pGameInfo->Class >> pGameInfo->gname >> pGameInfo->galias >> pGameInfo->grungame >> pGameInfo->gexefile >> pGameInfo->gparameter >> pGameInfo->gsvrpath >> pGameInfo->gclipath;
					pkg >> pGameInfo->gprogress >> pGameInfo->gsize >> pGameInfo->gtoolbar >> pGameInfo->gpush >> pGameInfo->desklnk >> pGameInfo->toolbar >> pGameInfo->SvrVer >> pGameInfo->svrclick >> pGameInfo->invalue;
					pkg >> pGameInfo->runtype >> pGameInfo->hottag >> pGameInfo->ideclick >> pGameInfo->gConfig >> pGameInfo->idcdate >> pGameInfo->iWait;
				}
				else
				{
					pkg >> pGameInfo->gid >> pGameInfo->Class >> pGameInfo->gname >> pGameInfo->galias >> pGameInfo->grungame >> pGameInfo->gexefile >> pGameInfo->gparameter >> pGameInfo->gsvrpath >> pGameInfo->gclipath;
					pkg >> pGameInfo->gprogress >> pGameInfo->gsize >> pGameInfo->gtoolbar >> pGameInfo->gpush >> pGameInfo->desklnk >> pGameInfo->toolbar >> pGameInfo->SvrVer >> pGameInfo->svrclick >> pGameInfo->invalue;
					pkg >> pGameInfo->runtype >> pGameInfo->hottag >> pGameInfo->ideclick >> pGameInfo->gConfig >> pGameInfo->idcdate;
				}


				
				if (lstrlen(pGameInfo->gname) == 0 || lstrlen(pGameInfo->Class) == 0)
					continue;

				PathAddBackslash(pGameInfo->gsvrpath);
				int nLen = lstrlen(pGameInfo->gprogress);
				if (nLen>2)
				{
					lstrcpy(pGameInfo->gprogress,AfxBase64Decode(pGameInfo->gprogress));
				}
				nLen = lstrlen(pGameInfo->gparameter);
				if (nLen>2)
				{
					lstrcpy(pGameInfo->gparameter,AfxBase64Decode(pGameInfo->gparameter));
				}
				nLen = lstrlen(pGameInfo->gConfig);
				if (nLen>2)
				{
					lstrcpy(pGameInfo->gConfig,AfxBase64Decode(pGameInfo->gConfig));
				}
				if (pGameInfo->runtype == GAME_RUNTYPE_UPDATE)
				{
					CString strDriver = GetStrOpt(OPT_M_GAMEDISK,_T(""),true);
					strDriver.Trim();
					lstrcpy(pGameInfo->gclipath, pGameInfo->gsvrpath);
					if (!strDriver.IsEmpty())
					{
						pGameInfo->gclipath[0] = strDriver[0];
					}
				}
				else
				{
					lstrcpy(pGameInfo->gclipath, pGameInfo->gsvrpath);
				}

				ServerClass.insert(pGameInfo->Class);
				GetGameNamePy(pGameInfo);
				//pGameInfo->hIcon = FindIconOfGame_(pGameInfo->gid);
				m_mapGameInfo.insert(std::make_pair(pGameInfo->gid, pGameInfo));
			}
		}
	}
	catch (...) 
	{ 
		_com_error error(GetLastError());
		WriteLog(TEXT("data package exeception.%s"), error.ErrorMessage()); 
		CoTaskMemFree(pData);
		return;
	}

	CoTaskMemFree(pData);


	WriteLog(TEXT("Load Game Info success"));
}

void CClientDbMgr::GetGameNamePy(tagGameInfo* pGameInfo)
{
	pGameInfo->szMultiPy[0] = 0;
	_bstr_t bstrPy = GetPY(pGameInfo->gname);
	lstrcpy(pGameInfo->szPy, bstrPy);
	int nNameLen = lstrlen(pGameInfo->gname), nIndex = 0;
	std::map<TCHAR, WCHAR*>::iterator it = m_mapMultiChar.end();
	for (nIndex = 0; nIndex < nNameLen; ++nIndex)
	{
		it = m_mapMultiChar.find(pGameInfo->gname[nIndex]);
		if (it != m_mapMultiChar.end()) 
			break;
	}
	if (it != m_mapMultiChar.end())
	{
		lstrcpy(pGameInfo->szMultiPy, pGameInfo->szPy);
		pGameInfo->szPy[nIndex] = it->second[0];
		pGameInfo->szMultiPy[nIndex] = it->second[1];
	}
}
void CClientDbMgr::LoadIconInfoAll()
{
	WriteLog(TEXT("Load Icon..."));
	FreeIconInfo();
	/*DWORD dwLength = 0;
	try
	{
	TCHAR szBuffer[MAX_PATH] = {0};
	tstring strFile = AfxGetAppPath(szBuffer,MAX_PATH,CLIENT_DATA_GAMEICON);

	sqlite3*    pIconDb;
	DWORD_PTR   dwIconCookie;
	if (!AfxOpenSqliteMemory(strFile.c_str(), pIconDb, dwIconCookie) || pIconDb == NULL)
	return ;

	char szSql[PKG_MAXLENGTH] =	"SELECT gid, hash, data FROM tIconList";
	sqlite3_stmt *pstat = NULL;	
	int result = sqlite3_prepare(pIconDb, szSql, -1, &pstat, NULL);
	if (result != SQLITE_OK)
	{
	WriteLog(TEXT("load icon faild ")); 
	return;
	}

	while(sqlite3_step(pstat) == SQLITE_ROW)
	{
	tagIconInfo iconInfo = {0};
	iconInfo.dwGid = sqlite3_column_int(pstat, 0);
	iconInfo.dwHash = sqlite3_column_int(pstat, 1);
	iconInfo.dwLength = sqlite3_column_bytes(pstat, 2);

	//WriteLog(TEXT("gid:%d-%d"),*iter, iconInfo.dwLength); 

	if (iconInfo.dwLength > 0)
	{
	iconInfo.pIcon = new char[iconInfo.dwLength];
	memcpy(iconInfo.pIcon,(char*)sqlite3_column_blob(pstat, 2),iconInfo.dwLength);
	}
	m_mapIconInfo.insert(std::make_pair(iconInfo.dwGid, iconInfo));
	}
	sqlite3_finalize(pstat);

	if (pIconDb != NULL)
	{
	AfxCloseSqliteMemory(dwIconCookie);
	pIconDb = NULL;
	dwIconCookie = 0;
	}
	}
	catch (...)
	{ 
	WriteLog(TEXT("data package exeception.")); 
	return;
	}*/
	char* pData = NULL;
	DWORD dwLength = 0;
	try
	{
		if(ReadConfigDatFile(CLIENT_DATA_GAMEICON, pData, dwLength))
		{
			kmenu::CPkgHelper pkg(pData, dwLength, 0, 0);
			DWORD dwSize = 0;
			pkg.Pop(dwSize);
			for (DWORD i=0; i<dwSize; ++i)
			{
				tagIconInfo iconInfo = {0};
				pkg.Pop(iconInfo.dwGid);
				pkg.Pop(iconInfo.dwHash);
				pkg.Pop(iconInfo.dwLength);
				if (iconInfo.dwLength > 0)
				{
					iconInfo.pIcon = new char[iconInfo.dwLength];
					pkg.PopPointer(iconInfo.pIcon, iconInfo.dwLength);
				}
				m_mapIconInfo.insert(std::make_pair(iconInfo.dwGid, iconInfo));
			}
		}
	}
	catch (...)
	{ 
		WriteLog(TEXT("data package exeception.")); 
		CoTaskMemFree(pData);
		return;
	}

	CoTaskMemFree(pData);
	WriteLog(TEXT("Load Icon file success."));
}
void CClientDbMgr::LoadRate()
{
	WriteLog(TEXT("Load  rate Config..."));
	gRate.clear();
	char* pData = NULL;
	DWORD dwLength = 0;
	if(ReadConfigDatFile(CLIENT_DATA_RATE, pData, dwLength))
	{
		kmenu::CPkgHelper pkg(pData);
		DWORD dwSize = 0;
		pkg.Pop(dwSize);
		for (DWORD i=0; i<dwSize; ++i)
		{
			tRate rate = {0};
			pkg >> rate.szTitle >> rate.nStartIp >> rate.nEndIp >> rate.szContent;
			gRate.push_back(rate);
		}
	}
	WriteLog(TEXT("Load rate Config file success."));
	CoTaskMemFree(pData);
}


void CClientDbMgr::FreeGameInfo()
{
	std::map<DWORD, tagGameInfo*>::iterator it = m_mapGameInfo.begin();
	for (; it != m_mapGameInfo.end(); it ++)
		delete it->second;

	m_mapGameInfo.clear();
}

void CClientDbMgr::FreeIconInfo()
{
#if 1
	std::map<DWORD, tagIconInfo>::iterator iter = m_mapIconInfo.begin();
	for (iter; iter != m_mapIconInfo.end(); ++iter)
	{
		char* pData = iter->second.pIcon;
		if (pData != NULL)
		{
			delete [] pData;
			pData = NULL;
		}

		if(iter->second.pImage)
		{
			delete iter->second.pImage;
			iter->second.pImage = NULL;
		}
	}

	if (m_defaultImage)
	{
		delete m_defaultImage;
		m_defaultImage = NULL;
	}
#endif
	m_mapIconInfo.clear();
}

void CClientDbMgr::CheckCliService()
{
	TCHAR szPath[512] = {0};
	GetModuleFileName(NULL, szPath, _countof(szPath));
	PathRemoveFileSpec(szPath);
	PathAddBackslash(szPath);
	lstrcat(szPath, CLI_EXE_NAME);
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
	if (schSCManager == NULL)
		return;

	SC_HANDLE schService = CreateService(schSCManager, CLI_SERVICE_NAME, CLI_SERVICE_NAME, SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		szPath, NULL, NULL, NULL, NULL,NULL);
	if (schService == NULL)
		schService = OpenService(schSCManager, CLI_SERVICE_NAME, SERVICE_ALL_ACCESS);
	if (schService != NULL)
	{
		StartService(schService, 0, NULL);
		CloseServiceHandle(schService);
	}
	CloseServiceHandle(schSCManager);
}

DWORD AfxGetServiceState(LPCTSTR lpszSvrName)
{
	DWORD dwStatus = 0;
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hSCM == NULL)
		return dwStatus;

	SC_HANDLE hService = OpenService(hSCM, lpszSvrName, SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS);
	if (hService != NULL)
	{
		SERVICE_STATUS	serviceStatus = { 0 };
		if (QueryServiceStatus(hService, &serviceStatus))
		{
			dwStatus = serviceStatus.dwCurrentState;
		}
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return dwStatus;
	}
	CloseServiceHandle(hSCM);
	return dwStatus;
}

bool CClientDbMgr::ClientServiceIsRunning()
{
	bool bRunning = FALSE;
	bRunning = AfxGetServiceState(CLI_SERVICE_NAME)==SERVICE_RUNNING;
	return bRunning;
}
TCHAR CClientDbMgr::GetVdiskTmpDriver()
{
	TCHAR szDriver = TEXT('C');	//default driver c.
	/*std::map<CString, CString>::iterator iter = m_mapSysOpt.find(OPT_M_VDTMPDRV);
	if (iter != m_mapSysOpt.end() && iter->second.GetLength() && _istalpha(iter->second[0]))
	{
	szDriver = iter->second[0];
	}*/
	return szDriver;
}
#define SOFTWARE_KEY				TEXT("SOFTWARE\\{3798BE84-4E13-4b81-B8CF-5063730FF905}")
#define SOFTKEY_VDINFO				SOFTWARE_KEY TEXT("\\VDINFO")

CString CClientDbMgr::TransChToGBK( LPCTSTR lpszSouce )
{
	std::string source = CT2A(lpszSouce);
	char element[4];
	unsigned char chr;
	std::string res;
	for ( unsigned int i=0; i<source.length(); i++ )
	{
		chr = source[i];
		if (chr > 127)		//汉字
		{
			sprintf_s(element, "%%%02X", chr);				
			res += element;
			chr = source[++i];
			sprintf_s(element, "%%%02X", chr);
			res += element;
		}
		else if ( (chr >= '0' && chr <= '9') || 
			(chr >= 'a' && chr <= 'z') ||
			(chr >= 'A' && chr <= 'Z') )
		{
			res += chr;
		}
		//else if ( strchr(reserved,chr) || strchr(unsafe,chr) || strchr(other,chr) )
		else
		{
			sprintf_s( element, "%%%02X", chr); 
			res += element;
		}
	}	

	return CString(res.c_str());
}

void CClientDbMgr::LoadMultiChar()
{
	int nMultiCharCount = _countof(g_arMultiChar);
	for (int i = 0; i < nMultiCharCount; ++i)
	{
		m_mapMultiChar.insert(std::make_pair(g_arMultiChar[i].chChar, g_arMultiChar[i].szPy));
	}
}

BOOL CClientDbMgr::IsSetRate( DWORD nIp )
{
	BOOL bRate = FALSE;
	for(size_t i = 0;i<gRate.size();i++)
	{
		if(nIp>= ::ntohl(gRate[i].nStartIp) &&
			nIp <= ::ntohl(gRate[i].nEndIp))
		{
			bRate = TRUE;
			break;
		}
	}
	return bRate;
}

CString CClientDbMgr::GetRateText( DWORD nIp )
{
	CString strRate;
	for(size_t i = 0;i<gRate.size();i++)
	{
		if(nIp>= ::ntohl(gRate[i].nStartIp) &&
			nIp <= ::ntohl(gRate[i].nEndIp))
		{
			strRate = gRate[i].szContent;
			break;
		}
	}
	return strRate;
}

CString CClientDbMgr::GetRateTitleText( DWORD nIp )
{
	CString strRate;
	for(size_t i = 0;i<gRate.size();i++)
	{
		if(nIp>= ::ntohl(gRate[i].nStartIp) &&
			nIp <= ::ntohl(gRate[i].nEndIp))
		{
			strRate = gRate[i].szTitle;
			break;
		}
	}
	return strRate;
}

int  SkinNameToType(CString strName)
{
	int nType = -1;

	for (int i = 0; i < COUNT_SKIN; i++)
	{		
		TCHAR szTest[MAX_PATH] = { 0 };

		::LoadString(::GetModuleHandle(NULL), IDS_AYEL_NAME + i, szTest, MAX_PATH);

		CString strDefName = szTest;

		if (strDefName.CompareNoCase(strName) == 0)
		{
			nType = i;
			break;
		}
	}

	return nType;
}
CString GetMenuSkinPath(int nType)
{
	CString str;

	if (nType >= 0)
	{
		TCHAR szTest[MAX_PATH] = { 0 };

		::LoadString(::GetModuleHandle(NULL), IDS_AYEL_NAME + nType, szTest, MAX_PATH);

		str = szTest;
		str += L".jpg";
	}
	else
	{
		str = _T("default_skin.jpg");
	}

	return str;
}

bool CClientDbMgr::GetSkinPng( CString& strPng,int& nType )
{
	bool bResult = false;
#if 0
	if(GetBoolOpt(OPT_M_RANDSELMENUBG,false))
	{
		CString strCustomBg = GetStrOpt(OPT_M_CUSTOMMENUBG);
		std::vector<tstring> strSplit;
		kcore::AfxSplitString(strCustomBg.GetBuffer(),strSplit,_T('|'));
		srand(_time32(NULL));
		int nCoust = COUNT_SKIN + strSplit.size();
		int nIndex =  nCoust ? (rand() % nCoust) : 0;
		if(nIndex < COUNT_SKIN)
		{
			strPng = GetMenuSkinPath(nIndex);
			nType = 1;
			bResult = true;
		}
		else
		{
			nIndex -= COUNT_SKIN;
			CString strSrcFile = _T("config\\icon\\");
			strSrcFile += strSplit[nIndex].c_str();
			TCHAR szImage[MAX_PATH] = {0};
			AfxGetAppPath(szImage, MAX_PATH, _T("Skin\\Image\\"));
			lstrcat(szImage, strSplit[nIndex].c_str());
			if(PathFileExists(szImage))
			{
				strPng = strSplit[nIndex].c_str();
				nType = 2;
				bResult = true;
			}
			/*else
			{
				CServer server;
				if (!server.DownLoadFile(strSrcFile, szImage, -1, -1, TRUE))
				{
					AfxWriteLogger(g_Logger, TEXT("down menu bg file fail:%s"), szImage);
				}
				else
				{
					strPng = strSplit[nIndex].c_str();
					nType = 2;
					bResult = true;
				}
			}*/
		}
	}
	else
#endif
	{

		TCHAR szTest[MAX_PATH] = { 0 };

		::LoadString(::GetModuleHandle(NULL), IDS_LOL_NAME, szTest, MAX_PATH);

		strPng = GetStrOpt(OPT_M_MENUBG, szTest, true);
		int nIndex = SkinNameToType(strPng);
		//自定义皮肤先下载
		if(nIndex == -1)
		{
			CString strCustomBg = GetStrOpt(OPT_M_CUSTOMMENUBG);
			std::vector<tstring> strSplit;
			kcore::AfxSplitString(strCustomBg.GetBuffer(),strSplit,_T('|'));
			for(size_t i=0;i<strSplit.size();i++)
			{
				CString strOldFileName = strSplit[i].c_str();
				if(strOldFileName.CompareNoCase(strPng) == 0)
				{
					TCHAR szImage[MAX_PATH] = {0};
					AfxGetAppPath(szImage, MAX_PATH, _T("Skin\\Image\\"));
					lstrcat(szImage, strOldFileName);
					if(PathFileExists(szImage))
					{
						strPng = strOldFileName;
						nType = 2;
						bResult = true;
					}
					break;
				}
			}
		}
		else
		{
			strPng = GetMenuSkinPath(nIndex);
			nType = 1;
			bResult = true;
		}
	}

	return bResult;
}
bool CClientDbMgr::WriteDataFile(LPCTSTR szFileName, LPVOID pData, int length)
{
	TCHAR szFullFileName[MAX_PATH] = {0};
	if (lstrlen(szFileName) > 3 && szFileName[1] != TEXT(':') && szFileName[2] != TEXT('\\'))
		AfxGetAppPath(szFullFileName, MAX_PATH, szFileName);
	else
		lstrcpy(szFullFileName, szFileName);
	SetFileAttributes(szFullFileName, FILE_ATTRIBUTE_NORMAL);
	HANDLE hFile = ::CreateFile(szFullFileName, FILE_ALL_ACCESS, FILE_SHARE_WRITE | FILE_SHARE_READ, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwWrite = 0;
		WriteFile(hFile, pData, length, &dwWrite, NULL);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
		return true;
	}
	return false;
}

void CClientDbMgr::SetOpt( CString key,CString value )
{
	m_mapSysOpt[key] = value;
}

bool CClientDbMgr::SaveOpt()
{
	char  SendBuffer[PKG_MAXLENGTH] = {0};
	kmenu::CPkgHelper pkgSend(SendBuffer, CMD_CLI_GETSYSOPTION, OBJECT_CLIENT);
	pkgSend << m_mapSysOpt.size();
	std::map<CString, CString>::iterator itr = m_mapSysOpt.begin();
	for (;itr != m_mapSysOpt.end(); ++itr)
	{
		CString key = itr->first;
		CString vlue = itr->second;
		pkgSend << key.GetBuffer() << vlue.GetBuffer();
	}

	return WriteDataFile(TEXT("config\\koption.dat"), SendBuffer, pkgSend.GetLength());
}


CServer::CServer()
{
	m_sck = INVALID_SOCKET;
}

CServer::~CServer()
{
	CloseServer();
}

SOCKET AfxSocketConnect(LPCTSTR ip, WORD port, DWORD timeout)
{
	if (ip == NULL || port == 0)
		return INVALID_SOCKET;

	SOCKET sck = socket(AF_INET, SOCK_STREAM, 0);
	if (sck == INVALID_SOCKET)
		return INVALID_SOCKET;

	DWORD ul = 1;
	if (SOCKET_ERROR == ioctlsocket(sck, FIONBIO, &ul))
	{
		closesocket(sck);
		return INVALID_SOCKET;
	}

	sockaddr_in server = { 0 };
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(_bstr_t(ip));
	if (server.sin_addr.s_addr == INADDR_NONE)
	{
		closesocket(sck);
		return INVALID_SOCKET;
	}
	connect(sck, (PSOCKADDR)&server, sizeof(server));

	timeval to = { timeout, 0 };
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

LPTSTR AfxGetWinErrorText(LPTSTR buf, DWORD dwSize, DWORD dwError)
{
	LPTSTR lpszTemp = NULL;
	DWORD nCharNums = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
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
	if (lpszTemp)
		LocalFree((HLOCAL)lpszTemp);
	return buf;
}

void CServer::ReprotDataToServer(std::map<DWORD, DWORD>& GameClick,bool& bConnect, bool bAck /* = true */)
{
	m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);
	if (m_sck != INVALID_SOCKET)
	{
		bConnect = true;
		char buf[PKG_MAXLENGTH] = {0};
		kmenu::CPkgHelper pkg(buf, CMD_CLI_RPTGAMEMENUCLICK, OBJECT_CLIENT);

		//压游戏点击数
		pkg.Push(GameClick.size());
		std::map<DWORD, DWORD>::iterator iter = GameClick.begin();
		for (iter; iter != GameClick.end(); iter++)
		{
			pkg.Push(iter->first);
			pkg.Push(iter->second);
		}

		//压菜单点击数
		{
			//第一次上报就报了菜单运行了，以后就不需要报了，即上报为0.
			static bool bFirst = true;
			pkg.Push(bFirst ? 1ul : 0ul);

			//压入计算机名
			TCHAR Computer[MAX_PATH] = {0};
			DWORD dwSize = _countof(Computer);
			GetComputerName(Computer, &dwSize);
			_tcsupr_s(Computer);
			pkg.PushString(Computer);
			bFirst = false;
		}

		//清空己报过的游戏点击数。
		char response[PKG_MAXLENGTH] = {0};
		if (SendDataToServer(buf, pkg.GetLength(), bAck, response))
		{
			GameClick.clear();
		}
		CloseServer();
	}else
		bConnect = false;
}
bool CServer::ModifyOption(LPCTSTR szKey,LPCTSTR szValue,CString& strError)
{
	bool bResult = true;
	if(m_sck == INVALID_SOCKET)
		m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);
	if (m_sck == INVALID_SOCKET)
		return false;

	char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
	kmenu::CPkgHelper pkgSend(szSendBuffer, CMD_CLI_MODIFYTOPTION, OBJECT_CLIENT);
	pkgSend << szKey << szValue ;

	if (!SendDataToServer(szSendBuffer, pkgSend.GetLength(), true, szRecvBuffer))
	{
		kmenu::CPkgHelper pkgRecv(szRecvBuffer);
		if (!pkgRecv.IsStatusOk())
		{
			TCHAR szError[MAX_PATH] = {0};
			pkgRecv.PopString(szError);
			strError = szError;
			return false;
		}
		return false;
	}
	return true;
}
bool CServer::AddGameConfig( DWORD ngid,LPCTSTR configname,LPCTSTR filename,LPCTSTR pcname,CString& strError )
{
	bool bResult = true;
	if(m_sck == INVALID_SOCKET)
		m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);
	if (m_sck == INVALID_SOCKET)
		return false;

	char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
	kmenu::CPkgHelper pkgSend(szSendBuffer, CMD_CLI_ADDGAMECONFIG, OBJECT_CLIENT);
	pkgSend << ngid << configname << filename << pcname;

	if (!SendDataToServer(szSendBuffer, pkgSend.GetLength(), true, szRecvBuffer))
		return false;

	kmenu::CPkgHelper pkgRecv(szRecvBuffer);
	if (!pkgRecv.IsStatusOk())
	{
		TCHAR szError[MAX_PATH] = {0};
		pkgRecv.PopString(szError);
		strError = szError;
		return false;
	}

	return true;
}

HANDLE AfxCreateLogger(LPCTSTR szModuleName)
{
	CLogger* pLogger = new CLogger;
	pLogger->SetLogModule(szModuleName);

	return reinterpret_cast<HANDLE>(pLogger);
}

bool AfxIs64BitOpertionSystem()
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

void  AfxWriteLogger(HANDLE hLogger, LPCTSTR formater, ...)
{
	if (hLogger == NULL)
		return;

	CLogger* pLogger = reinterpret_cast<CLogger*>(hLogger);
	if (pLogger != NULL)
	{
		TCHAR xbuf[8192] = { 0 };
		int nLen = 0, nOffset = 0;

		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		nLen = _stprintf_s(&xbuf[nLen], _countof(xbuf) - nLen - 1, TEXT("%02d:%02d:%02d "), st.wHour, st.wMinute, st.wSecond);
		nOffset = nLen;

		va_list marker;
		va_start(marker, formater);
		nLen += _vstprintf_s(&xbuf[nLen], _countof(xbuf) - nLen - 1, formater, marker);
		va_end(marker);

		xbuf[nLen++] = TEXT('\r');
		xbuf[nLen++] = TEXT('\n');
		xbuf[nLen] = 0;
		pLogger->WriteLogger(xbuf, nLen);
		_tprintf_s(TEXT("%s"), &xbuf[nOffset]);
	}
}

void  AfxCloseLogger(HANDLE& hLogger)
{
	CLogger* pLogger = reinterpret_cast<CLogger*>(hLogger);
	if (pLogger != NULL)
	{
		delete pLogger;
		pLogger = NULL;
	}
	hLogger = NULL;
}
bool CServer::UploadFile( LPCTSTR szSrcFile,LPCTSTR szDstFile, BOOL bIsRelative /*= TRUE*/ )
{
	bool bResult = true;
	m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);
	if (m_sck == INVALID_SOCKET)
		return false;

	HANDLE	hFile = INVALID_HANDLE_VALUE;
	HANDLE  hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	static const DWORD FILE_BLOCK_SIZE = 0x8000;
	try
	{
		hFile = CreateFile(szSrcFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			throw GetLastError();

		LARGE_INTEGER liSize = {0};
		if(!GetFileSizeEx(hFile,&liSize))
			throw GetLastError();

		DWORD nError = 0;
		LONGLONG dwFileSize = liSize.QuadPart;
		LONGLONG left = dwFileSize;
		DWORD offset = 0;
		while (left)
		{
			char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
			char szDataBuffer[PKG_MAXLENGTH] = {0};
			DWORD dwBlkSize = left >= FILE_BLOCK_SIZE ? FILE_BLOCK_SIZE : (DWORD)left;

			kmenu::CPkgHelper pkgSend(szSendBuffer, CMD_CLI_UPLOADFILE, OBJECT_CLIENT);
			pkgSend << bIsRelative << szDstFile << offset << dwBlkSize;

			DWORD dwReadBytes = 0;
			OVERLAPPED ov = {0};
			ov.hEvent = hEvent;
			ov.Offset = offset;
			if (!ReadFile(hFile, (LPSTR)szDataBuffer, dwBlkSize, &dwReadBytes, &ov) && ERROR_IO_PENDING != GetLastError())
			{
				nError = GetLastError();
				break;
			}
			if (!GetOverlappedResult(hFile, &ov, &dwReadBytes, TRUE) || dwReadBytes != dwBlkSize)
			{
				nError = GetLastError();
				break;
			}
			pkgSend.PushPointer(szDataBuffer,dwBlkSize);
			if (!SendDataToServer(szSendBuffer, pkgSend.GetLength(), true, szRecvBuffer))
			{
				nError = 1000;
				break;
			}

			kmenu::CPkgHelper pkgRecv(szRecvBuffer);
			if (!pkgRecv.IsStatusOk())
			{
				nError = 1001;
				break;
			}

			offset += dwBlkSize;
			left -= dwBlkSize;
		}
		if(nError != 0)
			throw nError;
	
	}
	catch (DWORD dwError)
	{
		AfxWriteLogger(g_Logger, _T("upload %s failed:%d"), szSrcFile, dwError);
		bResult = false;
	}
	if (hEvent != NULL)
		CloseHandle(hEvent);
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return bResult;
}

void AfxCreateDirectory(LPCTSTR szDirectory, size_t offset /*= 0*/)
{
	tstring dir(szDirectory);
	size_t end = dir.rfind(TEXT('\\'));
	size_t pt = dir.find(TEXT('\\'), offset);

	while (pt != tstring::npos && pt <= end)
	{
		tstring path = dir.substr(0, pt + 1);
		DWORD dwRet = GetFileAttributes(path.c_str());
		if (dwRet == INVALID_FILE_ATTRIBUTES)
			::CreateDirectory(path.c_str(), NULL);
		pt = dir.find(TEXT('\\'), pt + 1);
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

void  AfxUtcTime2FileTime(DWORD time, FILETIME& ft)
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

LPTSTR AfxGetFileVersion(LPCTSTR pFileName, LPTSTR pVersion)
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

void AfxSocketClose(SOCKET& sck)
{
	if (sck != INVALID_SOCKET)
	{
		shutdown(sck, SD_BOTH);
		closesocket(sck);
		sck = INVALID_SOCKET;
	}
}

SOCKET AfxSocketListen(WORD port, LPCTSTR ip)
{
	SOCKET sckListen = INVALID_SOCKET;
	sockaddr_in addr = { 0 };
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

bool CServer::DownLoadFile( LPCTSTR szSrcFile, LPCTSTR szDstFile, DWORD dwFileSize /*= -1*/, DWORD dwFileModifyTime /*= -1*/, BOOL bIsRelative /*= TRUE*/ )
{
	m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);
	if (m_sck == INVALID_SOCKET)
		return false;

	static const DWORD FILE_BLOCK_SIZE = 0x8000;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	bool bRet = false;
	try
	{
		if (dwFileSize == -1)
		{
			char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
			kmenu::CPkgHelper pkgSend(szSendBuffer, CMD_CLI_QUERYFILEINFO, OBJECT_CLIENT);
			pkgSend << bIsRelative << szSrcFile;
			if (!SendDataToServer(szSendBuffer, pkgSend.GetLength(), true, szRecvBuffer))
				throw 0;

			kmenu::CPkgHelper pkgRecv(szRecvBuffer);
			if (!pkgRecv.IsStatusOk())
				throw 0;
			pkgRecv >> dwFileSize >> dwFileModifyTime;
		}

		SetFileAttributes(szDstFile, FILE_ATTRIBUTE_NORMAL);
		AfxCreateDirectory(szDstFile);
		if (PathIsDirectory(szDstFile))
			return true;
		hFile = ::CreateFile(szDstFile, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			throw 0;
		DWORD left = dwFileSize;
		while (left)
		{
			char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
			kmenu::CPkgHelper pkgSend(szSendBuffer, CMD_CLI_DOWNFILEBLOCK, OBJECT_CLIENT);
			DWORD dwBlkSize = left >= FILE_BLOCK_SIZE ? FILE_BLOCK_SIZE : left;
			pkgSend << bIsRelative << szSrcFile << dwFileSize - left << dwBlkSize;
			if (!SendDataToServer(szSendBuffer, pkgSend.GetLength(), true, szRecvBuffer))
				throw 0;

			kmenu::CPkgHelper pkgRecv(szRecvBuffer);
			if (!pkgRecv.IsStatusOk())
				throw 0;
			DWORD dwByteWrite = 0;
			if (!WriteFile(hFile, szRecvBuffer + sizeof(kmenu::PkgHeader), dwBlkSize, &dwByteWrite, NULL) || dwBlkSize != dwByteWrite)
				throw 0;
			left -= dwBlkSize;
		}
		SetEndOfFile(hFile);
		if (dwFileModifyTime != -1)
		{
			FILETIME ft = {0};
			AfxUtcTime2FileTime(dwFileModifyTime, ft);
			SetFileTime(hFile, &ft, &ft, &ft);
		}
		bRet = true;
	}
	catch (...) { 
		AfxWriteLogger(g_Logger, _T("down [%s] to [%s] failed!"), szSrcFile, szDstFile);
		bRet = false; 
	}
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if (!bRet)
		DeleteFile(szDstFile);
	return bRet;
}
#define CMD_CLI_QUERYKMEAUXMLEX			0x0425		//获取新菜单XML

bool CServer::DownloadMenuXML(char*& data, int& size)
{
	bool is_suceess = false;
	m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);
	if (m_sck != INVALID_SOCKET)
	{
		DWORD dwoffset = 0, dwlength = 0, dwall = 0;
		data = NULL;
		do
		{
			char buf[PKG_MAXLENGTH] = { 0 };
			kmenu::CPkgHelper pkg(buf, CMD_CLI_QUERYKMEAUXMLEX, OBJECT_CLIENT);
			pkg << dwall << dwoffset << dwlength;
			char RecvBuffer[PKG_MAXLENGTH] = { 0 };
			if (!SendDataToServer(buf, pkg.GetLength(), true, RecvBuffer))
			{
				CloseServer();
				return false;
			}

			kmenu::CPkgHelper pkgRecv(RecvBuffer);
			if (!pkgRecv.IsStatusOk())
			{
				CloseServer();
				return false;
			}

			pkgRecv >> dwall >> dwoffset >> dwlength;
			if (dwall <= 0)
			{
				CloseServer();
				return false;
			}

			if (data == NULL)
				data = (char*)malloc(dwall + 1);

			pkgRecv.PopPointer(data + dwoffset, dwlength);

			dwoffset = dwoffset + dwlength;
			dwlength = dwall - dwoffset;
		} while (dwlength > 0);

		data[dwall] = 0;
		size = dwall;
		is_suceess = true;
		
		CloseServer();
	}
	return is_suceess;
}

bool CServer::DownloadIcon(int gid)
{
	bool is_suceess = false;
	m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);
	if (m_sck != INVALID_SOCKET)
	{
		char buf[PKG_MAXLENGTH] = {0};
		kmenu::CPkgHelper pkg(buf, CMD_CLI_GETICONINFO, OBJECT_CLIENT);

		pkg.Push((DWORD)gid);
		char RecvBuffer[PKG_MAXLENGTH] = {0};
		if (SendDataToServer(buf, pkg.GetLength(), true, RecvBuffer))
		{
			kmenu::CPkgHelper pkgRecv(RecvBuffer);
			if (!pkgRecv.IsStatusOk())
			{
				CloseServer();
				return false;
			}

			tagIconInfo iconInfo = {0};
			pkgRecv >> iconInfo.dwGid;
			pkgRecv >> iconInfo.dwHash;
			pkgRecv >> iconInfo.dwLength;

			if (iconInfo.dwLength > 0)
			{
				iconInfo.pIcon = new char[iconInfo.dwLength + 1];
				pkgRecv.PopPointer(iconInfo.pIcon, iconInfo.dwLength);

				{
					// 创建目录
					TCHAR path[MAX_PATH];
					AfxGetAppPath(path, MAX_PATH, _T("config\\icon"));
					CreateDirectory(path, NULL);
				}

				CString path2;
				path2.Format(_T("config\\icon\\%i.ico"), gid);
				TCHAR path[MAX_PATH];
				AfxGetAppPath(path, MAX_PATH, path2);

				if (WriteFileData(path, iconInfo.pIcon, iconInfo.dwLength))
				{
					is_suceess = true;
				}

				delete [] iconInfo.pIcon;
			}

		}
		CloseServer();
	}
	return is_suceess;
}

bool CServer::UDiskRegistryUser(LPCTSTR user, LPCTSTR password, tstring& errinfo)
{
	TCHAR szTest[MAX_PATH] = { 0 };
	bool		ret = false;

	errinfo.clear();
	m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);
	if (m_sck == INVALID_SOCKET)
	{
		::LoadString(::GetModuleHandle(NULL), IDS_CONNECT_SERVER_FAILED, szTest, MAX_PATH);
		errinfo = szTest;
		return false;
	}

	char buf[PKG_MAXLENGTH] = {0}, response[PKG_MAXLENGTH] = {0};
	kmenu::CPkgHelper Out(buf, CMD_USER_REGISTER, OBJECT_CLIENT);
	Out << user << password;
	if (!SendDataToServer(buf, Out.GetLength(), true, response))
	{
		::LoadString(::GetModuleHandle(NULL), IDS_SENDRECV_FAILED, szTest, MAX_PATH);
		errinfo = szTest;
	}
	else
	{
		kmenu::CPkgHelper In(response);
		if (In.IsStatusOk())
		{
			if(AfxGetDbMgr()->GetUDiskDriver() == (TCHAR)0)
			{
				::LoadString(::GetModuleHandle(NULL), IDS_UDISK_LETTER_EXIST, szTest, MAX_PATH);
				errinfo = szTest;
				return false;
			}
			DWORD nRet = 0;
			ret = UDiskMount(user,nRet);
			if(!ret)
			{
				TCHAR szBuffer[MAX_PATH] = {0};
				::LoadString(::GetModuleHandle(NULL), IDS_LOAD_UDISK_FAILED_FMT, szTest, MAX_PATH);
				_stprintf_s(szBuffer, _countof(szBuffer), szTest, nRet);
				errinfo = szBuffer;
			}
		}
		else
			In >> errinfo;
	}
	CloseServer();
	return ret;
}

bool CServer::UDiskLoginUser(LPCTSTR user, LPCTSTR password, tstring& errinfo)
{
	TCHAR szTest[MAX_PATH] = { 0 };
	bool		ret = false;

	errinfo.clear();

	if(AfxGetDbMgr()->GetUDiskDriver() == (TCHAR)0)
	{
		::LoadString(::GetModuleHandle(NULL), IDS_UDISK_LETTER_EXIST, szTest, MAX_PATH);
		errinfo = szTest;
		return ret;
	}

	m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);
	if (m_sck == INVALID_SOCKET)
	{
		::LoadString(::GetModuleHandle(NULL), IDS_CONNECT_SERVER_FAILED, szTest, MAX_PATH);
		errinfo = szTest;
		return false;
	}

	char buf[PKG_MAXLENGTH] = {0}, response[PKG_MAXLENGTH] = {0};
	kmenu::CPkgHelper Out(buf, CMD_USER_LOGIN, OBJECT_CLIENT);
	Out << user << password;
	if (!SendDataToServer(buf, Out.GetLength(), true, response))
	{
		::LoadString(::GetModuleHandle(NULL), IDS_SENDRECV_FAILED, szTest, MAX_PATH);
		errinfo = szTest;
	}
	else
	{
		kmenu::CPkgHelper In(response);
		if (In.IsStatusOk())
		{
			DWORD nRet = 0;
			if(UDiskMount(user,nRet))
				ret = true;
			else
			{
				TCHAR szBuffer[MAX_PATH] = {0};
				::LoadString(::GetModuleHandle(NULL), IDS_LOAD_UDISK_FAILED_FMT, szTest, MAX_PATH);
				_stprintf_s(szBuffer, _countof(szBuffer), szTest, nRet);
				errinfo = szBuffer;
				ret = false;
			}
		}
		else
			In >> errinfo;
	}
	CloseServer();
	return ret;
}

bool CServer::UDiskModifyPassword(LPCTSTR user, LPCTSTR password, LPCTSTR newpassword, tstring& errinfo)
{
	TCHAR szTest[MAX_PATH] = { 0 };
	bool		ret = false;

	

	errinfo.clear();
	m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);
	if (m_sck == INVALID_SOCKET)
	{
		::LoadString(::GetModuleHandle(NULL), IDS_CONNECT_SERVER_FAILED, szTest, MAX_PATH);
		errinfo = szTest;
		return false;
	}

	char buf[PKG_MAXLENGTH] = {0}, response[PKG_MAXLENGTH] = {0};
	kmenu::CPkgHelper Out(buf, CMD_USER_MODIFYPASSWORD, OBJECT_CLIENT);
	Out << user << password << newpassword;
	if (!SendDataToServer(buf, Out.GetLength(), true, response))
	{
		::LoadString(::GetModuleHandle(NULL), IDS_SENDRECV_FAILED, szTest, MAX_PATH);
		errinfo = szTest;
	}
	else
	{
		kmenu::CPkgHelper In(response);
		if (In.IsStatusOk())
			ret = true;
		else
			In >> errinfo;
	}
	CloseServer();
	return ret;
}

bool CServer::AdminTestPassword(LPCTSTR password, tstring& errinfo)
{
	TCHAR szTest[MAX_PATH] = { 0 };
	bool		ret = false;

	errinfo.clear();
	m_sck = AfxSocketConnect(AfxGetDbMgr()->LoadIpAddr(), APP_TCP_PORT);
	if (m_sck == INVALID_SOCKET)
	{
		::LoadString(::GetModuleHandle(NULL), IDS_CONNECT_SERVER_FAILED, szTest, MAX_PATH);
		errinfo = szTest;
		return false;
	}

	char buf[PKG_MAXLENGTH] = { 0 }, response[PKG_MAXLENGTH] = { 0 };
	kmenu::CPkgHelper Out(buf, CMD_USER_QUERYPWD, OBJECT_CLIENT);
	Out << password;
	if (!SendDataToServer(buf, Out.GetLength(), true, response))
	{
		::LoadString(::GetModuleHandle(NULL), IDS_SENDRECV_FAILED, szTest, MAX_PATH);
		errinfo = szTest;
	}
	else
	{
		kmenu::CPkgHelper In(response);
		if (In.IsStatusOk())
		{
			DWORD r = 0;
			In.Pop(r);

			if (r == 1)
				ret = true;
			else
				In >> errinfo;
		}

		else
			In >> errinfo;
	}
	CloseServer();
	return ret;
}

#ifndef _WIN64
#define PLUG_UDISK_LIBRAY_NAME		TEXT("kapp.dll")
#else
#define PLUG_UDISK_LIBRAY_NAME		TEXT("kapp64.dll")
#endif

#define KDISK_NTDEVICE_NAME		L"\\Device\\KDiskVol%c"
#define KDISK_DOSDEVICE_NAME	L"\\DosDevices\\KDiskVol%c"

#define KDISK_WIN32_NAME		TEXT("\\\\.\\KDiskVol%c")
#define KDISK_DRIVER_NAME_X86	TEXT("kdisk.sys")
#define KDISK_DRIVER_NAME_X64	TEXT("kdisk64.sys")
#define KDISK_SERVICE_NAME		TEXT("kdisk")

#define IOCTL_KDISK_MOUNT		CTL_CODE(0x8000, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS|FILE_WRITE_ACCESS)
#define IOCTL_KDISK_UNMOUNT		CTL_CODE(0x8000, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS|FILE_WRITE_ACCESS)

DWORD FileDiskInstallDriver()
{
	bool b64Bit = AfxIs64BitOpertionSystem();
	TCHAR szDriverFile[MAX_PATH] = { 0 };
	AfxGetAppPath(szDriverFile, _countof(szDriverFile), b64Bit ? KDISK_DRIVER_NAME_X64 : KDISK_DRIVER_NAME_X86);
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager == NULL)
		return GetLastError();
	SC_HANDLE schService = CreateService(schSCManager, KDISK_SERVICE_NAME, KDISK_SERVICE_NAME,
		SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE,
		szDriverFile, NULL, NULL, NULL, NULL, NULL);
	if (schService == NULL)
	{
		DWORD dwError = GetLastError();
		if (ERROR_SERVICE_EXISTS != dwError)
		{
			CloseServiceHandle(schSCManager);
			return dwError;
		}
		schService = OpenService(schSCManager, KDISK_SERVICE_NAME, SERVICE_ALL_ACCESS);
		if (schService == NULL)
		{
			CloseServiceHandle(schSCManager);
			return GetLastError();
		}
	}
	StartService(schService, 0, NULL);
	SERVICE_STATUS status = { 0 };
	while (QueryServiceStatus(schService, &status))
	{
		Sleep(1000);
		if (status.dwCurrentState == SERVICE_RUNNING || status.dwCurrentState == SERVICE_STOPPED)
			break;
	}
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return (status.dwCurrentState == SERVICE_RUNNING);
}
DWORD FileDiskMount(DWORD DiskType, DWORD Ip, WORD Port, TCHAR DriverLetter, LPCTSTR UserId)
{
	DriverLetter = _totupper(DriverLetter);
	TCHAR DeviceName[MAX_PATH] = { 0 };
	_stprintf_s(DeviceName, _countof(DeviceName), KDISK_WIN32_NAME, DriverLetter);
	HANDLE hDevice = CreateFile(DeviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		FileDiskInstallDriver();
		hDevice = CreateFile(DeviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
		if (hDevice == INVALID_HANDLE_VALUE)
			return GetLastError();
	}

	MOUNTINFO info = { 0 };
	info.DiskType = DiskType;
	info.Ip = Ip;
	info.Port = Port;
	lstrcpyW(info.UserId, _bstr_t(UserId == NULL ? TEXT("") : UserId));

	DWORD BytesReturned = 0;
	if (!DeviceIoControl(hDevice, IOCTL_KDISK_MOUNT, &info, sizeof(info), NULL, 0, &BytesReturned, NULL))
	{
		DWORD dwError = GetLastError();
		CloseHandle(hDevice);
		return dwError;
	}
	CloseHandle(hDevice);

	TCHAR VolumeName[MAX_PATH] = { 0 };
	_stprintf_s(VolumeName, _countof(VolumeName), TEXT("\\\\.\\%c:"), DriverLetter);
	_stprintf_s(DeviceName, _countof(DeviceName), KDISK_NTDEVICE_NAME, DriverLetter);
	if (!DefineDosDevice(DDD_RAW_TARGET_PATH, &VolumeName[4], DeviceName))
		return GetLastError();

	::SHChangeNotify(SHCNE_DRIVEADD, SHCNF_PATH, VolumeName + 4, NULL);
	//	BroadDeviceChange( DBT_DEVICEARRIVAL, VolumeName[4]);
	return ERROR_SUCCESS;
}

DWORD FileDiskUnmount(TCHAR DriverLetter)
{
	TCHAR Buffer[MAX_PATH] = { 0 };
	TCHAR DeviceName[MAX_PATH] = { 0 };
	TCHAR szMsg[512] = { 0 };
	DWORD dwError = ERROR_SUCCESS;
	HANDLE hDevice = INVALID_HANDLE_VALUE;

	_stprintf_s(szMsg, 512, _T("FileDiskUnmount IN Letter=%c\n"), DriverLetter);
	::OutputDebugString(szMsg);
	do{
		DriverLetter = _totupper(DriverLetter);
		_stprintf_s(Buffer, _countof(Buffer), TEXT("%c:"), DriverLetter);
		QueryDosDevice(Buffer, DeviceName, _countof(DeviceName));
		_stprintf_s(Buffer, _countof(Buffer), KDISK_NTDEVICE_NAME, DriverLetter);
		if (lstrcmpi(Buffer, DeviceName) != 0)
		{
			//_stprintf_s(szMsg, 512, _T("check parameter DeviceName=%s, \n"), DeviceName);
			//::OutputDebugString(szMsg);

			dwError = ERROR_INVALID_PARAMETER;
			break;
		}

		_stprintf_s(DeviceName, _countof(DeviceName), KDISK_WIN32_NAME, DriverLetter);
		hDevice = CreateFile(DeviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
		if (hDevice == INVALID_HANDLE_VALUE)
		{
			dwError = GetLastError();
			_stprintf_s(szMsg, 512, _T("CreateFile err=%d, file=%s\n"), dwError, DeviceName);
			::OutputDebugString(szMsg);
			break;
		}

		TCHAR szVol[MAX_PATH];
		_stprintf_s(szVol, _countof(szVol), _T("\\\\.\\%c:"), DriverLetter);
		HANDLE hVolDevice = CreateFile(szVol, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
		if (hVolDevice != INVALID_HANDLE_VALUE)
		{
			FlushFileBuffers(hVolDevice);
			CloseHandle(hVolDevice);
		}

		DWORD   BytesReturned = 0;
		if (!DeviceIoControl(hDevice, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &BytesReturned, NULL))
		{
			//dwError = GetLastError();
		}
		if (!DeviceIoControl(hDevice, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &BytesReturned, NULL))
		{
			dwError = GetLastError();
			_stprintf_s(szMsg, 512, _T("Dismount_Volulme err=%d\n"), dwError);
			::OutputDebugString(szMsg);
			DeviceIoControl(hDevice, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &BytesReturned, NULL);
			break;
		}
		if (!DeviceIoControl(hDevice, IOCTL_KDISK_UNMOUNT, NULL, 0, NULL, 0, &BytesReturned, NULL))
		{
			dwError = GetLastError();
			_stprintf_s(szMsg, 512, _T("Disk_mount err=%d\n"), dwError);
			::OutputDebugString(szMsg);
		}
		if (!DeviceIoControl(hDevice, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &BytesReturned, NULL))
		{
			//dwError = GetLastError();
		}
		CloseHandle(hDevice);
		hDevice = INVALID_HANDLE_VALUE;

		TCHAR VolumeName[MAX_PATH] = { 0 };
		_stprintf_s(VolumeName, _countof(VolumeName), TEXT("\\\\.\\%c:"), DriverLetter);
		if (!DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL))
		{
			dwError = GetLastError();
			_stprintf_s(szMsg, 512, _T("Remove Volulme err=%d\n"), dwError);
			::OutputDebugString(szMsg);
		}

		dwError = ERROR_SUCCESS;
	} while (0);

	_stprintf_s(szMsg, 512, _T("FileDiskUnmount OUT err=%d, file=%s\n"), dwError, DeviceName);
	::OutputDebugString(szMsg);
	return dwError;
}

bool CServer::UDiskMount(LPCTSTR name,DWORD& nRet)
{
	TCHAR szBuffer[MAX_PATH] = {0};
	tstring ip = AfxGetSysString(CLIENT_INII_SECTION_SERVER, CLIENT_INII_KEY_IP,szBuffer,MAX_PATH, TEXT("127.0.0.1"));
	nRet = FileDiskMount(0,inet_addr(_bstr_t(ip.c_str())), htons(UDK_TCP_PORT), AfxGetDbMgr()->GetUDiskDriver(), name);
	if (nRet != 0)
	{
		//AfxMessageBox(TEXT("加载个人磁盘失败."));
		return false;
	}
	return true;
}

bool CServer::UDiskUnMount(bool showtip)
{
	CString drv;
	drv.Format(TEXT("%c:\\"), AfxGetDbMgr()->GetUDiskUnmoutDriver());
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(drv))
		return true;

	DWORD ret = FileDiskUnmount(AfxGetDbMgr()->GetUDiskUnmoutDriver());
	if (ret != 0 && showtip)
	{
		TCHAR szTitle[MAX_PATH] = { 0 };
		TCHAR szTmp[MAX_PATH] = { 0 };
		::LoadString(::GetModuleHandle(NULL), IDS_TITLE, szTitle, MAX_PATH);
		::LoadString(::GetModuleHandle(NULL), IDS_PLEASE_CLOSE_FILE_ON_UDISK, szTmp, MAX_PATH);

		::MessageBox(::GetActiveWindow(), szTitle, szTmp, MB_OK);
	}
	return (ret == 0);
	return true;
}

bool CServer::SendDataToServer(char* pData, int len, bool bAck, char* buf)
{
	//send data to server.
	int nSend = 0;
	while(nSend < len)
	{
		int nlen = send(m_sck, pData+nSend, len-nSend, 0);
		if(nlen <= 0)
		{
			CloseServer();;
			return false;
		}
		nSend += nlen;
	}
	if (!bAck)
		return true;

	kmenu::PkgHeader* pheader = (kmenu::PkgHeader*)buf;
	int   nRecvLen = 0;
	while (1)
	{
		FD_SET fdset, fderror;
		timeval tv = { 3, 0 };

		FD_ZERO(&fdset);
		FD_ZERO(&fderror);
		FD_SET(m_sck, &fdset);
		FD_SET(m_sck, &fderror);
		int ret = select(0, &fdset, NULL, &fderror, &tv);
		if (ret <= 0)
		{
			CloseServer();
			return false;
		}


		if (FD_ISSET(m_sck, &fderror))
		{
			CloseServer();
			return false;
		}

		if (FD_ISSET(m_sck, &fderror))
		{
			AfxWriteLogger(g_Logger, TEXT("socket error"));
			CloseServer();
			return false;
		}

		if (FD_ISSET(m_sck, &fdset))
		{
			int len = recv(m_sck, buf + nRecvLen, PKG_MAXLENGTH - nRecvLen, 0);
			if (len <= 0)
			{
				CloseServer();
				return false;
			}
			nRecvLen += len;

			if (nRecvLen >= sizeof(kmenu::PkgHeader) && nRecvLen >= (int)pheader->length)
				return true;
		}
	}
	return false;
}

void CServer::CloseServer()
{
	if (m_sck != INVALID_SOCKET)
	{
		closesocket(m_sck);
		m_sck = INVALID_SOCKET;
	}
}

bool SortIconEntry(ICONDIRENTRY& icon1, ICONDIRENTRY& icon2)
{
	return icon1.bWidth < icon2.bWidth;
}
Gdiplus::Bitmap* ReadIcon(LPCTSTR iconfile, int icon_size)
{
	errno_t err = 0;
	FILE* fp = NULL;
	err = _wfopen_s(&fp, iconfile, _T("rb"));
	if (err != 0)
		return NULL;

	unsigned char* data = NULL;
	Gdiplus::Bitmap* image = NULL;
	std::vector<ICONDIRENTRY>	icon_entrys;

	ICONHEADER icon_header;
	int ret = 0;
	ret = fread(&icon_header, sizeof(icon_header), 1, fp);
	if (ret != 1)
	{
		goto failed;
	}

	//icon_header.idType = icon_header.idType;
	//icon_header.idCount = icon_header.idCount;

	for (int i = 0; i < icon_header.idCount; ++i)
	{
		ICONDIRENTRY icon;
		ret = fread(&icon, sizeof(icon), 1, fp);
		if (ret != 1)
		{
			goto failed;
		}

		icon_entrys.push_back(icon);
	}

	// 查找32位色的最佳匹配
	ICONDIRENTRY* best_icon = NULL;
	for (int i = 0; i < (int)icon_entrys.size(); ++i)
	{
		ICONDIRENTRY& icon = icon_entrys[i];
		if (icon.wBitCount == 32)
		{
			if (icon.bWidth == icon_size)
			{
				best_icon = &icon;
				break;
			}
		}
	}
	if (best_icon == NULL)
	{
		std::sort(icon_entrys.begin(), icon_entrys.end(), SortIconEntry);
		for (int i = 0; i < (int)icon_entrys.size(); ++i)
		{
			ICONDIRENTRY& icon = icon_entrys[i];
			if (icon.wBitCount == 32 || icon.bColorCount == 32)
			{
				if (icon.bWidth >= icon_size)
				{
					best_icon = &icon;
					break;
				}
			}
		}
	}

	if (best_icon == NULL)
	{
		fclose(fp);
		// 没有32位色的最佳匹配，就用Windows API加载
		HICON hIcon = NULL;
		//LoadIconWithScaleDown(NULL, iconfile, 48, 48, &hIcon);
		hIcon = reinterpret_cast<HICON>(::LoadImage(0, iconfile, IMAGE_ICON,
			48, 48, LR_LOADFROMFILE | LR_DEFAULTCOLOR));
		if (hIcon)
		{
			Gdiplus::Bitmap* image = new Gdiplus::Bitmap(hIcon);
			DeleteObject(hIcon);
			return image;
		}
		return NULL;
	}

	ret = fseek(fp, best_icon->dwImageOffset, SEEK_SET);
	if (ret != 0)
	{
		goto failed;
	}

	// 读取头
	BITMAPINFOHEADER header;
	ret = fread(&header, sizeof(header), 1, fp);
	if (ret != 1)
	{
		goto failed;
	}

	int width = best_icon->bWidth;
	int height = best_icon->bWidth;
	if (height < 0) height = -height;
	int data_size = 4 * width * height;
	data = (unsigned char*)malloc(data_size);
	if (data == NULL)
	{
		goto failed;
	}

	ret = fread(data, data_size, 1, fp);
	if (ret != 1)
	{
		goto failed;
	}

	{
		image = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);

		// 图像是反的，要调转一次
		unsigned char* buffer = (unsigned char*)malloc(data_size);
		if (buffer == NULL)
		{
			goto failed;
		}
		int pitch = 4 * width;
		for (int i = 0; i < height; ++i)
		{
			memcpy(buffer + (pitch*(height - i - 1)), data + (pitch*i), pitch);
		}

		Gdiplus::BitmapData data2;
		Gdiplus::Rect rc(0, 0, width, height);
		data2.Scan0 = (void*)buffer;
		data2.Stride = 4 * width;
		if (Gdiplus::Ok == image->LockBits(&rc,
			Gdiplus::ImageLockModeWrite | Gdiplus::ImageLockModeUserInputBuf,
			PixelFormat32bppARGB, &data2))
		{
			image->UnlockBits(&data2);
		}
		free(buffer);
	}

	fclose(fp);
	fp = NULL;

	if (data)
	{
		free(data);
		data = NULL;
	}

	if (width == icon_size && height == icon_size)
	{
		return image;
	}

	{
		Gdiplus::Bitmap* image2 = new Gdiplus::Bitmap(icon_size, icon_size, PixelFormat32bppARGB);
		Gdiplus::Graphics g(image2);

		g.SetInterpolationMode(Gdiplus::InterpolationModeBicubic);
		g.DrawImage(image, Gdiplus::Rect(0, 0, icon_size, icon_size), 0, 0, width, height, Gdiplus::UnitPixel);

		delete image;
		return image2;
	}

failed:

	if (fp)
	{
		fclose(fp);
	}
	if (data) free(data);
	return NULL;
}
Gdiplus::Bitmap* ReadIcon(void* icon_data, unsigned int icon_data_size, int icon_size)
{
	ReadBuffer read_buffer((unsigned char*)icon_data, icon_data_size, false);

	unsigned char* data = NULL;
	Gdiplus::Bitmap* image = NULL;
	std::vector<ICONDIRENTRY>	icon_entrys;

	ICONHEADER icon_header;	if (!read_buffer.read_bytes((unsigned char*)&icon_header, sizeof(icon_header)))
	{
		goto failed;
	}

	icon_header.idType = icon_header.idType;
	icon_header.idCount = icon_header.idCount;

	for (int i = 0; i < icon_header.idCount; ++i)
	{
		ICONDIRENTRY icon;
		if (!read_buffer.read_bytes((unsigned char*)&icon, sizeof(icon)))
		{
			goto failed;
		}

		icon_entrys.push_back(icon);
	}

	// 查找32位色的最佳匹配
	ICONDIRENTRY* best_icon = NULL;
	for (int i = 0; i < (int)icon_entrys.size(); ++i)
	{
		ICONDIRENTRY& icon = icon_entrys[i];
		if (icon.wBitCount == 32 || icon.bColorCount == 32)
		{
			if (icon.bWidth == icon_size)
			{
				best_icon = &icon;
				break;
			}
		}
	}
	if (best_icon == NULL)
	{
		std::sort(icon_entrys.begin(), icon_entrys.end(), SortIconEntry);
		for (int i = 0; i < (int)icon_entrys.size(); ++i)
		{
			ICONDIRENTRY& icon = icon_entrys[i];
			if (icon.wBitCount == 32)
			{
				if (icon.bWidth >= icon_size)
				{
					best_icon = &icon;
					break;
				}
			}
		}
	}

	if (best_icon == NULL)
	{
		HICON hIcon = NULL;
		int offset = LookupIconIdFromDirectoryEx((PBYTE)icon_data, TRUE, icon_size, icon_size, LR_DEFAULTCOLOR);
		if (offset != 0) {
			hIcon = CreateIconFromResourceEx((PBYTE)icon_data + offset, 0, TRUE, 0x30000, icon_size, icon_size, LR_DEFAULTCOLOR);
		}
		//HICON hIcon = CreateIconFromResourceEx((PBYTE)icon_data, icon_data_size, TRUE, 0x00030000, icon_size, icon_size, LR_DEFAULTCOLOR);
		if (hIcon)
		{
			Gdiplus::Bitmap* image = new Gdiplus::Bitmap(hIcon);
			DestroyIcon(hIcon);
			if (image)
			{
				read_buffer.detach();
				return image;
			}
		}
		goto failed;
	}

	if (!read_buffer.set_pos(best_icon->dwImageOffset))
	{
		goto failed;
	}

	// 读取头
	BITMAPINFOHEADER header;
	if (!read_buffer.read_bytes((unsigned char*)&header, sizeof(header)))
	{
		goto failed;
	}

	int width = best_icon->bWidth;
	int height = best_icon->bWidth;
	if (height < 0) height = -height;
	int data_size = 4 * width * height;
	data = (unsigned char*)malloc(data_size);
	if (data == NULL)
	{
		goto failed;
	}

	if (!read_buffer.read_bytes((unsigned char*)data, data_size))
	{
		goto failed;
	}

	{
		image = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);

		// 图像是反的，要调转一次
		unsigned char* buffer = (unsigned char*)malloc(data_size);
		if (buffer == NULL)
		{
			goto failed;
		}
		int pitch = 4 * width;
		for (int i = 0; i < height; ++i)
		{
			memcpy(buffer + (pitch*(height - i - 1)), data + (pitch*i), pitch);
		}

		Gdiplus::BitmapData data2;
		Gdiplus::Rect rc(0, 0, width, height);
		data2.Scan0 = (void*)buffer;
		data2.Stride = 4 * width;
		if (Gdiplus::Ok == image->LockBits(&rc,
			Gdiplus::ImageLockModeWrite | Gdiplus::ImageLockModeUserInputBuf,
			PixelFormat32bppARGB, &data2))
		{
			image->UnlockBits(&data2);
		}
		free(buffer);
	}

	if (data)
	{
		free(data);
		data = NULL;
	}

	read_buffer.detach();

	if (width == icon_size && height == icon_size)
	{
		return image;
	}

	{
		Gdiplus::Bitmap* image2 = new Gdiplus::Bitmap(icon_size, icon_size, PixelFormat32bppARGB);
		Gdiplus::Graphics g(image2);

		g.SetInterpolationMode(Gdiplus::InterpolationModeBicubic);
		g.DrawImage(image, Gdiplus::Rect(0, 0, icon_size, icon_size), 0, 0, width, height, Gdiplus::UnitPixel);

		delete image;
		return image2;
	}

failed:

	read_buffer.detach();

	if (data) free(data);
	return NULL;
}

Gdiplus::Bitmap* GetGameIcon(int gid, int icon_size)
{
	std::map<DWORD, tagIconInfo>::iterator iter = AfxGetDbMgr()->m_mapIconInfo.find(gid);

	if (iter != AfxGetDbMgr()->m_mapIconInfo.end())
	{
		if(iter->second.pImage == NULL)
			iter->second.pImage = ReadIcon(iter->second.pIcon, iter->second.dwLength, icon_size);

		if (iter->second.pImage)
			return iter->second.pImage;
	}
	
	return NULL;
}

void FindClienPath(tagGameInfo*	pGame,LPTSTR pPath,int nLenth)
{
	if(pGame == NULL)
		return;
	CString strPath = GetGameConfigFilePath(pGame);
	if(!strPath.IsEmpty())
		_tcscpy_s(pPath,nLenth,strPath);
}
#define SOFTWARE_KEY				TEXT("SOFTWARE\\{3798BE84-4E13-4b81-B8CF-5063730FF905}")

CString GetGameConfigFilePath(tagGameInfo* pGame)
{
	if(pGame == NULL)
		return _T("");

	CStringArray	arDisks;
	AfxGetFixedDisks(arDisks);

	CString strGamePath;
	BOOL bSsdPath = FALSE;
	BOOL bVdiskPath = FALSE;
	CString strSsdGameDir;
	CString strVdiskGameDir;
	CString szWorkDir = pGame->gclipath;
	DWORD nDisk = 0, dwRegType = REG_DWORD,nLenth = sizeof(DWORD);
	SHGetValue(HKEY_LOCAL_MACHINE, SOFTWARE_KEY, _T("ssdgamedisk"), &dwRegType, &nDisk, &nLenth);
	if(nDisk > 0)
	{
		pGame->gclipath[0] = TCHAR(nDisk);
		szWorkDir = pGame->gclipath;
		if (PathFileExists(szWorkDir + pGame->grungame))
		{
			bSsdPath = TRUE;
			strSsdGameDir = szWorkDir;
		}
	}

	for (int i =arDisks.GetSize() -1;i>=0;i--)
	{
		if(toupper(arDisks[i][0]) == toupper(TCHAR(nDisk)))
			continue;

		CString strSzTemp;
		strSzTemp += arDisks[i][0];
		strSzTemp += szWorkDir.Right(szWorkDir.GetLength() -1);
		if (PathFileExists(strSzTemp + pGame->grungame))
		{
			bVdiskPath = TRUE;
			szWorkDir = strSzTemp;
			strVdiskGameDir = strSzTemp;
			_tcscpy_s(pGame->gclipath,szWorkDir.GetBuffer());
			break;
		}
	}
	if(bSsdPath && bVdiskPath)
	{
		//CString strSsdIndex = strSsdGameDir + _T("package.pkg");
		//CString strVdiskIndex = strVdiskGameDir + _T("package.pkg");
		strGamePath = strVdiskGameDir;
		DWORD nSsdIndex = kcore::Idxfile_GetVersion(strSsdGameDir);
		nSsdIndex = nSsdIndex == -2?0:nSsdIndex;
		DWORD nVdiskIndex = kcore::Idxfile_GetVersion(strVdiskGameDir);
		nVdiskIndex = nVdiskIndex == -2?0:nVdiskIndex;
		if(nSsdIndex >= nVdiskIndex)
			strGamePath = strSsdGameDir;
	}
	else if(bSsdPath)
		strGamePath = strSsdGameDir;
	else if(bVdiskPath)
		strGamePath = strVdiskGameDir;

	return strGamePath;
}

CString g_user;
CString g_password;

void SavePassword(CString user, CString password)
{
	g_user = user;
	g_password = password;
}

void GetPassword(CString& user, CString& password)
{
	password = g_password;
	user = g_user;
}

bool DownloadSingleFile(LPCTSTR lpUrl, Gdiplus::Bitmap*& pImage)
{
	bool bSuccess = true;
	HGLOBAL hGlobal = NULL;
	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
	try
	{
		wchar_t host[MAX_PATH] = {0};
		wchar_t req [MAX_PATH] = {0};
		URL_COMPONENTS urlComp = {0};
		urlComp.dwStructSize = sizeof(urlComp);
		urlComp.lpszHostName = host;
		urlComp.dwHostNameLength  = MAX_PATH;
		urlComp.lpszUrlPath = req;
		urlComp.dwUrlPathLength   = MAX_PATH;
		std::wstring str = _bstr_t(lpUrl);
		WinHttpCrackUrl(str.c_str(), (DWORD)str.size(), 0, &urlComp);

		if ((hSession = WinHttpOpen(L"A WinHTTP download Program/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
			WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0)) == NULL)
		{
			throw GetLastError();
		}

		if ((hConnect = WinHttpConnect( hSession, host, urlComp.nPort, 0)) == NULL)
		{
			throw GetLastError();
		}

		if ((hRequest = WinHttpOpenRequest( hConnect, L"GET", req, NULL, WINHTTP_NO_REFERER, 
			WINHTTP_DEFAULT_ACCEPT_TYPES, 0)) == NULL)
		{
			throw GetLastError();
		}

		if (!WinHttpSendRequest( hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
		{
			throw GetLastError();
		}

		if (!WinHttpReceiveResponse( hRequest, NULL))
		{
			throw GetLastError();
		}

		DWORD dwTotal = 0;
		DWORD dwSize = sizeof(DWORD);
		if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER ,
			NULL, &dwTotal, &dwSize, WINHTTP_NO_HEADER_INDEX))
		{
			throw GetLastError();
		}
		if (dwTotal != 200)
		{
			AfxWriteLogger(g_Logger, TEXT("down head picture file http reponse erorr:%d"), dwTotal);
			throw (DWORD)0;
		}
		dwSize = sizeof(DWORD);
		if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH|WINHTTP_QUERY_FLAG_NUMBER,
			NULL, &dwTotal, &dwSize, WINHTTP_NO_HEADER_INDEX) )
		{
			throw GetLastError();
		}

		hGlobal = GlobalAlloc(GMEM_FIXED,dwTotal);
		LPVOID pvData=GlobalLock(hGlobal); 
		DWORD dwReadBytes = 0;
		DWORD dwLeft = dwTotal;
		while (dwLeft > 0) 
		{
			dwSize = 0;
			if (!WinHttpQueryDataAvailable( hRequest, &dwSize))
			{
				GlobalUnlock(hGlobal); 
				throw GetLastError();
			}

			if (!WinHttpReadData( hRequest, (LPVOID)&((char*)pvData)[dwTotal-dwLeft], dwLeft, &dwReadBytes))
			{
				GlobalUnlock(hGlobal); 
				throw GetLastError();
			}
			dwLeft -= dwReadBytes;
		}

		GlobalUnlock(hGlobal); 
		IStream* pIStream = NULL;
		CreateStreamOnHGlobal(hGlobal,false,&pIStream);
		if (pIStream != NULL)
		{
			pImage = new Gdiplus::Bitmap(pIStream, FALSE);

			if (pImage->GetLastStatus() != Gdiplus::Ok)
			{
				if(pImage != NULL)
				{
					delete pImage;
					pImage = NULL;
				}
			}
			pIStream->Release();
		}

		if(pImage == NULL)
		{
			GlobalFree(hGlobal);
			hGlobal = NULL;
		}

		bSuccess = (dwLeft == 0);
	}
	catch (DWORD dwError)
	{
		if (dwError)
		{
			_com_error Error(dwError);
			AfxWriteLogger(g_Logger, TEXT("down head picture file Error:%s:%d:%s"), 
				lpUrl, dwError, (LPCTSTR)Error.Description());
		}
		bSuccess = false;
	}

	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	if(!bSuccess)
	{
		if(hGlobal)
		{
			GlobalFree(hGlobal);
			hGlobal = NULL;
		}
	}

	return bSuccess;
}


BOOL AfxDisableFsRedirection(PVOID* pOldValue)
{
	typedef BOOL(WINAPI *PFNDISABLEFSREDIRECTION)(PVOID *);

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

BOOL AfxRevertFsRedirection(PVOID oldValue)
{
	typedef BOOL(WINAPI *PFNREVERTFSREDIRECTION)(PVOID);

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

LPTSTR AfxGetIdcClassName(LPCTSTR szGuid, LPTSTR szName, DWORD dwSize)
{
	static TCHAR __IdcGuid__[][40] = { CLASS_WL_GUID, CLASS_DJ_GUID, CLASS_QP_GUID, CLASS_DZ_GUID, CLASS_LT_GUID,
		CLASS_WY_GUID, CLASS_XX_GUID, CLASS_PL_GUID, CLASS_CY_GUID, CLASS_YY_GUID, CLASS_GP_GUID, CLASS_GQ_GUID,
		CLASS_ZX_GUID, CLASS_XC_GUID, CLASS_SJ_GUID, CLASS_BG_GUID, CLASS_DPNP_GUID };

	static UINT __IdcClassNameId__[] = { IDS_NET_GAME, IDS_PC_GAME, IDS_CARD_GAME,
		IDS_BATTLE, IDS_IM, IDS_WEB_GAME, IDS_CASUAL_GAME,
		IDS_ASSIST, IDS_COMMON, IDS_VIDEO_TOOL, IDS_STOCK,
		IDS_HD_VIDEO, IDS_MUSIC, IDS_LIVE, IDS_MOBILE_GAME,
		IDS_OFFICE, IDS_DPNP_NAME, IDS_NB_NAME };

	//网吧自定义类型单独处理
	if (lstrcmpi(szGuid, CLASS_NB_GUID) == 0)
	{
		AfxGetLangStringEx(GetModuleFileName, IDS_NB_NAME, szName, dwSize);
		return szName;
	}

	for (size_t idx = 0; idx < CLASS_TOTAL_COUNT; idx++)
	{
		if (lstrcmpi(szGuid, __IdcGuid__[idx]) == 0)
		{
			AfxGetLangStringEx(::GetModuleHandle(NULL),
				__IdcClassNameId__[idx], szName, dwSize);
			break;
		}
	}
	return szName;
}

LONG AfxUnHandledExceptionFilter(PEXCEPTION_POINTERS pExcept)
{
	TCHAR szDir[MAX_PATH] = { 0 }, szExe[MAX_PATH] = { 0 }, szVer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szDir, _countof(szDir));
	AfxGetFileVersion(szDir, szVer);

	LPTSTR pFile = PathFindFileName(szDir);
	LPTSTR pExt = PathFindExtension(szDir);
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

	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	TCHAR szModuleName[MAX_PATH] = { 0 };
	_stprintf_s(szModuleName, _countof(szModuleName), TEXT("%s%s-%s-%04d%02d%02d-%02d%02d%02d.dmp"), szDir, szExe, szVer,
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	HANDLE hFile = CreateFile(szModuleName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo = { 0 };
		ExInfo.ThreadId = GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExcept;
		ExInfo.ClientPointers = false;

		BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
		CloseHandle(hFile);
	}
	return 0;
}

HANDLE AfxGetTokenByProcName(LPCTSTR lpName)
{
	HANDLE hToken = NULL;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return NULL;

	PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			if (_wcsicmp(pe32.szExeFile, lpName) == 0)
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
bool AfxRunProcess(LPCTSTR lpProcess, LPCTSTR lpParameter)
{
	bool bHideWindow = false;
	TCHAR szExePath[MAX_PATH] = { 0 }, szWorkDir[MAX_PATH] = { 0 };
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
			TCHAR szRootPath[MAX_PATH] = { 0 };
			AfxGetAppPath(szRootPath, MAX_PATH);
			_stprintf_s(szWorkDir, _countof(szWorkDir), _T("%s%s"), szRootPath, lpProcess);
			if (!PathRemoveFileSpec(szWorkDir))
				szWorkDir[0] = 0;
			_stprintf_s(szExePath, _countof(szExePath), TEXT("\"%s%s\" %s"), szRootPath, lpProcess, lpParameter != NULL ? lpParameter : TEXT(""));
		}
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
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
		if (bRet)
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

LPTSTR AfxGetLangStringEx(HANDLE hInstance, UINT uID, LPTSTR Value,
	DWORD dwSize, LPCTSTR Default)
{
	if (0 >= ::LoadString((HINSTANCE)hInstance, uID, Value, dwSize))
		::_tcscpy_s(Value, dwSize, Default);

	return Value;
}