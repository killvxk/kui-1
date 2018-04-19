//#include "stdafx.h"
#include <tchar.h>
#include "InfParse.h"
#include "..\..\include\libinf.h"

CInfParse::CInfParse() : m_hInfHandle(INVALID_HANDLE_VALUE)
{};

CInfParse::~CInfParse()
{};

BOOL CInfParse::OpenInfFile(const wchar_t *pFileName, const wchar_t *pFileRelativePath)
{
	m_strFileName = pFileRelativePath;
	m_hInfHandle = ::SetupOpenInfFile(pFileName, NULL, INF_STYLE_WIN4, NULL);
	int err = ::GetLastError();
	return INVALID_HANDLE_VALUE != m_hInfHandle;
};

BOOL CInfParse::CloseInfFile()
{
	if (INVALID_HANDLE_VALUE != m_hInfHandle)
		::SetupCloseInfFile(m_hInfHandle);
	m_hInfHandle = INVALID_HANDLE_VALUE;
	return TRUE;
};

BOOL CInfParse::ReadSectionKey(const wchar_t *pSection, const wchar_t *pKey, std::wstring &strValue)
{
	if (INVALID_HANDLE_VALUE == m_hInfHandle)
		return FALSE;

	DWORD dwSize = 0;
	wchar_t val[1024] = { 0 };

	if (!::SetupGetLineText(NULL, m_hInfHandle, pSection, pKey, val, _countof(val), &dwSize))
		return FALSE;

	strValue = val;
	return TRUE;
};

BOOL CInfParse::GetFields(INFCONTEXT &ctx, vecSectionFileds_t &fileds)
{
	DWORD dwCount = ::SetupGetFieldCount(&ctx);
	for (DWORD i = 1; i <= dwCount; ++i)
	{
		DWORD dwSize = 0;
		wchar_t buf[1024] = { 0 };
		if (::SetupGetStringField(&ctx, i, buf, _countof(buf), &dwSize))
			fileds.push_back(buf);
	}
	return TRUE;
};

BOOL CInfParse::ReadSectionKeyAllValue(const wchar_t *pSection, const wchar_t *pKey, vecSectionFileds_t &fields)
{
	INFCONTEXT ctx;
	if (!::SetupFindFirstLine(m_hInfHandle, pSection, pKey, &ctx))
		return FALSE;

	return GetFields(ctx, fields);
};

BOOL CInfParse::ReadSectionAllValue(const wchar_t *pSection, vecSectionDataKV_t &vecValue)
{
	LONG lLineCount = ::SetupGetLineCount(m_hInfHandle, pSection);
	for (LONG i = 0; i < lLineCount; ++i)
	{
		INFCONTEXT ctx;
		if (::SetupGetLineByIndex(m_hInfHandle, pSection, i, &ctx))
		{
			DWORD dwSize = 0;
			wchar_t name[MAX_INSTRUCTION_LEN] = { 0 }, value[1024] = { 0 };
			if (::SetupGetStringField(&ctx, 0, name, _countof(name), &dwSize)
				&& ::SetupGetLineText(&ctx, m_hInfHandle, NULL, NULL, value, _countof(value), &dwSize))
			{
				tDataKeyVal kv;
				kv.strKey = name;
				kv.strVal = value;
				vecValue.push_back(kv);
			}
		}
	}
	return TRUE;
};

BOOL CInfParse::ReadSectionAllValue(const wchar_t *pSection, const wchar_t *pKeyName, vecSectionFileds_t &vecAddRegSection)
{
	INFCONTEXT ctx;
	if (!::SetupFindFirstLine(m_hInfHandle, pSection, pKeyName, &ctx))
		return FALSE;

	do{
		DWORD dwSize = 0;
		wchar_t value[1024] = { 0 };
		if (::SetupGetLineText(&ctx, m_hInfHandle, pSection, pKeyName, value, _countof(value), &dwSize))
			vecAddRegSection.push_back(value);
	} while (::SetupFindNextMatchLine(&ctx, pKeyName, &ctx));

	return TRUE;
};

