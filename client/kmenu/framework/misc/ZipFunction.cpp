#include "atlstr.h"
#include "shlobj.h"
#include "ZipFunction.h"
#include <io.h>

namespace OpenHMI
{
	ZRESULT ExtractZipToDir(LPCTSTR lpszZipFullName, LPCTSTR lpszUnZipPath)
	{
		TCHAR buffer[MAX_PATH] = {0};
		CString strUnZipPath = lpszUnZipPath;
		DWORD zResult = ZR_OK;

		if (!strUnZipPath.IsEmpty())
		{
			// 如果文件路径不存在先创建,存在不做任何修改
			SHCreateDirectoryEx(NULL, lpszUnZipPath, NULL);
		}
		else
		{
			GetCurrentDirectory(MAX_PATH, (LPTSTR)&buffer);
			strUnZipPath = buffer;
			SHCreateDirectoryEx(NULL, strUnZipPath, NULL);
		}

		HZIP hz = OpenZip(lpszZipFullName, 0);
		ZIPENTRY ze;

		if (GetZipItem(hz, -1, &ze) == ZR_OK)
		{
			int numitems = ze.index;

			for (int zi = 0; zi < numitems; zi++)
			{
				ZIPENTRY ze;
				zResult = GetZipItem(hz, zi, &ze);
				if (zResult != ZR_OK)
				{
					CloseZip(hz);
					return zResult;
				}

				zResult = UnzipItem(hz, zi, (CString)strUnZipPath + _T("\\") + ze.name);
				if (zResult != ZR_OK)
				{
					CloseZip(hz);
					return zResult;
				}
			}
		}
		CloseZip(hz);
		return zResult;
	}
}