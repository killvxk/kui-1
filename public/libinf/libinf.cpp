//#include "stdafx.h"
#include "..\..\include\libinf.h"
//#include "..\..\include\kcore.h"

#include "comment.h"
#include "InfParse.h"
#include "XmlParse.h"
#include "RegWriter.h"

#include <tchar.h>

void CreateDirectoryTree(LPCTSTR szDirectory)
{
	std::wstring dir(szDirectory);
	size_t end = dir.rfind(TEXT('\\'));
	size_t pt = dir.find(TEXT('\\'), 0);

	while (pt != std::wstring::npos && pt <= end)
	{
		std::wstring path = dir.substr(0, pt + 1);
		DWORD dwRet = GetFileAttributes(path.c_str());
		if (dwRet == INVALID_FILE_ATTRIBUTES)
			::CreateDirectory(path.c_str(), NULL);
		pt = dir.find(TEXT('\\'), pt + 1);
	}
}

BOOL Inf2Xml(const wchar_t *pInfName, const wchar_t *pRelativePath, CXmlParse *xmlParse)
{
	CInfParse InfParser;
	InfParser.OpenInfFile(pInfName, pRelativePath);

	std::wstring strVer, strDate, strProvider, strInfName;
	InfParser.GetDriverDate(strDate);
	xmlParse->SetDriverTime(strDate.c_str());

	InfParser.GetDriverVer(strVer);
	xmlParse->SetFileVersion(strVer.c_str());

	InfParser.GetProvider(strProvider);
	xmlParse->SetProvider(strProvider.c_str());

	InfParser.GetInfName(strInfName);
	xmlParse->SetInfName(strInfName.c_str());

	vecSectionFileds_t vecSupportOSs, vecSectorName;
	InfParser.GetSupportOS(vecSupportOSs);
	InfParser.GetModulesSectionName(vecSectorName);

	for (vecSectionFileds_t::iterator itOS = vecSupportOSs.begin();
		itOS != vecSupportOSs.end(); ++itOS)
	{
		for (vecSectionFileds_t::iterator itMS = vecSectorName.begin();
			itMS != vecSectorName.end(); ++itMS)
		{
			vecSectionFileds_t values;
			std::wstring strSectorName = (*itMS) + TEXT(".") + (*itOS);
			if (!InfParser.GetModulesSectionInfo(strSectorName.c_str(), values))
				continue;

			if (values.size() <= 0)
				continue;

			xmlParse->WritePnpInfo(itOS->c_str());

			mapKeyCount_t mapKeyCount;
			for (vecSectionFileds_t::iterator it = values.begin();
				it != values.end();)
			{
				std::wstring v = *it; ++it;
				std::wstring d = *it; ++it;
				std::wstring r = *it; ++it;
				xmlParse->WriteDevInfo(itOS->c_str(), v.c_str(), d.c_str(), r.c_str());
			}
		}
	}

	InfParser.CloseInfFile();
	return TRUE;
};

