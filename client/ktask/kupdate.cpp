#include "stdafx.h"
#include <time.h>
#include "hy.h"

#define _RUN_TMP_FILE_						TEXT(".~tmp")				//更新正在占用的文件时，先改成一个临时文件.
#define _UPDATE_TMP_DIR_					TEXT("UpdateTemp\\")		//更新过程中，所有需要下载的文件的临时目录
#define _UPDATE_CLIENT_DIR_					TEXT("client\\")			//客户端升级的目录.

typedef struct tagFileInfo
{
	DWORD		dwSize;				//(dst file name)
	DWORD		dwModifyTime;		//(file.modifytime:time_t)
	DWORD		dwCrc;				//(file.crc)
	tstring		szFileName;
	tstring		szParseFile;
	tstring		szTmpName;
}tagFileInfo;

static std::vector<tagFileInfo> gFileInfoList;

bool IsWow64()
{
#ifdef _WIN64
	return TRUE;
#endif   
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL bIsWow64 = FALSE;    
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"IsWow64Process");      
	if (NULL != fnIsWow64Process)    
	{        
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{ 
			// handle error
		}
	}
	return bIsWow64 > 0;
}

bool FilterDiskless(LPCTSTR szFilename) // 返回false，表示不需要
{
	if (lstrcmpi(szFilename, _T("kfdisk.sys")) == 0 || lstrcmpi(szFilename, _T("kfdisk64.sys")) == 0)
	{
		return false;
	}
	if (lstrcmpi(szFilename, _T("kpdisk.sys")) == 0 || lstrcmpi(szFilename, _T("kpdisk64.sys")) == 0)
	{
		return false;
	}
	if (StrStrI(szFilename, _T("autonum")) != NULL)
	{
		return false;
	}
	if (lstrcmpi(szFilename, _T("kpdiskdll.dll")) == 0)
	{
		return false;
	}
	return true;
}

bool FilterDisk(LPCTSTR szFilename) // 返回false，表示不需要
{
	if (lstrcmpi(szFilename, _T("kboot.sys")) == 0 || lstrcmpi(szFilename, _T("kboot64.sys")) == 0)
	{
		return false;
	}
	if (lstrcmpi(szFilename, _T("kdump.sys")) == 0 || lstrcmpi(szFilename, _T("kdump64.sys")) == 0)
	{
		return false;
	}
	if (lstrcmpi(szFilename, _T("kshutdown.sys")) == 0 || lstrcmpi(szFilename, _T("kshutdown64.sys")) == 0)
	{
		return false;
	}
	if (lstrcmpi(szFilename, _T("kres.dll")) == 0 || lstrcmpi(szFilename, _T("kres64.dll")) == 0)
	{
		return false;
	}
	if (lstrcmpi(szFilename, _T("kshut.dll")) == 0 || lstrcmpi(szFilename, _T("kshut64.dll")) == 0)
	{
		return false;
	}
	if (StrStrI(szFilename, _T("kcopy")) != NULL)
	{
		return false;
	}
	if (StrStrI(szFilename, _T("superstation")) != NULL)
	{
		return false;
	}
	return true;
}


bool Filter64(LPCTSTR szFileName)
{
	if (StrStrI(szFileName, _T("kcopy")) != NULL)
	{
		if (_tcsstr(szFileName, _T("64")) != NULL)
		{

		}
		else
		{
			return false;
		}
	}
	else if (lstrcmpi(szFileName, _T("kres.dll")) == 0)
	{
		return false;
	}

	LPTSTR pExtName  = PathFindExtension(szFileName);

	if (pExtName && lstrcmpi(pExtName, TEXT(".sys")) == 0)
	{
		if (_tcsstr(szFileName, _T("64")) != NULL)
		{

		}
		else
		{
			return false;
		}
	}

	return true;
}

bool Filter32(LPCTSTR szFileName)
{
	if (_tcsstr(szFileName, _T("64")) != NULL)
	{
		return false;
	}
	return true;
}

