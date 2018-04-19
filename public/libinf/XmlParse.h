#ifndef _XML_PARSE_H_
#define _XML_PARSE_H_

#include "tinyxml2.h"
#include "comment.h"

class CXmlParse
{
public:
	CXmlParse() :m_pDoc(NULL), m_pRoot(NULL){};
	~CXmlParse(){};

	BOOL OpenXmlFile(const wchar_t *pFileName);
	BOOL CloseXmlFile(); //关闭，并存文件

	//注意：以下几个函数不能与上面的OpenXmlFile 和 CloseXmlFile 混用
	//将字符串解析成xml格式
	BOOL ParseXmlStr(const wchar_t *pXmlContext);
	//输出xml的内容到内存，返回buf长度buflen;
	int PrintStr(wchar_t **pXmlBuf, int &iBufLen);
	//关闭xml的doc
	BOOL CloseXmlFileNotSave(); //应用层不打算将xml结果存为文件，增加该接口，只清理内存，不存文件
	//

	void SetFileVersion(const wchar_t *pVersion);
	void SetInfName(const wchar_t *pFileName);
	void SetProvider(const wchar_t *pProviderName);
	void SetDriverTime(const wchar_t *pTime);
	void SetProductVersion(const wchar_t *pVersion);
	void SetUpdateTime(const wchar_t *pTime);
	void SetGID(const wchar_t *pGid);
	void SetDefault(const wchar_t *pDefualt);
	void SetIcafe(const wchar_t *pIcafe);

	BOOL WritePnpInfo(const wchar_t *pOSAttrVal);
	BOOL WriteDevInfo(const wchar_t *pOSAttrVal, const wchar_t *pVen, const wchar_t *pDev, const wchar_t *pRev);

	//BOOL ReadXml(mapXmlComtext_t &context);

	//以下函数可以单独使用，但再调用之前，必须先openxmlfile
	BOOL SetXmlProductVersion(const wchar_t *pVersion);
	BOOL SetXmlUpdateTime(const wchar_t *pTime);
	BOOL SetXmlGID(const wchar_t *pGid);
	BOOL SetXmlDefault(const wchar_t *pDefualt);
	BOOL SetXmlIcafe(const wchar_t *pIcafe);

private:
	tinyxml2::XMLElement * FindOrInsertPnp(const char *pOsInfo, BOOL bInsert = TRUE);
	tinyxml2::XMLElement * FindOrInsertDev();

	BOOL SetAttr2AllNode(const char *pAttrName, const char *pAttrValue);

	BOOL CheckKeyInXml(const char *pOS, std::string &ven, std::string &dev, std::string &rev);
	BOOL CheckKeyInXml(std::string &ven, std::string &dev, std::string &rev);
	BOOL CheckDevInSubNode(tinyxml2::XMLElement *pParent, std::string &strRest);

private:
	tinyxml2::XMLDocument *m_pDoc;
	tinyxml2::XMLElement *m_pRoot;

	std::string m_strXmlFileName;
	std::string m_strDriverVersion;
	std::string m_strProvider;
	std::string m_strDriverTime;
	std::string m_strInfFileName;
	std::string m_strProductVer;
	std::string m_strUpTime;
	std::string m_strGid;
	std::string m_strDefault;
	std::string m_strIcafe;

	BOOL m_bNewOrOpen;

	static const char *s_pRootName;
	static const char *s_pPnpInfo;
	static const char *s_pOsInfo;
	static const char *s_pBuild;
	static const char *s_pDev;
};
#endif