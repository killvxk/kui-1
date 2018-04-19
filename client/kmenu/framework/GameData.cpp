#include "GameData.h"
#include "./framework/EResLanguage.h"
#include <ShlObj.h>

CString AfxUDExpandPath2(const CString& filter, const CString& GamePath, CString& root, CString& match, CString& dir)
{
	LPCTSTR SUB_KEY = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders\\");
	CString dest(filter);
	int start = dest.Find(TEXT('%'));
	int end   = dest.ReverseFind(TEXT('%'));
	if (start != -1 && end != -1 && end > start)
	{
		TCHAR path[MAX_PATH] = {0}, realpath[MAX_PATH] = {0};
		DWORD dwType = REG_EXPAND_SZ;
		DWORD dwSize = _countof(path);

		CString sub = dest.Mid(start, end - start + 1);
		if (sub.CompareNoCase(TEXT("%appdata%")) == 0)
		{
			SHGetValue(HKEY_CURRENT_USER, SUB_KEY, TEXT("AppData"), &dwType, path, &dwSize);
			if (lstrlen(path) == 0)
				SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path);
			PathRemoveBackslash(path);
		}
		else if (sub.CompareNoCase(TEXT("%localappdata%")) == 0)
		{
			SHGetValue(HKEY_CURRENT_USER, SUB_KEY, TEXT("Local AppData"), &dwType, path, &dwSize);
			if (lstrlen(path) == 0)
				SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path);
			PathRemoveBackslash(path);
		}
		else if (sub.CompareNoCase(TEXT("%allappdata%")) == 0)
		{
			SHGetValue(HKEY_CURRENT_USER, SUB_KEY, TEXT("AllUsers AppData"), &dwType, path, &dwSize);
			if (lstrlen(path) == 0)
				SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, path);
			PathRemoveBackslash(path);
		}
		else if (sub.CompareNoCase(TEXT("%document%")) == 0)
		{
			SHGetValue(HKEY_CURRENT_USER, SUB_KEY, TEXT("Personal"), &dwType, path, &dwSize);
			if (lstrlen(path) == 0)
				SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path);
			PathRemoveBackslash(path);
		}
		else if (sub.CompareNoCase(TEXT("%currentuser%")) == 0)
		{
			ExpandEnvironmentStrings(TEXT("%USERPROFILE%"), path, _countof(path));
			PathRemoveBackslash(path);
		}
		else if (sub.CompareNoCase(TEXT("%gamedir%")) == 0)
		{
			lstrcpy(path, GamePath);
			PathRemoveBackslash(path);
		}

		if (!sub.IsEmpty())
		{
			dir = path;
			match = sub;
			dest.Delete(start, end - start + 1);
			dest.Insert(start, path);
		}
	}
	root = dest;
	int pos = root.ReverseFind(TEXT('\\'));
	if (pos != -1)
		root.Delete(pos+1, root.GetLength());
	return dest;
}

void AfxUDGetAllFile2(const CString& filter, OpenHMI::Array2<fileInfo>& info, const CString& root, const CString& match, const CString& dir)
{
	WIN32_FIND_DATA wfd = {0};
	HANDLE hFinder = FindFirstFile(filter, &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return ;

	do
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmp(wfd.cFileName, TEXT(".")) != 0 && lstrcmp(wfd.cFileName, TEXT("..")) != 0)
			{
				CString newroot   = root + wfd.cFileName + TEXT("\\");
				CString newfilter = root + wfd.cFileName + TEXT("\\*.*");
				AfxUDGetAllFile2(newfilter, info, newroot, match, dir);
			}
		}
		else
		{
			CString FileName = root + wfd.cFileName;
			if (!dir.IsEmpty())
			{
				FileName.Delete(0, dir.GetLength());
				FileName.Insert(0, match);
			}

			CString szText;
			if (wfd.nFileSizeLow >= 100 * 1024)
				szText.Format(TEXT("%.2fMB"), wfd.nFileSizeLow / 1024.0 / 1024.0);
			else
				szText.Format(TEXT("%.2fKB"), wfd.nFileSizeLow / 1024.0);
			fileInfo i(root + wfd.cFileName, szText, FileName);

			info.addItem(i);
		}
	}
	while (FindNextFile(hFinder, &wfd) == TRUE);
	FindClose(hFinder);
}

