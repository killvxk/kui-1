#ifndef __db_mgr_inc__
#define __db_mgr_inc__

#include <comutil.h>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <GdiPlus.h>
#include <locale.h>

#define START_NAMESPACE_KMENU		namespace kmenu {
#define END_NAMESPACE_KMENU			}
#define USE_NAMESPACE_KMENU			using namespace kmenu;

extern HANDLE g_Logger;
#ifdef _UNICODE
#define tstring std::wstring
#define tstringstream  std::wstringstream
#else
#define tstring std::string
#define tstringstream  std::stringstream
#endif

static int WM_EXEC_START_PARAM		= RegisterWindowMessage(TEXT("_pkg_exec_start_param"));
#define WriteLog(format, ...) AfxWriteLogger(g_Logger, format, ## __VA_ARGS__)



CString SKING_DEF_NAME[];

#define COUNT_SKIN			6
#define IDC_CLOSE			(1000+11)
#define IDC_OK				(1000+12)
#define IDC_CANCEL			(1000+13)

typedef struct tagIconDir {
	WORD          idReserved;
	WORD          idType;
	WORD          idCount;
} ICONHEADER;

typedef struct tagIconDirectoryEntry {
	BYTE  bWidth;
	BYTE  bHeight;
	BYTE  bColorCount;
	BYTE  bReserved;
	WORD  wPlanes;
	WORD  wBitCount;
	DWORD dwBytesInRes;
	DWORD dwImageOffset;
} ICONDIRENTRY;

class ReadBuffer
{
public:
	ReadBuffer()
	{
		m_data = NULL;
		m_size = 0;
		m_pos = 0;
	}

	ReadBuffer(unsigned int size)
	{
		if (size == 0)
		{
			m_data = NULL;
			m_size = 0;
		}
		else
		{
			m_data = (unsigned char*)malloc(size);
			m_size = size;
		}
		m_pos = 0;
	}

	ReadBuffer(void* data, unsigned long size, bool is_copy = true)
	{
		if (is_copy)
		{
			m_size = size;
			m_data = (unsigned char*)malloc(m_size);
			memcpy(m_data, data, m_size);
		}
		else
		{
			m_data = (unsigned char*)data;
			m_size = size;
		}
		m_pos = 0;
	}

	virtual ~ReadBuffer()
	{
		if (m_data)
		{
			free(m_data);
			m_data = NULL;
		}
	}

	unsigned char* data()
	{
		return m_data;
	}

	unsigned int size()
	{
		return m_size;
	}

	unsigned char* offset(unsigned int offset)
	{
		if (offset > m_size || offset < 0 || m_data == NULL)
		{
			return NULL;
		}
		return m_data + offset;
	}

	void set_zero()
	{
		if (m_data)
		{
			memset(m_data, 0, m_size);
		}
	}

	void assign(void* data, unsigned int size)
	{
		if (m_size > 0)
		{
			if (m_size >= size)
			{
				memcpy(m_data, data, size);
				m_size = size;
				return;
			}
			else
			{
				free(m_data);
				m_data = NULL;
				m_size = 0;
			}
		}
		m_size = size;
		m_data = (unsigned char*)malloc(m_size);
		memcpy(m_data, data, m_size);
	}

	void attach(void* data, unsigned int size)
	{
		if (m_size > 0)
		{
			if (data != m_data)
			{
				free(m_data);
				m_data = NULL;
				m_size = 0;
			}
		}
		m_size = size;
		m_data = (unsigned char*)data;
	}

	bool set_size(unsigned int size)
	{
		if (m_size == size)
		{
			return true;
		}

		if (size == 0)
		{
			if (m_data)
			{
				free(m_data);
				m_data = NULL;
			}
			m_size = 0;
			return true;
		}
		else if (size > m_size)
		{
			m_data = (unsigned char*)realloc(m_data, size);
			memset(m_data + m_size, 0, size - m_size);
			m_size = size;
			if (m_data == NULL)
			{
				m_size = 0;
			}
		}
		else if (m_size > 0)
		{
			m_size = size;
		}
		return m_data != NULL;
	}

	void raw_set_size(unsigned int size)
	{
		m_size = size;
	}

	bool set_pos(unsigned int pos)
	{
		if (pos >= 0 && pos <= m_size)
		{
			m_pos = pos;
			return true;
		}
		else
		{
			if (pos < 0) pos = 0;
			if (pos > m_size) pos = m_size;
			m_pos = pos;
			return false;
		}
	}

	unsigned int pos()
	{
		return m_pos;
	}

	bool read_byte(unsigned char& value)
	{
		if (m_pos + 1 <= m_size)
		{
			value = *((unsigned char*)(m_data + m_pos));
			m_pos += 1;
			return true;
		}
		return false;
	}

	bool read_int(int& value)
	{
		if (m_pos + 4 <= m_size)
		{
			value = *((int*)(m_data + m_pos));
			m_pos += 4;
			return true;
		}
		return false;
	}