//根据os信息，生成os的版本名称
//iSysType : SYSTEM_TYPE_xxxx
//iOSType  : OS_TYPE_xxxx
//iSubVer : windows系统编译的build版本号，比如：NTamd64.10.0...14393，则build版本号是14393
int GetOsInfo(std::wstring &strOSInfo, vecSectionFileds_t &tMfgName)
{
/*	TCHAR szTemp[MAX_PATH] = { 0 };
	if (iSysType == SYSTEM_TYPE_X86 && iOSType == OS_TYPE_XP)
	{
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("5.1.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("5.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT.%s"), TEXT("5.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s"), TEXT("NTx86"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT"));
		tMfgName.push_back(szTemp);
	}
	else if (iSysType == SYSTEM_TYPE_X86 && iOSType == OS_TYPE_WIN7)
	{
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.2.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.1.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT.%s"), TEXT("6.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s"), TEXT("NTx86"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.0"));
		tMfgName.push_back(szTemp);
	}
	else if (iSysType == SYSTEM_TYPE_X64 && iOSType == OS_TYPE_WIN7)
	{
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.2.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.1.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT.%s"), TEXT("6.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s"), TEXT("NTamd64"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.0"));
		tMfgName.push_back(szTemp);
	}
	else if (iSysType == SYSTEM_TYPE_X86 && iOSType == OS_TYPE_WIN10)
	{
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("10.0.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("10.0"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.3"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.2.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.1.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT.%s"), TEXT("6.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s"), TEXT("NTx86"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT"));
		tMfgName.push_back(szTemp);
	}
	else if (iSysType == SYSTEM_TYPE_X64 && iOSType == OS_TYPE_WIN10)
	{
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("10.0.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("10.0"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.3"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.2.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.1.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT.%s"), TEXT("6.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s"), TEXT("NTamd64"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT"));
		tMfgName.push_back(szTemp);
	}
	else
	{
		return -1;
	}
	return 0;*/

	TCHAR szTemp[MAX_PATH] = { 0 };
	if (0 == ::lstrcmpi(strOSInfo.c_str(), OS_51_X86))
	{
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("5.1.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("5.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT.%s"), TEXT("5.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s"), TEXT("NTx86"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT"));
		tMfgName.push_back(szTemp);
	}
	else if (0 == ::lstrcmpi(strOSInfo.c_str(), OS_61_X86))
	{
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.1.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT.%s"), TEXT("6.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.2.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s"), TEXT("NTx86"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("NT"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.0"));
		tMfgName.push_back(szTemp);
	}
	else if (0 == ::lstrcmpi(strOSInfo.c_str(), OS_61_X64))
	{
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.1.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.2.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s"), TEXT("NTamd64"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.0"));
		tMfgName.push_back(szTemp);
	}
	else if (0 == ::lstrcmpi(strOSInfo.c_str(), OS_10_X86))
	{
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("10.0.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("10.0"));
		tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.3"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.2.1"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.1.1"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTx86"), TEXT("6.1"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("NT.%s"), TEXT("6.1"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("%s"), TEXT("NTx86"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("NT"));
		//tMfgName.push_back(szTemp);
	}
	else if (0 == ::lstrcmpi(strOSInfo.c_str(), OS_10_X64))
	{
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("10.0.1"));
		tMfgName.push_back(szTemp);
		::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("10.0"));
		tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.3"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.2.1"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.1.1"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("%s.%s"), TEXT("NTamd64"), TEXT("6.1"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("NT.%s"), TEXT("6.1"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("%s"), TEXT("NTamd64"));
		//tMfgName.push_back(szTemp);
		//::_stprintf_s(szTemp, TEXT("NT"));
		//tMfgName.push_back(szTemp);
	}
	else
	{
		return -1;
	}
	return 0;

};

