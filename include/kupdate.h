#pragma once

#ifndef HAVE_NOT_USE_KCORE
#include "kcore.h"
#endif

// 与控制台交互命令
#define COM_SM_HEART							0x0001		// 控制台交互的心跳包
#define COM_SM_DELETEGAME						0x0002		// 删除游戏: 输入:gid(DWORD) + strServerPath(tstring)
#define COM_SM_ADDMOVEGAME						0x0003		// 添加游戏到移动队列：输入:gid(DWORD) + speed(DWORD) + strName(tstring) + strSrcPath(tstring) + strDestPath(tstring)
#define COM_SM_CLEARMOVEGAME					0x0004		// 清除移动队列：输入:gid(DWORD)(0:清除整个队列)
#define COM_SM_GETMOVEGAMESTATUS				0x0005		// 获取移动队列任务状态信息：输入:gid(DWORD)(0:获取整个队列状态信息) 
															// 输出:gid(DWORD) + Status(DWORD) + NeedsUpdateBytes(__int64) + UpdatedBytes(__int64)
															// 输出:count(DWORD) + count(DWORD) + count(DWORD) + 3 * array of (gid(DWORD) + Status(DWORD) + NeedsUpdateBytes(__int64) + UpdatedBytes(__int64))

// =================== P2P 常量   ===================
#define P2P_STATUS_WAITING						0x00000000L
#define P2P_STATUS_DWNIDX						0x00000001L
#define P2P_STATUS_COMPARE						0x00000002L
#define P2P_STATUS_DOWNING						0x00000003L
#define P2P_STATUS_COMMIT						0x00000004L
#define P2P_STATUS_ACTION						0x00000005L
#define P2P_STATUS_WAITCOMMIT					0x00000006L
#define P2P_STATUS_SUSPEND						0x00000007L
#define P2P_STATUS_WAITCOMPARE					0x00000008L
#define P2P_STATUS_PRECOMPARE					0x00000009L
#define P2P_STATUS_AFTERCOMPARE					0x0000000aL
#define P2P_STATUS_ERROR						0xfe000000L
#define P2P_STATUS_FINISH						0xff000000L

// 与控制台交互命令
#define P2P_SM_ADDTASK							0x1001		//	添加下载任务: 输入:count(DWORD) + array of (gid(DWORD) + force(DWORD:1/0表示是否强制更新))
#define P2P_SM_STOPTASK							0x1002		//	停止下载任务: 输入:count(DWORD) + array of (gid(DWORD)).
#define P2P_SM_SUSPENDTASK						0x1003		//	暂停下载任务: 输入:count(DWORD) + array of (gid(DWORD)).
#define P2P_SM_RESUMETASK						0x1004		//	恢复下载任务: 输入:count(DWORD) + array of (gid(DWORD)).
#define P2P_SM_ADJUSTTASK						0x1005		//	置顶，置底:   输入:gid(DWORD) + flag(DWORD:1:0)(置顶：置底)
#define P2P_SM_GETALLTASK						0x1006		//	取所有下载任务的状态:输出:xml（string).
#define P2P_SM_GETALLTASKID						0x1007		//	获取所有下载任务ID: 输出:count(DWORD) + array of (gid(DWORD)).
#define P2P_SM_FORCEUPDATE						0x1008		//  通知三层插件强制修复某个gid的游戏: 输入:gid(DWORD).
#define P2P_SM_UPDATELICENCE					0x1009		//	设置许可证到三层: 输入: flag1(DWORD) + flag2(DWORD) + nid(DWORD) +																  //  lasttime(DWORD) + session(tstring)
#define P2P_SM_CHECKPORTMAP						0x100A		//	检查三层端口映射是否生效: 输入:port(DWORD)；输出:result(bool)
#define P2P_SM_START_NOW						0x100B		//	立即开始任务：输入:count(DWORD) + array of (gid(DWORD)).
// =====================================================


// =================== UPDATE 常量  ====================
#define PKG_SUCCESS								0
#define PKG_ERROR								1

// update define.
#define UPDATE_FLAG_FORCE_UPDATE				0x01		//	强制更新
#define UPDATE_FLAG_QUICK_COMPARE				0x02		//	快速更新	//与强制更新只能二选一。
#define UPDATE_FLAG_DELETE_MOREFILE				0x04		//	执行删除多余文件
#define UPDATE_FLAG_DELETE_GAME					0x08		//	空间不足时，执行删除点击率低的游戏。
#define UPDATE_FLAG_NOTIFY_MAKEIDX				0x10		//	当索引不存在或者与磁盘上文件不符时，通知服务器重新制作索引
#define UPDATE_FLAG_NOTNOTIFY_INXERROR			0x20		//	不通知三层修复
#define UPDATE_FLAG_NOFILENOBUFFER				0x40		//	不开启文件NOBUFFER
#define UPDATE_FLAG_SYNCPKGMODE					0x80		//	小包发送
#define	UPDATE_FLAG_IS_SSD						0x100		//	是SSD

