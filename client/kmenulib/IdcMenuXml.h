#pragma once
#include <vector>
using namespace std;

struct tags
{
	int id;
	CString name;
	vector<int> arGid;
};

struct subclass
{
	int pos;
	CString name;
	CString effect;
	int type;
	int id;
	int order;
	int maxcount;
	CString url;
	vector<int> arGid;
	bool operator < (const subclass& thr) const
	{
		if (pos == thr.pos) return false;
		return (pos < thr.pos);
	}
};
struct leftclass
{
	int pos;
	int id;
	CString name;
	CString ico1;
	CString ico2;
	bool effect;
	bool adbar;
	vector<subclass>  arSubClass;

	bool operator < (const leftclass& thr) const
	{
		if (pos == thr.pos) return false;
		return (pos < thr.pos);
	}
};

struct hotgame
{
	int pos;
	int type;
	int gid;
	CString tag;
	CString effect;
	CString text;
	CString effectid;
	CString tagid;
	bool operator < (const hotgame& thr) const
	{
		if (pos == thr.pos) return false;
		return (pos < thr.pos);
	}
};
//自定义游戏资源
struct customgame
{
	int gid;
	CString fid;
	CString name;
	CString url;
};
struct hotclass
{
	int id;
	vector<hotgame> arHotGame;
};

struct ads
{
	int pos;
	int aid;
	int action;
	int gid;
	CString fid;
	CString text;
	CString textcoloer;
	CString backgroud;
	CString url;

	bool operator < (const ads& thr) const
	{
		if (pos == thr.pos) return false;
		return (pos < thr.pos);
	}
};

struct border
{
	int id;
	int action;
	int odds;
	int gid;
	CString fid;
	CString url;
};

struct slogan
{
	int id;
	int action;
	int odds;
	int gid;
	CString fid;
	CString url;
};


struct hotsearch
{
	int pos;
	int aid;
	int action;
	int type;
	int gid;
	CString name;
	CString url;
	CString icon;
	bool operator < (const hotsearch& thr) const
	{
		if (pos == thr.pos) return false;
		return (pos < thr.pos);
	}
};
struct adRc
{
	int pos;
	int aid;
	int action;
	int gid;
	CString file;
	CString name;
	CString url;
};

struct result
{
	int defSubID;
	vector<adRc> arrAdRc;
	vector<tagGameInfo*> arrGameRc;
	vector<tagGameInfo*> arrGameRcR;
};

struct backgroud
{
	int id;
	CString file;
};

class CIdcMenuXml
{
public:
	CIdcMenuXml(void);
	~CIdcMenuXml(void);
public:
	TCHAR	NameToAlpha(CString name);
	CString AlphaToName(TCHAR alpha);
	bool LoadIdcXml();
	void LoadDefautXml();
	bool			   GetLeftClassById(int id,leftclass& item);
	vector<leftclass>& GetXmlLeftClass();
	vector<hotsearch>& GetHotSearch();
	vector<subclass>&  GetSubClass(int id);
	vector<hotgame>&   GetHotGame(int id);
	customgame&		   GetCustomGame(int id);
	vector<ads>&     GetAdbar();
	vector<tags>&		GetTags()	{ return m_arTags; }

	bool				GetBorderItem(border& item);
	bool				GetSloganItem(slogan& item);
	bool			   GetClassNavBarId(CString className,int& leftid,int& subId);

	int					GetResW() const { return m_resW; }
	int					GetResH() const { return m_resH; }
	CString				GetSearchWord() const { return m_strSearchWord; }
	result&             GetResult()  { return m_result; }
	backgroud&     GetBackgroud() { return m_backgroud; }

private:
	vector<tags>			m_arTags;
	vector<leftclass>			m_arLeftClass;
	vector<ads>					m_arAdbar;
	vector<hotclass>			m_arHotClass;
	vector<hotsearch>			m_arHotSearch;	
	vector<customgame>			m_arCustomGame;
	vector<border>				m_arBorder;
	vector<slogan>				m_arSlogan;
	CString								m_strSearchWord;
	result									m_result;
	backgroud							m_backgroud;
	int							m_resW;
	int							m_resH;
	int							m_gidbrowser;
};