void SysInfo2DeviceInfo(tSysInfo &sysInfo, vecSectionFileds_t &vecDeviceInfo, std::wstring &strDeviceInfo)
{
	wchar_t deviceInfo[64] = { 0 };
	if (sysInfo.dwSubSys != 0xff && sysInfo.dwRev != 0xff)
	{
		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X"),
		sysInfo.dwVen, sysInfo.dwDev, sysInfo.dwSubSys, sysInfo.dwRev);
		vecDeviceInfo.push_back(deviceInfo);
		strDeviceInfo = deviceInfo;

		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X&SUBSYS_FF&REV_%02X"),
			sysInfo.dwVen, sysInfo.dwDev, sysInfo.dwRev);
		vecDeviceInfo.push_back(deviceInfo);

		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_FF"),
			sysInfo.dwVen, sysInfo.dwDev, sysInfo.dwSubSys);
		vecDeviceInfo.push_back(deviceInfo);

		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X&SUBSYS_FF&REV_FF"),
			sysInfo.dwVen, sysInfo.dwDev);
		vecDeviceInfo.push_back(deviceInfo);
	}
	else if (sysInfo.dwSubSys == 0xff && sysInfo.dwRev != 0xff)
	{
		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X&REV_%02X"),
			sysInfo.dwVen, sysInfo.dwDev, sysInfo.dwRev);
		strDeviceInfo = deviceInfo;

		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X&SUBSYS_FF&REV_%02X"),
			sysInfo.dwVen, sysInfo.dwDev, sysInfo.dwRev);
		vecDeviceInfo.push_back(deviceInfo);
		
		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X&SUBSYS_FF&REV_FF"),
			sysInfo.dwVen, sysInfo.dwDev);
		vecDeviceInfo.push_back(deviceInfo);
	}
	else if (sysInfo.dwSubSys != 0xff && sysInfo.dwRev == 0xff)
	{
		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X&SUBSYS_%08X"),
			sysInfo.dwVen, sysInfo.dwDev, sysInfo.dwSubSys);
		strDeviceInfo = deviceInfo;

		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_FF"),
			sysInfo.dwVen, sysInfo.dwDev, sysInfo.dwSubSys);
		vecDeviceInfo.push_back(deviceInfo);

		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X&SUBSYS_FF&REV_FF"),
			sysInfo.dwVen, sysInfo.dwDev);
		vecDeviceInfo.push_back(deviceInfo);
	}
	else if (sysInfo.dwSubSys == 0xff && sysInfo.dwRev == 0xff)
	{
		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X"),
			sysInfo.dwVen, sysInfo.dwDev);
		strDeviceInfo = deviceInfo;

		_stprintf_s(deviceInfo, _countof(deviceInfo), TEXT("VEN_%04X&DEV_%04X&SUBSYS_FF&REV_FF"),
			sysInfo.dwVen, sysInfo.dwDev);
		vecDeviceInfo.push_back(deviceInfo);
	}
};

//将nt开头的os名称转化成tosinfo结构
int OsNtName2OsInf(std::wstring &strOsName, tOsInfo &osInfo)
{
	if (std::wstring::npos == strOsName.find(TEXT("amd64")))
		osInfo.iArchType = SYSTEM_TYPE_X86;
	else
		osInfo.iArchType = SYSTEM_TYPE_X64;

	if (std::wstring::npos != strOsName.find(TEXT(".5.")))
		osInfo.iOSType = OS_TYPE_XP;
	else if (std::wstring::npos != strOsName.find(TEXT(".6.")))
		osInfo.iOSType = OS_TYPE_WIN7;
	else if (std::wstring::npos != strOsName.find(TEXT(".10.")))
		osInfo.iOSType = OS_TYPE_WIN10;
	else
		return -1;

	size_t pos = strOsName.find_last_of(TEXT('.'));
	if (pos == std::wstring::npos)
		return -2;

	osInfo.iBuild = ::_tstoi(strOsName.c_str() + pos + 1);
	if (osInfo.iBuild < 10000)
		osInfo.iBuild = 0;
	return 0;
};

//将xml开头的os名称转化成tosinfo结构
int OsName2OsInfo(tSysInfo &sysInfo, tOsInfo &osInfo)
{
	if (0 == ::lstrcmpi(sysInfo.m_strOSVer.c_str(), OS_51_X86))
	{
		osInfo.iArchType = SYSTEM_TYPE_X86;
		osInfo.iOSType = OS_TYPE_XP;
		osInfo.iBuild = sysInfo.dwBuild;
	}
	else if (0 == ::lstrcmpi(sysInfo.m_strOSVer.c_str(), OS_61_X86))
	{
		osInfo.iArchType = SYSTEM_TYPE_X86;
		osInfo.iOSType = OS_TYPE_WIN7;
		osInfo.iBuild = sysInfo.dwBuild;
	}
	else if (0 == ::lstrcmpi(sysInfo.m_strOSVer.c_str(), OS_61_X64))
	{
		osInfo.iArchType = SYSTEM_TYPE_X64;
		osInfo.iOSType = OS_TYPE_WIN7;
		osInfo.iBuild = sysInfo.dwBuild;
	}
	else if (0 == ::lstrcmpi(sysInfo.m_strOSVer.c_str(), OS_10_X86))
	{
		osInfo.iArchType = SYSTEM_TYPE_X86;
		osInfo.iOSType = OS_TYPE_WIN10;
		osInfo.iBuild = sysInfo.dwBuild;
	}
	else if (0 == ::lstrcmpi(sysInfo.m_strOSVer.c_str(), OS_10_X64))
	{
		osInfo.iArchType = SYSTEM_TYPE_X64;
		osInfo.iOSType = OS_TYPE_WIN10;
		osInfo.iBuild = sysInfo.dwBuild;
	}
	else
	{
		osInfo.iArchType = SYSTEM_TYPE_UNKNOW;
		osInfo.iOSType = OS_TYPE_UNKNOW;
		osInfo.iBuild = 0xff;
		return -1;
	}
	return 0;
};

