//#include "stdafx.h"
#include "XmlParse.h"
//#include "kcore.h"

const char *CXmlParse::s_pRootName = "pnpinfo";
const char *CXmlParse::s_pPnpInfo = "pnp";
const char *CXmlParse::s_pOsInfo = "os";
const char *CXmlParse::s_pBuild = "build";
const char *CXmlParse::s_pDev = "dev";

void ParseCmdWideChar2MultiByte(const wchar_t *pData, std::string &strRestCmd)
{
	int buflen = WideCharToMultiByte(CP_ACP, 0, pData, -1, NULL, 0, NULL, NULL);

	char *pbuf = new char[buflen];
	::WideCharToMultiByte(CP_ACP, 0, pData, -1, pbuf, buflen, NULL, NULL);

	strRestCmd = pbuf;
	delete[] pbuf;
};

void ParseCmdWideChar2MultiByte(std::wstring &strSrc, std::string &strRestCmd)
{
	ParseCmdWideChar2MultiByte(strSrc.c_str(), strRestCmd);
};

void ParseCmdMultiByte2WideChar(const char *pData, std::wstring &strRestCmd)
{
	int buflen = ::MultiByteToWideChar(CP_ACP, 0, pData, -1, NULL, 0);

	wchar_t *pDst = new wchar_t[buflen];
	::MultiByteToWideChar(CP_ACP, 0, pData, -1, pDst, buflen);

	strRestCmd = pDst;
	delete[] pDst;
}

void ParseCmdMultiByte2WideChar(std::string &strSrc, std::wstring &strRestCmd)
{
	ParseCmdMultiByte2WideChar(strSrc.c_str(), strRestCmd);
};

//打开文档，如果文档不存在，则创建新的xml文件，并加入pnpinfo的根节点
BOOL CXmlParse::OpenXmlFile(const wchar_t *pFileName)
{
	ParseCmdWideChar2MultiByte(pFileName, m_strXmlFileName);

	m_pDoc = new tinyxml2::XMLDocument;
	if (NULL != m_pDoc)
	{
		if (tinyxml2::XML_SUCCESS != m_pDoc->LoadFile(m_strXmlFileName.c_str()))
		{
			m_pRoot = m_pDoc->NewElement(s_pRootName);
			m_pDoc->InsertFirstChild(m_pRoot);
			m_bNewOrOpen = TRUE;
		}
		else
		{
			m_pRoot = m_pDoc->FirstChildElement(s_pRootName);
			m_bNewOrOpen = FALSE;
		}
	}

	return TRUE;
};

BOOL CXmlParse::ParseXmlStr(const wchar_t *pXmlContext)
{
	std::string strXmlContext;
	ParseCmdWideChar2MultiByte(pXmlContext, strXmlContext);

	m_pRoot = NULL;
	m_pDoc = new tinyxml2::XMLDocument;
	if (NULL != m_pDoc)
	{
		m_pDoc->Parse(strXmlContext.c_str());
		m_pRoot = m_pDoc->FirstChildElement(s_pRootName);
	}

	m_bNewOrOpen = TRUE;
	if (NULL == m_pRoot)
		return FALSE;
	else
		return TRUE;
};

BOOL CXmlParse::CloseXmlFile()
{
	if (NULL != m_pDoc)
	{
		m_pDoc->SaveFile(m_strXmlFileName.c_str());

		delete m_pDoc;
		m_pDoc = NULL;
	}
	return TRUE;
};

BOOL CXmlParse::CloseXmlFileNotSave()
{
	if (NULL != m_pDoc)
	{
		delete m_pDoc;
		m_pDoc = NULL;
	}
	return TRUE;
};

int CXmlParse::PrintStr(wchar_t **pXmlBuf, int &iBufLen)
{
	tinyxml2::XMLPrinter printer;
	m_pDoc->Print(&printer);

	std::wstring strRest;
	const char *pBuf = printer.CStr();
	ParseCmdMultiByte2WideChar(pBuf, strRest);

	iBufLen = static_cast<int>(strRest.size()*sizeof(wchar_t));
	*pXmlBuf = new wchar_t[strRest.size()+1];
	if (NULL != *pXmlBuf)
	{
		lstrcpy(*pXmlBuf, strRest.c_str());
		return iBufLen;
	}
	else
		return 0;
};

