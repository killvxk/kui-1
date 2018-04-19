#pragma once
#include "unzip.h"


namespace OpenHMI
{
	// ------------------------------------------------------------------------------------------------------------------------
	// Summary:
	//   解压zip文件到指定路径, 并返回解压文件路径和文件名。
	// Parameters:
	//   lpszZipFullName   - 待解压 zip压缩包所在文件夹路径和zip压缩名称
	//   lpszUnZipPath     - 解压出来的文件 所存放位置的完整路径
	//                       此参数省略时，默认解压到exe程序所在文件夹下。
	// Returns:
	//   解压成功返回ZR_OK，解压失败返回错误码。
	// ------------------------------------------------------------------------------------------------------------------------
	ZRESULT ExtractZipToDir(LPCTSTR lpszZipFullName, LPCTSTR lpszUnZipPath = NULL);
}