void AfxUDGetAllSaveDir2(const CString& GameName, CString& SaveDir)
{
	SaveDir = TEXT("");
	CString path, strTmp;
	strTmp = (const OpenHMI::CHAR*)OpenHMI::CLanguage::getString(OpenHMI::RES_LAG_GAME_SAVE);
	path.Format(TEXT("%c:\\%s\\%s\\*.*"), AfxGetDbMgr()->GetUDiskUnmoutDriver(), (LPCTSTR)strTmp, (LPCTSTR)GameName);

	WIN32_FIND_DATA wfd = {0};
	HANDLE hFinder = FindFirstFile(path, &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return ;

	do
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmp(wfd.cFileName, TEXT(".")) != 0 && lstrcmp(wfd.cFileName, TEXT("..")) != 0)
			{
				SaveDir += wfd.cFileName + CString(TEXT("|"));
			}
		}
	}
	while (FindNextFile(hFinder, &wfd) == TRUE);
	FindClose(hFinder);

	if (SaveDir.GetLength())
		SaveDir.Delete(SaveDir.GetLength() - 1);
}

void AfxUDCreateDirectory2(const CString& dir)
{
	int end = dir.ReverseFind(TEXT('\\'));
	int pt  = dir.Find(TEXT('\\'));

	while(pt != -1 && pt <= end)
	{
		tstring path = dir.Mid(0, pt+1);
		DWORD dwRet = GetFileAttributes(path.c_str());
		if(dwRet == INVALID_FILE_ATTRIBUTES)			
			CreateDirectory(path.c_str(), NULL);
		pt = dir.Find(TEXT('\\'), pt+1);
	}
}

bool AfxUDCopyFile2(const CString& root, const CString& match, const CString& dir)
{
	WIN32_FIND_DATA wfd = {0};
	HANDLE hFinder = FindFirstFile(root + TEXT("*.*"), &wfd);
	if (hFinder == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmp(wfd.cFileName, TEXT(".")) != 0 && lstrcmp(wfd.cFileName, TEXT("..")) != 0)
			{
				CString Subdir = root + wfd.cFileName + TEXT("\\");
				if (!AfxUDCopyFile2(Subdir, match, dir))
				{
					FindClose(hFinder);
					return false;
				}
			}
		}
		else
		{
			CString SrcFileName = root + wfd.cFileName;
			CString DstFileName = SrcFileName;
			if (!match.IsEmpty() && _tcsnicmp(DstFileName, match, match.GetLength()) == 0)
			{
				DstFileName.Delete(0, match.GetLength());
				DstFileName.Insert(0, dir);
			}

			DWORD dwAttr = GetFileAttributes(SrcFileName);
			AfxUDCreateDirectory2(DstFileName);
			SetFileAttributes(DstFileName, FILE_ATTRIBUTE_NORMAL);

			HANDLE hFileSrc = CreateFile(SrcFileName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			HANDLE hFileDst = CreateFile(DstFileName, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
			if (hFileSrc == INVALID_HANDLE_VALUE || hFileDst == INVALID_HANDLE_VALUE)
			{
				if (hFileSrc != INVALID_HANDLE_VALUE)
					CloseHandle(hFileSrc);
				if (hFileDst != INVALID_HANDLE_VALUE)
					CloseHandle(hFileDst);
				FindClose(hFinder);
				return false;
			}

			FILETIME ft = {0};
			GetFileTime(hFileSrc, NULL, NULL, &ft);
			DWORD left = GetFileSize(hFileSrc, NULL);

			DWORD dwReadBytes = 0, dwWriteBytes = 0;
			char buf[0x10000] = {0};
			while (left)
			{
				if (!ReadFile(hFileSrc, buf, _countof(buf), &dwReadBytes, NULL))
					break;
				if (!WriteFile(hFileDst, buf, dwReadBytes, &dwWriteBytes, NULL))
					break;
				left -= dwWriteBytes;
			}
			SetEndOfFile(hFileDst);
			SetFileAttributes(DstFileName, dwAttr);
			SetFileTime(hFileDst, NULL, NULL, &ft);
			CloseHandle(hFileSrc);
			CloseHandle(hFileDst);
			if (left)
			{
				FindClose(hFinder);
				return false;
			}
		}
	}
	while (FindNextFile(hFinder, &wfd) == TRUE);
	FindClose(hFinder);
	return true;
}