void CXmlParse::SetFileVersion(const wchar_t *pVersion)
{
	ParseCmdWideChar2MultiByte(pVersion, m_strDriverVersion);
};

void CXmlParse::SetInfName(const wchar_t *pFileName)
{
	ParseCmdWideChar2MultiByte(pFileName, m_strInfFileName);

	size_t pos = m_strInfFileName.find('\\');
	while (pos != std::string::npos)
	{
		m_strInfFileName.replace(pos, 1, "/");
		pos = m_strInfFileName.find('\\', pos + 1);
	}
};

void CXmlParse::SetProvider(const wchar_t *pProviderName)
{ 
	ParseCmdWideChar2MultiByte(pProviderName, m_strProvider);
};

void CXmlParse::SetDriverTime(const wchar_t *pTime)
{
	ParseCmdWideChar2MultiByte(pTime, m_strDriverTime);
};

void CXmlParse::SetProductVersion(const wchar_t *pVersion)
{
	ParseCmdWideChar2MultiByte(pVersion, m_strProductVer);
};

void CXmlParse::SetUpdateTime(const wchar_t *pTime)
{
	ParseCmdWideChar2MultiByte(pTime, m_strUpTime);
};

void CXmlParse::SetGID(const wchar_t *pGid)
{
	ParseCmdWideChar2MultiByte(pGid, m_strGid);
};

void CXmlParse::SetDefault(const wchar_t *pDefualt)
{
	ParseCmdWideChar2MultiByte(pDefualt, m_strDefault);
};

void CXmlParse::SetIcafe(const wchar_t *pIcafe)
{
	ParseCmdWideChar2MultiByte(pIcafe, m_strIcafe);
};

//添加pnp节点到pnpinfo下，并将os信息写入到pnp属性中
//如果节点已经存在，则直接返回
BOOL CXmlParse::WritePnpInfo(const wchar_t *pOSAttrVal)
{
	std::string strAttrVal;
	ParseCmdWideChar2MultiByte(pOSAttrVal, strAttrVal);

	if (NULL != FindOrInsertPnp(strAttrVal.c_str()))
		return TRUE;
	else
		return FALSE;
};

//添加dev到pnp节点中，pnp由os信息指定
//如果ven_dev_rev组成的字符串已经再xml文件中，则返回
BOOL CXmlParse::WriteDevInfo(const wchar_t *pOSAttrVal, const wchar_t *pVen, const wchar_t *pDev, const wchar_t *pRev)
{
	std::wstring strVen = pVen;
	std::wstring strDev = pDev;
	std::wstring strRev = pRev;
	
	std::string v, d, r;
	ParseCmdWideChar2MultiByte(pVen, v);
	ParseCmdWideChar2MultiByte(pDev, d);
	ParseCmdWideChar2MultiByte(pRev, r);

	//FF代表未定义，其中的dev，ven不能为FF
	if ((v == "FF" || v == "ff")
		|| (d == "FF" || d == "ff"))
	{
		return FALSE;
	}

	std::string os;
	ParseCmdWideChar2MultiByte(pOSAttrVal, os);
	std::string key = v + "_" + d + "_" + r;

	if (CheckKeyInXml(os.c_str() , v, d, r))
		return TRUE;

	tinyxml2::XMLElement *pPnpNode = FindOrInsertPnp(os.c_str());
	if (NULL == pPnpNode)
		return FALSE;

	tinyxml2::XMLElement *pNode = m_pDoc->NewElement(s_pDev);
	if (NULL == pNode)
		return FALSE;

	pPnpNode->InsertFirstChild(pNode);

	pNode->SetAttribute("ven", v.c_str());
	pNode->SetAttribute("dev", d.c_str());
	pNode->SetAttribute("rev", r.c_str());
	pNode->SetAttribute("fileversion", m_strDriverVersion.c_str());
	pNode->SetAttribute("inf", m_strInfFileName.c_str());
	pNode->SetAttribute("provider", m_strProvider.c_str());
	pNode->SetAttribute("time", m_strDriverTime.c_str());
	pNode->SetAttribute("productversion", m_strProductVer.c_str());
	pNode->SetAttribute("icafe", m_strIcafe.c_str());
	pNode->SetAttribute("default", m_strDefault.c_str());
	pNode->SetAttribute("updatetime", m_strUpTime.c_str());
	pNode->SetAttribute("gid", m_strGid.c_str());

	return TRUE;
};