BOOL CInfParse::ReadSectionAllValue(const wchar_t *pSection, vecSectionFileds_t &vecAddRegSection)
{
	LONG lLineCount = ::SetupGetLineCount(m_hInfHandle, pSection);
	for (LONG i = 0; i < lLineCount; ++i)
	{
		INFCONTEXT ctx;
		if (::SetupGetLineByIndex(m_hInfHandle, pSection, i, &ctx))
		{
			DWORD dwSize = 0;
			wchar_t value[1024] = { 0 };
			if (::SetupGetLineText(&ctx, m_hInfHandle, NULL, NULL, value, _countof(value), &dwSize))
			{
				int iCount = ::SetupGetFieldCount(&ctx);

				//0x00010008 append
				std::wstring strTmpVal(value);
				size_t start = strTmpVal.find(TEXT("0x00010000"));
				if (std::wstring::npos != start)
				{
					size_t end = strTmpVal.find(TEXT(','), start);
					if (iCount <= 5)
					{
						strTmpVal.insert(end + 1, TEXT("\""));
						strTmpVal.insert(strTmpVal.end(), TEXT('\"'));
					}
				}
				else
				{
					start = strTmpVal.find(TEXT("0x00010008"));
					if (std::wstring::npos != start)
					{
						size_t end = strTmpVal.find(TEXT(','), start);
						std::wstring strSubAppendVal = strTmpVal.substr(end + 1);

						start = strTmpVal.find_first_of(TEXT(','));
						start = strTmpVal.find_first_of(TEXT(','), start + 1);
						end = strTmpVal.find_first_of(TEXT(','), start + 1);
						std::wstring strSubAppendKey = strTmpVal.substr(start+1, end - start-1);

						if (iCount <= 5)
						{
							for (vecSectionFileds_t::iterator it = vecAddRegSection.begin();
								it != vecAddRegSection.end(); ++it)
							{
								size_t poss = it->find_first_of(TEXT(','));
								poss = it->find_first_of(TEXT(','), poss + 1);
								size_t pose = it->find_first_of(TEXT(','), poss + 1);

								std::wstring strSubAppKey = it->substr(poss+1, pose - poss-1);
								if (strSubAppendKey == strSubAppKey)
								{
									//*it += it->substr(pose);
									it->insert(it->end(), TEXT(','));
									it->insert(it->end(), TEXT('\"'));
									it->insert(it->end(), strSubAppendVal.begin(), strSubAppendVal.end());
									it->insert(it->end(), TEXT('\"'));
									break;
								}
							}
						}
						else
						{
							for (vecSectionFileds_t::iterator it = vecAddRegSection.begin();
								it != vecAddRegSection.end(); ++it)
							{
								size_t poss = it->find_first_of(TEXT(','));
								poss = it->find_first_of(TEXT(','), poss + 1);
								size_t pose = it->find_first_of(TEXT(','), poss + 1);

								std::wstring strSubAppKey = it->substr(poss + 1, pose - poss - 1);
								if (strSubAppendKey == strSubAppKey)
								{
									*it += it->substr(pose);
									break;
								}
							}
						}
						continue;
					}					
				}

				vecAddRegSection.push_back(strTmpVal);
			}
		}
	}
	return TRUE;
};

BOOL CInfParse::ReadSectionSubkey(const wchar_t *pSection, const wchar_t *pKey, DWORD index, std::wstring &strValue)
{
	if (INVALID_HANDLE_VALUE == m_hInfHandle)
		return FALSE;

	INFCONTEXT ctx;
	if (!::SetupFindFirstLine(m_hInfHandle, pSection, pKey, &ctx))
		return FALSE;

	if (index > ::SetupGetFieldCount(&ctx))
		return FALSE;

	DWORD dwSize = 0;
	wchar_t buf[1024] = { 0 };
	if (!::SetupGetStringField(&ctx, index, buf, _countof(buf), &dwSize))
		return FALSE;

	strValue = buf;
	return TRUE;
};

//os信息从[Manufacturer]段获取
BOOL CInfParse::GetSupportOS(vecSectionFileds_t &vecSupportOSs)
{
	wchar_t *pSection = TEXT("Manufacturer");
	
	vecSectionDataKV_t vecValue;
	if (!ReadSectionAllValue(pSection, vecValue))
		return FALSE;

	for (vecSectionDataKV_t::iterator it = vecValue.begin(); it != vecValue.end(); ++it)
	{
		vecSectionFileds_t fields;
		if (ReadSectionKeyAllValue(pSection, it->strKey.c_str(), fields))
			vecSupportOSs.insert(vecSupportOSs.end(), fields.begin() + 1, fields.end()); //fields第一个是ddinstall section名称
	}
	return TRUE;
};