static bool GetNeedUpdateList()
{
	int nDirLen = lstrlen(_UPDATE_CLIENT_DIR_);
	TCHAR szRootDir[MAX_PATH] = {0};
	AfxGetAppPath(szRootDir);

	char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};
	CPkgHelper pkgSend(szSendBuffer, CMD_CLI_CLIENTUPDATEINDEX, OBJECT_CLIENT);
	if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, szSendBuffer, pkgSend.GetLength(), szRecvBuffer, sizeof(szRecvBuffer)))
	{
		AfxWriteLogger(ghLogger, TEXT("query update list fail."));
		return false;
	}

	CPkgHelper pkgRecv(szRecvBuffer);
	if (!pkgRecv.IsStatusOk())
	{
		AfxWriteLogger(ghLogger, TEXT("query update list fail."));
		return false;
	}

	DWORD IsDiskless = 0;
	ScsiDiskQueryDisklessInfo(&IsDiskless, NULL, NULL);

	if (IsDiskless)
	{
		AfxWriteLogger(ghLogger, TEXT("update run in diskless mode"));
	}
	else
	{
		AfxWriteLogger(ghLogger, TEXT("update run in disk mode"));
	}

	if (IsWow64())
	{
		AfxWriteLogger(ghLogger, TEXT("update run in 64bits mode"));
	}
	else
	{
		AfxWriteLogger(ghLogger, TEXT("update run in 32bits mode"));
	}

	DWORD dwFileNums = 0;
	pkgRecv >> dwFileNums;
	for (DWORD idx=0; idx<dwFileNums; ++idx)
	{
		TCHAR szFileName[MAX_PATH] = {0};
		tagFileInfo fileInfo;
		pkgRecv >> szFileName >> fileInfo.dwSize >> fileInfo.dwModifyTime >> fileInfo.dwCrc;

		fileInfo.szFileName  = szFileName;
		fileInfo.szParseFile = fileInfo.szFileName;
		fileInfo.szTmpName   = fileInfo.szFileName;
		fileInfo.szParseFile.replace(0, nDirLen, szRootDir);
		fileInfo.szTmpName.replace(0, nDirLen, tstring(szRootDir) + _UPDATE_TMP_DIR_);

		if (IsDiskless)
		{
			if (!FilterDiskless(PathFindFileName(szFileName)))
			{
				continue;
			}
		}
		else
		{
			if (!FilterDisk(PathFindFileName(szFileName)))
			{
				continue;
			}
		}

		if (IsWow64())
		{
			// 64位系统不下载32位驱动
			if (!Filter64(PathFindFileName(szFileName)))
			{
				continue;
			}
		}
		else
		{
			// 32位系统不下载64位驱动
			if (!Filter32(PathFindFileName(szFileName)))
			{
				continue;
			}
		}

		if (AfxGetFileCrc32(fileInfo.szParseFile.c_str()) != fileInfo.dwCrc)
		{
			gFileInfoList.push_back(fileInfo);
			AfxWriteLogger(ghLogger, TEXT("Need Update File:%s"), fileInfo.szParseFile.c_str());
			continue;
		}
		else
		{
			LPTSTR pFileName = PathFindFileName(szFileName);
			LPTSTR pExtName  = PathFindExtension(szFileName);
			if (pFileName!= NULL && pExtName != NULL && lstrcmpi(pExtName, TEXT(".sys")) == 0)
			{
				TCHAR szSys[MAX_PATH] = {0};
				//GetSystemDirectory(szSys, MAX_PATH);
				//PathAddBackslash(szSys);
				//lstrcat(szSys, TEXT("drivers\\"));
				ExpandEnvironmentStrings(_T("%SYSTEMROOT%\\system32\\drivers\\"), szSys, MAX_PATH - 1);

				TCHAR szFileName[MAX_PATH];
				lstrcpy(szFileName, pFileName);
				// 以下两个驱动是需要改名的，其他的不需要改名
				if (lstrcmpi(szFileName, _T("kfdisk64.sys")) == 0)
				{
					lstrcpy(szFileName, _T("kfdisk.sys"));
				}
				else if (lstrcmpi(szFileName, _T("kpdisk64.sys")) == 0)
				{
					lstrcpy(szFileName, _T("kpdisk.sys"));
				}
				
				lstrcat(szSys, szFileName);
				if (fileInfo.dwCrc != AfxGetFileCrc32(szSys))
				{
					gFileInfoList.push_back(fileInfo);
					AfxWriteLogger(ghLogger, TEXT("Need Update File:%s"), fileInfo.szParseFile.c_str());
					continue;
				}
			}
		}
		//AfxWriteLogger(ghLogger, TEXT("CRC is same, not need update File:%s"), fileInfo.szParseFile.c_str());
	}
	return true;
}