#define UPDATE_STATUS_FINISH					0x01		//	更新成功完成(单)
#define UPDATE_STAUTS_COMPARE					0x02		//	对比文件
#define UPDATE_STATUS_UPDATE					0x04		//	更新文件块
#define UPDATE_STATUS_DELETE_MOREFILE			0x08		//	删除多余文件
#define UPDATE_STATUS_DELETE_GAME				0x010		//	删除游戏
#define UPDATE_STATUS_WAITING					0x020		//	正在排队
#define UPDATE_STATUS_FINISHALL					0x040		//	更新成功完成
#define UPDATE_STATUS_ERROR						0xffffffff	//	更新出错

#define DELETE_STATUS_FINISH					0x01		//	删除成功完成(单)
#define DELETE_STATUS_PATH						0x02		//	计算本地路径
#define DELETE_STATUS_DELETE_GAME				0x04		//	删除游戏
#define DELETE_STATUS_WAITING					0x08		//	正在排队
#define DELETE_STATUS_FINISHALL					0x10		//	删除成功完成
#define DELETE_STATUS_ERROR						0xffffffff	//	删除出错

// 更新类型
#define LIBUPDATE_TYPE_PUSH						0
#define LIBUPDATE_TYPE_SYNC						1
#define LIBUPDATE_TYPE_UPDATE					2
#define LIBUPDATE_TYPE_MOVE						3

// 强推
#define PUSH_SM_SETGAME							0
#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN							64
#endif


// 与控制台交互命令
#define UPDATE_SM_ADDTASK						0x2001		// 添加同步任务: 输入:count(DWORD) + array of (gid(DWORD) + nForce(DWORD:1:0))
#define UPDATE_SM_ALLTASK						0x2002		// 刷新同步队列: 输入:
#define UPDATE_SM_ALLTASKSTATUS					0x2003		// 取所有同步任务的状态: 输入:Detail(DWORD:1:0) 输出:count(DWORD) + count(DWORD)  + 2 * array of (gid(DWORD) + Status(DWORD) + NeedsUpdateBytes(__int64) + UpdatedBytes(__int64) + Speed(DWORD)).
#define UPDATE_SM_ADJUSTTASK					0x2004		// 置顶，置底:   输入:gid(DWORD) + flag(DWORD:1:0)(置顶：置底)
#define UPDATE_SM_FRESH							0x2005		// 刷新队列:	 输入:
#define UPDATE_SM_GETUPDATGAME					0x2006		// 获取内网更新的状态信息: 输出：xml文件
#define UPDATE_SM_GETLOCALPATH					0x2007		// 获取游戏本地路径：输入: count(DWORD) + array of gid(DWORD), 输出: count(DWORD) + array of (gid(DWORD) + path(tstring))
#define UPDATE_SM_CANCELTASK					0x2008      // 取消同步任务：输入:count(DWORD) + array of (gid(DWORD)

//	与kapp上报中心相关命令 
#define KUPDATE_APP_IDCUPDATECOUNTS				0x3301		// 中心今日更新数据总和:输入: 无 输出: DWORD(总数)
#define KUPDATE_APP_DOWNLIST					0x3303		// 获取三层信息，游戏队列（当天已完成/正在下载）：输入: 无 输出: DWORD(当天已完成) + DWORD(正在下载)
#define KUPDATE_APP_DOWNSPEED					0x3304		// 获取当前下载速度：输入: 无 输出: DWORD(速度 kb/s)
#define KUPDATE_APP_NEEDDOWNCOUNTS				0x3305		// 获取本地版本与中心版本不匹配的(自动更新)统计个数：输入: 无 输出: DWORD(总数)
#define KUPDATE_APP_UPFAILDCOUNTS				0x3306		// 获取更新失败统计个数(获取一次，计数值清零)：输入: 无 输出: DWORD(总数)
#define KUPDATE_APP_SYNCFAILCOUNTS				0x3307		// 获取同步失败统计个数(获取一次，计数值清零)：输入: 无 输出: DWORD(总数)
// =====================================================

START_NAMESPACE_KCORE

/// 推送游戏属性
typedef struct tagPushGameinfo
{
	DWORD gid;
	DWORD flag;
	DWORD speed;
	TCHAR Name[MAX_NAME_LEN];
	TCHAR SvrPath[MAX_PATH];
	TCHAR CliPath[MAX_PATH];
}tagPushGameinfo;