	bool read_uint(unsigned int& value)
	{
		if (m_pos + 4 <= m_size)
		{
			value = *((unsigned int*)(m_data + m_pos));
			m_pos += 4;
			return true;
		}
		return false;
	}

	bool read_ushort(unsigned short& value)
	{
		if (m_pos + 2 <= m_size)
		{
			value = *((unsigned short*)(m_data + m_pos));
			m_pos += 2;
			return true;
		}
		return false;
	}

	bool read_short(short& value)
	{
		if (m_pos + 2 <= m_size)
		{
			value = *((short*)(m_data + m_pos));
			m_pos += 2;
			return true;
		}
		return false;
	}

	bool read_float(float& value)
	{
		if (m_pos + 4 <= m_size)
		{
			value = *((float*)(m_data + m_pos));
			m_pos += 4;
			return true;
		}
		return false;
	}

	bool read_long_long(long long& value)
	{
		if (m_pos + 8 <= m_size)
		{
			value = *((long long*)(m_data + m_pos));
			m_pos += 8;
			return true;
		}
		return false;
	}

	bool read_string(char* value, int len)
	{
		if (m_pos + len <= m_size)
		{
			memcpy(value, m_data + m_pos, len);
			value[len] = '\0';
			m_pos += len;
			return true;
		}
		return false;
	}

	bool read_string(std::string& value, int len)
	{
		if (m_pos + len <= m_size)
		{
			value.assign((const char*)(m_data + m_pos), len);
			m_pos += len;
			return true;
		}
		return false;
	}

	bool read_bytes(unsigned char* value, int len)
	{
		if (m_pos + len <= m_size)
		{
			memcpy(value, m_data + m_pos, len);
			//value[len] = '\0';
			m_pos += len;
			return true;
		}
		return false;
	}

	void clear()
	{
		if (m_data)
		{
			::free(m_data);
			m_data = NULL;
			m_size = 0;
			m_pos = 0;
		}
	}

	void detach()
	{
		m_data = NULL;
		m_size = 0;
		m_pos = 0;
	}

public:

	unsigned char*	m_data;
	unsigned long	m_size;
	unsigned int	m_pos;
};

typedef struct tagSTMultiChar
{
	WCHAR chChar;
	TCHAR szPy[3];
}STMultiChar;

CString GetPY(CString& sChinese);
LPTSTR AfxGetAppPath(LPTSTR szPath, DWORD dwSize = MAX_PATH, LPCTSTR szAppend = NULL);
HANDLE AfxCreateLogger(LPCTSTR szModuleName);


//return is 64bit operation system.
bool AfxIs64BitOpertionSystem();
void  AfxCloseLogger(HANDLE& hLogger);
void  AfxWriteLogger(HANDLE hLogger, LPCTSTR formater, ...);
//create directory tree.
void AfxCreateDirectory(LPCTSTR szDirectory, size_t offset = 0);

//delete all file and sub dir.
void AfxDeleteDirectory(LPCTSTR szDirectory, HANDLE hExit = NULL);

//convert time_t to filetime.
void AfxUtcTime2FileTime(DWORD time, FILETIME& ft);

//get file version.x.x.x.x
LPTSTR AfxGetFileVersion(LPCTSTR pFileName, LPTSTR pVersion);

//socket close.
void  AfxSocketClose(SOCKET& sck);

//network connect with timeout(second).
SOCKET AfxSocketConnect(LPCTSTR ip, WORD port, DWORD timeout = 2);

typedef struct MOUNTINFO
{
	ULONG   DiskType;
	ULONG	Ip;
	USHORT	Port;
	WCHAR	UserId[64];
}MOUNTINFO, *PMOUNTINFO;

//////////////////////////////////////////////////////////////////////////
//file disk driver function.
DWORD FileDiskInstallDriver();

DWORD FileDiskMount(DWORD DiskType, DWORD Ip, WORD Port, TCHAR DriverLetter, LPCTSTR UserId);

DWORD FileDiskUnmount(TCHAR DriverLetter);

//游戏排序方式，但这两种排序，都是增值游戏一定会排在前面。
#define SORT_NAME		1	//按windows资源管理器默认方式排序
#define SORT_LCLICK		2	//按游戏点击率排序
#define SORT_SCLICK		3   //中心游戏点击率排序
#define SORT_ADDTIME	4   //中心添加时间

#define SORT_FINDPOS	5   //搜索位置排序

template<int N = SORT_NAME>
struct Pred
{
	bool operator()(tGameInfo* p1, tGameInfo* p2)
	{
		int nLen1 = sizeof(p1->gname)/sizeof(TCHAR);
		int nLen2 = sizeof(p2->gname)/sizeof(TCHAR);
		int nLen = nLen1 > nLen2 ? nLen1 : nLen2;

		//按名称的排列方式来排。(升序)
		return StrNCmpI(p1->gname, p2->gname, nLen) < 0;
	}
};