int GetOsName(CInfParse &infParser, tSysInfo &sysInfo, std::wstring &strDeviceInfo)
{
	tOsInfo osInfo;
	if (0 != OsName2OsInfo(sysInfo, osInfo))
		return -1;

	vecSectionFileds_t tMfgName;
	if (osInfo.iOSType == OS_TYPE_WIN10)
	{
		vecSectionFileds_t OSs;
		infParser.GetSupportOS(OSs);
		for (vecSectionFileds_t::iterator it = OSs.begin();
			it != OSs.end(); ++it)
		{
			tOsInfo ntOsInfo;
			if ((0 == OsNtName2OsInf(*it, ntOsInfo)) && (ntOsInfo.iOSType == OS_TYPE_WIN10))
			{
				if (osInfo.iBuild > 0 && ntOsInfo.iBuild >= osInfo.iBuild)
					tMfgName.insert(tMfgName.begin(), *it);
				else if (osInfo.iBuild > 0 && ntOsInfo.iBuild == 0)
					tMfgName.push_back(*it);
				else if (osInfo.iBuild <= 0 && ntOsInfo.iBuild == 0)
					tMfgName.push_back(*it);
			}
		}
	}
	else
		GetOsInfo(sysInfo.m_strOSVer, tMfgName);

	if (tMfgName.empty())
		return -2;

	
	vecSectionFileds_t vecModuleSecName;
	vecSectionFileds_t vecDeviceInfo;

	SysInfo2DeviceInfo(sysInfo, vecDeviceInfo, strDeviceInfo);
	if (!infParser.GetModulesSectionName(vecModuleSecName))
		return -1;

	sysInfo.m_strOSVer.clear();
	for (vecSectionFileds_t::iterator itMS = vecModuleSecName.begin();
		itMS != vecModuleSecName.end(); ++itMS)
	{
		for (vecSectionFileds_t::iterator itOS = tMfgName.begin();
			itOS != tMfgName.end(); ++itOS)
		{
			vecSectionFileds_t vecSectionDevInfo;
			std::wstring strSecName = *itMS + TEXT(".") + *itOS;

			infParser.GetModuleSectionAllData(strSecName.c_str(), vecSectionDevInfo);
			if (vecSectionDevInfo.empty())
				continue;

			for (vecSectionFileds_t::iterator itDevSrc = vecDeviceInfo.begin();
				itDevSrc != vecDeviceInfo.end(); ++itDevSrc)
			{
				for (vecSectionFileds_t::iterator itDev = vecSectionDevInfo.begin();
					itDev != vecSectionDevInfo.end();)
				{
					wchar_t tmp[1024] = { 0 };
					_stprintf_s(tmp, _countof(tmp), TEXT("VEN_%s&DEV_%s&SUBSYS_%s&REV_%s"), 
						(itDev)->c_str(), (itDev+1)->c_str(), (itDev+3)->c_str(), (itDev+2)->c_str());
					itDev += 4;

					if (0 == ::lstrcmpi(itDevSrc->c_str(), tmp))
					{
						sysInfo.m_strOSVer = *itOS;
						itOS = tMfgName.end()-1;
						itDev = vecSectionDevInfo.end()-1;
						itMS = vecModuleSecName.end()-1;
						itDevSrc = vecDeviceInfo.end()-1;
						break;
					}
				}
			}
		}
	}

	if (sysInfo.m_strOSVer.empty())
		return -3;
	else
		return 0;
};