BOOL CInfParse::GetModulesSectionName(vecSectionFileds_t &vecNames)
{
	wchar_t *pSection = TEXT("Manufacturer");
	vecSectionDataKV_t vecValue;
	if (!ReadSectionAllValue(pSection, vecValue))
		return FALSE;

	for (vecSectionDataKV_t::iterator it = vecValue.begin(); it != vecValue.end(); ++it)
	{
		std::wstring strName;
		if (ReadSectionSubkey(pSection, it->strKey.c_str(), 1, strName))
			vecNames.push_back(strName);
	}
	return TRUE;
};

BOOL CInfParse::GetInfName(std::wstring &name)
{
	//size_t pos = m_strFileName.find_last_of(TEXT('\\'));
	//if (pos == std::wstring::npos)
	//	name = m_strFileName;
	//else
	//	name = m_strFileName.substr(pos + 1);
	name = m_strFileName;
	return TRUE;
};

//日期信息从[Version]DriverVer获取
BOOL CInfParse::GetDriverDate(std::wstring &strDate)
{
	return ReadSectionSubkey(TEXT("version"), TEXT("driverver"), 1, strDate);
};

//版本信息从[Version]DriverVer获取
BOOL CInfParse::GetDriverVer(std::wstring &strVer)
{
	return ReadSectionSubkey(TEXT("version"), TEXT("driverver"), 2, strVer);
};

//厂商信息从[Version]Provider获取
BOOL CInfParse::GetProvider(std::wstring &strProvider)
{
	return ReadSectionKey(TEXT("Version"), TEXT("provider"), strProvider);
};

BOOL CInfParse::GetSubStringValue(std::wstring &strSrcString, wchar_t *pSubName, std::wstring &strValue)
{
	strValue = TEXT("FF");
	size_t pos = strSrcString.find(pSubName);
	if (pos != std::wstring::npos)
	{
		size_t end = strSrcString.find_first_of(TEXT('&'), pos);
		if (end == std::wstring::npos)
			end = strSrcString.size();

		size_t start = strSrcString.find_first_of(TEXT('_'), pos);
		if (start == std::wstring::npos)
			start = pos;

		strValue = strSrcString.substr(start + 1, end-start-1);
	}
	return TRUE;
};

BOOL CInfParse::GetModulesSectionInfo(const wchar_t *pModuleSection, vecSectionFileds_t &value)
{
	if (m_hInfHandle == INVALID_HANDLE_VALUE)
		return FALSE;

	vecSectionDataKV_t vecValue;
	if (!ReadSectionAllValue(pModuleSection, vecValue))
		return FALSE;

	for (vecSectionDataKV_t::iterator it = vecValue.begin();
		it != vecValue.end(); ++it)
	{
		//std::wstring strValue;
		//if (ReadSectionSubkey(pModuleSection, it->strKey.c_str(), 2, strValue))
		//{
		//	std::wstring ven, dev, rev;
		//	GetSubStringValue(strValue, TEXT("VEN"), ven);
		//	GetSubStringValue(strValue, TEXT("DEV"), dev);
		//	GetSubStringValue(strValue, TEXT("REV"), rev);

		//	value.push_back(ven);
		//	value.push_back(dev);
		//	value.push_back(rev);
		//}

		std::wstring strValue;
		size_t pos = it->strVal.find_first_of(TEXT(','));
		if (pos != std::wstring::npos)
		{
			strValue = it->strVal.substr(pos+1);

			std::wstring ven, dev, rev;
			GetSubStringValue(strValue, TEXT("VEN"), ven);
			GetSubStringValue(strValue, TEXT("DEV"), dev);
			GetSubStringValue(strValue, TEXT("REV"), rev);

			value.push_back(ven);
			value.push_back(dev);
			value.push_back(rev);
		}
	}
	return TRUE;
};

BOOL CInfParse::GetModuleSectionAllData(const wchar_t *pMfgName, vecSectionFileds_t &value)
{
	if (m_hInfHandle == INVALID_HANDLE_VALUE)
		return FALSE;

	vecSectionDataKV_t vecValue;
	if (!ReadSectionAllValue(pMfgName, vecValue))
		return FALSE;

	for (vecSectionDataKV_t::iterator it = vecValue.begin();
		it != vecValue.end(); ++it)
	{
		std::wstring strValue;
		size_t pos = it->strVal.find_first_of(TEXT(','));
		if (pos != std::wstring::npos)
		{
			strValue = it->strVal.substr(pos + 1);

			std::wstring ven, dev, rev, subsys;
			GetSubStringValue(strValue, TEXT("VEN"), ven);
			GetSubStringValue(strValue, TEXT("DEV"), dev);
			GetSubStringValue(strValue, TEXT("REV"), rev);
			GetSubStringValue(strValue, TEXT("SUBSYS"), subsys);

			value.push_back(ven);
			value.push_back(dev);
			value.push_back(rev);
			value.push_back(subsys);
		}
	}

	return TRUE;
};