template<>
struct Pred<SORT_LCLICK>
{
	bool operator()(tGameInfo* p1, tGameInfo* p2)
	{
		//按本地点击数的升序来排
		return p1->svrclick > p2->svrclick;
	}
};

template<>
struct Pred<SORT_SCLICK>
{
	bool operator()(tGameInfo* p1, tGameInfo* p2)
	{
		//按中心点击数的升序来排
		return p1->ideclick > p2->ideclick;
	}
};

template<>
struct Pred<SORT_ADDTIME>
{
	bool operator()(tGameInfo* p1, tGameInfo* p2)
	{
		//按添加时间的升序来排
		return p1->idcdate > p2->idcdate;
	}
};

template<>
struct Pred<SORT_FINDPOS>
{
	bool operator()(tagGameInfo* p1, tagGameInfo* p2)
	{
		//按别名排序
		if (p1->bGalias == p2->bGalias && p1->bGalias == true)
		{
			bool bResult = lstrcmpi(p1->gname, p2->gname) < 0;
			return bResult;
		}
		
		if (p2->bGalias)
		{
			return false;
		}
		if (p1->bGalias)
		{
			return true;
		}

		//按搜索位置升序来排
		if (p1->dwFindPos != p2->dwFindPos)
		{
			return p1->dwFindPos < p2->dwFindPos;
		}

		//按名字长度
		int nLen1 = _tcsclen(p1->gname);
		int nLen2 = _tcsclen(p2->gname);
		if (nLen1 != nLen2)
			return nLen1 < nLen2;

		//
		int nLen = nLen1 > nLen2 ? nLen1 : nLen2;
		return StrNCmpI(p1->gname, p2->gname, nLen) < 0;
		
		
	}
};


template <int N>
void SortGameList(std::vector<tagGameInfo*>& GameList)
{
	std::sort(GameList.begin(), GameList.end(), Pred<N>());
}
void AfxGetFixedDisks(CStringArray& arDisks);

class CClientDbMgr
{
public:
	~CClientDbMgr();
	static CClientDbMgr* getInstance();
	void CheckCliService();
	bool ClientServiceIsRunning();
	CString GetConfigDir();
	bool ReadConfigDatFile(LPCTSTR lpFileName, char*& pData, DWORD& dwLength);
public:
	std::map<CString, CString>			m_mapSysOpt;	//系统选项列表
	std::vector<tDiskMgr>				gDiskMgr;		//虚拟磁盘
	std::vector<tRate>					gRate;			//费率表
	std::map<DWORD, tagIconInfo>		m_mapIconInfo;	//图标列表
	std::map<tstring, tstring>			gSvrList;		//多服务器列表
	std::map<DWORD, tagGameInfo*>		m_mapGameInfo;	//游戏信息列表
	Gdiplus::Bitmap*					m_defaultImage;
public:	
	void Initialize();
	void unInitialize();
	void LoadMultiChar();
	void LoadSysOpt();
	void LoadIconInfoAll();
	void LoadGameInfoAll();
	void LoadRate();
	void RefreshOfflineData();
	TCHAR GetVdiskTmpDriver();
	void SetOpt(CString key,CString value);
	bool WriteDataFile(LPCTSTR szFileName, LPVOID pData, int length);
	bool SaveOpt();

	//查询系统选项
	bool	GetBoolOpt(LPCTSTR lpOptName, bool nOptDef = false);
	long	GetIntOpt(LPCTSTR lpOptName, long nOptDef = 0);
	__int64 GetInt64Opt(LPCTSTR lpOptName, __int64 nOptDef = 0);
	CString GetStrOpt(LPCTSTR lpOptName, LPCTSTR lpOptDef = TEXT(""), bool bEmptyIsDef = false);
	TCHAR   GetUDiskDriver();
	TCHAR   GetUDiskUnmoutDriver();

	CString GetRateText(DWORD nIp);
	CString GetRateTitleText(DWORD nIp);
	BOOL IsSetRate( DWORD nIp );

	CString TransChToGBK(LPCTSTR lpszSouce);
	CString GetIniFile();
	CString LoadIpAddr();
	CString LoadLocalIpAddr();
	void    SaveIpAddr(CString str);
	CString	GetUptSvrList();
	void    GetGameNamePy(tagGameInfo* pGameInfo);
	tagGameInfo* FindGameInfo(DWORD gid);
	//tVDisk* FindVDiskInfo(TCHAR chSvr);
	//Dno't DestroyIcon return hIcon.(if hIcon != NULL)
	bool	GetSkinPng(CString& strPng,int& nType);

	bool	IsGameClassExist(CString szClass); // 游戏分类是否有游戏

private:
	CClientDbMgr();
	std::map<TCHAR, WCHAR*> m_mapMultiChar;
	DWORD m_LocalIp;
	CString	 m_ServerName;

