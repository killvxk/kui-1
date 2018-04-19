#ifndef _INF_PARSE_H_
#define _INF_PARSE_H_

#include "comment.h"
#include <SetupAPI.h>
#pragma comment(lib, "setupapi.lib")

class CInfParse
{
	struct tDataKeyVal
	{
		std::wstring strKey;
		std::wstring strVal;
	};

	typedef std::vector<tDataKeyVal> vecSectionDataKV_t;

public:
	CInfParse();
	~CInfParse();

public:
	BOOL OpenInfFile(const wchar_t *pFileName, const wchar_t *pFileRelativePath);
	//BOOL OpenInfFile(const wchar_t *pFileName);
	BOOL CloseInfFile();
	
	//
	//以下函数用于inf转xml时 提取信息

	BOOL GetDriverDate(std::wstring &strDate);
	BOOL GetDriverVer(std::wstring &strVer);
	BOOL GetProvider(std::wstring &strProvider);
	BOOL GetSupportOS(vecSectionFileds_t &vecSupportOSs);
	BOOL GetInfName(std::wstring &name);

	//获取Manufacturer中，os前面的字段值：mfg名称
	BOOL GetModulesSectionName(vecSectionFileds_t &vecNames);

	//获取ven，dev，rev的值
	//存入value的顺序按ven，dev，rev，每三个一组
	//所以value的长度应该是3的倍数
	BOOL GetModulesSectionInfo(const wchar_t *pModuleSection, vecSectionFileds_t &value);
	
	//与GetModulesSectionInfo相比，获取所有值，包括subsys
	BOOL GetModuleSectionAllData(const wchar_t *pMfgName, vecSectionFileds_t &value);
	
	//
	//以下函数用于inf转reg时 提取信息

	//根据os,ven,dev,rev,subsys等信息，获取段信息
	//false表示没有适合的段
	//输入信息必须保证精确匹配
	BOOL GetSectionName(tSysInfo &sysInfo, std::wstring &strSectionName, std::wstring &strDisplayName, std::wstring &strDevSection);

	//获取addreg的信息
	//这个函数主要用于services中的addreg信息
	//返回结果
	//key:AddService中的第一个子节的名称
	//value:addreg的内容
	BOOL GetAddRegInfo(const wchar_t *pSectionName, mapRetDataInfo_t& mapAddRegInfo, std::wstring &strSvrName);
	
	//这个函数主要用于获取非services的addreg信息
	BOOL GetAddRegInfo(const wchar_t *pSectionName, vecSectionFileds_t& vecAddRegInfo);

	//获取delreg信息
	//这个函数主要用于获取service中的delreg信息
	BOOL GetDelRegInfo(const wchar_t *pSectionName, mapRetDataInfo_t& mapDelRegInfo);
	
	//这个函数主要用于获取非services中的delreg信息
	BOOL GetDelRegInfo(const wchar_t *pSectionName, vecSectionFileds_t& vecDelRegInfo);

	//获取servies中ServiceType、StartType、ErrorControl、ServiceBinary、LoadOrderGroup等信息
	//返回结果
	//key:AddService中的第一个子节的名称
	//value:按照ServiceType、StartType、ErrorControl、ServiceBinary、LoadOrderGroup、displayname排列的内容
	BOOL GetServiceInfo(const wchar_t *pSectionName, mapRetDataInfo_t& mapServiceInfo);

	//获取copyfile的信息
	BOOL GetCopyFileInfo(const wchar_t *pSectionName, mapRetDataInfo_t& mapDelRegInfo);

	//获取copyfile的路径信息
	BOOL GetCopyFilePath(std::wstring &strOSArch, mapSectionKeyValue_t &mapFilePaths);

	//获取厂商信息
	BOOL GetManufacture(std::wstring &strMfg);

	//获取cat文件名称
	BOOL GetCatFileName(std::wstring &strCatFileName);

private:
	//读取指定段的指定内容
	BOOL ReadSectionKey(const wchar_t *pSection, const wchar_t *pKey, std::wstring &strValue);

	//读取指定段的所有内容
	BOOL ReadSectionAllValue(const wchar_t *pSection, vecSectionDataKV_t &vecValue);

	//读取指定段的同名所有内容
	BOOL ReadSectionAllValue(const wchar_t *pSection, const wchar_t *pKeyName, vecSectionFileds_t &vecSection);

	//读取指定段的所有内容
	BOOL ReadSectionAllValue(const wchar_t *pSection, vecSectionFileds_t &vecAddRegSection);

	//读取指定段，指定key的部分value
	//注意:index中，0表示key，1开始表示value，每个逗号一个value
	BOOL ReadSectionSubkey(const wchar_t *pSection, const wchar_t *pKey, DWORD index, std::wstring &strValue);

	//读取指定段，指定值的所有结果，返回vector
	BOOL ReadSectionKeyAllValue(const wchar_t *pSection, const wchar_t *pKey, vecSectionFileds_t &fileds);
	BOOL GetFields(INFCONTEXT &ctx, vecSectionFileds_t &fileds);

	//获取字符串中的指定小字段的内容
	BOOL GetSubStringValue(std::wstring &strSrcString, wchar_t *pSubName, std::wstring &strValue);

	//根据dev,ven,rev,subsys信息，转化成字符串
	void DevDwordInfo2StrInfo(tSysInfo &sysInfo, std::wstring &strDevInfo);

	//从字符串中提取dev,ven,rev,subsys信息
	bool DevStrInfo2DwordInfo(std::wstring &strDevInfo, tSysInfo &sysInfo);

	//获取services信息中AddService字段的内容：reg名称 和 对应的reg内容段名称，即第一和第三小段内容
	BOOL GetServicesAddServiceSection(const wchar_t *pSection, vecSectionFileds_t &vecRegSectionInfo);

	//
	BOOL GetAddServiceOperator(const wchar_t *pSection, const wchar_t *pKey, mapRetDataInfo_t& mapDelRegInfo);

	//获取services中，addreg和delreg字段的内容
	BOOL GetServicesRegInfo(const wchar_t *pSection, const wchar_t *pKey, vecSectionFileds_t &vecAddRegSection);

	//分解字符串
	BOOL SplitString(const std::wstring &strSrc, const wchar_t mask, std::vector<std::wstring> &vecStrings);

	//获取文件的目的路径
	BOOL GetDstPath(const std::wstring *pKey, mapSectionKeyValue_t &mapDstPath);

private:
	HANDLE m_hInfHandle;
	std::wstring m_strFileName;
};
#endif