void CInfParse::DevDwordInfo2StrInfo(tSysInfo &sysInfo, std::wstring &strDevInfo)
{
	wchar_t devInfo[MAX_PATH] = { 0 };
	_stprintf_s(devInfo, TEXT("PCI\\VEN_%x&DEV_%d&SUBSYS_%x&REV_%x"), sysInfo.dwVen, sysInfo.dwDev, sysInfo.dwSubSys, sysInfo.dwRev);
	
	strDevInfo = devInfo;
};

bool CInfParse::DevStrInfo2DwordInfo(std::wstring &strDevInfo, tSysInfo &sysInfo)
{
	if (4 == _stscanf_s(strDevInfo.c_str(), TEXT("PCI\\VEN_%04x&DEV_%04x&SUBSYS_%08x&REV_%02x"),
		&sysInfo.dwVen, &sysInfo.dwDev, &sysInfo.dwSubSys, &sysInfo.dwRev))
		return true;
	else if (3 == _stscanf_s(strDevInfo.c_str(), TEXT("PCI\\VEN_%04x&DEV_%04x&SUBSYS_%08x"),
		&sysInfo.dwVen, &sysInfo.dwDev, &sysInfo.dwSubSys))
	{
		sysInfo.dwRev = 0xff;
		return true;
	}
	else if (3 == _stscanf_s(strDevInfo.c_str(), TEXT("PCI\\VEN_%04x&DEV_%04x&REV_%02x"),
		&sysInfo.dwVen, &sysInfo.dwDev, &sysInfo.dwRev))
	{
		sysInfo.dwSubSys = 0xff;
		return true;
	}
	else if (2 == _stscanf_s(strDevInfo.c_str(), TEXT("PCI\\VEN_%04x&DEV_%04x"),
		&sysInfo.dwVen, &sysInfo.dwDev))
	{
		sysInfo.dwRev = 0xff;
		sysInfo.dwSubSys = 0xff;
		return true;
	}
	else
		return false;

};