	void FreeGameInfo();
	void FreeIconInfo();
};

typedef struct
{
	DWORD gameconfigid;
	DWORD gid;
	TCHAR configname[MAX_NAME_LEN];
	DWORD uploadtime;
	TCHAR filename[MAX_NAME_LEN];
	TCHAR uploadpc[MAX_NAME_LEN];
	DWORD uploadip;
}tUserConfig;

START_NAMESPACE_KMENU
//////////////////////////////////////////////////////////////////////////
//net function.
#define PKG_VERSION				0x0100		//1.0
#define PKG_SIG					'.pkg'
#define PKG_MAXLENGTH			0x10000
#define PKG_STA_SUCCESS			0
#define PKG_STA_ERROR			1

#pragma warning(disable:4200)
struct PkgHeader
{
	DWORD sig;					//package start flag.	
	DWORD length;				//total length, include package header
	DWORD command;				//command.
	DWORD type;					//object type.OBJECT_
	DWORD version;				//version.
	DWORD status;				//status.
	DWORD reserved[4];			//reserved, fill 0.
	char  data[0];				//data.
};

class CPkgHelper
{
private:
	LPSTR  m_lpPackage;
	size_t m_nBufSize;
	size_t m_dwPosition;
public:
	//for push construct.
	CPkgHelper(LPSTR lpPackage, size_t nBufSize, DWORD cmd, DWORD type)
		: m_lpPackage(lpPackage), m_nBufSize(nBufSize), m_dwPosition(sizeof(PkgHeader))
	{
		GetHeader()->sig = PKG_SIG;
		GetHeader()->length = sizeof(PkgHeader);
		GetHeader()->command = cmd;
		GetHeader()->type = type;
		GetHeader()->version = PKG_VERSION;
		GetHeader()->status = PKG_STA_SUCCESS;
		ZeroMemory(&GetHeader()->reserved, sizeof(GetHeader()->reserved));
	}

	template<size_t N>
	explicit CPkgHelper(char(&arr)[N], DWORD cmd, DWORD type)
		: m_lpPackage(arr), m_nBufSize(N), m_dwPosition(sizeof(PkgHeader))
	{
		GetHeader()->sig = PKG_SIG;
		GetHeader()->length = sizeof(PkgHeader);
		GetHeader()->command = cmd;
		GetHeader()->type = type;
		GetHeader()->version = PKG_VERSION;
		GetHeader()->status = PKG_STA_SUCCESS;
		ZeroMemory(&GetHeader()->reserved, sizeof(GetHeader()->reserved));
	}

	//for pop construct.
	explicit CPkgHelper(LPCSTR lpPackage) : m_lpPackage((LPSTR)lpPackage), m_nBufSize(0), m_dwPosition(sizeof(PkgHeader))
	{
		m_nBufSize = GetHeader()->length;
	}
public:
	PkgHeader* GetHeader()				{ return reinterpret_cast<PkgHeader*>(m_lpPackage); }
	LPCSTR	GetBuffer(void) const		{ return m_lpPackage; }
	void	SetLength(DWORD dwLength)	{ GetHeader()->length = dwLength; }
	DWORD	GetLength()					{ return GetHeader()->length; }
	void	SetOffset(DWORD offset)		{ m_dwPosition = offset; }
	DWORD	GetOffset()					{ return (DWORD)m_dwPosition; }
	void	SetStatus(DWORD status)		{ GetHeader()->status = status; }
	bool	IsStatusOk()				{ return (GetHeader()->status == PKG_STA_SUCCESS); }
	void    PushErrorInfo(DWORD status, LPCTSTR info)
	{
		GetHeader()->status = status;
		GetHeader()->length = sizeof(PkgHeader);
		PushString(info, _tcslen(info));
	}

	void	pushHeader(PkgHeader& header)  { *GetHeader() = header; }
	void	popHeader(PkgHeader& header)   { header = *GetHeader(); }

	template <typename T>
	void Push(const T& Elem)
	{
		size_t nSize = sizeof(T);
		memcpy(m_lpPackage + m_dwPosition, (void*)&Elem, nSize);
		m_dwPosition += nSize;
		GetHeader()->length = (DWORD)m_dwPosition;
	}

	template <typename T>
	void PushPointer(const T* pointer, size_t nCount = 1)
	{
		size_t nSize = sizeof(T) * nCount;
		memcpy(&m_lpPackage[m_dwPosition], (void*)pointer, nSize);
		m_dwPosition += nSize;
		GetHeader()->length = (DWORD)m_dwPosition;
	}

	template <typename T>
	void PushString(const T* pString, size_t nCharNums)
	{
		Push((DWORD)nCharNums);
		PushPointer(pString, nCharNums);
		Push(T(0));
	}