int ScanDir(std::wstring dir, std::vector<std::wstring> &inflist, LPTSTR szErrorInfo, DWORD dwCharSize)
{
	DWORD dwRet = ERROR_SUCCESS;
	bool bNullDirectory = true;
	WIN32_FIND_DATA wfd = { 0 };
	HANDLE hFinder = ::FindFirstFile((dir + TEXT("*.*")).c_str(), &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
	{
		_stprintf_s(szErrorInfo, dwCharSize, TEXT("FindFirstFile fail err:%d,"), GetLastError());
		return GetLastError();
	}
	do
	{

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!(_tcsicmp(wfd.cFileName, TEXT(".")) == 0 || _tcsicmp(wfd.cFileName, TEXT("..")) == 0))
			{
				std::wstring subdir = dir + wfd.cFileName + TEXT("\\");
				ScanDir(subdir, inflist, szErrorInfo, dwCharSize);
			}
		}
		else
		{
			std::wstring  filename = dir + wfd.cFileName;
			if (filename.length() - 4 > 0)
			{
				size_t start = filename.length() - 4;
				std::wstring tmp = filename;
				tmp = tmp.substr(start, 4);
				if (wmemcmp(tmp.c_str(), TEXT(".inf"), 4) == 0)
					inflist.push_back(filename);
			}

		}

	} while (::FindNextFile(hFinder, &wfd));
	::FindClose(hFinder);
	return 0;
}

BOOL CXml2Inf::ParseInf2Xml(const wchar_t *pInfName, const wchar_t *pXmlName)
{
	CXmlParse *xmlParse = new CXmlParse;
	xmlParse->OpenXmlFile(pXmlName);

	Inf2Xml(pInfName, pInfName, xmlParse);
	
	xmlParse->CloseXmlFile();
	delete xmlParse;

	return TRUE;
};

int CXml2Inf::ParseInf2Xml(const tInfDriverInfo &infInfo,
	wchar_t **pOutXmlDataBuf, int& iOutDataLen,
	wchar_t *pErrInfoBuf, const int iErrBufLen)
{
	static const wchar_t *pTmpXmlFileName = TEXT("temp.xml");
	static const wchar_t *pErrorParamEmpty = TEXT("error : gid or updatetime empyt!");
	static const wchar_t *pErrorFileOutofBuffer = TEXT("error : inf file is too big to parse");

	if (infInfo.m_strGID.empty() || infInfo.m_strUpdateTime.empty())
	{
		if (NULL != pErrInfoBuf)
			memcpy(pErrInfoBuf, pErrorParamEmpty, lstrlen(pErrorParamEmpty)*sizeof(wchar_t));
		return -1;
	}

	wchar_t errBuf[256] = { 0 };
	std::vector<std::wstring> infList;
	ScanDir(infInfo.m_strInfFullPath.c_str(), infList, errBuf, _countof(errBuf));

	*pOutXmlDataBuf = NULL;

	for (std::vector<std::wstring>::iterator it = infList.begin();
		it != infList.end(); ++it)
	{
		CXmlParse *xmlParse = new CXmlParse;
		if (NULL != *pOutXmlDataBuf)
			xmlParse->ParseXmlStr(*pOutXmlDataBuf);
		else
			xmlParse->OpenXmlFile(pTmpXmlFileName);

		xmlParse->SetProductVersion(infInfo.m_strProductVer.c_str());
		xmlParse->SetUpdateTime(infInfo.m_strUpdateTime.c_str());
		xmlParse->SetGID(infInfo.m_strGID.c_str());
		xmlParse->SetDefault(infInfo.m_strDefault.c_str());
		xmlParse->SetIcafe(infInfo.m_strIcafe.c_str());
	
		const wchar_t *pRelativePath = it->c_str() + infInfo.m_strInfFullPath.size();
		Inf2Xml(it->c_str(), pRelativePath, xmlParse);
	
		wchar_t *pRestBuf = NULL;
		iOutDataLen = xmlParse->PrintStr(&pRestBuf, iOutDataLen);
		xmlParse->CloseXmlFileNotSave();
		delete xmlParse;

		if (*pOutXmlDataBuf != NULL)
			delete [] *pOutXmlDataBuf;

		*pOutXmlDataBuf = pRestBuf;
	}

	if (iOutDataLen <= 0)
	{
		if (NULL != pErrInfoBuf)
			memcpy(pErrInfoBuf, pErrorFileOutofBuffer, lstrlen(pErrorFileOutofBuffer)*sizeof(wchar_t));
		return -2;
	}
	return 0;
};