BOOL CInfParse::GetSectionName(tSysInfo &sysInfo, std::wstring &strSectionName, std::wstring &strDisplayName, std::wstring &strDevSection)
{
	vecSectionFileds_t vecMfgInfos;
	GetModulesSectionName(vecMfgInfos);

	for (vecSectionFileds_t::iterator it = vecMfgInfos.begin();
		it != vecMfgInfos.end(); ++it)
	{
		std::wstring strTmpName = *it + TEXT(".") + sysInfo.m_strOSVer;
		strDevSection = strTmpName;

		vecSectionDataKV_t vecValue;
		ReadSectionAllValue(strTmpName.c_str(), vecValue);

		//精确匹配
		for (vecSectionDataKV_t::iterator itVal = vecValue.begin();
			itVal != vecValue.end(); ++itVal)
		{
			size_t pos = itVal->strVal.find_last_of(L',');
			std::wstring strDevInfo = itVal->strVal.substr(pos + 1);

			tSysInfo tmpInfo;
			if (DevStrInfo2DwordInfo(strDevInfo, tmpInfo))
			{
				if (tmpInfo.dwDev == sysInfo.dwDev
					&& tmpInfo.dwRev == sysInfo.dwRev
					&& tmpInfo.dwSubSys == sysInfo.dwSubSys
					&& tmpInfo.dwVen == sysInfo.dwVen)
				{
					strSectionName = itVal->strVal.substr(0, pos);
					strDisplayName = itVal->strKey;
					return TRUE;
				}
			}
		}

		//模糊匹配ven,dev,rev
		for (vecSectionDataKV_t::iterator itVal = vecValue.begin();
			itVal != vecValue.end(); ++itVal)
		{
			size_t pos = itVal->strVal.find_last_of(L',');
			std::wstring strDevInfo = itVal->strVal.substr(pos + 1);

			tSysInfo tmpInfo;
			if (DevStrInfo2DwordInfo(strDevInfo, tmpInfo))
			{
				if (tmpInfo.dwDev == sysInfo.dwDev
					&& tmpInfo.dwRev == sysInfo.dwRev
					&& tmpInfo.dwVen == sysInfo.dwVen)
				{
					strSectionName = itVal->strVal.substr(0, pos);
					strDisplayName = itVal->strKey;
					return TRUE;
				}
			}
		}

		//模糊匹配ven,dev,subsys
		for (vecSectionDataKV_t::iterator itVal = vecValue.begin();
			itVal != vecValue.end(); ++itVal)
		{
			size_t pos = itVal->strVal.find_last_of(L',');
			std::wstring strDevInfo = itVal->strVal.substr(pos + 1);

			tSysInfo tmpInfo;
			if (DevStrInfo2DwordInfo(strDevInfo, tmpInfo))
			{
				if (tmpInfo.dwDev == sysInfo.dwDev
					&& tmpInfo.dwSubSys == sysInfo.dwSubSys
					&& tmpInfo.dwVen == sysInfo.dwVen)
				{
					strSectionName = itVal->strVal.substr(0, pos);
					strDisplayName = itVal->strKey;
					return TRUE;
				}
			}
		}

		//模糊匹配ven,dev
		for (vecSectionDataKV_t::iterator itVal = vecValue.begin();
			itVal != vecValue.end(); ++itVal)
		{
			size_t pos = itVal->strVal.find_last_of(L',');
			std::wstring strDevInfo = itVal->strVal.substr(pos + 1);

			tSysInfo tmpInfo;
			if (DevStrInfo2DwordInfo(strDevInfo, tmpInfo))
			{
				if (tmpInfo.dwDev == sysInfo.dwDev && tmpInfo.dwVen == sysInfo.dwVen)
				{
					strSectionName = itVal->strVal.substr(0, pos);
					strDisplayName = itVal->strKey;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
};

BOOL CInfParse::GetServicesAddServiceSection(const wchar_t *pSection, vecSectionFileds_t &vecRegSectionInfo)
{
	vecSectionFileds_t vecValues;
	if (!ReadSectionAllValue(pSection, TEXT("AddService"), vecValues))
		return FALSE;

	for (vecSectionFileds_t::iterator it = vecValues.begin();
		it != vecValues.end(); ++it)
	{
		size_t pos = it->find_first_of(L',');
		vecRegSectionInfo.push_back(it->substr(0, pos));

		size_t start = it->find_first_of(L',', pos + 1);
		size_t end = it->find_first_of(L',', start + 1);
		vecRegSectionInfo.push_back(it->substr(start+1, end-start-1));
	}
	return TRUE;
};

BOOL CInfParse::GetServicesRegInfo(const wchar_t *pSection, const wchar_t *pKey, vecSectionFileds_t &vecAddRegSection)
{
	vecSectionFileds_t vecValues;
	if (!ReadSectionAllValue(pSection, pKey, vecValues))
		return FALSE;

	for (vecSectionFileds_t::iterator it = vecValues.begin(); it != vecValues.end(); ++it)
	{
		size_t start = 0;
		size_t end = it->find(TEXT(','));
		while (start != std::wstring::npos)
		{
			std::wstring strKey;
			if (end == std::wstring::npos)
			{
				strKey = it->substr(start);
				start = std::wstring::npos;
			}
			else
			{
				strKey = it->substr(start, end - start);
				start = end + 1;
				end = it->find(TEXT(','), start);
			}

			vecSectionFileds_t vecAddReg;
			if (ReadSectionAllValue(strKey.c_str(), vecAddReg))
				vecAddRegSection.insert(vecAddRegSection.end(), vecAddReg.begin(), vecAddReg.end());
		}
	}
	return TRUE;
};

BOOL CInfParse::GetAddServiceOperator(const wchar_t *pSection, const wchar_t *pKey, mapRetDataInfo_t& mapRegInfo)
{
	vecSectionFileds_t vecAddServiceInfo;
	if (!GetServicesAddServiceSection(pSection, vecAddServiceInfo))
		return FALSE;

	for (vecSectionFileds_t::iterator it = vecAddServiceInfo.begin();
		it != vecAddServiceInfo.end();)
	{
		vecSectionFileds_t vecRegInfo;
		if (GetServicesRegInfo((it + 1)->c_str(), pKey, vecRegInfo))
			mapRegInfo[*it] = vecRegInfo;

		it += 2;
	}
	return TRUE;
};

BOOL CInfParse::GetAddRegInfo(const wchar_t *pSectionName, mapRetDataInfo_t& mapAddRegInfo, std::wstring &strSvrName)
{
	vecSectionFileds_t vecSection;
	ReadSectionAllValue(pSectionName, TEXT("AddService"), vecSection);
	if (vecSection.empty())
		return FALSE;

	for (vecSectionFileds_t::iterator it = vecSection.begin();
		it != vecSection.end(); ++it)
	{
		if (std::wstring::npos != it->find(TEXT("0x00000002")))
		{
			size_t pos = it->find_first_of(TEXT(','));
			strSvrName = it->substr(0, pos);
			break;
		}
	}

	return GetAddServiceOperator(pSectionName, TEXT("addreg"), mapAddRegInfo);
};

BOOL CInfParse::GetAddRegInfo(const wchar_t *pSectionName, vecSectionFileds_t& vecAddRegInfo)
{
	return GetServicesRegInfo(pSectionName, TEXT("addreg"), vecAddRegInfo);
};

BOOL CInfParse::GetDelRegInfo(const wchar_t *pSectionName, mapRetDataInfo_t& mapDelRegInfo)
{
	return GetAddServiceOperator(pSectionName, TEXT("delreg"), mapDelRegInfo);
};

BOOL CInfParse::GetDelRegInfo(const wchar_t *pSectionName, vecSectionFileds_t& vecDelRegInfo)
{
	return GetServicesRegInfo(pSectionName, TEXT("delreg"), vecDelRegInfo);
};

BOOL CInfParse::SplitString(const std::wstring &strSrc, const wchar_t mask, std::vector<std::wstring> &vecStrings)
{
	size_t start = 0;
	size_t end = strSrc.find_first_of(mask);
	while (start != std::wstring::npos)
	{
		vecStrings.push_back(strSrc.substr(start, end - start));
	
		if (end == std::wstring::npos)
			start = std::wstring::npos;
		else
			start = end + 1;
		end = strSrc.find_first_of(mask, start);
	}
	return TRUE;
};

BOOL CInfParse::GetDstPath(const std::wstring *pKey, mapSectionKeyValue_t &mapDstPath)
{
	return TRUE;
};

BOOL CInfParse::GetCopyFileInfo(const wchar_t *pSectionName, mapRetDataInfo_t& mapDelRegInfo)
{
	vecSectionFileds_t vecCopyFiles;
	ReadSectionAllValue(pSectionName, TEXT("copyfiles"), vecCopyFiles);
	for (vecSectionFileds_t::iterator it = vecCopyFiles.begin();
		it != vecCopyFiles.end(); ++it)
	{
		std::vector<std::wstring> vecStrings;
		SplitString(*it, TEXT(','), vecStrings);
		
		for (std::vector<std::wstring>::iterator itStr = vecStrings.begin();
			itStr != vecStrings.end(); ++itStr)
		{
			std::wstring strDstPath;
			//ReadSectionSubkey(TEXT("DestinationDirs"), itStr->c_str(), 1, strDstPath);
			ReadSectionKey(TEXT("DestinationDirs"), itStr->c_str(), strDstPath);
			if (strDstPath.empty())
				ReadSectionKey(TEXT("DestinationDirs"), TEXT("DefaultDestDir"), strDstPath);

			if (strDstPath.empty())
				continue;
			else
			{
				size_t pos = strDstPath.find(TEXT(','));
				if (std::wstring::npos != pos)
					strDstPath[pos] = TEXT('\\');
			}

			vecSectionFileds_t fileInfos;
			ReadSectionAllValue(itStr->c_str(), fileInfos);

			if (mapDelRegInfo.end() == mapDelRegInfo.find(strDstPath))
				mapDelRegInfo[strDstPath] = fileInfos;
			else
				mapDelRegInfo[strDstPath].insert(mapDelRegInfo[strDstPath].end(), fileInfos.begin(), fileInfos.end());
		}
	}

	return TRUE;
};

BOOL CInfParse::GetCopyFilePath(std::wstring &strOSArch, mapSectionKeyValue_t &mapFilePaths)
{
	vecSectionDataKV_t vecPaths;
	ReadSectionAllValue(TEXT("SourceDisksNames"), vecPaths);
	if (vecPaths.empty())
	{
		if (std::wstring::npos != strOSArch.find(TEXT("x64")))
			ReadSectionAllValue(TEXT("SourceDisksNames.amd64"), vecPaths);
		else 
			ReadSectionAllValue(TEXT("SourceDisksNames.x86"), vecPaths);
	}
	if (vecPaths.empty())
		return FALSE;

	mapSectionKeyValue_t mapPaths;
	for (vecSectionDataKV_t::iterator itp = vecPaths.begin();
		itp != vecPaths.end(); ++itp)
	{
		int posCount = 4;
		size_t pos = 0;
		while (pos != std::wstring::npos && posCount > 0)
		{
			pos += 1;
			pos = itp->strVal.find_first_of(TEXT(','), pos);
			posCount--;
		}

		size_t start = pos + 1;
		size_t end = -1;
		if (pos != std::wstring::npos)
		{
			end = itp->strVal.find_first_of(TEXT(','), start);
		}

		std::wstring strTmpPath;
		if (pos != std::wstring::npos)
			strTmpPath = itp->strVal.substr(start, end-start);
		else
			strTmpPath = TEXT("");

		start = 0;
		end = strTmpPath.size();
		for (std::wstring::iterator it = strTmpPath.begin(); 
			it != strTmpPath.end(); ++it)
		{
			if (*it == TEXT('.') || *it == TEXT('\\'))
				start++;
			else
				break;
		}

		mapPaths[itp->strKey] = strTmpPath.substr(start, end - start);
	}

	vecSectionDataKV_t vecFiles;
	if (!ReadSectionAllValue(TEXT("SourceDisksFiles"), vecFiles))
		return FALSE;

	for (vecSectionDataKV_t::iterator itf = vecFiles.begin();
		itf != vecFiles.end(); ++itf)
	{
		if (mapPaths.find(itf->strVal) != mapPaths.end())
		{
			for (std::wstring::iterator it = itf->strKey.begin();
				it != itf->strKey.end(); ++it)
			{
				if (*it >= TEXT('a') && *it <= TEXT('z'))
					*it = *it - 32;
			}

			mapFilePaths[itf->strKey] = mapPaths[itf->strVal];
		}
	}
	return TRUE;
};

BOOL CInfParse::GetServiceInfo(const wchar_t *pSectionName, mapRetDataInfo_t& mapServiceInfo)
{
	vecSectionFileds_t vecAddServiceInfo;
	if (!GetServicesAddServiceSection(pSectionName, vecAddServiceInfo))
		return FALSE;

	for (vecSectionFileds_t::iterator it = vecAddServiceInfo.begin();
		it != vecAddServiceInfo.end();)
	{
		vecSectionFileds_t vecSerInfo;
		std::wstring strValue;
		ReadSectionKey((it+1)->c_str(), TEXT("ServiceType"), strValue);
		vecSerInfo.push_back(strValue);

		strValue.clear();
		ReadSectionKey((it + 1)->c_str(), TEXT("StartType"), strValue);
		vecSerInfo.push_back(strValue);

		strValue.clear();
		ReadSectionKey((it + 1)->c_str(), TEXT("ErrorControl"), strValue);
		vecSerInfo.push_back(strValue);

		strValue.clear();
		ReadSectionKey((it + 1)->c_str(), TEXT("ServiceBinary"), strValue);
		vecSerInfo.push_back(strValue);

		strValue.clear();
		ReadSectionKey((it + 1)->c_str(), TEXT("LoadOrderGroup"), strValue);
		vecSerInfo.push_back(strValue);

		strValue.clear();
		ReadSectionKey((it + 1)->c_str(), TEXT("DisplayName"), strValue);
		vecSerInfo.push_back(strValue);

		mapServiceInfo[*it] = vecSerInfo;
		it += 2;
	}
	return TRUE;
};

BOOL CInfParse::GetManufacture(std::wstring &strMfg)
{
	vecSectionDataKV_t vecValue;
	ReadSectionAllValue(TEXT("Manufacturer"), vecValue);

	if (!vecValue.empty())
		strMfg = vecValue[0].strKey;

	return !vecValue.empty();
};

BOOL CInfParse::GetCatFileName(std::wstring &strCatFileName)
{
	return ReadSectionKey(TEXT("Version"), TEXT("CatalogFile"), strCatFileName);
};