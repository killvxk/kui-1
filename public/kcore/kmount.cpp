#include "stdafx.h"
#include "kcore.h"
#include <ShlObj.h>
#include <WinIoCtl.h>
#include <dbt.h>
#include <comutil.h>
#include "kdisk.h"

START_NAMESPACE_KCORE

#define DRIVERDEVICENAME		TEXT("\\\\.\\KScsiDisk")
#define HARDWAREID				TEXT("KScsiDiskAdapter\0\0")
#define HARDWAREID_SIZE			sizeof(HARDWAREID)
#define KSCSIDISK_INF_NAME		TEXT("KScsiDisk.inf")
#define KSCSIDISK_SYS_NAME		TEXT("KScsiDisk.sys")

#pragma warning(disable:4200)
#pragma warning(disable:4996)

#define IOCTL_VDISK_MOUNT				CTL_CODE(0x8000, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS|FILE_WRITE_ACCESS)
#define IOCTL_VDISK_UNMOUNT				CTL_CODE(0x8000, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS|FILE_WRITE_ACCESS)
#define IOCTL_VDISK_QUERY_DISKLESS		CTL_CODE(0x8000, 0x803, METHOD_BUFFERED, FILE_READ_ACCESS|FILE_WRITE_ACCESS)

#define	MODE_READ_ONLY		0
#define	MODE_TEMP_WRITE		1
#define	MODE_REAL_READWRITE	2
#define	MODE_MEMORY_DISK	4

#pragma pack(push, 1)

struct tag_MOUNT_PACK
{
	BYTE		dwDisk;
	BYTE		bIsAdminClient;
	BYTE		bAdminClientNumber;
	BYTE		bDelTempWritebackFile;
	WORD		wUserClientNumber;
	WORD		dwMode;
	WORD		wReserved;
	DWORD		dwCacheSize;
	DWORD		dwCacheBlockSize;
	//	LONGLONG	qwReseaved;

	LONGLONG	qwStartOffset;
	LONGLONG	qwSize;
	union{
		LONGLONG		dwSourceMac;
		struct tag_ip
		{
			DWORD	dwSourceIP;
			DWORD	dwSourcePort;
		}ip;
	}Source;

	union{
		LONGLONG		dwTempMac;
		struct tag_ip
		{
			DWORD	dwTempIp;
			DWORD	dwTempPort;
		}ip;
	}Temp;
	BYTE	bIsPreRead:1;
	BYTE	bBlockSize:7;		//K
	WORD	wReadOneCacheSize;		//K
	WORD	wAllReadCacheSize;	//K
	BYTE	bPreReadThread;//

	BYTE	bReseaved0[2];
	//	LONGLONG	qwVerValue;

	DWORD		dwReadLocateCacheSize;
	DWORD		dwReseaved1;
	//	LONGLONG	qwReseaved1;
	char		szSourceDataPath[MAX_PATH];
	union{
		char		szTempDataPath[MAX_PATH];
		union{
			LONGLONG dwSourceMacList[MAX_PATH/sizeof(LONGLONG)];
			struct tag_ip
			{
				DWORD	dwSourceIP;
				DWORD	dwSourcePort;
			}iplist[MAX_PATH/sizeof(struct tag_ip)];

		};
	};
};

typedef struct
{
	BOOLEAN			bMirrorDisk;
	//for kvdisk
	ULONG			ip;
	USHORT			port;
	WCHAR			DriveLetter;		//要挂出的盘符
	WCHAR			TempDriveLetter;	//回写文件盘符
	BOOLEAN			bMimicDiskHeader;
	//for mirror disk
	tag_MOUNT_PACK	MountPack;
}MOUNT_CTX, *LPMOUNT_CTX;

#pragma pack()

DWORD __stdcall ScsiDiskInstallDriver()
{
	return ERROR_INVALID_FUNCTION;
}

