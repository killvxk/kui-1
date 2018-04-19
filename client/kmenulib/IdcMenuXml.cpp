#include "StdAfx.h"
#include "IdcMenuXml.h"
#include "xml/tinyxml.h"
#include <algorithm>
#include "dbmgr.h"
#include "UI/UIX.h"
#include "resource.h"


static TiXmlElement* FindXmlNode(TiXmlNode* node, const char* szNodeName)
{
	TiXmlNode* c = node->FirstChild();
	while (c)
	{
		if (c->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			TiXmlElement* e = c->ToElement();
			if (strcmp(e->Value(),szNodeName) == 0)
			{
				return e;
			}
		}
		c = c->NextSibling();
	}
	return NULL;
}

static void GetChildXmlElement(TiXmlElement* e, std::vector<TiXmlElement*>& childs)
{
	TiXmlNode* c = e->FirstChild();
	while (c)
	{
		if (c->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			TiXmlElement* e = c->ToElement();
			childs.push_back(e);
		}
		c = c->NextSibling();
	}
}

static std::wstring Utf82Unicode(const std::string& utf8string)
{
	int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{

	}
	if (widesize == 0)
	{

	}

	std::vector<wchar_t> resultstring(widesize + 1);
	resultstring.assign(widesize + 1, 0);

	int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);

	if (convresult != widesize)
	{

	}

	return std::wstring(&resultstring[0]);
}

static CString GetXmlAttribute(TiXmlElement* e, const char* name)
{
	TiXmlAttribute* a = e->FirstAttribute();
	while (a)
	{
		if (strcmp(a->Name(), name) == 0)
		{
			return Utf82Unicode(a->Value()).c_str();
		}
		a = a->Next();
	}
	return CString();
}

static int GetXmlAttributeInt(TiXmlElement* e, const char* name)
{
	TiXmlAttribute* a = e->FirstAttribute();
	while (a)
	{
		if (strcmp(a->Name(), name) == 0)
		{
			return atoi(a->Value());
		}
		a = a->Next();
	}
	return -1;
}

CIdcMenuXml::CIdcMenuXml(void)
{
	m_resW = 1660;
	m_resH = 935;
	m_gidbrowser = 0;
}