	template <typename T, size_t N>
	void PushString(const T(&arr)[N])
	{
		struct StringLenHelper
		{
			size_t len_;

			StringLenHelper(const char *str)
			{
				len_ = strlen(str);
			}
			StringLenHelper(const wchar_t *str)
			{
				len_ = wcslen(str);
			}

			size_t operator()() const
			{
				return len_;
			}
		};
		size_t len = StringLenHelper(arr)();

		PushString(arr, len);
	}

	template <typename T>
	void Push(long offset, const T& Elem)
	{
		size_t nSize = sizeof(T);
		memcpy(m_lpPackage + offset, (void*)&Elem, nSize);
	}

	template <typename T>
	T Pop(T& Elem)
	{
		if (m_nBufSize - m_dwPosition < sizeof(T))
			throw std::out_of_range("packageex out of range");

		memcpy((void*)&Elem, m_lpPackage + m_dwPosition, sizeof(T));
		m_dwPosition += sizeof(T);
		return Elem;
	}

	template <typename T>
	void PopPointer(T* pointer, size_t nCount = 1)
	{
		size_t nSize = nCount * sizeof(T);

		if (m_nBufSize - m_dwPosition < nSize)
			throw std::out_of_range("packageex out of range");

		memcpy((void*)pointer, m_lpPackage + m_dwPosition, nSize);
		m_dwPosition += nSize;
	}

	template <typename T>
	DWORD PopString(T* pString, size_t nCharNums)
	{
		DWORD length = 0;
		Pop(length);

		if (nCharNums < (size_t)length + 1)
			throw std::out_of_range("package out of range");

		PopPointer(pString, length + 1);
		return length;
	}

	template <typename T, size_t N>
	DWORD PopString(T(&arr)[N])
	{
		DWORD length = 0;
		Pop(length);

		if (N < length + 1)
			throw std::out_of_range("package out of range");

		PopPointer(arr, (length + 1));
		return length;
	}
};

namespace detail
{
	template < typename T >
	struct Select
	{
		static CPkgHelper &Dispacth(CPkgHelper &package, const T &val)
		{
			package.Push(val);
			return package;
		}
	};

	template <>
	struct Select<char *>
	{
		static CPkgHelper &Dispacth(CPkgHelper &package, const char* const val)
		{
			package.PushString(val, strlen(val));
			return package;
		}
	};

	template <>
	struct Select<const char *>
	{
		static CPkgHelper &Dispacth(CPkgHelper &package, const char* const val)
		{
			return Select<char *>::Dispacth(package, val);
		}
	};


	template <>
	struct Select<wchar_t *>
	{
		static CPkgHelper &Dispacth(CPkgHelper &package, const wchar_t* const val)
		{
			package.PushString(val, wcslen(val));
			return package;
		}
	};

	template <>
	struct Select<const wchar_t *>
	{
		static CPkgHelper &Dispacth(CPkgHelper &package, const wchar_t* const val)
		{
			return Select<wchar_t *>::Dispacth(package, val);
		}
	};
}

//CPkgHelper stream operator.
template <typename T>
inline CPkgHelper& operator<<(CPkgHelper& package, const T& Value)
{
	return detail::Select<T>::Dispacth(package, Value);
}


// -----------
template<typename CharT>
inline CPkgHelper& operator<<(CPkgHelper& package, const std::basic_string<CharT>& Value)
{
	package.PushString(Value.c_str(), Value.length());
	return package;
}

template<typename CharT, size_t N>
inline CPkgHelper &operator<<(CPkgHelper &package, const CharT(&arr)[N])
{
	package.PushString(arr);
	return package;
}


template <typename T>
inline CPkgHelper& operator>>(CPkgHelper& package, T& Value)
{
	package.Pop(Value);
	return package;
}

template<typename CharT>
inline CPkgHelper& operator>>(CPkgHelper& package, std::basic_string<CharT>& Value)
{
	DWORD length = 0;
	CharT end = 0;

	package.Pop(length);
	Value.resize(length);
	package.PopPointer<CharT>(&Value[0], length);
	package.Pop(end);
	return package;
}

template<typename CharT, size_t N>
inline CPkgHelper &operator>>(CPkgHelper &package, CharT(&arr)[N])
{
	package.PopString(arr);
	return package;
}
END_NAMESPACE_KMENU

class CLock
{
private:
	CRITICAL_SECTION m_lock;
public:
	CLock()			{ InitializeCriticalSection(&m_lock); }
	~CLock()		{ DeleteCriticalSection(&m_lock); }
	void Lock()		{ EnterCriticalSection(&m_lock); }
	void UnLock()	{ LeaveCriticalSection(&m_lock); }
};
template <typename T>
class ObjectLock
{
private:
	T& _lock;
public:
	ObjectLock(T& lock) : _lock(lock) { _lock.Lock(); }
	~ObjectLock()	{ _lock.UnLock(); }
};

#define KMENU_OBJECT_LOCK(lock) ObjectLock<CLock> __xlock__(lock)


