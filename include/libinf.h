#ifndef _INF_2_XML_H_
#define _INF_2_XML_H_

#include <string>
#include <vector>
#include <Windows.h>

//系统类型
#define SYSTEM_TYPE_UNKNOW	0
#define SYSTEM_TYPE_X86		1
#define SYSTEM_TYPE_X64		2

//系统版本
#define OS_TYPE_UNKNOW		0
#define OS_TYPE_XP			1
#define OS_TYPE_WIN7		2
#define OS_TYPE_WIN10		3

#define OS_51_X86	TEXT("5.1_x86")
#define OS_61_X86	TEXT("6.1_x86")
#define OS_61_X64	TEXT("6.1_x64")
#define OS_10_X86	TEXT("10.0_x86")
#define OS_10_X64	TEXT("10.0_x64")

typedef std::vector<std::wstring> vecSectionFileds_t;

struct tInfDriverInfo
{
	std::wstring m_strInfFullPath;
	std::wstring m_strUpdateTime;
	std::wstring m_strProductVer;
	std::wstring m_strGID;
	std::wstring m_strDefault;
	std::wstring m_strIcafe;

	tInfDriverInfo() : m_strProductVer(TEXT("0"))
		, m_strDefault(TEXT("0"))
		, m_strIcafe(TEXT("0"))
	{}
};

struct tSysInfo
{
	std::wstring m_strOSVer;
	DWORD dwVen;
	DWORD dwDev;
	DWORD dwRev;
	DWORD dwSubSys;
	DWORD dwBuild;

	tSysInfo() : dwVen(0xff)
		, dwDev(0xff)
		, dwRev(0xff)
		, dwSubSys(0xff)
		, dwBuild(0)
	{}
};

struct tOsInfo
{
	int iArchType; //系统架构，SYSTEM_TYPE_xxx
	int iOSType; //系统版本，OS_TYPE_xxxx
	int iBuild; //win10 build号，暂时不用

	tOsInfo() : iArchType(SYSTEM_TYPE_UNKNOW)
		, iOSType(OS_TYPE_UNKNOW)
		, iBuild(-1)
	{};
};

class CXml2Inf
{
public:
	CXml2Inf(){};
	~CXml2Inf(){};

public:
	//读取inf文件，解析成xml，并存入文件
	BOOL ParseInf2Xml(const wchar_t *pInfName, const wchar_t *pXmlName);

	//往xml文件中写入字段信息
	//第一个参数：xml文件全路径
	void SetXmlProductVersion(const wchar_t *pXmlName, const wchar_t *pVersion);
	void SetXmlUpdateTime(const wchar_t *pXmlName, const wchar_t *pTime);
	void SetXmlGID(const wchar_t *pXmlName, const wchar_t *pGid);

	//读取inf，解析成xml，不存文件
	//如果pInXmlData不为空，则将这里的内容合并到pOutXmlDataBuf中
	//注意：pInXmlData 和 pOutXmlDataBuf 的内存由外层释放
	//返回 0：成功，小于0，失败，失败信息再pErrInfoBuf中存储
	int ParseInf2Xml(const tInfDriverInfo &infInfo,
		wchar_t **pOutXmlDataBuf, int& iOutDataLen,
		wchar_t *pErrInfoBuf = NULL, const int iErrBufLen = 0);

	//读取inf，根据sysInfo中的参数，解析生成reg文件和ini文件
	//文件名包含文件全路径+文件名称
	int ParseInf2Reg(const wchar_t *pInfName, const wchar_t *pRootPath, tSysInfo &sysInfo, 
		const wchar_t *pRegFileFullPath, const wchar_t *pIniName,
		std::wstring &strDisplayName, wchar_t* pErrInfoBuf = NULL, int iErrBufLen = 0);

//	int GetOsName(CInfParse &infParser, tSysInfo &sysInfo, tOsInfo &osInfo, std::wstring &strOSName);
};
#endif