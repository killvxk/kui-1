#ifndef __hy_inc__
#define __hy_inc__

#include <string>

#define PAINFO_STATE_NORMAL			0
#define PAINFO_STATE_RESTORE		1
#define PAINFO_STATE_NOT_PROTECT	3

#define PAINFO_CMD_NONE				0		// Œﬁ√¸¡Ó
#define PAINFO_CMD_INIT				1
#define PAINFO_CMD_SAVE				2

bool hy_DriverProtected(TCHAR drv);
bool hy_LoadVirtualDevice(TCHAR drv);
tstring hy_ConvertPath(const tstring& path);
bool hy_CreateDirectory(const tstring& dir);
bool hy_GetDiskRoomSize(TCHAR drv, PLARGE_INTEGER pliSize);
bool hy_ExueteUpdateDir(LPCTSTR dir);
void hy_DyncaRefreDriver(TCHAR drv);

bool hy_IsInstall();
bool hy_Install(LPCTSTR lpProtectList);
bool hy_Uninstall();
bool AfxControlMachine(bool bReBoot = true);
bool hy_PortArea(LPCTSTR lpPortList, bool bReboot = true);
bool hy_UnPortArea(LPCTSTR lpPortList, bool bReboot = true);
bool hy_PortFile(LPCTSTR lpFile);
bool hy_InstallAndStartAntiDog(bool binstall, bool bStart);



#endif
bool IsWow64();
void EnabelWow64FsRedirection(BOOL is_enable);