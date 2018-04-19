#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifndef START_NAMESPACE_KCORE
#define START_NAMESPACE_KCORE		namespace kcore {
#endif
#ifndef END_NAMESPACE_KCORE
#define END_NAMESPACE_KCORE			}
#endif
#ifndef USE_NAMESPACE_KCORE
#define USE_NAMESPACE_KCORE			using namespace kcore;
#endif

START_NAMESPACE_KCORE

// =============================== function ============================================

typedef LPTSTR(__stdcall *PGETLOCALSTRINGFUN)(__inout LPTSTR lpBuffer, __in DWORD dwSize,
__in LPCTSTR szUrlKey, __in LPCTSTR szDefault);

typedef long(__stdcall *PGETLOCALLONGFUN)(__in LPCTSTR szUrlKey, __in long lDefault);

// =====================================================================================

// =====================================================================================

extern PGETLOCALSTRINGFUN g_pGetLocalString;

extern PGETLOCALLONGFUN g_pGetLocalLong;

// =============================== function ============================================

// DWORD LoadLocalConfigFun();
// 
// void UnLoadLocalConfigFun();


LPTSTR __stdcall GetLocalStringFun(LPTSTR lpBuffer, DWORD dwSize, LPCTSTR szUrlKey, LPCTSTR szDefault);

long __stdcall GetLocalLongFun(LPCTSTR szUrlKey, long lDefault);

END_NAMESPACE_KCORE

