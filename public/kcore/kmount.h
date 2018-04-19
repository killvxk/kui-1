#pragma once

//////////////////////////////////////////////////////////////////////////
//scsi disk driver function.

DWORD __stdcall ScsiDiskInstallDriver();

//加载一个虚拟盘分区.ip, port:网络序,DriverLetter,指定的客户机盘符,TempDirName指定临时文件的目录
DWORD __stdcall ScsiDiskMount(DWORD Ip, WORD Port, TCHAR DriverLetter, TCHAR TmpFileDriverLetter, BOOL bMirrorDisk = FALSE, int nDiskNum = 100, BOOL bIsAdmin = FALSE);

//卸载加载的虚拟盘分区。DriverLetter:虚拟盘符.
DWORD __stdcall ScsiDiskUnmount(TCHAR DriverLetter, BOOL bMirrorDisk = FALSE, int nDiskNum = 100, BOOL IsAdmin = FALSE);

DWORD __stdcall ScsiDiskQueryDisklessInfo(PDWORD DiskLess, PDWORD IsAdmin, PDWORD ServerIp);
DWORD __stdcall ScsiDiskQueryDisklessInfoEx(PDISKLESS_INFO pDiskLessInfo);

//利用diskpart.exe重新初始化磁盘
DWORD __stdcall ScsiDiskReinitImage(int nDiskNum, TCHAR cVol, DWORD nSizeInMb);

//利用format.exe重新格式化磁盘
DWORD __stdcall ScsiDiskReformatImage(TCHAR szVol);


//////////////////////////////////////////////////////////////////////////
//file disk driver function.
DWORD __stdcall FileDiskInstallDriver();

DWORD __stdcall FileDiskMount(DWORD DiskType, DWORD Ip, WORD Port, TCHAR DriverLetter, LPCTSTR UserId);

DWORD __stdcall FileDiskUnmount(TCHAR DriverLetter);