// idx file define
typedef enum Idx_UI_Status
{
	IDX_STATUS_CALLBACK			=   0,			//	only for exit,ui not show the status.
	IDX_STATUS_PARSE_OLDIDX		=	1,			//	only for check.wParam = 0, lParam = 0.
	IDX_STATUS_SCAN_DIRECTORY	=	2,			//	wParam = 0, lParam = 0.
	IDX_STATUS_GET_FILESIZE		=	3,			//	wParam = lowsize. lParam = highsize.
	IDX_STATUS_CALCRC			=	4,			//	wParam = progress.(n:0-100).lParam = 0
	IDX_STATUS_CHECK_IDXFILE	=	5,			//	only for check.wParam = progress(n:0-100), lParam = 0.
	IDX_STATUS_NOTIFY			=	6,			//	wParam = LPCTSTR. text tip information.
	IDX_NO_DIRECTORY			=	7,			//	
	IDX_STATUS_INFO				=	8,
	IDX_STATUS_FINISH			=   0xffffffff	//	wParam = erroce code.(0:success.).lParam:0
}Idx_UI_Status;

// ====================== 索引文件相关导出函数 ===============================

/// 如果需要中断，则返回false。
typedef bool (__stdcall *IDXUICALLBACK)(void* pcookie, Idx_UI_Status status, DWORD_PTR wParam, DWORD_PTR lPraram);

/// 生成索引文件
DWORD __stdcall Idxfile_Make(DWORD gid, LPCTSTR gamename, LPCTSTR dir, IDXUICALLBACK pCallback = NULL, void* pCookie = NULL);

/// 检验索引
DWORD __stdcall Idxfile_Check(DWORD gid, LPCTSTR gamename, LPCTSTR dir, IDXUICALLBACK pCallback, void* pCookie);
/// 
DWORD __stdcall Idxfile_DeleteMorefileEx(LPCTSTR dir, char* pidxfile, DWORD length);

/// 得到索引文件版本号.失败返回-1
DWORD __stdcall Idxfile_GetVersion(LPCTSTR file);

/// 得到内存索引文件版本号.失败返回-1
DWORD __stdcall Idxfile_GetVersionEx(char* pfile, DWORD length);

/// 从索引中得到游戏的大小.
__int64 __stdcall Idxfile_GetGameSize(LPCTSTR file);

/// 从索引中得到游戏的gid,gname.
bool __stdcall Idxfile_GetGameInfo(LPCTSTR file, DWORD& gid, LPTSTR gname = NULL);

// =====================================================

// ======================== 移动新增状态 ===============

#define UPDATE_STATUS_CLEAR			0xfffffffd		// 清理操作
#define UPDATE_STATUS_ERRORALL		0xfffffffe		// 错误消息

// =====================================================

/// 同步参数
typedef struct tagUpdateParameter
{
	DWORD			cbSize;				//	=sizeof(tagUpdateParameter)
	DWORD			UpdateType;			//	LIBUPDATE_TYPE_*.
	DWORD			UpdateFlag;			//	UPDATE_FLAG_*
	DWORD			speed;				//	limit speed.(k/s).
	DWORD			gid;				//	game id.
	TCHAR			gname[MAX_PATH];	//	game name.
	TCHAR			srcdir[MAX_PATH];	//	server directory of game.
	TCHAR			dstdir[MAX_PATH];	//	client directory of game.
	TCHAR			svrip[MAX_PATH];	//	server ip.
	TCHAR			cliip[MAX_PATH];	//	client ip.
}tagUpdateParameter;

/// 同步状态
typedef struct tagUpdateStatus
{
	DWORD   cbSize;						//	=sizeof(tagUpdateStatus)
	DWORD	Status;						//	更新状态
	__int64	m_qNeedsUpdateBytes;		//	需要更新的字节数
	__int64	m_qUpdatedBytes;			//	己经更新的字节数
	DWORD	m_dwLeftTime;				//	估计剩余时间
	DWORD	m_dwSpeed;					//	更新速度k/s
	TCHAR	Info[1024];					//	相关信息，文件名或者错误信息
}tagUpdateStatus;

/// 删除游戏状态
typedef struct tagDeleteGameStatus
{
	DWORD	Status;						//	删除状态
	TCHAR	Info[1024];					//	相关信息，文件名或者错误信息
}tagDeleteGameStatus;

// ====================== 同步相关导出函数 ===============================

typedef bool(__stdcall *UPDATE_BEFORE)(void * ptr, __int64 need_size);
typedef bool(__stdcall *UPDATE_AFTER)(void * ptr);
/// 开始更新
HANDLE __stdcall LibUpdate_StartUpdate(tagUpdateParameter& UpdateParameter);

/// 开始更新(带回调)
HANDLE __stdcall LibUpdate_StartUpdateBeforeAfter(tagUpdateParameter& UpdateParameter, 
	UPDATE_BEFORE pBefore, void *pBeforCont, 
	UPDATE_AFTER pAfter, void *pAfterCont);

/// 停止更新
void __stdcall LibUpdate_StopUpdate(HANDLE handle);

/// 获取更新状态
bool __stdcall LibUpdate_GetStatus(HANDLE handle, tagUpdateStatus& UpdateStatus);

// =====================================================

END_NAMESPACE_KCORE