static bool DownloadFile(tagFileInfo& fileInfo)
{
	static const DWORD FILE_BLOCK_SIZE = 0x8000;
	TCHAR szErrInfo[MAX_PATH] = {0};

	AfxCreateDirectory(fileInfo.szTmpName.c_str());
	HANDLE hFile = ::CreateFile(fileInfo.szTmpName.c_str(), GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
	{
		DWORD dwError = GetLastError();
		AfxGetWinErrorText(szErrInfo, _countof(szErrInfo), dwError);
		AfxWriteLogger(ghLogger, TEXT("open file fail:%d:%s"), dwError, szErrInfo);
		return false;
	}

	DWORD left = fileInfo.dwSize;
	while (left)
	{
		char szSendBuffer[PKG_MAXLENGTH] = {0}, szRecvBuffer[PKG_MAXLENGTH] = {0};

		CPkgHelper pkgSend(szSendBuffer, CMD_CLI_DOWNFILEBLOCK, OBJECT_CLIENT);
		DWORD dwBlkSize = left >= FILE_BLOCK_SIZE ? FILE_BLOCK_SIZE : left;
		pkgSend << 1 << fileInfo.szFileName << fileInfo.dwSize - left << dwBlkSize;
		if (ERROR_SUCCESS != AfxSocketExecuteLock(gSocket, szSendBuffer, pkgSend.GetLength(), szRecvBuffer, sizeof(szRecvBuffer)))
		{
			AfxWriteLogger(ghLogger, TEXT("download file block fail."));
			return false;
		}
		CPkgHelper pkgRecv(szRecvBuffer);
		if (!pkgRecv.IsStatusOk())
		{
			AfxWriteLogger(ghLogger, TEXT("down file fail."));
			return false;
		}
		if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
			return false;
		
		DWORD dwByteWrite = 0;
		if (!WriteFile(hFile, szRecvBuffer + sizeof(PkgHeader), dwBlkSize, &dwByteWrite, NULL) || dwByteWrite != dwByteWrite)
		{
			DWORD dwError = GetLastError();
			AfxGetWinErrorText(szErrInfo, _countof(szErrInfo), dwError);
			AfxWriteLogger(ghLogger, TEXT("write file fail:%d:%s"), dwError, szErrInfo);
			CloseHandle(hFile);
			return false;
		}
		left -= dwBlkSize;
	}

	SetEndOfFile(hFile);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME ft = {0};
		AfxUtcTime2FileTime(fileInfo.dwModifyTime, ft);
		SetFileTime(hFile, &ft, &ft, &ft);
	}
	CloseHandle(hFile);
	return true;
}

static bool DownloadFileList()
{
	for (std::vector<tagFileInfo>::iterator it = gFileInfoList.begin(); it != gFileInfoList.end(); ++it)
	{
		if (PathFileExists(it->szTmpName.c_str()))
		{
			SetFileAttributes(it->szTmpName.c_str(), FILE_ATTRIBUTE_NORMAL);
			DeleteFile(it->szTmpName.c_str());
		}
		if (WAIT_OBJECT_0 == WaitForSingleObject(ghExited, 0))
			return false;

		if (!DownloadFile(*it))
			return false;
	}
	return true;
}