CIdcMenuXml::~CIdcMenuXml(void)
{
}
bool CIdcMenuXml::LoadIdcXml()
{
	CServer server;
	char* data = NULL;
	int size = 0;

	AfxWriteLogger(g_Logger, TEXT("download menu start"));
	if (!server.DownloadMenuXML(data, size))
	{
		AfxWriteLogger(g_Logger, TEXT("download menu xml failed"));
		return false;
	}
	AfxWriteLogger(g_Logger, TEXT("download menu success"));
	TCHAR filepath[MAX_PATH];
	AfxGetAppPath(filepath, MAX_PATH, _T("config\\kmenu.dat"));
	WriteFileData(filepath, data, size);

	TiXmlDocument* doc = new TiXmlDocument();

	if (!doc->Parse(data, 0, TIXML_ENCODING_UTF8))
	{
		//ShowMessageBox(NULL, LANG_EX("错误"), LANG_EX("加载XML失败"));

		delete doc;
		free(data);
		data = NULL;
		return false;
	}
	if(data)
	{
		free(data);
		data = NULL;
	}

	TiXmlElement* doc_element = FindXmlNode(doc, "doc");
	TiXmlElement* menu_element = NULL;
	if (doc_element == NULL)
	{
		menu_element = FindXmlNode(doc, "kmenu");
	}
	else
	{
		menu_element = FindXmlNode(doc_element, "kmenu");
	}

	if (menu_element == NULL)
	{
		delete doc;
		return false;
	}

	m_arTags.clear();
	TiXmlElement* tags_element = FindXmlNode(menu_element, "Tags");
	if (tags_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(tags_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e = childs[i];
			tags item;
			item.id = GetXmlAttributeInt(e, "id");
			item.name = GetXmlAttribute(e, "name");

			std::vector<TiXmlElement*>	childs2;
			GetChildXmlElement(e, childs2);
			for (int j = 0; j < (int)childs2.size(); ++j)
			{
				TiXmlElement* e2 = childs2[j];
				int gid = GetXmlAttributeInt(e2, "gid");
				item.arGid.push_back(gid);
			}

			m_arTags.push_back(item);
		}
	}
	m_arLeftClass.clear();
	TiXmlElement* navibar_element = FindXmlNode(menu_element, "navibar");
	if (navibar_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(navibar_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e = childs[i];
			leftclass item;
			item.pos = GetXmlAttributeInt(e, "pos");
			item.id = GetXmlAttributeInt(e, "id");
			item.name = GetXmlAttribute(e, "name");
			item.ico1 = GetXmlAttribute(e, "ico1");
			item.ico2 = GetXmlAttribute(e, "ico2");
			item.adbar = (GetXmlAttributeInt(e, "adbar") == 1);
			std::vector<TiXmlElement*>	childs2;
			GetChildXmlElement(e, childs2);
			for (int j = 0; j < (int)childs2.size(); ++j)
			{
				TiXmlElement* e2 = childs2[j];
				subclass item2;
				item2.pos = GetXmlAttributeInt(e2, "pos");
				item2.id = GetXmlAttributeInt(e2, "id");
				item2.name = GetXmlAttribute(e2, "name");
				item2.order = GetXmlAttributeInt(e2, "order");
				item2.maxcount = GetXmlAttributeInt(e2, "maxcount");
				item2.effect = GetXmlAttribute(e2, "effect");
				item2.type = GetXmlAttributeInt(e2, "type");
				item2.url = GetXmlAttribute(e2, "url");

				for (size_t j = 0; j < m_arTags.size(); j++)
				{
					if (item2.id == m_arTags[j].id)
					{
						for (size_t k = 0; k < m_arTags[j].arGid.size(); k++)
						{
							item2.arGid.push_back(m_arTags[j].arGid[k]);
						}
					}
				}

				if (item2.type == 2)
				{
					CString strTags = GetXmlAttribute(e2, "tags");
					tstring strSouce = strTags.GetBuffer();
					vector<tstring> arTags;
					AfxSplitString(strSouce, arTags, _T(','));
					for (size_t i = 0; i < arTags.size(); i++)
					{
						for (size_t j = 0; j < m_arTags.size(); j++)
						{
							if (_ttoi(arTags[i].c_str()) == m_arTags[j].id)
							{
								for (size_t k = 0; k < m_arTags[j].arGid.size(); k++)
								{
									item2.arGid.push_back(m_arTags[j].arGid[k]);
								}
							}
						}
					}
				}

				item.arSubClass.push_back(item2);
			}

			std::sort(item.arSubClass.begin(), item.arSubClass.end());
			m_arLeftClass.push_back(item);
		}
		std::sort(m_arLeftClass.begin(), m_arLeftClass.end());
	}

	m_arAdbar.clear();
	TiXmlElement* adbar_element = FindXmlNode(menu_element, "adbar");
	if (adbar_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(adbar_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e = childs[i];

			ads item;
			item.pos = GetXmlAttributeInt(e, "pos");
			item.aid = GetXmlAttributeInt(e, "aid");
			item.action = GetXmlAttributeInt(e, "action");;
			item.textcoloer = GetXmlAttribute(e, "textcoloer");
			item.backgroud = GetXmlAttribute(e, "backgroud");
			item.gid = GetXmlAttributeInt(e, "gid");
			item.text = GetXmlAttribute(e, "text");
			item.fid = GetXmlAttribute(e, "file");
			item.url = GetXmlAttribute(e, "url");

			m_arAdbar.push_back(item);
		}
		std::sort(m_arAdbar.begin(), m_arAdbar.end());
	}

	m_arHotClass.clear();
	TiXmlElement* hotgame_element = FindXmlNode(menu_element, "gameicon");
	if (hotgame_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(hotgame_element, childs);

		RECT rt;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);

		int sw = rt.right - rt.left;

		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e = childs[i];

			if (sw < GetXmlAttributeInt(e, "desktop") && i >= 1 || i + 1 >= (int)childs.size())
			{
				if (sw < GetXmlAttributeInt(e, "desktop") && i >= 1)
					e = childs[i - 1];
				CString menu = GetXmlAttribute(e, "menu");

				CString w, h;

				AfxExtractSubString(w, menu, 0, '*');
				AfxExtractSubString(h, menu, 1, '*');

				m_resW = _ttoi(w);
				m_resH = _ttoi(h);

				std::vector<TiXmlElement*>	childs2;
				GetChildXmlElement(e, childs2);
				for (int j = 0; j < (int)childs2.size(); ++j)
				{
					hotclass item;
					TiXmlElement* e2 = childs2[j];

					item.id = GetXmlAttributeInt(e2, "id");

					std::vector<TiXmlElement*>	childs3;
					GetChildXmlElement(e2, childs3);
					for (int k = 0; k < (int)childs3.size(); ++k)
					{
						TiXmlElement* e3 = childs3[k];

						hotgame item2;
						item2.pos = GetXmlAttributeInt(e3, "pos");
						item2.type = GetXmlAttributeInt(e3, "type");
						item2.gid = GetXmlAttributeInt(e3, "gid");
						item2.tag = GetXmlAttribute(e3, "tag");
						item2.effect = GetXmlAttribute(e3, "effect");
						item2.text = GetXmlAttribute(e3, "text");
						item2.tagid = GetXmlAttribute(e3, "tagid");
						item2.effectid = GetXmlAttribute(e3, "effectid");

						// XML中配置的游戏资源“41949穿越火线”，若不是本地资源，且type=0是内置资源，则不应在菜单显示；
						tagGameInfo* pGame = AfxGetDbMgr()->FindGameInfo(item2.gid);
						if (item2.type == 0 && pGame == NULL)
							continue;

						item.arHotGame.push_back(item2);
					}

					m_arHotClass.push_back(item);
				}
				break;
			}
		}
	}

	m_arBorder.clear();
	TiXmlElement* border_element = FindXmlNode(menu_element, "border");
	if (border_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(border_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e = childs[i];
			border item;

			item.id = GetXmlAttributeInt(e, "aid");
			item.odds = GetXmlAttributeInt(e, "display");
			item.fid = GetXmlAttribute(e, "file");
			item.action = GetXmlAttributeInt(e, "action");
			item.gid = GetXmlAttributeInt(e, "gid");
			item.url = GetXmlAttribute(e, "url");
			m_arBorder.push_back(item);
		}
	}

	m_arSlogan.clear();
	TiXmlElement* slogan_element = FindXmlNode(menu_element, "slogan");
	if (slogan_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(slogan_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			slogan item;
			TiXmlElement* e2 = childs[i];

			item.id = GetXmlAttributeInt(e2, "id");
			item.odds = GetXmlAttributeInt(e2, "display");
			item.fid = GetXmlAttribute(e2, "file");
			item.action = GetXmlAttributeInt(e2, "action");
			item.gid = GetXmlAttributeInt(e2, "gid");
			item.url = GetXmlAttribute(e2, "url");

			m_arSlogan.push_back(item);
		}
	}

	TiXmlElement* hotsearch_element = FindXmlNode(menu_element, "search");
	if (hotsearch_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(hotsearch_element, childs);
		TiXmlElement* e = childs[0];
		m_strSearchWord = GetXmlAttribute(e, "text");

		m_arHotSearch.clear();
		std::vector<TiXmlElement*> childs1;
		GetChildXmlElement(e, childs1);
		for (int i = 0; i < (int)childs1.size(); ++i)
		{
			TiXmlElement* e = childs1[i];
			hotsearch item;
			item.pos = GetXmlAttributeInt(e, "pos");
			item.aid = GetXmlAttributeInt(e, "aid");
			item.action = GetXmlAttributeInt(e, "action");
			item.gid = GetXmlAttributeInt(e, "gid");
			item.name = GetXmlAttribute(e, "name");
			item.icon = GetXmlAttribute(e, "icon");
			item.url = GetXmlAttribute(e, "url");
			m_arHotSearch.push_back(item);
		}
		std::sort(m_arHotSearch.begin(), m_arHotSearch.end());

		e = childs[1];
		std::vector<TiXmlElement*> childs2;
		GetChildXmlElement(e, childs2);
		e = childs2[0];

		m_result.defSubID = GetXmlAttributeInt(e, "subclass");
		std::vector<TiXmlElement*> childs3;
		GetChildXmlElement(e, childs3);

		m_result.arrAdRc.clear();
		for (int i = 0; i < (int)childs3.size(); ++i)
		{
			TiXmlElement* e = childs3[i];
			adRc item;

			item.pos = GetXmlAttributeInt(e, "pos");
			item.aid = GetXmlAttributeInt(e, "aid");
			item.gid = GetXmlAttributeInt(e, "gid");
			item.action = GetXmlAttributeInt(e, "action");
			item.file = GetXmlAttribute(e, "file");
			item.url = GetXmlAttribute(e, "url");

			m_result.arrAdRc.push_back(item);
		}


		e = childs2[1];
		childs3.clear();
		GetChildXmlElement(e, childs3);
		m_result.arrGameRc.clear();
		m_result.arrGameRcR.clear();

		for (int i = 0; i < (int)childs3.size(); ++i)
		{
			TiXmlElement* e = childs3[i];

			int type = GetXmlAttributeInt(e, "type");
			int gid = GetXmlAttributeInt(e, "gid");

			std::map<DWORD, tagGameInfo*>& games = AfxGetDbMgr()->m_mapGameInfo;
			std::map<DWORD, tagGameInfo*>::iterator iter = games.begin();
			for (; iter != games.end(); ++iter)
			{
				tagGameInfo* pGame = iter->second;

				if (pGame->gid == gid)
				{
					if (type == 0)
						m_result.arrGameRc.push_back(pGame);
					else
						m_result.arrGameRcR.push_back(pGame);
				}
			}

		}
	}

	m_arCustomGame.clear();
	TiXmlElement* customgame_element = FindXmlNode(menu_element, "customgame");
	if (customgame_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(customgame_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e = childs[i];
			customgame item;
			item.gid = GetXmlAttributeInt(e, "gid");
			item.name = GetXmlAttribute(e, "name");
			item.url = GetXmlAttribute(e, "url");
			m_arCustomGame.push_back(item);
		}
	}

	TiXmlElement* browser_element = FindXmlNode(menu_element, "browser");
	if (browser_element)
		m_gidbrowser = GetXmlAttributeInt(browser_element, "gid");


	TiXmlElement* back_element = FindXmlNode(menu_element, "backgroud");
	if (back_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(back_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e2 = childs[i];

			m_backgroud.id = GetXmlAttributeInt(e2, "id");
			m_backgroud.file = GetXmlAttribute(e2, "file");
		}


	}
	delete doc;
	doc = NULL;

	return true;
}
void CIdcMenuXml::LoadDefautXml()
{
	TiXmlDocument* doc = new TiXmlDocument();
	bool is_load = false;
	while (true)
	{
		HRSRC hRes = FindResource(GetModuleHandle(NULL), _T("MENU"), _T("FILE"));
		if (hRes == NULL)
			return;

		HGLOBAL hData = LoadResource(GetModuleHandle(NULL), hRes);
		if (hData == NULL)
			break;

		DWORD size = SizeofResource(GetModuleHandle(NULL), hRes);
		void* p = LockResource(hData);

		char* data = (char*)malloc(size + 1);
		memcpy(data, p, size);
		data[size] = 0;

		bool ret = doc->Parse(data, 0, TIXML_ENCODING_UTF8) != NULL;
		free(data);
		FreeResource(hData);
		if (!ret)
			break;
		is_load = true;
		break;
	}
	if (!is_load)
		return;

	TiXmlElement* menu_element = FindXmlNode(doc, "kmenu");
	if (menu_element == NULL)
	{
		delete doc;
		return;
	}
	m_arTags.clear();
	TiXmlElement* tags_element = FindXmlNode(menu_element, "Tags");
	if (tags_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(tags_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e = childs[i];
			tags item;
			item.id = GetXmlAttributeInt(e, "id");
			item.name = GetXmlAttribute(e, "name");

			std::vector<TiXmlElement*>	childs2;
			GetChildXmlElement(e, childs2);
			for (int j = 0; j < (int)childs2.size(); ++j)
			{
				TiXmlElement* e2 = childs2[j];
				int gid = GetXmlAttributeInt(e2, "gid");
				item.arGid.push_back(gid);
			}

			m_arTags.push_back(item);
		}
	}
	m_arLeftClass.clear();
	TiXmlElement* navibar_element = FindXmlNode(menu_element, "navibar");
	if (navibar_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(navibar_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e = childs[i];
			leftclass item;
			item.pos = GetXmlAttributeInt(e,"pos");
			item.id = GetXmlAttributeInt(e,"id");
			item.name = GetXmlAttribute(e,"name");
			item.ico1 = GetXmlAttribute(e, "ico1");
			item.ico2 = GetXmlAttribute(e, "ico2");
			item.adbar = (GetXmlAttributeInt(e, "adbar") == 1);
			std::vector<TiXmlElement*>	childs2;
			GetChildXmlElement(e, childs2);
			for (int j = 0; j < (int)childs2.size(); ++j)
			{
				TiXmlElement* e2 = childs2[j];
				subclass item2;
				item2.pos = GetXmlAttributeInt(e2, "pos");
				item2.id = GetXmlAttributeInt(e2, "id");
				item2.name = GetXmlAttribute(e2, "name");
				item2.order = GetXmlAttributeInt(e2, "order");
				item2.maxcount = GetXmlAttributeInt(e2, "maxcount");
				item2.effect = GetXmlAttribute(e2, "effect");
				item2.type = GetXmlAttributeInt(e2, "type");
				item2.url = GetXmlAttribute(e2, "url");

				for (size_t j = 0; j < m_arTags.size(); j++)
				{
					if (item.id == m_arTags[j].id)
					{
						for (size_t k = 0; k < m_arTags[j].arGid.size(); k++)
						{
							item2.arGid.push_back(m_arTags[j].arGid[k]);
						}
					}
				}

				if (item2.type == 2)
				{
					CString strTags = GetXmlAttribute(e2, "tags");
					tstring strSouce = strTags.GetBuffer();
					vector<tstring> arTags;
					AfxSplitString(strSouce, arTags, _T(','));
					for (size_t i = 0; i < arTags.size(); i++)
					{
						for (size_t j = 0; j < m_arTags.size(); j++)
						{
							if (_ttoi(arTags[i].c_str()) == m_arTags[j].id)
							{
								for (size_t k = 0; k < m_arTags[j].arGid.size(); k++)
								{
									item2.arGid.push_back(m_arTags[j].arGid[k]);
								}
							}
						}
					}
				}

				item.arSubClass.push_back(item2);
			}

			std::sort(item.arSubClass.begin(), item.arSubClass.end());
			m_arLeftClass.push_back(item);
		}
		std::sort(m_arLeftClass.begin(), m_arLeftClass.end());
	}

	m_arAdbar.clear();
	TiXmlElement* adbar_element = FindXmlNode(menu_element, "adbar");
	if (adbar_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(adbar_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e = childs[i];

			ads item;
			item.pos = GetXmlAttributeInt(e, "pos");
			item.aid = GetXmlAttributeInt(e, "aid");
			item.action = GetXmlAttributeInt(e, "action");;
			item.textcoloer = GetXmlAttribute(e, "textcoloer");
			item.backgroud = GetXmlAttribute(e, "backgroud");
			item.gid = GetXmlAttributeInt(e, "gid");
			item.text = GetXmlAttribute(e, "text");
			item.fid = GetXmlAttribute(e, "file");
			item.url = GetXmlAttribute(e, "url");
		
			m_arAdbar.push_back(item);
		}
		std::sort(m_arAdbar.begin(), m_arAdbar.end());
	}

	m_arHotClass.clear();
	TiXmlElement* hotgame_element = FindXmlNode(menu_element, "gameicon");
	if (hotgame_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(hotgame_element, childs);

		RECT rt;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);

		int sw = rt.right - rt.left;

		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e = childs[i];

			if (sw <= GetXmlAttributeInt(e, "desktop") && i >= 1 || i + 1 >= (int)childs.size())
			{
				if (sw <= GetXmlAttributeInt(e, "desktop") && i >= 1)
					e = childs[i - 1];
				CString menu = GetXmlAttribute(e, "menu");

				CString w, h;

				AfxExtractSubString(w, menu, 0, '*');
				AfxExtractSubString(h, menu, 1, '*');

				m_resW = _ttoi(w);
				m_resH = _ttoi(h);

				std::vector<TiXmlElement*>	childs2;
				GetChildXmlElement(e, childs2);
				for (int j = 0; j < (int)childs2.size(); ++j)
				{
					hotclass item;
					TiXmlElement* e2 = childs2[j];

					item.id = GetXmlAttributeInt(e2, "id");

					std::vector<TiXmlElement*>	childs3;
					GetChildXmlElement(e2, childs3);
					for (int k = 0; k < (int)childs3.size(); ++k)
					{
						TiXmlElement* e3 = childs3[k];

						hotgame item2;
						item2.pos = GetXmlAttributeInt(e3, "pos");
						item2.type = GetXmlAttributeInt(e3, "type");
						item2.gid = GetXmlAttributeInt(e3, "gid");
						item2.tag = GetXmlAttribute(e3, "tag");
						item2.effect = GetXmlAttribute(e3, "effect");
						item2.text = GetXmlAttribute(e3, "text");
						item2.tagid = GetXmlAttribute(e3, "tagid");
						item2.effectid = GetXmlAttribute(e3, "effectid");

						// XML中配置的游戏资源“41949穿越火线”，若不是本地资源，且type=0是内置资源，则不应在菜单显示；
						tagGameInfo* pGame = AfxGetDbMgr()->FindGameInfo(item2.gid);
						if (item2.type == 0 && pGame == NULL)
							continue;

						item.arHotGame.push_back(item2);
					}

					m_arHotClass.push_back(item);
				}
				break;
			}
		}
	}

	m_arBorder.clear();
	TiXmlElement* border_element = FindXmlNode(menu_element, "border");
	if (border_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(border_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{			
			border item;
			TiXmlElement* e = childs[i];
#if 0
			std::vector<TiXmlElement*>	childs2;
			GetChildXmlElement(e, childs2);
			for (int j = 0; j < (int)childs2.size(); ++j)
			{
				border item;
				TiXmlElement* e2 = childs2[j];
#endif
				item.id = GetXmlAttributeInt(e, "id");
				item.odds = GetXmlAttributeInt(e, "display");
				item.fid = GetXmlAttribute(e, "file");
				item.action = GetXmlAttributeInt(e, "action");
				item.gid = GetXmlAttributeInt(e, "gid");
				item.url = GetXmlAttribute(e, "url");

				m_arBorder.push_back(item);
			//}
		}
	}

	m_arSlogan.clear();
	TiXmlElement* slogan_element = FindXmlNode(menu_element, "slogan");
	if (slogan_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(slogan_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			slogan item;
			TiXmlElement* e2 = childs[i];

			item.id = GetXmlAttributeInt(e2, "id");
			item.odds = GetXmlAttributeInt(e2, "display");
			item.fid = GetXmlAttribute(e2, "file");
			item.action = GetXmlAttributeInt(e2, "action");
			item.gid = GetXmlAttributeInt(e2, "gid");
			item.url = GetXmlAttribute(e2, "url");

			m_arSlogan.push_back(item);
		}
	}

	TiXmlElement* hotsearch_element = FindXmlNode(menu_element, "search");
	if (hotsearch_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(hotsearch_element, childs);
		TiXmlElement* e = childs[0];
		m_strSearchWord = GetXmlAttribute(e, "text");

		m_arHotSearch.clear();
		std::vector<TiXmlElement*> childs1;
		GetChildXmlElement(e, childs1);
		for (int i = 0; i < (int)childs1.size(); ++i)
		{
			TiXmlElement* e = childs1[i];
			hotsearch item;
			item.pos = GetXmlAttributeInt(e,"pos");
			item.aid = GetXmlAttributeInt(e,"aid");
			item.action = GetXmlAttributeInt(e, "action");
			item.gid = GetXmlAttributeInt(e,"gid");
			item.name = GetXmlAttribute(e,"name");
			item.icon = GetXmlAttribute(e, "icon");
			item.url = GetXmlAttribute(e, "url");
			m_arHotSearch.push_back(item);
		}

		e = childs[1];
		std::vector<TiXmlElement*> childs2;
		GetChildXmlElement(e, childs2);		
		e = childs2[0];

		m_result.defSubID = GetXmlAttributeInt(e, "subclass");
		std::vector<TiXmlElement*> childs3;
		GetChildXmlElement(e, childs3);

		m_result.arrAdRc.clear();
		for (int i = 0; i < (int)childs3.size(); ++i)
		{
			TiXmlElement* e = childs3[i];
			adRc item;

			item.pos = GetXmlAttributeInt(e, "pos");
			item.aid = GetXmlAttributeInt(e, "aid");
			item.gid = GetXmlAttributeInt(e, "gid");
			item.action = GetXmlAttributeInt(e, "action");
			item.file = GetXmlAttribute(e, "file");
			item.url = GetXmlAttribute(e, "url");

			m_result.arrAdRc.push_back(item);
		}

		
		e = childs2[1];
		childs3.clear();
		GetChildXmlElement(e, childs3);
		m_result.arrGameRc.clear();
		m_result.arrGameRcR.clear();

		for (int i = 0; i < (int)childs3.size(); ++i)
		{
			TiXmlElement* e = childs3[i];

			int type = GetXmlAttributeInt(e, "type");
			int gid = GetXmlAttributeInt(e, "gid" );

			std::map<DWORD, tagGameInfo*>& games = AfxGetDbMgr()->m_mapGameInfo;
			std::map<DWORD, tagGameInfo*>::iterator iter = games.begin();
			for (; iter != games.end(); ++iter)
			{
				tagGameInfo* pGame = iter->second;

				if (pGame->gid == gid)
				{
					if (type == 0)
						m_result.arrGameRc.push_back(pGame);
					else
						m_result.arrGameRcR.push_back(pGame);
				}
			}

		}
	}

	m_arCustomGame.clear();
	TiXmlElement* customgame_element = FindXmlNode(menu_element, "customgame");
	if (customgame_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(customgame_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{
			TiXmlElement* e = childs[i];
			customgame item;
			item.gid = GetXmlAttributeInt(e,"gid");
			item.name = GetXmlAttribute(e,"name");
			item.url = GetXmlAttribute(e,"url");
			m_arCustomGame.push_back(item);
		}
	}

	TiXmlElement* browser_element = FindXmlNode(menu_element, "browser");
	if (browser_element)
		m_gidbrowser = GetXmlAttributeInt(browser_element,"gid");


	TiXmlElement* back_element = FindXmlNode(menu_element, "backgroud");
	if (back_element)
	{
		std::vector<TiXmlElement*> childs;
		GetChildXmlElement(back_element, childs);
		for (int i = 0; i < (int)childs.size(); ++i)
		{	
			TiXmlElement* e2 = childs[i];

			m_backgroud.id = GetXmlAttributeInt(e2, "id");
			m_backgroud.file = GetXmlAttribute(e2, "file");
		}

		
	}

	
	delete doc;
	doc = NULL;
}
bool CIdcMenuXml::GetLeftClassById(int id,leftclass& item)
{
	for (size_t i=0;i<m_arLeftClass.size();i++)
	{
		if(m_arLeftClass[i].id == id)
		{
			item = m_arLeftClass[i];
			return true;
		}
	}
	return false;
}
vector<leftclass>& CIdcMenuXml::GetXmlLeftClass()
{
	return m_arLeftClass;
}

vector<hotsearch>& CIdcMenuXml::GetHotSearch()
{
	return m_arHotSearch;
}

vector<subclass>& CIdcMenuXml::GetSubClass( int id )
{
	static vector<subclass> ar;
	for (size_t i=0;i<m_arLeftClass.size();i++)
	{
		if(m_arLeftClass[i].id == id)
			return m_arLeftClass[i].arSubClass;
	}
	return ar;
}


vector<hotgame>& CIdcMenuXml::GetHotGame( int id )
{
	static vector<hotgame> temp;

	for(size_t i=0;i<m_arHotClass.size();i++)
	{
		if(m_arHotClass[i].id == id)
			return m_arHotClass[i].arHotGame;
	}
	return temp;
}

TCHAR CIdcMenuXml::NameToAlpha(CString name)
{
	TCHAR szTest[MAX_PATH] = { 0 };

	::LoadString(::GetModuleHandle(NULL), IDS_NET_GAME, szTest, MAX_PATH);

	if (name.CompareNoCase(szTest) == 0)
		return _T('a');

	::LoadString(::GetModuleHandle(NULL), IDS_PC_GAME, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('b');

	::LoadString(::GetModuleHandle(NULL), IDS_CARD_GAME, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('c');

	::LoadString(::GetModuleHandle(NULL), IDS_BATTLE, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('d');

	::LoadString(::GetModuleHandle(NULL), IDS_IM, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('e');

	::LoadString(::GetModuleHandle(NULL), IDS_WEB_GAME, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('f');

	::LoadString(::GetModuleHandle(NULL), IDS_CASUAL_GAME, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('g');

	::LoadString(::GetModuleHandle(NULL), IDS_ASSIST, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('h');

	::LoadString(::GetModuleHandle(NULL), IDS_COMMON, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('i');

	::LoadString(::GetModuleHandle(NULL), IDS_VIDEO_TOOL, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('j');

	::LoadString(::GetModuleHandle(NULL), IDS_STOCK, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('k');

	::LoadString(::GetModuleHandle(NULL), IDS_HD_VIDEO, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('l');

	::LoadString(::GetModuleHandle(NULL), IDS_MUSIC, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('m');

	::LoadString(::GetModuleHandle(NULL), IDS_LIVE, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('n');

	::LoadString(::GetModuleHandle(NULL), IDS_MOBILE_GAME, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('o');

	::LoadString(::GetModuleHandle(NULL), IDS_OFFICE, szTest, MAX_PATH);
	if (name.CompareNoCase(szTest) == 0)
		return _T('p');

	return _T('a');
}

CString CIdcMenuXml::AlphaToName( TCHAR alpha )
{
	CString str;
	TCHAR szTest[MAX_PATH] = { 0 };

	switch(alpha)
	{
	case _T('a'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_NET_GAME, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('b'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_PC_GAME, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('c'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_CARD_GAME, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('d'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_BATTLE, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('e'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_IM, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('f'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_WEB_GAME, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('g'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_CASUAL_GAME, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('h'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_ASSIST, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('i'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_COMMON, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('j'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_VIDEO_TOOL, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('k'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_STOCK, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('l'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_HD_VIDEO, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('m'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_MUSIC, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('n'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_LIVE, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('o'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_MOBILE_GAME, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	case _T('p'):
	{
		::LoadString(::GetModuleHandle(NULL), IDS_OFFICE, szTest, MAX_PATH);
		str = szTest;
	}
		break;
	default:
		break;
	}
	return str;
}

customgame& CIdcMenuXml::GetCustomGame( int id )
{
	static customgame tem;

	for(size_t i=0;i<m_arCustomGame.size();i++)
	{
		if(m_arCustomGame[i].gid == id)
			return m_arCustomGame[i];
	}

	return tem;
}

vector<ads>& CIdcMenuXml::GetAdbar()
{
	return m_arAdbar;
}


bool CIdcMenuXml::GetBorderItem(border& item)
{
	bool bResult = false;
	vector<int> aryid;
	vector<int> aryOdds;
	int count = 0;

	int id = 0;

	for (size_t i = 0; i < m_arBorder.size(); i++)
	{
		aryid.push_back(m_arBorder[i].id);
		count += m_arBorder[i].odds;
		aryOdds.push_back(count);
	}
	if (aryid.size() > 0)
	{
		srand(GetTickCount());
		int nIndex = rand() % aryOdds[aryOdds.size() - 1];
		for (size_t i = 0; i < aryid.size(); i++)
		{
			if (nIndex < aryOdds[i])
			{
				id = aryid[i];
				bResult = true;
				break;
			}
		}

		if (bResult)
		{
			for (size_t i = 0; i < m_arBorder.size(); i++)
			{
				if (m_arBorder[i].id == id)
				{
					item = m_arBorder[i];
					break;
				}
			}
		}
		
	}

	return bResult;
}

bool CIdcMenuXml::GetSloganItem(slogan& item)
{
	bool bResult = false;
	vector<int> aryid;
	vector<int> aryOdds;
	int count = 0;

	int id = 0;

	for (size_t i = 0; i < m_arSlogan.size(); i++)
	{
		aryid.push_back(m_arSlogan[i].id);
		count += m_arSlogan[i].odds;
		aryOdds.push_back(count);
	}
	if (aryid.size() > 0)
	{
		srand(GetTickCount());
		int nIndex = rand() % aryOdds[aryOdds.size() - 1];
		for (size_t i = 0; i < aryid.size(); i++)
		{
			if (nIndex < aryOdds[i])
			{
				id = aryid[i];
				bResult = true;
				break;
			}
		}

		if (bResult)
		{
			for (size_t i = 0; i < m_arSlogan.size(); i++)
			{
				if (m_arSlogan[i].id == id)
				{
					item = m_arSlogan[i];
					break;
				}
			}
		}

	}

	return bResult;
}


bool CIdcMenuXml::GetClassNavBarId(CString className,int& leftid,int& subId )
{
#if 0
	TCHAR alpha = NameToAlpha(className);
	if(alpha == 0)
		return false;
#endif

	for (size_t i=0;i<m_arLeftClass.size();i++)
	{
		if (m_arLeftClass[i].arSubClass.size() < 1)
		{
			if (m_arLeftClass[i].name.CompareNoCase(className) == 0)
			{
				leftid = m_arLeftClass[i].id;
				return true;
			}
		}
		for(size_t j=0;j<m_arLeftClass[i].arSubClass.size();j++)
		{
			if (m_arLeftClass[i].arSubClass[j].name.CompareNoCase(className) == 0)
			{
				leftid = m_arLeftClass[i].id;
				subId = m_arLeftClass[i].arSubClass[j].id;
				return true;
			}
		}
	}
	return false;
}