void NtOsInfo2SysName(const char *pOsInfo, std::string &strSysName, std::string &strBuild)
{
	std::string strOsInfo(pOsInfo);
	BOOL bIsX64 = TRUE;
	BOOL bIsWin10 = FALSE;

	size_t pos = strOsInfo.find("amd64");
	if (pos == std::string::npos)
		bIsX64 = FALSE;

	if (std::string::npos != strOsInfo.find(".5") && !bIsX64)
		strSysName = OS_51_X86_A;
	else if (std::string::npos != strOsInfo.find(".6"))
		strSysName = !bIsX64 ? OS_61_X86_A : OS_61_X64_A;
	else if (std::string::npos != strOsInfo.find(".10"))
	{
		strSysName = !bIsX64 ? OS_10_X86_A : OS_10_X64_A;
		bIsWin10 = TRUE;
	}
	else
		strSysName.clear();

	strBuild = "0";
	if (bIsWin10)
	{
		pos = strOsInfo.find_last_of('.');
		strBuild = strOsInfo.substr(pos + 1);
		if (strBuild.size() < 5)
			strBuild = "0";
	}
};

//根据OS信息，查找或者插入pnp节点
tinyxml2::XMLElement * CXmlParse::FindOrInsertPnp(const char *pOsInfo, BOOL bInsert)
{
	//std::string strOS;
	//ParseCmdWideChar2MultiByte(pOsInfo, strOS);
	std::string strSysInfo;
	std::string strBuild;
	NtOsInfo2SysName(pOsInfo, strSysInfo, strBuild);

	tinyxml2::XMLElement *pNode = m_pRoot->FirstChildElement(s_pPnpInfo);
	while (pNode != NULL)
	{
		if (NULL != pNode->Attribute(s_pOsInfo, strSysInfo.c_str()) &&
			NULL != pNode->Attribute(s_pBuild, strBuild.c_str()))
			break;
		else
			pNode = pNode->NextSiblingElement(s_pPnpInfo);
	}

	if ((NULL == pNode) && bInsert)
	{
		pNode = m_pDoc->NewElement(s_pPnpInfo);
		m_pRoot->InsertFirstChild(pNode);
		pNode->SetAttribute(s_pOsInfo, strSysInfo.c_str());
		pNode->SetAttribute(s_pBuild, strBuild.c_str());
	}
	return pNode;
};

BOOL CXmlParse::CheckDevInSubNode(tinyxml2::XMLElement *pParent, std::string &strRest)
{
	tinyxml2::XMLElement *pNode = pParent->FirstChildElement(s_pDev);
	while (NULL != pNode)
	{
		std::string v, d, r;
		v = pNode->Attribute("ven");
		d = pNode->Attribute("dev");
		r = pNode->Attribute("rev");

		std::string key = v + "_" + d + "_" + r;
		if (key == strRest)
			return TRUE;
		else
			pNode = pNode->NextSiblingElement(s_pDev);
	}
	return FALSE;
};

//检查ven_dev_rev组成的节点是否已经再xml文件中
BOOL CXmlParse::CheckKeyInXml(const char *pOS, std::string &ven, std::string &dev, std::string &rev)
{
	tinyxml2::XMLElement *pNode = FindOrInsertPnp(pOS, FALSE);
	if (pNode == NULL)
		return FALSE;

	std::string strRest = ven + ("_") + dev + ("_") + rev;
	return CheckDevInSubNode(pNode, strRest);
	
};