static bool RebootClientSoft()
{
	TCHAR szTemp[MAX_PATH] = {0}, szPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, szTemp, MAX_PATH);
	_stprintf_s(szPath, _countof(szPath), TEXT("\"%s\" %s"), szTemp, AUTOUPDATE_PARAMETER);

	STARTUPINFO si = {sizeof(si)};
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi = {0};
	if (CreateProcess(NULL, szPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		AfxWriteLogger(ghLogger, TEXT("Run Self by AutoUpdate Success."));
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return true;
	}
	return false;
}

void EnabelWow64FsRedirection(BOOL is_enable)
{
	typedef BOOLEAN (WINAPI * LPFN_FUNC) (BOOLEAN);

	LPFN_FUNC func;
	BOOL bIsWow64 = FALSE;    
	func = (LPFN_FUNC)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"Wow64EnableWow64FsRedirection"); 

	if (func)
	{
		func(is_enable);
	}
}

bool ForceUpdateFile(CString szSrcfile, CString szDstFile, DWORD dwModifyTime, bool& bNeedReboot)
{
	AfxCreateDirectory(szDstFile);
	SetFileAttributes(szDstFile, FILE_ATTRIBUTE_NORMAL);
	DeleteFile(szDstFile);

	if (CopyFile(szSrcfile, szDstFile, FALSE))
	{
		AfxWriteLogger(ghLogger, TEXT("Copy File Success:%s."), (LPCTSTR)szDstFile);

		// 设置更改时间
		HANDLE hFile = CreateFile(szDstFile, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			FILETIME ft = {0};
			AfxUtcTime2FileTime(dwModifyTime, ft);
			SetFileTime(hFile, &ft, &ft, &ft);
			CloseHandle(hFile);
		}
		return true;
	}

	TCHAR szBuffer[MAX_PATH] = {0};
	DWORD dwError = GetLastError();
	AfxGetWinErrorText(szBuffer, _countof(szBuffer), dwError);
	AfxWriteLogger(ghLogger, TEXT("Copy File Fail:%s:%d:%s"), (LPCTSTR)szDstFile, dwError, szBuffer);

	tstring tmp = szDstFile + _RUN_TMP_FILE_;
	SetFileAttributes(tmp.c_str(), FILE_ATTRIBUTE_NORMAL);
	DeleteFile(tmp.c_str());

	if (!MoveFileEx(szDstFile, tmp.c_str(), MOVEFILE_REPLACE_EXISTING))
	{
		DWORD dwError = GetLastError();
		AfxGetWinErrorText(szBuffer, _countof(szBuffer), dwError);
		AfxWriteLogger(ghLogger, TEXT("Rename File Fail:%s:%d:%s"), (LPCTSTR)szDstFile, dwError, szBuffer);
		return false;
	}
	if (!CopyFile(szSrcfile, szDstFile, FALSE))
	{
		DWORD dwError = GetLastError();
		AfxGetWinErrorText(szBuffer, _countof(szBuffer), dwError);
		AfxWriteLogger(ghLogger, TEXT("ReCopy File Fail:%s:%d:%s"), (LPCTSTR)szDstFile, dwError, szBuffer);
		return false;
	}
	else
	{
		// 设置更改时间
		HANDLE hFile = CreateFile(szDstFile, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			FILETIME ft = {0};
			AfxUtcTime2FileTime(dwModifyTime, ft);
			SetFileTime(hFile, &ft, &ft, &ft);
			CloseHandle(hFile);
		}

		hy_PortFile(szDstFile);
		bNeedReboot = true;
		AfxWriteLogger(ghLogger, TEXT("ReCopy File :%s:Sucess"), (LPCTSTR)szDstFile);
		return true;
	}
}

