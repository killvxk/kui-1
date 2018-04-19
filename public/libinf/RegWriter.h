#ifndef _REG_WRITER_H_
#define _REG_WRITER_H_

#include "comment.h"

class CRegWriter
{
	struct RegInfo
	{
		std::wstring strValueType;
		std::wstring strValue;

		RegInfo()
		{};
		RegInfo(std::wstring &strVt, std::wstring &strV) : strValueType(strVt), strValue(strV)
		{};
	};

	typedef std::map<std::wstring, RegInfo> mapRegValueInfo_t;
	typedef std::map<std::wstring, mapRegValueInfo_t> mapRegInfo_t;

public:
	CRegWriter();
	~CRegWriter();

public:
	BOOL OpenRegFile(const wchar_t *pFileName);
	BOOL CloseRegFile();
	int WriteReg();
	int WriteCopyFile(const wchar_t *pFileName, const wchar_t *pRootPath, const wchar_t *pCatFileName);

	//设备的ven、dev、rev信息
	void SetDeviceInfo(const wchar_t *pDeviceInfo){
		m_strDeviceInfo = pDeviceInfo;
	};
	//inf文件名称
	void SetInfName(const wchar_t *pFileName)
	{
		m_strInfName = pFileName;
		size_t pos = m_strInfName.find_last_of(TEXT('\\'));
		if (pos != std::wstring::npos)
		{
			m_strInfName = m_strInfName.substr(pos + 1);
		}
	};

	//设置文件的路径
	void SetFileRootPath(std::wstring &strPath);

	void SetServiceInfo(mapRetDataInfo_t& mapServiceInfo);
	void SetAddReg(mapRetDataInfo_t& mapServiceInfo);
	void SetAddReg(vecSectionFileds_t& vecDelRegInfo, DWORD dwType = HKR_TYEP_NORM);
	void SetDelReg(mapRetDataInfo_t& mapServiceInfo);
	void SetDelReg(vecSectionFileds_t& vecDelRegInfo);
	void SetCopyFile(mapRetDataInfo_t& mapCopyFiles);
	void SetCopyFilePath(mapSectionKeyValue_t &mapPaths);
	void SetSvrName(std::wstring &strSvrName, std::wstring &strMfgName, 
		std::wstring &strProductName, std::wstring &strSectionName,
		std::wstring &strDevSection);
	void SetDriverInfo(std::wstring &strDate, std::wstring &strVer);

private:
	////以下函数用于写入inf中 services的相关信息
	//BOOL WriteServiceInfo();
	//BOOL WriteServiceAddReg();
	//BOOL WriteServiceDelReg();

	////以下函数用于写入inf中 非services的相关信息
	//BOOL WriteNormAddReg();
	//BOOL WriteNormDelReg();
	//BOOL WriteNormCopyFileReg();
	
	void GetRegDataFromString(std::wstring &strSrc,
		std::wstring &strRoot, std::wstring &strPath,
		std::wstring &strKey, std::wstring &strValType, std::wstring &strVal);

	void GetCopyInfoFromString(std::wstring &strSrc,
		std::wstring &strDstFile, std::wstring &strSrcFile, DWORD &dwFlag);

	//设置文件的路径
	void SetFilePath(std::wstring &strDrivicePath, std::wstring &strInfPath){
		m_strDriviceFileRootPath = strDrivicePath;
		m_strInfFilePath = strInfPath;
	};

	//根据文件路径信息，生成源文件的根路径
	void ConstuctDriviceRootPath(const wchar_t *pVirtualRootPath, std::wstring &strFile, std::wstring &strRootPath);

	//将数据写入reg文件
	BOOL WriteServiceInfo();
	BOOL WriteAddReg();
	BOOL WriteDelReg();
	BOOL WritePCI();
	BOOL WriteOther();

	//辅助数据写入
	void WriteDwordValue(const wchar_t *pKey, const wchar_t *pVal);
	void WriteStringValue(const wchar_t *pKey, const wchar_t *pVal);
	void WriteBinaryValue(const wchar_t *pKey, const wchar_t *pVal);
	void WriteESzValue(const wchar_t *pKey, const wchar_t *pVal);
	void WriteMSzValue(const wchar_t *pKey, const wchar_t *pVal);
	void WriteData(const wchar_t *pData, size_t iLen);