inline void AfxSplitString(const tstring& source, std::vector<tstring>& subs, TCHAR chr)
{
	subs.clear();
	size_t first = 0;
	size_t second = 0;
	do
	{
		second = source.find_first_of(chr, first);
		if (second != tstring::npos)
		{
			subs.push_back(source.substr(first, second - first));
			first = second + 1;
		}
		else
		{
			if (first < source.size())
			{
				subs.push_back(source.substr(first).c_str());
			}
		}
	} while (second != tstring::npos);
}

class CServer
{
public:
	CServer();
	~CServer();
public:
	bool ModifyOption(LPCTSTR szKey,LPCTSTR szValue,CString& strError);
	bool AddGameConfig(DWORD ngid,LPCTSTR configname,LPCTSTR filename,LPCTSTR pcname,CString& strError);
	bool UploadFile(LPCTSTR szSrcFile,LPCTSTR szDstFile, BOOL bIsRelative = TRUE);
	void ReprotDataToServer(std::map<DWORD, DWORD>& GameClick,bool& bConnect, bool bAck = true);

	bool DownloadMenuXML(char*& data, int& size); // 下载中心的菜单XML
	bool DownLoadFile(LPCTSTR szSrcFile, LPCTSTR szDstFile, DWORD dwFileSize = -1, DWORD dwFileModifyTime = -1, BOOL bIsRelative = TRUE);
	bool DownloadIcon(int gid);

	bool UDiskRegistryUser(LPCTSTR user, LPCTSTR password, tstring& errinfo);
	bool UDiskLoginUser(LPCTSTR user, LPCTSTR password, tstring& errinfo);
	bool UDiskModifyPassword(LPCTSTR user, LPCTSTR password, LPCTSTR newpassword, tstring& errinfo);
	bool AdminTestPassword(LPCTSTR password, tstring& errinfo);
	bool UDiskMount(LPCTSTR name,DWORD& nRet);
	bool UDiskUnMount(bool showtip = true);

private:
	bool SendDataToServer(char* pData, int len, bool bAck, char* buf);
	void CloseServer();
	
private:
	SOCKET m_sck;
};

inline CClientDbMgr* AfxGetDbMgr()
{
	return CClientDbMgr::getInstance();
}

Gdiplus::Bitmap* GetGameIcon(int gid, int icon_size);
Gdiplus::Bitmap* ReadIcon(void* icon_data, unsigned int icon_data_size, int icon_size);
CString GetMenuSkinPath( int nType );
int SkinNameToType(CString strName);

void FindClienPath(tagGameInfo*	pGame,LPTSTR pPath,int nLenth);
CString GetGameConfigFilePath(tagGameInfo* pGame);

void SavePassword(CString user, CString password);
void GetPassword(CString& user, CString& password);

bool DownloadSingleFile(LPCTSTR lpUrl, Gdiplus::Bitmap*& pImage);

class CLogger
{
private:
	CLock		m_lock;
	HANDLE		m_hLogFile;
	SYSTEMTIME	m_SysTime;
	TCHAR		m_logFileModuleName[MAX_PATH];
	TCHAR		m_logFileFullName[MAX_PATH];
public:
	CLogger() : m_hLogFile(INVALID_HANDLE_VALUE)
	{
		ZeroMemory(&m_SysTime, sizeof(m_SysTime));
		ZeroMemory(m_logFileModuleName, sizeof(m_logFileModuleName));
		ZeroMemory(m_logFileFullName, sizeof(m_logFileFullName));
		lstrcpy(m_logFileModuleName, TEXT("bflog"));
		setlocale(LC_ALL, "chs");
	}
	virtual ~CLogger()
	{
		if (m_hLogFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hLogFile);
			m_hLogFile = INVALID_HANDLE_VALUE;
		}
	}
public:
	void SetLogModule(LPCTSTR szModule)
	{
		KMENU_OBJECT_LOCK(m_lock);
		if (szModule != NULL && lstrlen(szModule))
		{
			if (m_hLogFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = INVALID_HANDLE_VALUE;
			}

			lstrcpy(m_logFileModuleName, szModule);
		}
	}
	LPCTSTR GetLogFileFullName()
	{
		KMENU_OBJECT_LOCK(m_lock);

		TCHAR szDir[MAX_PATH] = { 0 };
		AfxGetAppPath(szDir, MAX_PATH, TEXT("log\\"));
		CreateDirectory(szDir, NULL);

		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		_stprintf_s(m_logFileFullName, TEXT("%s%s-%04d%02d%02d.log"),
			szDir, m_logFileModuleName, st.wYear, st.wMonth, st.wDay);
		return m_logFileFullName;
	}
	void WriteLogger(LPCTSTR szLog, int nCharsNums = -1)
	{
		KMENU_OBJECT_LOCK(m_lock);
		if (EnsureOpenFile())
		{
			DWORD dwWriteBytes = 0;
			int nLen = (nCharsNums == -1) ? lstrlen(szLog) : nCharsNums;
			nLen *= sizeof(TCHAR);
			WriteFile(m_hLogFile, szLog, nLen, &dwWriteBytes, NULL);
		}
	}
