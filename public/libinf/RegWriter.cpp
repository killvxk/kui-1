
#include <tchar.h>
//#include <devpkey.h>
#include "RegWriter.h"

const std::wstring CRegWriter::HKR(TEXT("HKR"));
const std::wstring CRegWriter::HKCR(TEXT("HKCR")); //代表HKEY_CLASSES_ROOT;
const std::wstring CRegWriter::HKCU(TEXT("HKCU")); // 代表HKEY_CURRENT_USER;
const std::wstring CRegWriter::HKLM(TEXT("HKLM")); // 代表HKEY_LOCAL_MACHINE
const std::wstring CRegWriter::HKU(TEXT("HKU")); //代表HKEY_USERS

const std::wstring CRegWriter::HKR_SERVICE(TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\currentcontrolset\\services\\%s]")); //代表service下的hKR的路径
const std::wstring CRegWriter::HKR_HW(TEXT("[HKLM\\SYSTEM\\CurrentControlSet\\Enum\\%s\\%s]")); //代表hw下的hkr路径
const std::wstring CRegWriter::HKR_NORM(TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\Class]")); //代表正常情况下的hkr路径
const std::wstring CRegWriter::HKR_COINSTALLERS(TEXT("")); //代表coinstallers下的hkr路径
const std::wstring CRegWriter::HKR_GENCONFIGDATA(TEXT("")); //GeneralConfigData下的hkr路径

const DWORD CRegWriter::HKR_TYEP_NORM = 1;
const DWORD CRegWriter::HKR_TYEP_SERVICE = 2;
const DWORD CRegWriter::HKR_TYEP_HW = 3;
const DWORD CRegWriter::HKR_TYEP_COINSTALLERS = 4;
const DWORD CRegWriter::HKR_TYEP_GENCONFIGDATA = 5;

const std::wstring CRegWriter::s_strEndLine = TEXT("\r\n");
const std::wstring CRegWriter::s_strDwordType(TEXT("0x00010001"));
const std::wstring CRegWriter::s_strStringType(TEXT("0"));
const std::wstring CRegWriter::s_strRegSzType2(TEXT("0x0"));
const std::wstring CRegWriter::s_strRegSzType(TEXT("0x00000000"));
const std::wstring CRegWriter::s_strBinaryType(TEXT("0x00000001"));
const std::wstring CRegWriter::s_strRegESzType(TEXT("0x00020000"));
const std::wstring CRegWriter::s_strRegMSzType(TEXT("0x00010000"));
const std::wstring CRegWriter::s_strRegASzType(TEXT("0x00010008"));

CRegWriter::CRegWriter() : m_dwBufSize(2048)
{
	m_pBufData = new wchar_t[m_dwBufSize];
};

CRegWriter::~CRegWriter()
{
	if (m_pBufData != NULL)
		delete[] m_pBufData;
};

