#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>

#ifndef START_LIBBOOTPATCH_NAME_SPACE
#define START_LIBBOOTPATCH_NAME_SPACE	namespace libbootpatch {
#endif
#ifndef END_LIBBOOTPATCH_NAME_SPACE
#define END_LIBBOOTPATCH_NAME_SPACE		}
#endif

START_LIBBOOTPATCH_NAME_SPACE

/// 开机动画补丁
DWORD BootFlashPatch(LPCTSTR lpBootresFile, LPCTSTR lpWinloadFile, LPCTSTR lpWimFile,
	LPTSTR szErrorInfo, DWORD dwLength);

/// 开机背景补丁
DWORD BootBackgroundPatch(LPCTSTR lpWinloadFile, LPCTSTR lpBackgroundFile,
	LPTSTR szErrorInfo, DWORD dwLength);

/// 开机文字补丁
DWORD BootStringPatch(LPCTSTR lpWinloadMuiFile, LPCTSTR lpWinloadFile, LPCTSTR lpNewStirng,
	LPTSTR szErrorInfo, DWORD dwLength);


typedef struct tagBootPatchParam
{
	std::wstring strWinloadFile;

	/// 开机动画补丁
	std::wstring strBootresFile;
	std::wstring strWimFile;

	/// 开机背景补丁
	std::wstring strBackgroundFile;

	/// 开机文字补丁
	std::wstring strWinloadMuiFile;
	std::wstring strNewStirng;

	bool bIsValid() const
	{
		return strWinloadMuiFile.length() > 0 &&
			((strBootresFile.length() > 0 && strWimFile.length() > 0) ||
			strBackgroundFile.length() > 0 ||
			(strWinloadMuiFile.length() > 0 && strNewStirng.length() > 0));
	}
} BOOTPATCHPARAM, *PBOOTPATCHPARAM;
/// 设置开机补丁
DWORD BootPatch(const BOOTPATCHPARAM& tBootPatchParam, LPTSTR szErrorInfo, DWORD dwLength);


END_LIBBOOTPATCH_NAME_SPACE