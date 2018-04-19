
inline bool AfxIsIdcAreaGuid(LPCTSTR guid)
{
	if (lstrcmpi((guid), DEF_AREA_GUID) == 0 || lstrcmpi((guid), ALL_AREA_GUID) == 0)
		return true;
	return false;
}

//inline bool AfxIsIdcAreaName(LPCTSTR name)
//{
//	if (lstrcmpi((name), DEF_AREA_NAME) == 0 || lstrcmpi((name), ALL_AREA_NAME) == 0)
//		return true;
//	return false;
//}

inline LPCTSTR AfxGetGUIDByAlpha(TCHAR alpha)
{
	TCHAR tIndex = _totlower(alpha) - TEXT('a');
	if (tIndex < 0 || tIndex >= CLASS_TOTAL_COUNT)
		return TEXT("");

	static TCHAR __IdcGuid__[][40] = {CLASS_WL_GUID, CLASS_DJ_GUID, CLASS_QP_GUID, CLASS_DZ_GUID, CLASS_LT_GUID, 
		CLASS_WY_GUID, CLASS_XX_GUID, CLASS_PL_GUID, CLASS_CY_GUID, CLASS_YY_GUID,CLASS_GP_GUID,CLASS_GQ_GUID,
		CLASS_ZX_GUID, CLASS_XC_GUID, CLASS_SJ_GUID, CLASS_BG_GUID, CLASS_DPNP_GUID };
	return __IdcGuid__[tIndex];
}

inline bool AfxIsIdcClassGUID(LPCTSTR szName)
{
	static TCHAR __IdcName__[][40] = {CLASS_WL_GUID, CLASS_DJ_GUID, CLASS_QP_GUID, CLASS_DZ_GUID, CLASS_LT_GUID, 
		CLASS_WY_GUID, CLASS_XX_GUID, CLASS_PL_GUID, CLASS_CY_GUID, CLASS_YY_GUID,CLASS_GP_GUID,CLASS_GQ_GUID,
		CLASS_ZX_GUID, CLASS_XC_GUID, CLASS_SJ_GUID, CLASS_BG_GUID, CLASS_DPNP_GUID };
	for (size_t idx=0; idx<CLASS_TOTAL_COUNT; idx++)
	{
		if (lstrcmpi(szName, __IdcName__[idx]) == 0)
			return true;
	}
	return false;
}

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
			subs.push_back(source.substr(first, second-first));
			first = second + 1;
		}
		else
		{
			if (first< source.size())
			{
				subs.push_back(source.substr(first).c_str());
			}
		}
	}while (second != tstring::npos);
}

inline void AfxDeleteDirFilter(const tstring& dir, const tstring& filter, HANDLE hExited)
{
	tstring src = dir;
	if (src.size() && *src.rbegin() != TEXT('\\'))
		src += TEXT("\\");

	FILETIME  ftBefore7Day = {0};
	GetSystemTimeAsFileTime(&ftBefore7Day);
	__int64* p = (__int64*)&ftBefore7Day;
	*p -= (7LL * 24 * 60 * 60 * 10000000);

	WIN32_FIND_DATA wfd = {0};
	HANDLE hFinder = FindFirstFile((src + filter).c_str(), &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return ;

	while (hExited == NULL || (WAIT_OBJECT_0 != WaitForSingleObject(hExited, 0)))
	{
		if (CompareFileTime(&wfd.ftLastWriteTime, &ftBefore7Day) == -1)
		{
			tstring file = src + wfd.cFileName;
			SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_NORMAL);
			DeleteFile(file.c_str());
		}

		if (!FindNextFile(hFinder, &wfd))
			break;
	}

	FindClose(hFinder);
}