BOOL CRegWriter::OpenRegFile(const wchar_t *pFileName)
{
	m_strRegFileName = pFileName;
	m_hFileHandle = ::CreateFile(pFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	return INVALID_HANDLE_VALUE != m_hFileHandle;
};

BOOL CRegWriter::CloseRegFile()
{
	if (INVALID_HANDLE_VALUE != m_hFileHandle)
		::CloseHandle(m_hFileHandle);
	m_hFileHandle = INVALID_HANDLE_VALUE;
	return TRUE;
};

void CRegWriter::DoDelReg()
{
	for (mapRegInfo_t::iterator itDel = m_mapDelReg.begin();
		itDel != m_mapDelReg.end(); ++itDel)
	{
		mapRegInfo_t::iterator itAddBuf = m_mapAddReg.find(itDel->first);
		if (itAddBuf == m_mapAddReg.end())
			continue;

		for (mapRegValueInfo_t::iterator itDelItem = itDel->second.begin();
			itDelItem != itDel->second.end(); ++itDelItem)
		{
			for (mapRegValueInfo_t::iterator itAddItem = itAddBuf->second.begin();
				itAddItem != itAddBuf->second.end(); ++itAddItem)
			{
				if (itDelItem->first == itAddItem->first)
				{
					itAddBuf->second.erase(itAddItem);
					break;
				}
			}
		}

		if (0 >= itAddBuf->second.size())
			m_mapAddReg.erase(itAddBuf);
	}
};

int CRegWriter::WriteReg()
{
	if (INVALID_HANDLE_VALUE != m_hFileHandle)
	{
		unsigned char cflag = 0xFF;
		DWORD dwSize = 1;
		DWORD dwWriteBytes = 0;
		::WriteFile(m_hFileHandle, &cflag, dwSize, &dwWriteBytes, NULL);
		
		cflag = 0xFE;
		dwSize = 1;
		dwWriteBytes = 0;
		::WriteFile(m_hFileHandle, &cflag, dwSize, &dwWriteBytes, NULL);

		wchar_t verInfo[] = TEXT("Windows Registry Editor Version 5.00\r\n\r\n");
		WriteData(verInfo, ::lstrlen(verInfo));
	}

	DoDelReg();

	if (!WriteOther())
		return -5;

	if (!WritePCI())
		return -2;

	if (!WriteServiceInfo())
		return -1;

	if(!WriteAddReg())
		return -3;

	if(!WriteDelReg())
		return -4;

	return 0;
};

void WriteMultiByteData(HANDLE hFileHandle, const wchar_t* pData, size_t szDataLen)
{
	DWORD dwWriteSize = 0;
	std::string strData;
	ParseCmdWideChar2MultiByte(pData, strData);
	::WriteFile(hFileHandle, strData.c_str(), static_cast<DWORD>(strData.size()), &dwWriteSize, NULL);
};
int CRegWriter::WriteCopyFile(const wchar_t *pFileName, const wchar_t *pRootPath, const wchar_t *pCatFileName)
{
	//需要直接拷贝的文件，区别于通过建立硬链接的文件
	//如果需要新增文件，直接插入mapCopyFileList即可
	//注意：在插入时，文件名称必须大写
	typedef std::map<std::wstring, std::wstring> mapCopyFileList_t;
	mapCopyFileList_t mapCopyFileList;
	mapCopyFileList.clear();
	mapCopyFileList.insert(std::make_pair(TEXT("ATIUMDAG.DLL"), TEXT("C:\\Windows\\SysWOW64\\")));
	mapCopyFileList.insert(std::make_pair(TEXT("NVD3DUM.DLL"), TEXT("C:\\Windows\\SysWOW64\\")));
	//

	HANDLE hIniFile = ::CreateFile(pFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hIniFile)
		return -1;

	//写ini段名称，以设备[ven_dev_subsys_rev]命名
	std::wstring strDstRegPath(TEXT("["));
	strDstRegPath += m_strDeviceInfo + TEXT("]");
	std::wstring strTmp = strDstRegPath + s_strEndLine;
	WriteMultiByteData(hIniFile, strTmp.c_str(), strTmp.size());

	//文件总数量
	DWORD dwFileCount = 0; //建立硬链接的文件数量
	DWORD dwCopyFileCount = 0; //拷贝的文件数量

	//用于去重
	std::map<std::wstring, int> mapWritedFile;

	//写段内容
	for (mapRetDataInfo_t::iterator itPath = m_mapCopyFile.begin();
		itPath != m_mapCopyFile.end(); ++itPath)
	{
		std::wstring strDstPath = GetDstPath(itPath->first.c_str());
		if (strDstPath.empty())
		{
			::OutputDebugString(TEXT("warm:destination path error!"));
			continue;
		}

		for (vecSectionFileds_t::iterator itFile = itPath->second.begin();
			itFile != itPath->second.end(); ++itFile)
		{
			DWORD dwFlag;
			std::wstring strDst, strSrc;
			std::wstring strTmp, strFileName;
			GetCopyInfoFromString(*itFile, strDst, strSrc, dwFlag);
			if (!strSrc.empty())
			{
				ConstuctDriviceRootPath(pRootPath, strSrc, strTmp);
				strSrc = strTmp + TEXT("\\") + strSrc;
				strFileName = strSrc;
			}
			else
			{
				strFileName = strDst;
				size_t pos = strDst.find_last_of(TEXT('\\'));
				if (pos != std::wstring::npos)
					strFileName = strDst.substr(pos + 1);

				ConstuctDriviceRootPath(pRootPath, strFileName, strTmp);
				strSrc = strTmp + TEXT("\\") + strFileName;
			}

			strDst = strDstPath + strDst;

			//去除重复内容
			std::wstring strDstTmp;
			toUpper(strDst, strDstTmp);
			if (mapWritedFile.find(strDstTmp) != mapWritedFile.end())
			{
				mapWritedFile[strDstTmp] += 1;
				continue;
			}
			else
			{
				mapWritedFile[strDstTmp] = 1;

				wchar_t FileKey[64] = { 0 };
				wchar_t FileCon[512] = { 0 };

				//注意：srcfile前面必须有斜杠('\')
				mapCopyFileList_t::iterator itCopy = mapCopyFileList.find(strFileName);
				if (itCopy == mapCopyFileList.end())
				{
					_stprintf_s(FileKey, _countof(FileKey), TEXT("lk%u"), dwFileCount++);
					_stprintf_s(FileCon, _countof(FileCon), TEXT("%s,\\%s"), strDst.c_str(), strSrc.c_str());
				}
				else
				{
					strDst = itCopy->second + itCopy->first;
					_stprintf_s(FileKey, _countof(FileKey), TEXT("cp%u"), dwCopyFileCount++);
					_stprintf_s(FileCon, _countof(FileCon), TEXT("%s,\\%s"), strDst.c_str(), strSrc.c_str());
				}

				std::wstring strIniData(FileKey);
				strIniData += TEXT("=");
				strIniData += FileCon;
				strIniData += s_strEndLine;
				WriteMultiByteData(hIniFile, strIniData.c_str(), strIniData.size());
			}
		}
	}

	//cat文件
	wchar_t catFileInfo[512] = { 0 };
	_stprintf_s(catFileInfo, _countof(catFileInfo), TEXT("cp%u=C:\\Windows\\System32\\CatRoot\\{F750E6C3-38EE-11D1-85E5-00C04FC295EE}\\%s,\\%s\\%s\r\n"),
		dwCopyFileCount, pCatFileName, pRootPath, pCatFileName);
	WriteMultiByteData(hIniFile, catFileInfo, ::lstrlen(catFileInfo));
	dwCopyFileCount++;

	//
	wchar_t FileCountBuf[32] = { 0 };
	_stprintf_s(FileCountBuf, _countof(FileCountBuf), TEXT("lcount=%u\r\ncpcount=%u"), dwFileCount, dwCopyFileCount);
	WriteMultiByteData(hIniFile, FileCountBuf, ::lstrlen(FileCountBuf));

	strTmp = s_strEndLine + s_strEndLine;
	WriteMultiByteData(hIniFile, strTmp.c_str(), strTmp.size());
	::CloseHandle(hIniFile);
	return 0;
};

void CRegWriter::SetServiceInfo(mapRetDataInfo_t& mapServiceInfo)
{
	m_mapServiceInfo = mapServiceInfo;
};

void CRegWriter::GetRegDataFromString(std::wstring &strSrc,
	std::wstring &strRoot, std::wstring &strPath,
	std::wstring &strKey, std::wstring &strValType, std::wstring &strVal)
{

	size_t start = 0;
	size_t end = strSrc.find_first_of(TEXT(','));
	if (end != std::wstring::npos){
		strRoot = strSrc.substr(start, end - start);
		start = end + 1;
		end = strSrc.find_first_of(TEXT(','), start);
	}
	else {
		strRoot = strSrc;
		return;
	}

	if (end != std::wstring::npos){
		strPath = strSrc.substr(start, end - start);
		start = end + 1;
		end = strSrc.find_first_of(TEXT(','), start);
	}
	else {
		strPath = strSrc.substr(start);
		return;
	}

	if (end != std::wstring::npos) {
		strKey = strSrc.substr(start, end - start);
		start = end + 1;
		end = strSrc.find_first_of(TEXT(','), start);
	}
	else {
		strKey = strSrc.substr(start);
		return;
	}

	if (end != std::wstring::npos) {
		strValType = strSrc.substr(start, end - start);
		start = end + 1;
		end = strSrc.find_first_of(TEXT(','), start);
	}
	else {
		strValType = strSrc.substr(start);
		return;
	}

	strVal = strSrc.substr(start);
};

void CRegWriter::GetCopyInfoFromString(std::wstring &strSrc,
	std::wstring &strDstFile, std::wstring &strSrcFile, DWORD &dwFlag)
{
	size_t start = 0;
	size_t end = strSrc.find_first_of(TEXT(','));
	if (end == std::wstring::npos)
	{
		strDstFile = strSrc;
		dwFlag = 0x00004000;
		return;
	}

	strDstFile = strSrc.substr(start, end - start);

	start = end + 1;
	end = strSrc.find_first_of(TEXT(','), start);
	strSrcFile = strSrc.substr(start, end - start);

	end = strSrc.find_first_of(TEXT(','), end + 1);
	start = end + 1;

	const wchar_t *pStart = &strSrc[start];
	_stscanf_s(pStart, TEXT("0x%x"), &dwFlag);
};

void CRegWriter::ConstructRegPath(const DWORD dwHkrType, const std::wstring &strRoot,
	const std::wstring &strPath, std::wstring &strRestPath)
{
	wchar_t cPathBuf[1024] = { 0 };
	if (strRoot == HKLM)
	{
		::_stprintf_s(cPathBuf, _countof(cPathBuf), 
			TEXT("[HKEY_LOCAL_MACHINE\\system\\kpnp\\%s\\%s]"),
			m_strDeviceInfo.c_str(), strPath.c_str());
		strRestPath = cPathBuf;
		return;
	}
	else if (strRoot == HKCR)
	{
		//strRestPath = TEXT("[HKEY_CLASSES_ROOT\\") + strPath + TEXT("]");
		::_stprintf_s(cPathBuf, _countof(cPathBuf),
			TEXT("[HKEY_LOCAL_MACHINE\\system\\kpnp\\%s\\root\\%s]"),
			m_strDeviceInfo.c_str(), strPath.c_str());
		strRestPath = cPathBuf;
		return;
	}
	else if (strRoot == HKCU)
	{
		strRestPath = TEXT("[HKEY_CURRENT_USER\\") + strPath + TEXT("]");
		return;
	}
	else if (strRoot == HKU)
	{
		strRestPath = TEXT("[HKEY_USERS\\") + strPath + TEXT("]");
		return;
	}
	else  if (strRoot == HKR)
	{
		switch (dwHkrType)
		{
		case HKR_TYEP_NORM:
			if (strPath.empty())
				_stprintf_s(cPathBuf, _countof(cPathBuf),
				TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\Class]"), m_strDeviceInfo.c_str());
			else
				_stprintf_s(cPathBuf, _countof(cPathBuf),
				TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\Class\\%s]"), m_strDeviceInfo.c_str(), strPath.c_str());
			break;
		case HKR_TYEP_SERVICE:
			_stprintf_s(cPathBuf, _countof(cPathBuf),
				TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\currentcontrolset\\services\\%s]"), strPath.c_str());
			break;
		case HKR_TYEP_HW:
			if (strPath.empty())
				_stprintf_s(cPathBuf, _countof(cPathBuf),
					TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\PCI\\Device Parameters]"), m_strDeviceInfo.c_str());
			else
				_stprintf_s(cPathBuf, _countof(cPathBuf),
					TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\PCI\\Device Parameters\\%s]"), m_strDeviceInfo.c_str(), strPath.c_str());
			break;
		default:
			break;
		}
		strRestPath = cPathBuf;
		return;
	}
	else
		::OutputDebugString(TEXT("reg root error\r\n"));
};

//注意：这里的buf只有2048
void CRegWriter::toUpper(const std::wstring &strSrc, std::wstring &strDst)
{
	memset(m_pBufData, 0, m_dwBufSize*sizeof(wchar_t));
	wchar_t *pBuf = m_pBufData;
	const wchar_t *pStr = strSrc.c_str();
	while (*pStr)
	{
		if (*pStr >= TEXT('a') && *pStr <= TEXT('z'))
		{
			*pBuf = (*pStr) - (TEXT('a') - TEXT('A'));
		}
		else
		{
			*pBuf = *pStr;
		}
		pBuf++;
		pStr++;
	}
	*pBuf = TEXT('\0');
	strDst = m_pBufData;
};

void CRegWriter::SetRegData(std::wstring &strRegPath, std::wstring strKey, RegInfo  &regInfo, mapRegInfo_t &regDatas)
{
	mapRegInfo_t::iterator itAddReg = regDatas.find(strRegPath);
	if (itAddReg == regDatas.end())
	{
		mapRegValueInfo_t values;
		regDatas[strRegPath] = values;
		itAddReg = regDatas.find(strRegPath);
	}

	if (itAddReg != regDatas.end())
	{
		//mapRegValueInfo_t::iterator itTmp = itAddReg->second.find(strKey);
		//if (itAddReg->second.end() != itTmp)
		//{
		//	if (regInfo.strValueType != itTmp->second.strValueType || regInfo.strValue != itTmp->second.strValue)
		//	{
		//		std::wstring strTmp = strKey + TEXT(" : ") + regInfo.strValue + TEXT(" | ") + itTmp->second.strValue + TEXT("\n");
		//		::OutputDebugString(strTmp.c_str());
		//	}
		//}
		//itAddReg->second[strKey] = regInfo;

		//去重
		std::wstring strTmpKey;
		toUpper(strKey, strTmpKey);

		mapRegValueInfo_t::iterator itTmp = itAddReg->second.begin();
		for (;itTmp != itAddReg->second.end(); ++itTmp)
		{
			std::wstring strDstKeyTmp;
			toUpper(itTmp->first, strDstKeyTmp);
			if (strDstKeyTmp == strTmpKey)
			{
				break;
			}
		}

		if (itTmp == itAddReg->second.end())
		{
			itAddReg->second[strKey] = regInfo;
		}
		else
		{
			itTmp->second = regInfo;
		}
	}
};

void CRegWriter::DoRegDataAddDel(mapRetDataInfo_t& mapRegDataFrom, mapRegInfo_t &mapRegDataTo)
{
	for (mapRetDataInfo_t::iterator it = mapRegDataFrom.begin();
		it != mapRegDataFrom.end(); ++it)
	{
		for (vecSectionFileds_t::iterator itVal = it->second.begin();
			itVal != it->second.end(); ++itVal)
		{
			std::wstring strRoot, strPath, strKey, strValType, strVal;
			GetRegDataFromString(*itVal, strRoot, strPath, strKey, strValType, strVal);
			if (strValType.empty())
			{
				strValType = TEXT("0x00000000");
			}

			//根据需要暂时只写入HKR的内容，
			//不需要写入HKLM,HKCU等字段
			//如果需要，只需要删除改代码即可
			if (strRoot != HKR)
				continue;

			std::wstring strRegPath;
			if (strPath.empty())
				ConstructRegPath(HKR_TYEP_SERVICE, strRoot, it->first, strRegPath);
			else
			{
				if (strRoot == HKR)
					ConstructRegPath(HKR_TYEP_SERVICE, strRoot, it->first + TEXT("\\") + strPath, strRegPath);
				else
					ConstructRegPath(HKR_TYEP_SERVICE, strRoot, strPath, strRegPath);
			}

			SetRegData(strRegPath, strKey, RegInfo(strValType, strVal), mapRegDataTo);
		}
	}
};

void CRegWriter::DoRegDataAddDel(vecSectionFileds_t& vecRegDataFrom, mapRegInfo_t &mapRegTo, DWORD dwType)
{
	for (vecSectionFileds_t::iterator itVal = vecRegDataFrom.begin();
		itVal != vecRegDataFrom.end(); ++itVal)
	{
		std::wstring strRoot, strPath, strKey, strValType, strVal;
		GetRegDataFromString(*itVal, strRoot, strPath, strKey, strValType, strVal);
		if (strValType.empty())
		{
			strValType = TEXT("0x00000000");
		}


		std::wstring strRegPath;
		ConstructRegPath(dwType, strRoot, strPath, strRegPath);

		//根据需要暂时只写入HKR的内容，
		//不需要写入HKLM,HKCU等字段
		//如果需要，只需要删除改代码即可
		if (strRoot != HKR)
			continue;

		SetRegData(strRegPath, strKey, RegInfo(strValType, strVal), mapRegTo);
	}
};

void CRegWriter::SetSvrName(std::wstring &strSvrName, 
	std::wstring &strMfgName, 
	std::wstring &strProductName, 
	std::wstring &strSectionName,
	std::wstring &strDevSection)
{
	m_strMfgName = strMfgName;
	m_strProductName = strProductName;
	m_strSvrName = strSvrName;
	m_strSectionName = strSectionName;
	m_strDevSection = strDevSection;
};

void CRegWriter::SetDriverInfo(std::wstring &strDate, std::wstring &strVer)
{
	m_strDate = strDate;
	m_strVer = strVer;
	//m_strCompID = strCompID;
};

void CRegWriter::SetAddReg(mapRetDataInfo_t& mapAddReg)
{
	DoRegDataAddDel(mapAddReg, m_mapAddReg);
};

void CRegWriter::SetAddReg(vecSectionFileds_t& vecAddReg, DWORD dwType)
{
	return DoRegDataAddDel(vecAddReg, m_mapAddReg, dwType);
};

void CRegWriter::SetDelReg(mapRetDataInfo_t& mapDelReg)
{
	DoRegDataAddDel(mapDelReg, m_mapDelReg);
};

void CRegWriter::SetDelReg(vecSectionFileds_t& vecDelReg)
{
	DoRegDataAddDel(vecDelReg, m_mapDelReg);
};

void CRegWriter::SetCopyFile(mapRetDataInfo_t& mapCopyFiles)
{
	for (mapRetDataInfo_t::iterator it = mapCopyFiles.begin();
		it != mapCopyFiles.end(); ++it)
	{
		mapRetDataInfo_t::iterator itTmp = m_mapCopyFile.find(it->first);
		if (itTmp != m_mapCopyFile.end())
		{
			itTmp->second.insert(itTmp->second.end(), it->second.begin(), it->second.end());
		}
		else
		{
			m_mapCopyFile[it->first] = it->second;
		}
	}
};

void CRegWriter::SetCopyFilePath(mapSectionKeyValue_t &mapPaths)
{
	m_mapCopyFilePaths = mapPaths;
};

void CRegWriter::WriteData(const wchar_t *pData, size_t iLen)
{
	//DWORD dwWriteSize = 0;
	//std::string strData;
	//ParseCmdWideChar2MultiByte(pData, strData);
	//::WriteFile(m_hFileHandle, strData.c_str(), static_cast<DWORD>(strData.size()), &dwWriteSize, NULL);

	DWORD dwWriteSize = 0;
	::WriteFile(m_hFileHandle, pData, static_cast<DWORD>(::lstrlen(pData))*sizeof(wchar_t), &dwWriteSize, NULL);
};

void CRegWriter::CheckString(const wchar_t *pVal, std::wstring &strRest)
{
	strRest = pVal;
	size_t pos = strRest.find_first_of(TEXT('\\'));
	while (pos != std::wstring::npos)
	{
		strRest.insert(pos, TEXT("\\"));
		pos = strRest.find_first_of(TEXT('\\'), pos + 2);
	}
};

void CRegWriter::WriteDwordValue(const wchar_t *pKey, const wchar_t *pVal)
{
	std::wstring strKey;
	CheckString(pKey, strKey);

	DWORD dwVal = 0;
	if (0 == _stscanf_s(pVal, TEXT("0x%x"), &dwVal))
	{
		dwVal = _tstoi(pVal);
	}

	wchar_t buf[1024] = { 0 };
	_stprintf_s(buf, _countof(buf), TEXT("\"%s\"=dword:%08x\r\n"), strKey.c_str(), dwVal);
	WriteData(buf, ::lstrlen(buf));
};

void CRegWriter::WriteStringValue(const wchar_t *pKey, const wchar_t *pVal)
{
	std::wstring strVal;
	CheckString(pVal, strVal);

	std::wstring strKey;
	CheckString(pKey, strKey);
	
	wchar_t buf[1024] = { 0 };
	_stprintf_s(buf, _countof(buf), TEXT("\"%s\"=\"%s\"\r\n"), strKey.c_str(), strVal.c_str());
	WriteData(buf, ::lstrlen(buf));
};

void CRegWriter::WriteBinaryValue(const wchar_t *pKey, const wchar_t *pVal)
{
	std::wstring strKey;
	CheckString(pKey, strKey);

	//int i = 0;
	//DWORD val;
	//std::wstring strVal;
	//while (1 == _stscanf_s(pVal + i, TEXT("%02x"), &val))
	//{
	//	wchar_t formatVal[4] = { 0 };
	//	::_stprintf_s(formatVal, _countof(formatVal), TEXT("%02x"), val);
	//	strVal += val;
	//}

	std::wstring strVal;
	const wchar_t *start = pVal;
	const wchar_t *end = start;
	while (*start != TEXT('\0'))
	{
		while (*end != TEXT(',') && *end != TEXT('\0')){
			end++;
		}

		DWORD val;
		if (1 == _stscanf_s(start, TEXT("%02x"), &val))
		{
			wchar_t formatVal[4] = { 0 };
			::_stprintf_s(formatVal, _countof(formatVal), TEXT("%02x"), val);
			strVal += formatVal;
		}

		if (*end == TEXT(',')){
			end++;
			strVal += TEXT(',');
		}
		start = end;
	}

	wchar_t buf[4096] = { 0 };
	_stprintf_s(buf, _countof(buf), TEXT("\"%s\"=hex:%s\r\n"), strKey.c_str(), strVal.c_str());
	WriteData(buf, ::lstrlen(buf));
};

void CRegWriter::Chars2String16(const wchar_t *pSrc, wchar_t *pBuf)
{
	wchar_t *p = pBuf;
	while (*p != TEXT(':')) ++p;
	++p;

	while (*pSrc != TEXT('\0'))
	{
		if (*pSrc == TEXT(','))
			_stprintf_s(p, 6 * sizeof(wchar_t), TEXT("00,00,"));
		else
			_stprintf_s(p, 6 * sizeof(wchar_t), TEXT("%02x,00,"), *((unsigned short*)(pSrc)));

		p += 6;
		++pSrc;
	}
	_stprintf_s(p, 6 * sizeof(wchar_t), TEXT("00,00\r\n"), *((unsigned short*)(pSrc)));
};

void CRegWriter::Chars2hex16(const wchar_t *pSrc, wchar_t *pBuf)
{
	wchar_t *p = pBuf;
	while (*pSrc != TEXT('\0'))
	{
		_stprintf_s(p, 6 * sizeof(wchar_t), TEXT("%02x,00,"), *((unsigned short*)(pSrc)));
		p += 6;
		++pSrc;
	}

	_stprintf_s(p, 6 * sizeof(wchar_t), TEXT("00,00"), *((unsigned short*)(pSrc)));
};

void CRegWriter::WriteESzValue(const wchar_t *pKey, const wchar_t *pVal)
{
	std::wstring strKey;
	CheckString(pKey, strKey);

	wchar_t buf[4096] = { 0 };
	_stprintf_s(buf, _countof(buf), TEXT("\"%s\"=hex(2):"), strKey.c_str());

	Chars2String16(pVal, buf);
	WriteData(buf, ::lstrlen(buf));
};

void CRegWriter::WriteMSzValue(const wchar_t *pKey, const wchar_t *pVal)
{
	std::wstring strKey;
	CheckString(pKey, strKey);

	wchar_t buf[4096] = { 0 };
	_stprintf_s(buf, _countof(buf), TEXT("\"%s\"=hex(7):"), strKey.c_str());
	//Chars2String16(pVal, buf);

	const wchar_t *ps = pVal;
	wchar_t *p = buf;
	while (*p != TEXT(':')) ++p;
	++p;

	BOOL bIsInColon = FALSE;
	while (*pVal != TEXT('\0'))
	{
		if (*pVal == TEXT('"'))
		{
			if (bIsInColon)
				bIsInColon = FALSE;
			else
				bIsInColon = TRUE;
			pVal++;
			continue;
		}

		if (*pVal == TEXT(',') && !bIsInColon)
			_stprintf_s(p, 6 * sizeof(wchar_t), TEXT("00,00,"));
		else
			_stprintf_s(p, 6 * sizeof(wchar_t), TEXT("%02x,00,"), *((unsigned short*)(pVal)));

		p += 6;
		pVal++;
	}
	
	_stprintf_s(p, 6 * sizeof(wchar_t), TEXT("00,00,"));
	p += 6;
	_stprintf_s(p, 6 * sizeof(wchar_t), TEXT("00,00\r\n"));
	WriteData(buf, ::lstrlen(buf));
};

BOOL CRegWriter::WriteServiceInfo()
{
	static const std::wstring strServiceType(TEXT("ServiceType")), strService(TEXT("Type"));
	static const std::wstring strStartType(TEXT("StartType")), strStart(TEXT("Start"));;
	static const std::wstring strErrorControl(TEXT("ErrorControl")), strErrCtl(TEXT("ErrorControl"));
	static const std::wstring strLoadOrderGroup(TEXT("LoadOrderGroup")), strGroup(TEXT("Group"));
	static const std::wstring strServiceBinary(TEXT("ServiceBinary")), strImgPath(TEXT("ImagePath"));
	static const std::wstring strDisplayName(TEXT("DisplayName"));

	static const std::wstring strKeyStart(TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\currentcontrolset\\services\\"));

	for (mapRetDataInfo_t::iterator it = m_mapServiceInfo.begin();
		it != m_mapServiceInfo.end(); ++it)
	{
		if (it->second.size() != 6)
			return FALSE;

		//注册表路径
		std::wstring sKeyFull = strKeyStart + it->first + TEXT("]");
		//WriteData(sKeyFull.c_str(), sKeyFull.size());
	
		vecSectionFileds_t::iterator itVal = it->second.begin();

		//注册表的内容

		//itVal中的数据按照ServiceType、StartType、ErrorControl、ServiceBinary、LoadOrderGroup、displayname顺序排列
		if (!itVal->empty())
		{
			//WriteDwordValue(strService.c_str(), itVal->c_str()); //ServiceType
			RegInfo data;
			data.strValueType = TEXT("0x00010001");
			data.strValue = itVal->c_str();

			SetRegData(sKeyFull, strService, data, m_mapAddReg);
		}
		++itVal;
		if (!itVal->empty())
		{
			//WriteDwordValue(strStart.c_str(), itVal->c_str()); //StartType
			RegInfo data;
			data.strValueType = TEXT("0x00010001");
			data.strValue = itVal->c_str();

			SetRegData(sKeyFull, strStart, data, m_mapAddReg);
		}
		++itVal;
		if (!itVal->empty())
		{
			//WriteDwordValue(strErrCtl.c_str(), itVal->c_str()); //ErrorControl
			RegInfo data;
			data.strValueType = TEXT("0x00010001");
			data.strValue = itVal->c_str();

			SetRegData(sKeyFull, strErrCtl, data, m_mapAddReg);
		}
		++itVal;
		
		if (!itVal->empty())
		{
			//std::wstring strVal = *itVal;
			//size_t pos = itVal->find_last_of(TEXT('\\'));
			//if (pos != std::wstring::npos)
			//{
			//	strVal = TEXT("system32\\DRIVERS\\") + itVal->substr(pos + 1);
			//}
			//WriteStringValue(strImgPath.c_str(), strVal.c_str()); //ServiceBinary

			for (std::wstring::iterator itStr = itVal->begin();
				itStr != itVal->end(); ++itStr)
			{
				if (*itStr >= TEXT('a') && *itStr <= TEXT('z'))
					*itStr = *itStr - 32;
			}

			std::wstring strVal = *itVal;
			size_t pos = itVal->find(TEXT("C:\\WINDOWS\\"));

			if (pos != std::wstring::npos)
			{
				if (it->second[0] == TEXT("1"))
					strVal = TEXT("\\systemroot\\");
				else
					strVal = TEXT("%systemroot%");

				strVal += itVal->substr(pos + ::lstrlen(TEXT("C:\\Windows\\")));
				
				//WriteESzValue(strImgPath.c_str(), strVal.c_str());
				RegInfo data;
				data.strValueType = TEXT("0x00020000");
				data.strValue = strVal;

				SetRegData(sKeyFull, strImgPath, data, m_mapAddReg);
			}
			else
			{
				//WriteStringValue(strImgPath.c_str(), strVal.c_str());
				RegInfo data;
				data.strValueType = TEXT("0x00000000");
				data.strValue = strVal;

				SetRegData(sKeyFull, strImgPath, data, m_mapAddReg);
			}
		}

		++itVal;
		if (!itVal->empty())
		{
			//WriteStringValue(strGroup.c_str(), itVal->c_str()); //group
			RegInfo data;
			data.strValueType = TEXT("0x00000000");
			data.strValue = itVal->c_str();

			SetRegData(sKeyFull, strGroup, data, m_mapAddReg);
		}

		++itVal;
		if (!itVal->empty())
		{
			//WriteStringValue(strDisplayName.c_str(), itVal->c_str()); //strDisplayName
			RegInfo data;
			data.strValueType = TEXT("0x00000000");
			data.strValue = itVal->c_str();

			SetRegData(sKeyFull, strDisplayName, data, m_mapAddReg);
		}
	}

	//WriteData(s_strEndLine.c_str(), s_strEndLine.size());
	return TRUE;
};

//将数据写入reg文件
BOOL CRegWriter::WriteAddReg()
{
	if (m_mapAddReg.empty())
		return TRUE;

	for (mapRegInfo_t::iterator itAR = m_mapAddReg.begin();
		itAR != m_mapAddReg.end(); ++itAR)
	{
		std::wstring strTmp = itAR->first + s_strEndLine;
		WriteData(strTmp.c_str(), strTmp.size());

		for (mapRegValueInfo_t::iterator itReg = itAR->second.begin();
			itReg != itAR->second.end(); ++itReg)
		{
			int type = -1;
			if (1 != _stscanf_s(itReg->second.strValueType.c_str(), TEXT("0x%x"), &type, sizeof(int)))
			{
				if (1 != _stscanf_s(itReg->second.strValueType.c_str(), TEXT("0X%x"), &type, sizeof(int)))
				{
					_stscanf_s(itReg->second.strValueType.c_str(), TEXT("%x"), &type, sizeof(int));
				}
			}

			if (type < 0)
				continue;

			if (type == 0x00010001)
				WriteDwordValue(itReg->first.c_str(), itReg->second.strValue.c_str());
			else if (type == 0x00000000)
				WriteStringValue(itReg->first.c_str(), itReg->second.strValue.c_str());
			else if (type == 0x00000001)
				WriteBinaryValue(itReg->first.c_str(), itReg->second.strValue.c_str());
			else if (type == 0x00020000)
				WriteESzValue(itReg->first.c_str(), itReg->second.strValue.c_str());
			else if (type == 0x00010000)
				WriteMSzValue(itReg->first.c_str(), itReg->second.strValue.c_str());
			/*
			from msdn:
			0x00000010 (FLG_ADDREG_KEYONLY)
			Create the given subkey, but ignore any supplied value-entry-name and/or value.
			*/
			else if (type = 0x00000010)
				continue;
			else //默认信息，或者没有标明值类型，都已string形似写入，即：0x00000000
				WriteStringValue(itReg->first.c_str(), itReg->second.strValue.c_str());
		}

		strTmp = s_strEndLine + s_strEndLine;
		WriteData(strTmp.c_str(), strTmp.size());
	}

	return TRUE;
};

BOOL CRegWriter::WriteDelReg()
{
	return TRUE;
};

void CRegWriter::ConstuctDriviceRootPath(const wchar_t *pVirtualRootPath, std::wstring &strFile, std::wstring &strRootPath)
{
	//strRootPath = TEXT("\\");
	//if(m_strDriviceFileRootPath.empty())
	//{
	//	strRootPath += pVirtualRootPath;
	//}
	//else
	//{
	//	strRootPath += pVirtualRootPath;
	//	strRootPath += TEXT("\\") + m_strDriviceFileRootPath;
	//}

	//strRootPath = TEXT("\\");
	//strRootPath += pVirtualRootPath;
	strRootPath = pVirtualRootPath;

	for (std::wstring::iterator it = strFile.begin();
		it != strFile.end(); ++it)
	{
		if (*it >= TEXT('a') && *it <= TEXT('z'))
			*it = *it - 32;
	}

	mapSectionKeyValue_t::iterator it = m_mapCopyFilePaths.find(strFile);
	if (it != m_mapCopyFilePaths.end()
		&& !it->second.empty())
	{
		strRootPath += TEXT("\\");
		strRootPath += it->second;
	}
};

BOOL CRegWriter::WritePCI()
{
	//wchar_t cRegPathBuf[1024] = { 0 };
	//::_stprintf_s(cRegPathBuf, _countof(cRegPathBuf),
	//	TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\PCI]\r\n"), m_strDeviceInfo.c_str());
	//WriteData(cRegPathBuf, ::lstrlen(cRegPathBuf));

	//WriteStringValue(TEXT("Class"), TEXT("Display"));
	//WriteStringValue(TEXT("ClassGUID"), TEXT("{4d36e968-e325-11ce-bfc1-08002be10318}"));
	//WriteDwordValue(TEXT("ConfigFlags"), TEXT("0"));
	//WriteStringValue(TEXT("Driver"), TEXT("{4d36e968-e325-11ce-bfc1-08002be10318}\\2000"));
	//WriteStringValue(TEXT("Service"), m_strSvrName.c_str());
	//WriteStringValue(TEXT("Mfg"), m_strMfgName.c_str());
	//WriteStringValue(TEXT("DeviceDesc"), m_strProductName.c_str());

	//std::wstring strTmp = s_strEndLine + s_strEndLine;
	//WriteData(strTmp.c_str(), strTmp.size());

	wchar_t cRegPathBuf[1024] = { 0 };
	::_stprintf_s(cRegPathBuf, _countof(cRegPathBuf),
		TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\PCI]"), m_strDeviceInfo.c_str());
	std::wstring strRegPath = cRegPathBuf;

	RegInfo data;
	std::wstring strKey;

	strKey = TEXT("Class");
	data.strValueType = TEXT("0x00000000");
	data.strValue = TEXT("Display");
	SetRegData(strRegPath, strKey, data, m_mapAddReg);

	strKey = TEXT("ClassGUID");
	data.strValueType = TEXT("0x00000000");
	data.strValue = TEXT("{4d36e968-e325-11ce-bfc1-08002be10318}");
	SetRegData(strRegPath, strKey, data, m_mapAddReg);

	strKey = TEXT("ConfigFlags");
	data.strValueType = TEXT("0x00010001");
	data.strValue = TEXT("0");
	SetRegData(strRegPath, strKey, data, m_mapAddReg);

	strKey = TEXT("Driver");
	data.strValueType = TEXT("0x00000000");
	data.strValue = TEXT("{4d36e968-e325-11ce-bfc1-08002be10318}\\2000");
	SetRegData(strRegPath, strKey, data, m_mapAddReg);

	strKey = TEXT("Service");
	data.strValueType = TEXT("0x00000000");
	data.strValue = m_strSvrName;
	SetRegData(strRegPath, strKey, data, m_mapAddReg);

	strKey = TEXT("Mfg");
	data.strValueType = TEXT("0x00000000");
	data.strValue = m_strMfgName;
	SetRegData(strRegPath, strKey, data, m_mapAddReg);

	strKey = TEXT("DeviceDesc");
	data.strValueType = TEXT("0x00000000");
	data.strValue = m_strProductName;
	SetRegData(strRegPath, strKey, data, m_mapAddReg);

	return TRUE;
};

BOOL CRegWriter::WriteOther()
{
	//写入[HKEY_LOCAL_MACHINE\SYSTEM\kpnp\{device_id}\Class\Settings]
	wchar_t cRegPathBuf[1024] = { 0 };
	wchar_t valBuf[2048 * 2] = { 0 };
	_stprintf_s(cRegPathBuf, _countof(cRegPathBuf),
		TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\Class]"), m_strDeviceInfo.c_str());
	//WriteData(cRegPathBuf, ::lstrlen(cRegPathBuf));
	std::wstring strRegPath = cRegPathBuf;

	RegInfo data;
	std::wstring key;

	key = TEXT("InfSection");
	data.strValueType = TEXT("0x00000000");
	data.strValue = m_strSectionName;
	SetRegData(strRegPath, key, data, m_mapAddReg);
	//WriteStringValue(TEXT("InfSection"), m_strSectionName.c_str());
	
	key = TEXT("DriverDesc");
	data.strValueType = TEXT("0x00000000");
	data.strValue = m_strProductName;
	SetRegData(strRegPath, key, data, m_mapAddReg);
	//WriteStringValue(TEXT("DriverDesc"), m_strProductName.c_str());

	key = TEXT("DriverVersion");
	data.strValueType = TEXT("0x00000000");
	data.strValue = m_strVer;
	SetRegData(strRegPath, key, data, m_mapAddReg);
	//WriteStringValue(TEXT("DriverVersion"), m_strVer.c_str());
	
	key = TEXT("DriverDate");
	data.strValueType = TEXT("0x00000000");
	data.strValue = m_strDate;
	SetRegData(strRegPath, key, data, m_mapAddReg);
	//WriteStringValue(TEXT("DriverDate"), m_strDate.c_str());

	key = TEXT("ProviderName");
	data.strValueType = TEXT("0x00000000");
	data.strValue = m_strMfgName;
	SetRegData(strRegPath, key, data, m_mapAddReg);
	//WriteStringValue(TEXT("ProviderName"), m_strMfgName.c_str());

	key = TEXT("ComponentId");
	data.strValueType = TEXT("0x00000000");
	data.strValue = TEXT("PCI\\") + m_strDeviceInfo;
	SetRegData(strRegPath, key, data, m_mapAddReg);
	//WriteStringValue(TEXT("ComponentId"), m_strDeviceInfo.c_str());
	
	key = TEXT("DriverDesc");
	data.strValueType = TEXT("0x00000000");
	data.strValue = m_strProductName;
	SetRegData(strRegPath, key, data, m_mapAddReg);

	//std::wstring strTmp = s_strEndLine + s_strEndLine;
	//WriteData(strTmp.c_str(), strTmp.size());

	::_stprintf_s(cRegPathBuf, _countof(cRegPathBuf),
		TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\Class\\Settings]"), m_strDeviceInfo.c_str());
	//WriteData(cRegPathBuf, ::lstrlen(cRegPathBuf));
	strRegPath = cRegPathBuf;

	key = TEXT("Device Description");
	data.strValueType = TEXT("0x00000000");
	data.strValue = m_strProductName;
	SetRegData(strRegPath, key, data, m_mapAddReg);
	//WriteStringValue(TEXT("Device Description"), m_strProductName.c_str());

	//strTmp = s_strEndLine + s_strEndLine;
	//WriteData(strTmp.c_str(), strTmp.size());

	//wchar_t *pSetting = TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\2000\\Settings]\r\n");
	//WriteData(pSetting, ::lstrlen(pSetting));
	//WriteStringValue(TEXT("Device Description"), m_strProductName.c_str());
	//strTmp = s_strEndLine + s_strEndLine;
	//WriteData(strTmp.c_str(), strTmp.size());

	//DriverDate
	::_stprintf_s(cRegPathBuf, _countof(cRegPathBuf),
		TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\PCI\\Properties\\{a8b865dd-2e3d-4094-ad97-e593a70c75d6}\\00000002\\00000000]"),
		m_strDeviceInfo.c_str());
	strRegPath = cRegPathBuf;

	key = TEXT("Type");
	data.strValueType = TEXT("0x00000001"); //binary
	data.strValue = TEXT("10,00,00,00");// TEXT("0x00000010");
	SetRegData(strRegPath, key, data, m_mapAddReg);

	FILETIME ft;
	DriverTime2FileTime(m_strDate, ft);
	::_stprintf_s(valBuf, _countof(valBuf), TEXT("%08x%08x"), ft.dwHighDateTime, ft.dwLowDateTime);

	std::wstring strTmpDate = valBuf;
	memset(valBuf, 0, _countof(valBuf)*sizeof(wchar_t));
	valBuf[0] = strTmpDate[14];
	valBuf[1] = strTmpDate[15];
	valBuf[2] = TEXT(',');
	valBuf[3] = strTmpDate[12];
	valBuf[4] = strTmpDate[13];
	valBuf[5] = TEXT(','); 
	valBuf[6] = strTmpDate[10];
	valBuf[7] = strTmpDate[11];
	valBuf[8] = TEXT(',');
	valBuf[9] = strTmpDate[8];
	valBuf[10] = strTmpDate[9];
	valBuf[11] = TEXT(',');
	valBuf[12] = strTmpDate[6];
	valBuf[13] = strTmpDate[7];
	valBuf[14] = TEXT(',');
	valBuf[15] = strTmpDate[4];
	valBuf[16] = strTmpDate[5];
	valBuf[17] = TEXT(',');
	valBuf[18] = strTmpDate[2];
	valBuf[19] = strTmpDate[3];
	valBuf[20] = TEXT(',');
	valBuf[21] = strTmpDate[0];
	valBuf[22] = strTmpDate[1];
	valBuf[23] = TEXT('\0');

	key = TEXT("Data");
	data.strValueType = TEXT("0x00000001"); //binary
	data.strValue = valBuf;
	SetRegData(strRegPath, key, data, m_mapAddReg);

	//DriverVersion
	::_stprintf_s(cRegPathBuf, _countof(cRegPathBuf),
		TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\PCI\\Properties\\{a8b865dd-2e3d-4094-ad97-e593a70c75d6}\\00000003\\00000000]"),
		m_strDeviceInfo.c_str());
	strRegPath = cRegPathBuf;

	key = TEXT("Type");
	data.strValueType = TEXT("0x00000001"); //binary
	data.strValue = TEXT("12,00,00,00");//TEXT("0x00000012");
	SetRegData(strRegPath, key, data, m_mapAddReg);

	Chars2hex16(m_strVer.c_str(), valBuf);
	key = TEXT("Data");
	data.strValueType = TEXT("0x00000001"); //binary
	data.strValue = valBuf;
	SetRegData(strRegPath, key, data, m_mapAddReg);

	//DriverProvider
	::_stprintf_s(cRegPathBuf, _countof(cRegPathBuf),
		TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\PCI\\Properties\\{a8b865dd-2e3d-4094-ad97-e593a70c75d6}\\00000009\\00000000]"),
		m_strDeviceInfo.c_str());
	strRegPath = cRegPathBuf;

	key = TEXT("Type");
	data.strValueType = TEXT("0x00000001"); //binary
	data.strValue = TEXT("12,00,00,00"); //TEXT("0x00000012");
	SetRegData(strRegPath, key, data, m_mapAddReg);

	Chars2hex16(m_strMfgName.c_str(), valBuf);
	key = TEXT("Data");
	data.strValueType = TEXT("0x00000001"); //binary
	data.strValue = valBuf;
	SetRegData(strRegPath, key, data, m_mapAddReg);

	::_stprintf_s(cRegPathBuf, _countof(cRegPathBuf),
		TEXT("[HKEY_LOCAL_MACHINE\\SYSTEM\\kpnp\\%s\\PCI\\Properties\\{83da6326-97a6-4088-9453-a1923f573b29}\\00000003\\00000000]"),
		m_strDeviceInfo.c_str());
	strRegPath = cRegPathBuf;

	key = TEXT("Type");
	data.strValueType = TEXT("0x00000001"); //binary
	data.strValue = TEXT("12,00,00,00");//TEXT("0x00000012");
	SetRegData(strRegPath, key, data, m_mapAddReg);

	std::wstring strVal;
	strVal = m_strInfName + TEXT(":");
	strVal += m_strDevSection + TEXT(":");
	strVal += m_strSectionName + TEXT(":");
	strVal += m_strVer + TEXT(":");
	strVal += TEXT("PCI\\") + m_strDeviceInfo;
	
	Chars2hex16(strVal.c_str(), valBuf);
	key = TEXT("Data");
	data.strValueType = TEXT("0x00000001"); //binary
	data.strValue = valBuf;
	SetRegData(strRegPath, key, data, m_mapAddReg);
	return TRUE;
};