private:
	bool EnsureOpenFile()
	{
		SYSTEMTIME st = { 0 };
		GetLocalTime(&st);
		if (st.wYear != m_SysTime.wYear || st.wMonth != m_SysTime.wMonth ||
			st.wDay != m_SysTime.wDay)
		{
			if (m_hLogFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = INVALID_HANDLE_VALUE;
			}
		}
		if (m_hLogFile == INVALID_HANDLE_VALUE)
		{
			m_hLogFile = CreateFile(GetLogFileFullName(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, OPEN_ALWAYS, 0, NULL);
			if (m_hLogFile == INVALID_HANDLE_VALUE)
				return false;

			//write unicode file sig.
#ifdef _UNICODE
			short start = 0;
			DWORD dwBytes = 0;
			if (!ReadFile(m_hLogFile, &start, 2, &dwBytes, NULL) || start != 0xfeff)
			{
				SetFilePointer(m_hLogFile, 0, NULL, FILE_BEGIN);
				start = (short)0xfeff;
				WriteFile(m_hLogFile, &start, sizeof(start), &dwBytes, NULL);
			}
#endif
			SetFilePointer(m_hLogFile, 0, 0, FILE_END);
			m_SysTime = st;
		}
		return true;
	}

};


#define APP_TCP_PORT						5761
#define CMD_CLI_RPTGAMEMENUCLICK			0x030A		//菜单上报游戏点击数,菜单运行次数

//游戏运行方式
#define GAME_RUNTYPE_UNKNOW				0			//未设置.就是游戏不在该分区显示
#define GAME_RUNTYPE_VDISK				1			//虚拟盘方式运行
#define GAME_RUNTYPE_UPDATE				2			//本地更新，通过内网更新到客户机后，再运行
#define GAME_RUNTYPE_DIRECT				3			//直接运行，支持第三方虚拟盘，直接运行客户机的游戏

#define OPT_M_MENUBG					TEXT("mmenubg")				//客户机:当前选中菜单皮肤
#define OPT_M_CUSTOMMENUBG				TEXT("mcustommenubg")		//客户机:自定义菜单皮肤（|线分割）
#define OPT_M_ENABLEUDISK				TEXT("menableudisk")		//个人磁盘:允许个人磁盘(0,1)
#define OPT_M_ARCHIVECOUNT				TEXT("marchivecount")		//个人磁盘:游戏存档数量
#define OPT_M_HIDEMENU					TEXT("mhidemenu")			//客户机:启动游戏后是否隐藏菜单.1表示隐藏.

//get windows system error text.
LPTSTR AfxGetWinErrorText(LPTSTR buf, DWORD dwSize, DWORD dwError);

#define CLASS_TOTAL_COUNT			17


#define CLASS_WL_GUID				TEXT("{1AF9A1A1-FF46-41B7-BC7A-D1FAE3E25B01}")	//网络游戏a
#define CLASS_DJ_GUID				TEXT("{2A687865-B630-4FA2-A5AC-EA9C9C4AABB2}")	//精品单机b
#define CLASS_QP_GUID				TEXT("{3A761798-CD7D-4761-81CB-8513B17F69E3}")	//棋牌游戏c
#define CLASS_DZ_GUID				TEXT("{4E33B955-25C7-40e9-B391-EE4CE049A592}")	//对战平台d
#define CLASS_LT_GUID				TEXT("{5AAAF78F-3FF2-4834-99AF-744233CEC6D4}")  //聊天交友e
#define CLASS_WY_GUID				TEXT("{6A9363C6-EC57-4A65-9D71-043B80A38F05}")	//网页游戏f
#define CLASS_XX_GUID				TEXT("{7AAD06BD-819C-48D1-9CB0-47DF498CF256}")	//休闲益智g
#define CLASS_PL_GUID				TEXT("{8A999DC2-2676-4FF2-8D62-7AB625F02857}")	//游戏辅助h
#define CLASS_CY_GUID				TEXT("{9A76073C-DDB1-4939-B85C-521F283B0198}")	//常用软件i
#define CLASS_YY_GUID				TEXT("{A258A554-6D7E-4369-8E67-4339CD7A0F59}")	//影音工具j
#define CLASS_GP_GUID				TEXT("{B3BAFF9D-5015-422A-A0CB-4CBC3DFAC49A}")	//股票证券k
#define CLASS_GQ_GUID				TEXT("{C945C2D1-E039-4c14-9D40-8789059493FB}")	//高清影视l
#define CLASS_ZX_GUID				TEXT("{DA2F5C0F-AC11-4720-A6CD-2C72EE891E6B}")	//在线音乐m
#define CLASS_XC_GUID				TEXT("{E0B0A5D9-E267-4cdf-8E84-0E8CA13C16F5}")	//秀场直播n
#define CLASS_SJ_GUID				TEXT("{F566239D-B794-4751-8BE5-A09A994C15D1}")	//手机游戏o
#define CLASS_BG_GUID				TEXT("{G6D14FF3-2EBC-4750-91C7-77C0284B090B}")	//办公软件p
#define CLASS_DPNP_GUID				TEXT("{4ADDE7DA-1AED-43BC-8803-93763DFF00EC}")	//显卡pnp q

