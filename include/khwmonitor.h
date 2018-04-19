#ifndef __khwmonitor_h__
#define __khwmonitor_h__

#include <tchar.h>
#include <string>
#include <sstream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma once


#ifndef KHWMONITOR_EXPORTS
#define HWMONITOR_API  _declspec(dllimport)
#else
#define HWMONITOR_API _declspec(dllexport)
#endif

#ifdef _UNICODE
#define tstring std::wstring
#define tstringstream  std::wstringstream
#else
#define tstring std::string
#define tstringstream  std::stringstream
#endif

//stru
//云更新控制台
#define MAX_NAME_LEN				64
struct tTemperature
{
	TCHAR	Machine[MAX_NAME_LEN];
	DWORD	CpuVal;
	DWORD	CpuMaxVal;
	DWORD	BoardVal;
	DWORD	BoardMaxVal;
	DWORD	DisplayVal;
	DWORD	DisplayMaxVal;
	DWORD	DiskVal;
	DWORD	DiskMaxVal;
	DWORD	CpuFanSpeed;
	DWORD	LastTime;
};
//CPU
typedef struct tagCPU
{
	DWORD dwID;
	TCHAR szModel[512];//型号
	TCHAR szSerialNumber[512];//序列号
	float fTemperature;//温度
	int   iFanSpeed[2];//风扇转速，风扇一般是1，最多一般不超过2个
}CPU, *LPCPU;
//显卡
typedef struct tagDisplayAdapter
{
	DWORD dwID;
	TCHAR szModel[512];//型号
	float fTemperature;//温度
}DISPLAYADAPTER, *LPDISPLAYADAPTER;
//主板
typedef struct tagMainBoard
{
	DWORD dwID;
	TCHAR szVendor[512];//制造厂商
	TCHAR szModel[512];//型号
	float fTemperature;//温度
	TCHAR szSerialNumber[512];//序列号
}MAINBOARD, *LPMAINBOARD;
//内存条
typedef struct tagMem
{
	DWORD dwID;
	TCHAR szModel[512];//型号
	int fSize;//大小（单位M）
}MEM, *LPMEM;
//声卡
typedef struct tagAudioCard
{
	DWORD dwID;
	TCHAR szModel[512];//型号
}AUDIOCARD, *LPAUDIOCARD;
//硬盘
typedef struct tagHardDisk
{
	DWORD dwID;
	TCHAR szModel[512];//型号
	UINT64 ullDiskSize;//硬盘大小
	float fTemperature;//温度
	bool  bSSDDisk;//是否是SSD
}HARDDISK, *LPHARDDISK;
//键盘
typedef struct tagKeysBoard
{
	DWORD dwID;
	TCHAR szModel[512];//型号
}KEYBOARD, *LPKEYBOARD;
//鼠标
typedef struct tagMouse
{
	DWORD dwID;
	TCHAR szModel[512];//型号
}MOUSE, *LPMOUSE;
//摄像头
typedef struct tagCamera
{
	DWORD dwID;
	TCHAR szModel[512];//型号
}CAMERA, *LPCAMERA;
//麦克风
typedef struct tagMicroPhone
{
	DWORD dwID;
	TCHAR szModel[512];//型号
}MICROPHONE, *LPMICROPHONE;

extern "C" {
	HWMONITOR_API bool __stdcall HWMONITOR_Init();
	HWMONITOR_API void __stdcall HWMONITOR_UnInit();

	HWMONITOR_API bool __stdcall HWMONITOR_GetCPUInfo(__out LPCPU lpCPUInfo);
	HWMONITOR_API int  __stdcall HWMONITOR_GetDisplayAdapterCount();
	HWMONITOR_API bool __stdcall HWMONITOR_GetDisplayAdapterInfo(__out LPDISPLAYADAPTER lpDisplayAdapterInfo,__in int iNextIndex);
	HWMONITOR_API bool __stdcall HWMONITOR_GetMainBoardInfo(__out LPMAINBOARD lpMainBoardInfo);
	HWMONITOR_API int  __stdcall HWMONITOR_GetMemCount();
	HWMONITOR_API bool __stdcall HWMONITOR_GetMemInfo(__out LPMEM lpMemInfo,__in int iNextIndex);
	HWMONITOR_API bool __stdcall HWMONITOR_GetAudioCardInfo(__out LPAUDIOCARD lpAudioCardInfo);
	HWMONITOR_API int  __stdcall HWMONITOR_GetHardDiskCount();
	HWMONITOR_API bool __stdcall HWMONITOR_GetHardDiskInfo(__out LPHARDDISK lpHardDiskInfo,__in int iNextIndex);
	HWMONITOR_API bool __stdcall HWMONITOR_GetKeysBoardInfo(__out LPKEYBOARD lpKeyBoardInfo);
	HWMONITOR_API bool __stdcall HWMONITOR_GetMouseInfo(__out LPMOUSE lpMouseInfo);
	HWMONITOR_API bool __stdcall HWMONITOR_GetCameraInfo(__out LPCAMERA lpCameraInfo);
	HWMONITOR_API bool __stdcall HWMONITOR_GetMicroPhoneInfo(__out LPMICROPHONE lpMicroPhoneInfo);

	//获取所有显卡信息
	HWMONITOR_API bool __stdcall HWMONITOR_GetDisplayAdapterInfoALL(
		__out LPDISPLAYADAPTER *&lpDisplayAdapterInfo,//多组显卡信息，使用完之后务必删除<delete>
		__out int& iDisplayAdapterCount//显卡个数
		);
	//获取所有内存条信息
	HWMONITOR_API bool __stdcall HWMONITOR_GetMemInfoAll(
		__out LPMEM *&lpMemInfo,//多组内存条信息，使用完之后务必删除<delete>
		__out int& iMemCount//内存条个数
		);
	//获取所有硬盘信息
	HWMONITOR_API bool __stdcall HWMONITOR_GetHardDiskInfoALL(
		__out LPHARDDISK *&lpHardDiskInfoArray,//多组硬盘信息，使用完之后务必删除<delete>
		__out int& iHardDiskCount//硬盘个数
		);

	//云更新控制台规范接口
	HWMONITOR_API bool __stdcall GetTemperature(__inout tTemperature* pBuffer);//返回所有硬件温度信息
	HWMONITOR_API bool __stdcall GetMainBoardNameInfo(__inout TCHAR* pBuffer,__in int nLenth);//获取主板信息
	HWMONITOR_API bool __stdcall GetRamInfo(__inout TCHAR* pBuffer,__in int nLenth);//获取内存信息
}

#endif