static bool UpdateFile(bool& bNeedReboot)
{
	TCHAR szBuffer[MAX_PATH] = {0};
	bNeedReboot = false;

	for (std::vector<tagFileInfo>::iterator iter = gFileInfoList.begin(); iter != gFileInfoList.end(); ++iter)
	{
		if (!PathFileExists(iter->szTmpName.c_str()))
		{
			AfxWriteLogger(ghLogger, TEXT("download's tmp file is not exist:%s."), iter->szTmpName.c_str());
			return false;
		}

		// 拷贝临时文件到客户端目录
		if (!ForceUpdateFile(iter->szTmpName.c_str(), iter->szParseFile.c_str(), iter->dwModifyTime, bNeedReboot))
		{
			return false;
		}

		//AfxCreateDirectory(iter->szParseFile.c_str());
		//SetFileAttributes(iter->szParseFile.c_str(), FILE_ATTRIBUTE_NORMAL);
		//DeleteFile(iter->szParseFile.c_str());
		//if (CopyFile(iter->szTmpName.c_str(), iter->szParseFile.c_str(), FALSE))
		//{
		//	AfxWriteLogger(ghLogger, TEXT("Copy File Success:%s."), iter->szParseFile.c_str());
		//}
		//else
		//{
		//	DWORD dwError = GetLastError();
		//	AfxGetWinErrorText(szBuffer, _countof(szBuffer), dwError);
		//	AfxWriteLogger(ghLogger, TEXT("Copy File Fail:%s:%d:%s"), iter->szParseFile.c_str(), dwError, szBuffer);

		//	tstring tmp = iter->szParseFile + _RUN_TMP_FILE_;
		//	SetFileAttributes(tmp.c_str(), FILE_ATTRIBUTE_NORMAL);
		//	DeleteFile(tmp.c_str());
		//	if (!MoveFileEx(iter->szParseFile.c_str(), tmp.c_str(), MOVEFILE_REPLACE_EXISTING))
		//	{
		//		DWORD dwError = GetLastError();
		//		AfxGetWinErrorText(szBuffer, _countof(szBuffer), dwError);
		//		AfxWriteLogger(ghLogger, TEXT("Rename File Fail:%s:%d:%s"), iter->szParseFile.c_str(), dwError, szBuffer);
		//		return false;
		//	}
		//	if (!CopyFile(iter->szTmpName.c_str(), iter->szParseFile.c_str(), FALSE))
		//	{
		//		DWORD dwError = GetLastError();
		//		AfxGetWinErrorText(szBuffer, _countof(szBuffer), dwError);
		//		AfxWriteLogger(ghLogger, TEXT("ReCopy File Fail:%s:%d:%s"), iter->szParseFile.c_str(), dwError, szBuffer);
		//		return false;
		//	}
		//	else
		//	{
		//		bNeedReboot = true;
		//		AfxWriteLogger(ghLogger, TEXT("ReCopy File :%s:Sucess"), iter->szParseFile.c_str());
		//	}
		//}

		//HANDLE hFile = CreateFile(iter->szParseFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
		//	NULL, OPEN_EXISTING, 0, NULL);
		//if (hFile != INVALID_HANDLE_VALUE)
		//{
		//	FILETIME ft = {0};
		//	AfxUtcTime2FileTime(iter->dwModifyTime, ft);
		//	SetFileTime(hFile, &ft, &ft, &ft);
		//	CloseHandle(hFile);
		//}

		LPTSTR pFileName = PathFindFileName(iter->szFileName.c_str());
		LPTSTR pExtName  = PathFindExtension(iter->szFileName.c_str());
		if (pFileName!= NULL && pExtName != NULL && lstrcmpi(pExtName, TEXT(".sys")) == 0)
		{
			// 拷贝驱动
			TCHAR szFileName[MAX_PATH] = {0};
			lstrcpy(szFileName, pFileName);
			// 以下两个驱动是需要改名的，其他的不需要改名
			if (lstrcmpi(pFileName, _T("kfdisk64.sys")) == 0)
			{
				lstrcpy(szFileName, _T("kfdisk.sys"));
			}
			else if (lstrcmpi(pFileName, _T("kpdisk64.sys")) == 0)
			{
				lstrcpy(szFileName, _T("kpdisk.sys"));
			}

			TCHAR szSys[MAX_PATH] = {0};
			ExpandEnvironmentStrings(_T("%SYSTEMROOT%\\system32\\drivers\\"), szSys, MAX_PATH - 1);

			lstrcat(szSys, szFileName);
			if (CopyFile(iter->szTmpName.c_str(), szSys, FALSE) && hy_DriverProtected(szSys[0]))
			{
				hy_PortFile(szSys);
			}
		}
		else
		{
			// 拷贝需要放到system32或SysWOW64下的dll
			if (IsWow64())
			{
				// 64位系统
				if (lstrcmpi(pFileName, _T("kres64.dll")) == 0)
				{
					TCHAR szDst[MAX_PATH] = {0};
					ExpandEnvironmentStrings(_T("%SYSTEMROOT%\\system32\\kres64.dll"), szDst, MAX_PATH - 1);
					if (!::ForceUpdateFile(iter->szTmpName.c_str(), szDst, iter->dwModifyTime, bNeedReboot))
					{
						return false;
					}
				}
				else if (lstrcmpi(pFileName, _T("kshut64.dll")) == 0)
				{
					TCHAR szDst[MAX_PATH] = {0};
					ExpandEnvironmentStrings(_T("%SYSTEMROOT%\\system32\\kshut.dll"), szDst, MAX_PATH - 1);
					if (!::ForceUpdateFile(iter->szTmpName.c_str(), szDst, iter->dwModifyTime, bNeedReboot))
					{
						return false;
					}
				}
				else if (lstrcmpi(pFileName, _T("kshut.dll")) == 0)
				{
					TCHAR szDst[MAX_PATH] = {0};
					ExpandEnvironmentStrings(_T("%SYSTEMROOT%\\SysWOW64\\kshut.dll"), szDst, MAX_PATH - 1);
					if (!::ForceUpdateFile(iter->szTmpName.c_str(), szDst, iter->dwModifyTime, bNeedReboot))
					{
						return false;
					}
				}
			}
			else
			{
				// 32位系统
				if (lstrcmpi(pFileName, _T("kres.dll")) == 0 || lstrcmpi(pFileName, _T("kshut.dll")) == 0)
				{
					TCHAR szDst[MAX_PATH] = {0};
					ExpandEnvironmentStrings(_T("%SYSTEMROOT%\\system32\\"), szDst, MAX_PATH - 1);
					lstrcat(szDst, pFileName);

					if (!::ForceUpdateFile(iter->szTmpName.c_str(), szDst, iter->dwModifyTime, bNeedReboot))
					{
						return false;
					}
				}				
			}

		}
		hy_PortFile(iter->szParseFile.c_str());
	}

	AfxGetAppPath(szBuffer, MAX_PATH, _UPDATE_TMP_DIR_);
	AfxDeleteDirectory(szBuffer);
	gFileInfoList.clear();
	if(bNeedReboot)
	{
		RebootClientSoft();
	}
	return true;
}

bool AfxExecuteSoftUpdate(bool& bNeedReboot)
{
	AfxWriteLogger(ghLogger, TEXT("start auto update..."));

	TCHAR szPath[MAX_PATH] = {0};
	AfxDeleteMatchFile(AfxGetAppPath(szPath), TEXT("*")_RUN_TMP_FILE_);
	gFileInfoList.clear();
	bNeedReboot = false;

	DWORD bNotAutoUpdate = AfxGetSysLong(TEXT("system"), TEXT("noupdate"), 0);
	if (bNotAutoUpdate ==  1)
	{
		AfxWriteLogger(ghLogger, TEXT("not need update."));
	}
	else
	{
		EnabelWow64FsRedirection(FALSE);

		if (!GetNeedUpdateList())
		{
			EnabelWow64FsRedirection(TRUE);
			return false;
		}

		if (!DownloadFileList())
		{
			EnabelWow64FsRedirection(TRUE);
			return false;
		}
		
		if (!UpdateFile(bNeedReboot))
		{
			EnabelWow64FsRedirection(TRUE);
			return false;
		}
		EnabelWow64FsRedirection(TRUE);
	}
	AfxWriteLogger(ghLogger, TEXT("end soft auto udpate."));
	return true;
}