//网吧添加的游戏类别
#define CLASS_NB_GUID				TEXT("{39F925AE-5972-454C-86DD-5A554F2C73A2}")  //网吧自定义游戏分类

inline bool AfxIsIdcClassGUID(LPCTSTR szName)
{
	static TCHAR __IdcName__[][40] = { CLASS_WL_GUID, CLASS_DJ_GUID, CLASS_QP_GUID, CLASS_DZ_GUID, CLASS_LT_GUID,
		CLASS_WY_GUID, CLASS_XX_GUID, CLASS_PL_GUID, CLASS_CY_GUID, CLASS_YY_GUID, CLASS_GP_GUID, CLASS_GQ_GUID,
		CLASS_ZX_GUID, CLASS_XC_GUID, CLASS_SJ_GUID, CLASS_BG_GUID, CLASS_DPNP_GUID };
	for (size_t idx = 0; idx < CLASS_TOTAL_COUNT; idx++)
	{
		if (lstrcmpi(szName, __IdcName__[idx]) == 0)
			return true;
	}
	return false;
}

BOOL AfxDisableFsRedirection(PVOID* pOldValue);
BOOL AfxRevertFsRedirection(PVOID oldValue);

//get idc class name
LPTSTR AfxGetIdcClassName(LPCTSTR szGuid, LPTSTR szName, DWORD dwSize = MAX_PATH);

//return process token by process name.
HANDLE AfxGetTokenByProcName(LPCTSTR lpName);

//run process by "explore.exe" token.(current user.)
bool AfxRunProcess(LPCTSTR lpProcess, LPCTSTR lpParameter = NULL);

//generate exception dump file. 
LONG AfxUnHandledExceptionFilter(PEXCEPTION_POINTERS pExcept);

inline void AfxSetExptHandle() { SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)AfxUnHandledExceptionFilter); }

LPTSTR AfxGetLangStringEx(HANDLE hInstance, UINT uID, LPTSTR Value,
	DWORD dwSize, LPCTSTR Default = TEXT(""));

//set language id
void AfxSetLangId(WORD wLangID);


//=================== CEvenyOneSD(类) ==================
class CEvenyOneSD
{
public:

	CEvenyOneSD()
	{
		ptr = NULL;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = &sd;
		sa.bInheritHandle = FALSE;
		ptr = Build(&sd);
	};

	virtual ~CEvenyOneSD()
	{
		if (ptr)
		{
			::HeapFree(GetProcessHeap(), 0, ptr);
		}
	}

public:

	PVOID Build(PSECURITY_DESCRIPTOR pSD)
	{
		PSID   psidEveryone = NULL;
		PACL   pDACL = NULL;
		BOOL   bResult = FALSE;
		PACCESS_ALLOWED_ACE pACE = NULL;

		try
		{
			SID_IDENTIFIER_AUTHORITY siaWorld = SECURITY_WORLD_SID_AUTHORITY;
			SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;

			if (!::InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
				throw GetLastError();

			if (!::AllocateAndInitializeSid(&siaWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &psidEveryone))
				throw GetLastError();

			DWORD dwAclLength = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + ::GetLengthSid(psidEveryone);

			pDACL = (PACL)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, dwAclLength);
			if (!pDACL)
				throw GetLastError();

			if (!::InitializeAcl(pDACL, dwAclLength, ACL_REVISION))
				throw GetLastError();

			if (!::AddAccessAllowedAce(pDACL, ACL_REVISION, GENERIC_ALL, psidEveryone))
				throw GetLastError();

			if (!::SetSecurityDescriptorDacl(pSD, TRUE, pDACL, FALSE))
				throw GetLastError();
			bResult = TRUE;
		}
		catch (...)  {}

		if (psidEveryone)
		{
			::FreeSid(psidEveryone);
			psidEveryone = NULL;
		}
		if (bResult == FALSE)
		{
			if (pDACL)
				::HeapFree(::GetProcessHeap(), 0, pDACL);
			pDACL = NULL;
		}

		return pDACL;
	}

	SECURITY_ATTRIBUTES* GetSA()
	{
		return (ptr != NULL) ? &sa : NULL;
	}

protected:
	PVOID  ptr;
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
};
//======================================================
#define OPT_M_CLIPWD					TEXT("mclipwd")				//客户机:高级设置密码(密码的CRC32字符串)
DWORD AfxCrc32Encode(DWORD crc, const void* data, DWORD ByteLen);

#endif