BOOL CXmlParse::CheckKeyInXml(std::string &ven, std::string &dev, std::string &rev)
{
	std::string strRest = ven + "_" + dev + "_" + rev;
	tinyxml2::XMLElement *pPnpNode = m_pRoot->FirstChildElement(s_pPnpInfo);
	if (NULL == pPnpNode)
		return FALSE;

	BOOL bFound = FALSE;
	while (NULL != pPnpNode)
	{
		bFound = CheckDevInSubNode(pPnpNode, strRest);
		if (bFound)
			break;
		else
			pPnpNode = pPnpNode->NextSiblingElement(s_pPnpInfo);
	}
	return bFound;
};
/*
//将整个xml文件读取出来，并以key:value格式存储
//key : os
//value : type : map, context: key : ven_dev_rev, value:count
BOOL CXmlParse::ReadXml(mapXmlComtext_t &context)
{
	if (NULL == m_pDoc)
		return FALSE;

	tinyxml2::XMLElement *pRoot = m_pDoc->FirstChildElement(s_pRootName);
	tinyxml2::XMLElement *pNode = pRoot->FirstChildElement(s_pPnpInfo);
	while (pNode != NULL)
	{
		const char *pAttr = pNode->Attribute(s_pOsInfo);

		mapXmlComtext_t::iterator it = context.find(pAttr);
		if (it == context.end())
		{
			context.insert(std::make_pair(pAttr, mapKeyInfo_t()));
			it = context.find(pAttr);
			//*it = context.insert(std::make_pair(pAttr, mapKeyInfo_t()));
		}

		tinyxml2::XMLElement *pSubNode = pNode->FirstChildElement(s_pDev);
		while (pSubNode != NULL)
		{
			const char *ven = pSubNode->Attribute("ven");
			const char *dev = pSubNode->Attribute("dev");
			const char *rev = pSubNode->Attribute("rev");

			std::string strKey = ven;
			strKey += "_";
			strKey += dev;
			strKey += "_";
			strKey += rev;

			if (it->second.find(strKey) != it->second.end())
				it->second[strKey] += 1;
			else
				it->second[strKey] = 1;

			pSubNode = pSubNode->NextSiblingElement(s_pDev);
		}

		pNode = pNode->NextSiblingElement(s_pPnpInfo);
	}
	return TRUE;
};
*/
BOOL CXmlParse::SetAttr2AllNode(const char *pAttrName, const char *pAttrValue)
{
	tinyxml2::XMLElement *pSubNode = m_pRoot->FirstChildElement(s_pPnpInfo);
	while (pSubNode != NULL)
	{
		tinyxml2::XMLElement *pDevNode = pSubNode->FirstChildElement(s_pDev);
		while (pDevNode != NULL)
		{
			pDevNode->SetAttribute(pAttrName, pAttrValue);
			pDevNode = pDevNode->NextSiblingElement(s_pDev);
		}

		pSubNode = pSubNode->NextSiblingElement(s_pPnpInfo);
	}
	return TRUE;
};

BOOL CXmlParse::SetXmlProductVersion(const wchar_t *pVersion)
{
	std::string version;
	ParseCmdWideChar2MultiByte(pVersion, version);

	return SetAttr2AllNode("productversion", version.c_str());
};

BOOL CXmlParse::SetXmlUpdateTime(const wchar_t *pTime)
{
	std::string time;
	ParseCmdWideChar2MultiByte(pTime, time);

	return SetAttr2AllNode("updatetime", time.c_str());
};

BOOL CXmlParse::SetXmlGID(const wchar_t *pGid)
{
	std::string guid;
	ParseCmdWideChar2MultiByte(pGid, guid);

	return SetAttr2AllNode("gid", guid.c_str());
};

BOOL CXmlParse::SetXmlDefault(const wchar_t *pDefualt)
{
	std::string strDefault;
	ParseCmdWideChar2MultiByte(pDefualt, strDefault);

	return SetAttr2AllNode("default", strDefault.c_str());
};

BOOL CXmlParse::SetXmlIcafe(const wchar_t *pIcafe)
{
	std::string strICafe;
	ParseCmdWideChar2MultiByte(pIcafe, strICafe);

	return SetAttr2AllNode("icafe", strICafe.c_str());
};