#include <time.h>
#include <sstream>
#include <iostream> 
void CRegWriter::DriverTime2FileTime(std::wstring str, FILETIME &ft)
{
	int year = 0, month = 0, day = 0;
	for (std::wstring::iterator it = str.begin(); it != str.end(); ++it)
	{
		if (*it == TEXT('/') || *it == TEXT('-'))
		{
			*it = TEXT(' ');
		}
	}

	std::wstringstream(str) >> month >> day >> year;
	time_t rawtime = 0;
	struct tm timeinfo = { 0 };

	localtime_s(&timeinfo, &rawtime);
	timeinfo.tm_year = year - 1900;
	timeinfo.tm_mon = month - 1;
	timeinfo.tm_mday = day;
	rawtime = mktime(&timeinfo);

	LONGLONG ll = Int32x32To64(rawtime, 10000000) + 116444736000000000;
	ft.dwLowDateTime = (DWORD)ll;
	ft.dwHighDateTime = ll >> 32;
};

std::wstring CRegWriter::GetDstPath(const wchar_t *pPath)
{
	const wchar_t *pAppendPath = NULL;
	std::wstring strPath(pPath);
	size_t pos = strPath.find_first_of(TEXT('\\'));
	if (pos != std::wstring::npos)
	{
		strPath[pos] = TEXT('\0');
		pPath = strPath.c_str();
		pAppendPath = strPath.c_str() + pos + 1;
	}

	int iPath = -1;
	_stscanf_s(pPath, TEXT("%d"), &iPath);
	const wchar_t *pRootPath = NULL;

	//from msdn : SourceDrive:\pathname (the directory from which the INF file was installed)
	//from msdn : Driver package's Driver Store directory.	For Windows 8.1 and later versions of Windows, specifies the path to the Driver Store directory where the driver package was imported.
	if (1 == iPath || 13 == iPath
		|| 17 == iPath)
	{
		static const wchar_t * pSystem32 = TEXT("C:\\Windows\\System32\\");
		pRootPath = pSystem32;
	}
	else if (10 == iPath)
	{
		static const wchar_t * pWindows = TEXT("C:\\Windows\\");
		pRootPath = pWindows;
	}
	else if (11 == iPath || 16425 == iPath)
	{
		static const wchar_t * pSystem32 = TEXT("C:\\Windows\\System32\\");
		pRootPath = pSystem32;
	}
	else if (50 == iPath)
	{
		static const wchar_t * pSystem = TEXT("C:\\Windows\\System\\");
		pRootPath = pSystem;
	}
	else if (12 == iPath)
	{
		static const wchar_t * pDrivers = TEXT("C:\\Windows\\System32\\drivers\\");
		pRootPath = pDrivers;
	}
	else if (24 == iPath)
	{
		static const wchar_t * pRoot = TEXT("C:\\");
		pRootPath = pRoot;
	}
	else if (16427 == iPath || 16428 == iPath)
	{
		static const wchar_t * pProgramFilesCommon = TEXT("C:\\Program Files\\Common\\");
		pRootPath = pProgramFilesCommon;
	}
	else if (16426 == iPath || 16422 == iPath)
	{
		static const wchar_t * pProgramfiles = TEXT("C:\\Program Files\\");
		pRootPath = pProgramfiles;
	}
	else if (16430 == iPath)
	{
	}
	else if (16406 == iPath)
	{
	}
	else if (16407 == iPath)
	{
	}
	else if (16408 == iPath)
	{
	}
	else if (16409 == iPath)
	{
	}
	else if (16415 == iPath)
	{
	}
	else if (16419 == iPath)
	{
	}
	else
	{ }

	std::wstring strRestPath;
	if (pRootPath == NULL)
		return strRestPath;
	else if (pAppendPath == NULL)
		return pRootPath;
	else
	{
		strRestPath = pRootPath;
		strRestPath += pAppendPath;
		strRestPath += TEXT('\\');
		return strRestPath;
	}
};