DWORD __stdcall ScsiDiskMount(DWORD Ip, WORD Port, TCHAR DriverLetter, TCHAR TmpFileDriverLetter, BOOL bMirrorDisk, int nDiskNum, BOOL bIsAdmin)
{
	DriverLetter = _totupper(DriverLetter);
	TmpFileDriverLetter = _totupper(TmpFileDriverLetter);
	HANDLE hDevice = CreateFile(DRIVERDEVICENAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
		return GetLastError();

	MOUNT_CTX mc = { 0 };
	mc.bMirrorDisk = bMirrorDisk;
	mc.DriveLetter = DriverLetter;
	mc.TempDriveLetter = TmpFileDriverLetter;
	mc.ip = Ip;
	mc.port = Port;
	if(bMirrorDisk)
	{
		mc.MountPack.dwMode = MODE_REAL_READWRITE;
		mc.MountPack.dwDisk = nDiskNum;
		mc.MountPack.bIsAdminClient = bIsAdmin;
		if (!bMirrorDisk)
		{
			//mc.MountPack.bIsPreRead = 0;
			//mc.MountPack.bPreReadThread = 0;
			//mc.MountPack.wReadOneCacheSize = 0;
			//mc.MountPack.wAllReadCacheSize = 0;
		}
		mc.MountPack.Source.ip.dwSourceIP = Ip;
		mc.MountPack.Source.ip.dwSourcePort = Port;
	}

	DWORD BytesReturned = 0;
	if (!DeviceIoControl(hDevice, IOCTL_VDISK_MOUNT, &mc, sizeof(mc), NULL, 0, &BytesReturned, NULL))
	{
		DWORD dwError = GetLastError();
		CloseHandle(hDevice);
		return dwError;
	}
	CloseHandle(hDevice);
	return ERROR_SUCCESS;
}

DWORD __stdcall ScsiDiskUnmount(TCHAR DriverLetter, BOOL bMirrorDisk, int nDiskNum, BOOL bIsAdmin)
{
	DriverLetter = _totupper(DriverLetter);
	HANDLE hDevice = CreateFile(DRIVERDEVICENAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
		return GetLastError();

	MOUNT_CTX	mc = { 0 };
	mc.bMirrorDisk = bMirrorDisk;
	mc.DriveLetter = DriverLetter;
	if(bMirrorDisk)
	{
		mc.MountPack.dwMode = MODE_REAL_READWRITE;
		mc.MountPack.dwDisk = nDiskNum;
		mc.MountPack.bIsAdminClient = bIsAdmin;
		mc.MountPack.bIsPreRead = 0;
		mc.MountPack.bPreReadThread = 0;
		//mc.MountPack.dwSourceIP = Ip;
		//mc.MountPack.dwSourcePort = Port;
	}

	TCHAR szVol[MAX_PATH];
	_stprintf_s(szVol, _countof(szVol), _T("\\\\.\\%c:"), DriverLetter);
	HANDLE hVolDevice = CreateFile(szVol, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	if (hVolDevice != INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(hVolDevice);
		CloseHandle(hVolDevice);
	}

	DWORD   BytesReturned = 0;
	if (!DeviceIoControl(hDevice, IOCTL_VDISK_UNMOUNT, &mc, sizeof(MOUNT_CTX), NULL, 0, &BytesReturned, NULL))
	{
		DWORD dwError = GetLastError();
		CloseHandle(hDevice);
		return dwError;
	}	
	CloseHandle(hDevice);
	return ERROR_SUCCESS;
}

DWORD __stdcall ScsiDiskQueryDisklessInfo(PDWORD DiskLess, PDWORD IsAdmin, PDWORD ServerIp)
{
	HANDLE hDevice = CreateFile(DRIVERDEVICENAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
		return GetLastError();

	DWORD dwBytesReturned = 0;
	DISKLESS_INFO Info = {0};
	if (!DeviceIoControl(hDevice, IOCTL_VDISK_QUERY_DISKLESS, NULL, 0, &Info, sizeof(DISKLESS_INFO), &dwBytesReturned, NULL))
	{
		DWORD dwError = GetLastError();
		CloseHandle(hDevice);
		return dwError;
	}
	if (DiskLess)
		*DiskLess = Info.diskless;
	if (IsAdmin)
		*IsAdmin = Info.is_admin;
	if (ServerIp)
		*ServerIp = Info.server_ip;
	CloseHandle(hDevice);
	return ERROR_SUCCESS;
}

DWORD __stdcall ScsiDiskQueryDisklessInfoEx( PDISKLESS_INFO pDiskLessInfo )
{
	if(pDiskLessInfo == NULL)
		return ERROR_INVALID_PARAMETER;

	HANDLE hDevice = CreateFile(DRIVERDEVICENAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
		return GetLastError();

	DWORD dwBytesReturned = 0;
	DISKLESS_INFO Info = {0};
	if (!DeviceIoControl(hDevice, IOCTL_VDISK_QUERY_DISKLESS, NULL, 0, &Info, sizeof(DISKLESS_INFO), &dwBytesReturned, NULL))
	{
		DWORD dwError = GetLastError();
		CloseHandle(hDevice);
		return dwError;
	}
	memcpy(pDiskLessInfo,&Info,sizeof(DISKLESS_INFO));
	CloseHandle(hDevice);
	return ERROR_SUCCESS;
}

//利用diskpart.exe重新初始化磁盘
DWORD __stdcall ScsiDiskReinitImage(int nDiskNum, TCHAR cVol, DWORD nSizeInMb)
{
	//	select disk 1
	//	clean
	//	create partition primary size=10208
	//	active
	//	assign letter=V
	//	exit

	//create bat file.
	tstringstream stream;
	stream << TEXT("select disk ") << nDiskNum << TEXT("\r\n");
	stream << TEXT("clean\r\n");
	if (nSizeInMb != 0)
		stream << TEXT("create partition primary size=") << nSizeInMb << TEXT("\r\n");
	else
		stream << TEXT("create partition primary\r\n");
	stream << TEXT("active\r\n");
	stream << TEXT("assign letter=") << cVol << TEXT("\r\n");
	stream << TEXT("exit\r\n");

	TCHAR szPath[MAX_PATH] = { 0 };
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, TRUE);
	if (szPath[_tcslen(szPath)-1] != _T('\\'))
		_tcscat_s(szPath, MAX_PATH, _T("\\"));
	_tcscat_s(szPath, MAX_PATH, _T("reinit.bat"));
	AfxCreateDirectory(szPath, 0);

	HANDLE hFile = CreateFile(szPath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return GetLastError();
	DWORD	dwWritten = 0;
	tstring data = stream.str();
	WriteFile(hFile, data.c_str(), (DWORD)data.size(), &dwWritten, NULL);
	if (dwWritten != data.size())
	{
		DWORD dwError = GetLastError();
		CloseHandle(hFile);
		return dwError;
	}
	CloseHandle(hFile);

	//execute bat file.
	TCHAR strCmdLine[1024] = {0};
	_stprintf_s(strCmdLine, _countof(strCmdLine), TEXT("diskpart.exe /s \"%s\""), szPath);
	STARTUPINFO	si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	BOOL bCreated = CreateProcess(NULL, strCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (!bCreated)
		return GetLastError();

	::WaitForSingleObject(pi.hProcess, INFINITE);
	DWORD dwError = 0;
	GetExitCodeProcess(pi.hProcess, &dwError);
	if (dwError != 0)
	{
		CloseHandle(pi.hProcess);
		return dwError;
	}
	CloseHandle(pi.hProcess);

	return ERROR_SUCCESS;
}

//利用format.exe重新格式化磁盘
DWORD __stdcall ScsiDiskReformatImage(TCHAR szVol)
{
	TCHAR strCmdLine[MAX_PATH] = {0};
	_stprintf_s(strCmdLine, _countof(strCmdLine), TEXT("format.com %c: /FS:ntfs /Q /Y"), szVol);

	STARTUPINFO	si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	BOOL bCreated = CreateProcess(NULL, strCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (!bCreated)
		return GetLastError();
	::WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD dwError = 0;
	GetExitCodeProcess(pi.hProcess, &dwError);
	CloseHandle(pi.hProcess);
	return dwError;
}

//////////////////////////////////////////////////////////////////////////
//fdisk driver mount/umount function.
static void BroadDeviceChange(DWORD dwNotification, TCHAR Driver)
{
	DEV_BROADCAST_VOLUME    DevHdr = {0};
	DWORD    dwFlag = BSM_ALLCOMPONENTS;
	DWORD    volumeMask = 1 << (_totupper(Driver) - TEXT('A'));

	DevHdr.dbcv_devicetype  = DBT_DEVTYP_VOLUME;
	DevHdr.dbcv_size        = sizeof(DEV_BROADCAST_VOLUME);	
	DevHdr.dbcv_flags       = DBTF_NET;
	DevHdr.dbcv_unitmask    = volumeMask;

	BroadcastSystemMessage( BSF_IGNORECURRENTTASK | BSF_FORCEIFHUNG, &dwFlag, 
		WM_DEVICECHANGE, dwNotification, (LPARAM)&DevHdr );
}

DWORD __stdcall FileDiskInstallDriver()
{
	bool b64Bit = AfxIs64BitOpertionSystem();
	TCHAR szDriverFile[MAX_PATH] = {0};
	AfxGetAppPath(szDriverFile, _countof(szDriverFile), b64Bit ? KDISK_DRIVER_NAME_X64 : KDISK_DRIVER_NAME_X86);
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager == NULL)
		return GetLastError();
	SC_HANDLE schService = CreateService(schSCManager, KDISK_SERVICE_NAME, KDISK_SERVICE_NAME, 
		SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE,
		szDriverFile, NULL, NULL, NULL, NULL, NULL);
	if (schService == NULL)
	{
		DWORD dwError = GetLastError();
		if (ERROR_SERVICE_EXISTS != dwError)
		{
			CloseServiceHandle(schSCManager);
			return dwError;
		}
		schService = OpenService(schSCManager, KDISK_SERVICE_NAME, SERVICE_ALL_ACCESS);
		if (schService == NULL)
		{
			CloseServiceHandle(schSCManager);
			return GetLastError();
		}
	}
	StartService(schService, 0, NULL);
	SERVICE_STATUS status = {0};
	while (QueryServiceStatus(schService, &status))
	{
		Sleep(1000);
		if (status.dwCurrentState == SERVICE_RUNNING || status.dwCurrentState == SERVICE_STOPPED)
			break;
	}
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return (status.dwCurrentState == SERVICE_RUNNING);
}

DWORD __stdcall FileDiskMount(DWORD DiskType, DWORD Ip, WORD Port, TCHAR DriverLetter, LPCTSTR UserId)
{
	DriverLetter = _totupper(DriverLetter);
	TCHAR DeviceName[MAX_PATH] = {0};
	_stprintf_s(DeviceName, _countof(DeviceName), KDISK_WIN32_NAME, DriverLetter);
	HANDLE hDevice = CreateFile(DeviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		FileDiskInstallDriver();
		hDevice = CreateFile(DeviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
		if (hDevice == INVALID_HANDLE_VALUE)
			return GetLastError();
	}

	MOUNTINFO info = {0};
	info.DiskType = DiskType;
	info.Ip = Ip;
	info.Port = Port;
	lstrcpyW(info.UserId, _bstr_t(UserId == NULL ? TEXT("") : UserId));

	DWORD BytesReturned = 0;
	if (!DeviceIoControl(hDevice, IOCTL_KDISK_MOUNT, &info, sizeof(info), NULL, 0, &BytesReturned, NULL))
	{
		DWORD dwError = GetLastError();
		CloseHandle(hDevice);
		return dwError;
	}
	CloseHandle(hDevice);

	TCHAR VolumeName[MAX_PATH] = {0};
	_stprintf_s(VolumeName, _countof(VolumeName), TEXT("\\\\.\\%c:"), DriverLetter);
	_stprintf_s(DeviceName, _countof(DeviceName), KDISK_NTDEVICE_NAME, DriverLetter);
	if (!DefineDosDevice(DDD_RAW_TARGET_PATH, &VolumeName[4], DeviceName))
		return GetLastError();

	::SHChangeNotify(SHCNE_DRIVEADD, SHCNF_PATH, VolumeName + 4, NULL);
//	BroadDeviceChange( DBT_DEVICEARRIVAL, VolumeName[4]);
	return ERROR_SUCCESS;
}

DWORD __stdcall FileDiskUnmount(TCHAR DriverLetter)
{
	TCHAR Buffer[MAX_PATH] = { 0 };
	TCHAR DeviceName[MAX_PATH] = { 0 };
	TCHAR szMsg[512] = { 0 };
	DWORD dwError = ERROR_SUCCESS;
	HANDLE hDevice = INVALID_HANDLE_VALUE;

	_stprintf_s(szMsg, 512, _T("FileDiskUnmount IN Letter=%c\n"), DriverLetter);
	::OutputDebugString(szMsg);
	do{
		DriverLetter = _totupper(DriverLetter);
		_stprintf_s(Buffer, _countof(Buffer), TEXT("%c:"), DriverLetter);
		QueryDosDevice(Buffer, DeviceName, _countof(DeviceName));
		_stprintf_s(Buffer, _countof(Buffer), KDISK_NTDEVICE_NAME, DriverLetter);
		if (lstrcmpi(Buffer, DeviceName) != 0)
		{
			//_stprintf_s(szMsg, 512, _T("check parameter DeviceName=%s, \n"), DeviceName);
			//::OutputDebugString(szMsg);

			dwError = ERROR_INVALID_PARAMETER;
			break;
		}

		_stprintf_s(DeviceName, _countof(DeviceName), KDISK_WIN32_NAME, DriverLetter);
		hDevice = CreateFile(DeviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
		if (hDevice == INVALID_HANDLE_VALUE)
		{
			dwError = GetLastError();
			_stprintf_s(szMsg, 512, _T("CreateFile err=%d, file=%s\n"), dwError, DeviceName);
			::OutputDebugString(szMsg);
			break;
		}

		TCHAR szVol[MAX_PATH];
		_stprintf_s(szVol, _countof(szVol), _T("\\\\.\\%c:"), DriverLetter);
		HANDLE hVolDevice = CreateFile(szVol, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
		if (hVolDevice != INVALID_HANDLE_VALUE)
		{
			FlushFileBuffers(hVolDevice);
			CloseHandle(hVolDevice);
		}

		DWORD   BytesReturned = 0;
		if (!DeviceIoControl(hDevice, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &BytesReturned, NULL))
		{
			//dwError = GetLastError();
		}
		if (!DeviceIoControl(hDevice, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &BytesReturned, NULL))
		{
			dwError = GetLastError();
			_stprintf_s(szMsg, 512, _T("Dismount_Volulme err=%d\n"), dwError);
			::OutputDebugString(szMsg);
			DeviceIoControl(hDevice, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &BytesReturned, NULL);
			break;
		}
		if (!DeviceIoControl(hDevice, IOCTL_KDISK_UNMOUNT, NULL, 0, NULL, 0, &BytesReturned, NULL))
		{
			dwError = GetLastError();
			_stprintf_s(szMsg, 512, _T("Disk_mount err=%d\n"), dwError);
			::OutputDebugString(szMsg);
		}
		if (!DeviceIoControl(hDevice, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &BytesReturned, NULL))
		{
			//dwError = GetLastError();
		}
		CloseHandle(hDevice);
		hDevice = INVALID_HANDLE_VALUE;

		TCHAR VolumeName[MAX_PATH] = { 0 };
		_stprintf_s(VolumeName, _countof(VolumeName), TEXT("\\\\.\\%c:"), DriverLetter);
		if (!DefineDosDevice(DDD_REMOVE_DEFINITION, &VolumeName[4], NULL))
		{
			dwError = GetLastError();
			_stprintf_s(szMsg, 512, _T("Remove Volulme err=%d\n"), dwError);
			::OutputDebugString(szMsg);
		}

		dwError = ERROR_SUCCESS;
	} while (0);

	_stprintf_s(szMsg, 512, _T("FileDiskUnmount OUT err=%d, file=%s\n"), dwError, DeviceName);
	::OutputDebugString(szMsg);
	return dwError;
}

END_NAMESPACE_KCORE