void CXml2Inf::SetXmlProductVersion(const wchar_t *pXmlName, const wchar_t *pVersion)
{
	CXmlParse xml;
	xml.OpenXmlFile(pXmlName);
	xml.SetXmlProductVersion(pVersion);
	xml.CloseXmlFile();
};

void CXml2Inf::SetXmlUpdateTime(const wchar_t *pXmlName, const wchar_t *pTime)
{
	CXmlParse xml;
	xml.OpenXmlFile(pXmlName);
	xml.SetXmlUpdateTime(pTime);
	xml.CloseXmlFile();
};

void CXml2Inf::SetXmlGID(const wchar_t *pXmlName, const wchar_t *pGid)
{
	CXmlParse xml;
	xml.OpenXmlFile(pXmlName);
	xml.SetXmlUpdateTime(pGid);
	xml.CloseXmlFile();
};

int CXml2Inf::ParseInf2Reg(const wchar_t *pInfName, 
	const wchar_t *pRootPath, tSysInfo &sysInfo,
	const wchar_t *pRegFileFullPath, const wchar_t *pIniName,
	std::wstring &strDisplayName, wchar_t* pErrInfoBuf, int iErrBufLen)
{
	static const wchar_t *pOsNotMatch = TEXT("error : OS not matched");
	static const wchar_t *pSectionErr = TEXT("error : section not matched");
	static const wchar_t *pOpenRegErr = TEXT("error : Open reg file error");
	static const wchar_t *pWriteRegErr = TEXT("error : write reg file error");
	static const wchar_t *pGetInfDataErr = (TEXT("error : get inf data error"));
	static const wchar_t *pWriteCopyFileErr = TEXT("error : write copy file error");

	iErrBufLen = 0;
	int iRest = 0;

	mapRetDataInfo_t mapServiceInfo;
	mapRetDataInfo_t mapAddRegInfo;
	mapRetDataInfo_t mapCopyFile;
	mapRetDataInfo_t mapDelReg;

	vecSectionFileds_t addReg;
	vecSectionFileds_t vecDelRegInfo;

	std::wstring strSectionName;
	std::wstring strService;
	std::wstring strHwName;
	std::wstring strCoInstallers;

	std::wstring strSvrName, strMfgName;
	std::wstring strDate, strVer;
	std::wstring strCatFileName;
	std::wstring strDevSection;

	CRegWriter regWriter;
	CInfParse infParser;
	infParser.OpenInfFile(pInfName, pInfName);
	
	mapSectionKeyValue_t mapFilePaths;
	infParser.GetCopyFilePath(sysInfo.m_strOSVer, mapFilePaths);

	std::wstring strDeviceInfo;
	if (0 > GetOsName(infParser, sysInfo, strDeviceInfo))
	{
		iRest = -1;
		::lstrcpy(pErrInfoBuf, pOsNotMatch);
		goto done;
	}

	if (!infParser.GetSectionName(sysInfo, strSectionName, strDisplayName, strDevSection))
	{
		iRest = -2;
		::lstrcpy(pErrInfoBuf, pSectionErr);
		goto done;
	}

	regWriter.SetDeviceInfo(strDeviceInfo.c_str());

	//services信息
	strService = strSectionName + TEXT(".Services");
	infParser.GetAddRegInfo(strService.c_str(), mapAddRegInfo, strSvrName);
	infParser.GetManufacture(strMfgName);
	regWriter.SetAddReg(mapAddRegInfo);

	infParser.GetDriverDate(strDate);
	infParser.GetDriverVer(strVer);

	regWriter.SetSvrName(strSvrName, strMfgName, strDisplayName, strSectionName, strDevSection);
	regWriter.SetDriverInfo(strDate, strVer);
		
	infParser.GetServiceInfo(strService.c_str(), mapServiceInfo);
	regWriter.SetServiceInfo(mapServiceInfo);
	
	//hw信息
	strHwName = strSectionName + TEXT(".hw");
	addReg.clear();
	infParser.GetAddRegInfo(strHwName.c_str(), addReg);
	regWriter.SetAddReg(addReg, 3); //hw的段值为3，参考regwriter.cpp中的HKR_TYEP_HW定义

	mapCopyFile.clear();
	infParser.GetCopyFileInfo(strHwName.c_str(), mapCopyFile);
	regWriter.SetCopyFile(mapCopyFile);

	//CoInstallers信息
	addReg.clear();
	strCoInstallers = strSectionName + TEXT(".CoInstallers");
	infParser.GetAddRegInfo(strCoInstallers.c_str(), addReg);
	regWriter.SetAddReg(addReg);

	mapCopyFile.clear();
	infParser.GetCopyFileInfo(strCoInstallers.c_str(), mapCopyFile);
	regWriter.SetCopyFile(mapCopyFile);

	//主段信息
	addReg.clear();
	infParser.GetAddRegInfo(strSectionName.c_str(), addReg);
	regWriter.SetAddReg(addReg);

	mapCopyFile.clear();
	infParser.GetCopyFileInfo(strSectionName.c_str(), mapCopyFile);
	regWriter.SetCopyFile(mapCopyFile);

	//copyfile的原路径
	regWriter.SetCopyFilePath(mapFilePaths);

	////delreg
	//mapDelReg.clear();
	//infParser.GetDelRegInfo(strService.c_str(), mapDelReg);
	//regWriter.SetDelReg(mapDelReg);

	//vecDelRegInfo.clear();
	//infParser.GetDelRegInfo(strSectionName.c_str(), vecDelRegInfo);
	//regWriter.SetDelReg(vecDelRegInfo);

	//写入数据
	CreateDirectoryTree(pRegFileFullPath);
	if (!regWriter.OpenRegFile(pRegFileFullPath))
	{
		iRest = -3;
		::lstrcpy(pErrInfoBuf, pOpenRegErr);
		goto done;
	}

	//if (addReg.empty() || strDeviceInfo.empty() || mapServiceInfo.empty())
	//{
	//	iRest = -4;
	//	::lstrcpy(pErrInfoBuf, pGetInfDataErr);
	//	goto done;
	//}
	
	regWriter.SetInfName(pInfName);
	iRest = regWriter.WriteReg();
	if (0 > iRest)
	{
		iRest = -5;
		_stprintf_s(pErrInfoBuf, iErrBufLen, TEXT("%s : %d"), pWriteRegErr, iRest);
		goto done;
	}

	infParser.GetCatFileName(strCatFileName);
	if (0 != regWriter.WriteCopyFile(pIniName, pRootPath, strCatFileName.c_str()))
	{
		iRest = -6;
		::lstrcpy(pErrInfoBuf, pWriteCopyFileErr);
		goto done;
	}

done:
	regWriter.CloseRegFile();
	infParser.CloseInfFile();
	return iRest;
};