inline void AfxDeleteDirFilterEx(LPCTSTR dir, LPCTSTR filter, HANDLE hExited)
{
	AfxDeleteDirFilter(dir, filter, hExited);
}

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
		GetHeader()->sig		= PKG_SIG;
		GetHeader()->length		= sizeof(PkgHeader);
		GetHeader()->command	= cmd;
		GetHeader()->type		= type;
		GetHeader()->version	= PKG_VERSION;
		GetHeader()->status		= PKG_STA_SUCCESS;
		ZeroMemory(&GetHeader()->reserved, sizeof(GetHeader()->reserved));
	}

	template<size_t N>
	explicit CPkgHelper(char (&arr)[N], DWORD cmd, DWORD type) 
		: m_lpPackage(arr), m_nBufSize(N), m_dwPosition(sizeof(PkgHeader))
	{
		GetHeader()->sig		= PKG_SIG;
		GetHeader()->length		= sizeof(PkgHeader);
		GetHeader()->command    = cmd;
		GetHeader()->type		= type;
		GetHeader()->version	= PKG_VERSION;
		GetHeader()->status		= PKG_STA_SUCCESS;
		ZeroMemory(&GetHeader()->reserved, sizeof(GetHeader()->reserved));
	}

	//for pop construct.
	explicit CPkgHelper(LPCSTR lpPackage) : m_lpPackage((LPSTR)lpPackage), m_nBufSize(0), m_dwPosition(sizeof(PkgHeader))
	{
		m_nBufSize = GetHeader()->length;
	}
public:	
	PkgHeader* GetHeader()				{ return reinterpret_cast<PkgHeader*>(m_lpPackage); }
	LPCSTR	GetBuffer(void) const		{ return m_lpPackage;								}
	void	SetLength(DWORD dwLength)	{ GetHeader()->length = dwLength;					}
	DWORD	GetLength()					{ return GetHeader()->length;						}
	void	SetOffset(DWORD offset)		{ m_dwPosition = offset;							}
	DWORD	GetOffset()					{ return (DWORD)m_dwPosition;						}
	void	SetStatus(DWORD status)		{ GetHeader()->status = status;						}
	bool	IsStatusOk()				{ return (GetHeader()->status == PKG_STA_SUCCESS);	}
	void    PushErrorInfo(DWORD status, LPCTSTR info)
	{
		GetHeader()->status = status;
		GetHeader()->length = sizeof(PkgHeader);
		PushString(info, _tcslen(info));
	}

	void	pushHeader(PkgHeader& header)  { *GetHeader() = header;			}
	void	popHeader(PkgHeader& header)   { header = *GetHeader();			}

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
	void PushString(const T (&arr)[N])
	{
		struct StringLenHelper
		{
			size_t len_;

			StringLenHelper(const char *str)
			{ len_ = strlen(str);	}
			StringLenHelper(const wchar_t *str)
			{ len_ = wcslen(str);	}

			size_t operator()() const
			{ return len_;			}
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

		if( nCharNums < (size_t)length + 1 )
			throw std::out_of_range("package out of range");

		PopPointer(pString, length + 1);
		return length;
	}

	template <typename T, size_t N>
	DWORD PopString(T (&arr)[N])
	{
		DWORD length = 0;
		Pop(length);

		if( N < length + 1  )
			throw std::out_of_range("package out of range");

		PopPointer(arr, (length+1));
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
inline CPkgHelper &operator<<(CPkgHelper &package, const CharT (&arr)[N])
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
inline CPkgHelper &operator>>(CPkgHelper &package, CharT (&arr)[N])
{
	package.PopString(arr);
	return package;
}


//=================== CEvenyOneSD(¿‡) ==================
class CEvenyOneSD
{
public:

	CEvenyOneSD() 
	{
		ptr=NULL;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = &sd;
		sa.bInheritHandle = FALSE;
		ptr = Build(&sd);
	};

	virtual ~CEvenyOneSD()
	{
		if(ptr)
		{
			::HeapFree(GetProcessHeap(), 0, ptr);
		}
	}

public:

	PVOID Build(PSECURITY_DESCRIPTOR pSD)
	{
		PSID   psidEveryone = NULL;
		PACL   pDACL   = NULL;
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
		catch (...)  { }

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