	//解决单斜杠的问题
	void CheckString(const wchar_t *pVal, std::wstring &strRest);

	//辅助完成数据插入和删除
	void ConstructRegPath(const DWORD dwHkrType, const std::wstring &strRoot, 
		const std::wstring &strPath, std::wstring &strRestPath);
	void SetRegData(std::wstring &strRegPath, std::wstring strKey, RegInfo &regInfo, mapRegInfo_t &regDatas);
	void DoRegDataAddDel(mapRetDataInfo_t& mapRegDataFrom, mapRegInfo_t &mapRegTo);
	void DoRegDataAddDel(vecSectionFileds_t& vecRegDataFrom, mapRegInfo_t &mapRegTo, DWORD dwType = HKR_TYEP_NORM);

	//将字符串转成16进制字符串
	void Chars2String16(const wchar_t *pSrc, wchar_t *pBuf);
	void CRegWriter::Chars2hex16(const wchar_t *pSrc, wchar_t *pBuf);

	//将inf中的数字转成实际路径
	std::wstring GetDstPath(const wchar_t *pPath);

	//从addreg中删除delreg中的内容
	void DoDelReg();

	//将驱动时间改成filetime
	void DriverTime2FileTime(std::wstring strDriverTime, FILETIME &ft);

private:
	static const std::wstring HKR;
	static const std::wstring HKCR; //代表HKEY_CLASSES_ROOT;
	static const std::wstring HKCU; // 代表HKEY_CURRENT_USER;
	static const std::wstring HKLM; // 代表HKEY_LOCAL_MACHINE
	static const std::wstring HKU; //代表HKEY_USERS

	static const std::wstring HKR_SERVICE; //代表service下的hKR的路径
	static const std::wstring HKR_HW; //代表hw下的hkr路径
	static const std::wstring HKR_NORM; //代表正常情况下的hkr路径
	static const std::wstring HKR_COINSTALLERS; //代表coinstallers下的hkr路径
	static const std::wstring HKR_GENCONFIGDATA; //GeneralConfigData下的hkr路径

	static const DWORD HKR_TYEP_NORM;
	static const DWORD HKR_TYEP_SERVICE;
	static const DWORD HKR_TYEP_HW;
	static const DWORD HKR_TYEP_COINSTALLERS;
	static const DWORD HKR_TYEP_GENCONFIGDATA;

	static const std::wstring s_strEndLine;
	static const std::wstring s_strDwordType;
	static const std::wstring s_strStringType;
	static const std::wstring s_strRegSzType2;
	static const std::wstring s_strRegSzType;
	static const std::wstring s_strBinaryType;
	static const std::wstring s_strRegESzType;
	static const std::wstring s_strRegMSzType;
	static const std::wstring s_strRegASzType;

	HANDLE m_hFileHandle;
	std::wstring m_strDeviceInfo;
	std::wstring m_strInfName;

	std::wstring m_strRegFileName;
	std::wstring m_strDriviceFileRootPath;
	std::wstring m_strInfFilePath;
	std::wstring m_strSvrName;
	std::wstring m_strMfgName;
	std::wstring m_strProductName;
	std::wstring m_strSectionName;
	std::wstring m_strDevSection;

	std::wstring m_strDate;
	std::wstring m_strVer;
	std::wstring m_strCompID;

	mapRetDataInfo_t m_mapServiceInfo;
	mapRegInfo_t m_mapAddReg;
	mapRegInfo_t m_mapDelReg;
	mapRetDataInfo_t m_mapCopyFile;
	mapSectionKeyValue_t m_mapCopyFilePaths;

	//用于变量大小写转化的缓冲区
	wchar_t *m_pBufData;
	const DWORD m_dwBufSize;
	void toUpper(const std::wstring &strSrc, std::wstring &strDst);
};
#endif