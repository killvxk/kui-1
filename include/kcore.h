#ifndef __kcore_h__
#define __kcore_h__

#pragma once

#include <tchar.h>
#include <winsock2.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "sqlite3.h"

#pragma comment(lib, "ws2_32.lib")

#if _MSC_VER > 1500

#include <memory>

namespace std { namespace tr1 {
	using std::shared_ptr;
}
}
#endif


#define START_NAMESPACE_KCORE		namespace kcore {
#define END_NAMESPACE_KCORE			}
#define USE_NAMESPACE_KCORE			using namespace kcore;

START_NAMESPACE_KCORE

#ifdef _UNICODE
#define tstring std::wstring
#define tstringstream  std::wstringstream
#else
#define tstring std::string
#define tstringstream  std::stringstream
#endif

#define ROUND_SIZE(size, round)						(((size) + (round) - 1) / (round) * (round))
#define ROUND_SIZE_4K(size)							(ROUND_SIZE((size), 0x1000))
#define ROUND_SIZE_64K(size)						(ROUND_SIZE((size), 0x10000))

template <typename T> void Delete_Interface(T& t)	{ if ((t)) { (t)->Release(); (t) = NULL; } }
template <typename T> void Delete_Pointer(T& t)		{ if ((t)) { delete (t);     (t) = NULL; } }
template <typename T> void Delete_Array(T& t)		{ if ((t)) { delete [](t);   (t) = NULL; } }

//各个EXE的单实例对象名。
#define KNBSERVICE_SINGLE_NAME				TEXT("Global\\YGX{87F4495C-8A1F-4f83-83CD-95B304F0170C}")
#define KNBUPDATE_SINGLE_NAME				TEXT("Global\\YGX{94CBEC72-CC62-4dc4-8035-388FCF5C7CD1}")
#define KNBVDISK_SINGLE_NAME				TEXT("Global\\YGX{D7362CE4-3FA4-4eb0-81A2-FCB4AD333A67}")
#define CONSOLE_SINGLE_NAME					TEXT("Global\\YGX{DBA3A10A-DCC8-4a66-A5B2-0A78BF12E5EE}")
#define KNBCLIENT_SINGLE_NAME				TEXT("Global\\YGX{E01770FE-6BE2-4024-9EA4-3462D5193D50}")
#define KNBMENU_SINGLE_NAME					TEXT("Global\\YGX{E3CDD1BA-5868-41e7-9A0D-02A5FD20E71F}")
#define KASSIST_SINGLE_NAME                 TEXT("Global\\YGX{E3FDG1BA-5868-4167-9AA4-02A5FB20EB1F}")
#define KASSISTHUB_SINGLE_NAME              TEXT("Global\\YGX{E3FDG1BA-78E8-4167-9AA4-02A5FB20EB1F}")


//内网更新与客户机推送的互斥事件名.
#define UPDATE_EVENT_NAME					TEXT("Global\\{8FC27450-DEF8-490a-9405-9A205853E8D8}")

//镜像同步与游戏同步的互斥事件名.
#define MIRROR_EVENT_NAME					TEXT("Global\\{77DB79A5-7295-4943-8691-989BF87157FC}")

//节点服务临时接管事件名
#define TAKEOVER_EVENT_NAME					TEXT("Global\\{D69F151A-DAC6-4795-9611-44DCF48395F5}")

//软件内部自动升级重启的命令行参数
#define AUTOUPDATE_PARAMETER				TEXT("-autoupdate")

//软件自动升级排除kvdisk
#define AUTOUPDATE_EXCLUDEVDISK				TEXT("-excludevdisk")

#define APP_TCP_PORT						5761
#define DB_TCP_PORT							5762
#define UPTSVR_TCP_PORT						5763
#define INUPT_TCP_PORT						5764
#define VDK_TCP_PORT						5765
#define UDK_TCP_PORT						5766
#define CLI_UDP_PORT						5767
#define CON_UDP_PORT						5768
#define SAFEDB_TCP_PORT						5769
#define VDK_TCP_BASEPORT					5770
#define IMG_TCP_BASEPORT					5870
#define CLI_TCP_PORT						5920
#define CHANNEL_TCP_PORT					5921
#define LOG_TCP_PORT						5922
#define DUMP_TCP_PORT						5923
#define DUMP_UDP_BASEPORT					5923
#define LOG_TCP_PORT_EX						5924
#define USERDISK_TCP_PORT					5925
#define CLEAER_TCP_PORT						5926

#define APP_BCAST_PORT						APP_TCP_PORT

inline WORD AfxGetVDiskPort(TCHAR szDriver) { return VDK_TCP_BASEPORT + _totupper(szDriver) - TEXT('C');}

#define DEF_AREA_GUID						TEXT("{0191103D-19D8-4F13-A647-BCD04DF9DE35}")
#define ALL_AREA_GUID						TEXT("{2579245B-7E08-46e2-9A66-24D3FBCC2829}")


inline bool AfxIsIdcAreaGuid(LPCTSTR guid);
//inline bool AfxIsIdcAreaName(LPCTSTR name);

#define CLASS_TOTAL_COUNT			17


#define CLASS_WL_GUID				TEXT("{1AF9A1A1-FF46-41B7-BC7A-D1FAE3E25B01}")	//网络游戏a
#define CLASS_DJ_GUID				TEXT("{2A687865-B630-4FA2-A5AC-EA9C9C4AABB2}")	//精品单机b
#define CLASS_QP_GUID				TEXT("{3A761798-CD7D-4761-81CB-8513B17F69E3}")	//棋牌游戏c
#define CLASS_DZ_GUID				TEXT("{4E33B955-25C7-40e9-B391-EE4CE049A592}")	//对战平台d
#define CLASS_LT_GUID				TEXT("{5AAAF78F-3FF2-4834-99AF-744233CEC6D4}")  //聊天交友e
#define CLASS_WY_GUID				TEXT("{6A9363C6-EC57-4A65-9D71-043B80A38F05}")	//网页游戏f
#define CLASS_XX_GUID				TEXT("{7AAD06BD-819C-48D1-9CB0-47DF498CF256}")	//休闲益智g
#define CLASS_PL_GUID				TEXT("{8A999DC2-2676-4FF2-8D62-7AB625F02857}")	//游戏辅助h
#define CLASS_CY_GUID				TEXT("{9A76073C-DDB1-4939-B85C-521F283B0198}")	//常用软件i
#define CLASS_YY_GUID				TEXT("{A258A554-6D7E-4369-8E67-4339CD7A0F59}")	//影音工具j
#define CLASS_GP_GUID				TEXT("{B3BAFF9D-5015-422A-A0CB-4CBC3DFAC49A}")	//股票证券k
#define CLASS_GQ_GUID				TEXT("{C945C2D1-E039-4c14-9D40-8789059493FB}")	//高清影视l
#define CLASS_ZX_GUID				TEXT("{DA2F5C0F-AC11-4720-A6CD-2C72EE891E6B}")	//在线音乐m
#define CLASS_XC_GUID				TEXT("{E0B0A5D9-E267-4cdf-8E84-0E8CA13C16F5}")	//秀场直播n
#define CLASS_SJ_GUID				TEXT("{F566239D-B794-4751-8BE5-A09A994C15D1}")	//手机游戏o
#define CLASS_BG_GUID				TEXT("{G6D14FF3-2EBC-4750-91C7-77C0284B090B}")	//办公软件p
#define CLASS_DPNP_GUID				TEXT("{4ADDE7DA-1AED-43BC-8803-93763DFF00EC}")	//显卡pnp q

//网吧添加的游戏类别
#define CLASS_NB_GUID				TEXT("{39F925AE-5972-454C-86DD-5A554F2C73A2}")  //网吧自定义游戏分类

inline LPCTSTR AfxGetGUIDByAlpha(TCHAR alpha);

//连接主服务器的客户端对象类型
#define OBJECT_UNKOWN					0			//未知
#define OBJECT_CONSOLE					1			//控制台
#define OBJECT_CLIENT					2			//客户端服务
#define OBJECT_MENU						3			//客户端游戏菜单
#define OBJECT_UPDATE					4			//更新服务器
#define OBJECT_VIDSK					5			//虚拟盘服务器
#define OBJECT_DOWNLOAD					6			//下载器（独立控制台）

//游戏运行方式
#define GAME_RUNTYPE_UNKNOW				0			//未设置.就是游戏不在该分区显示
#define GAME_RUNTYPE_VDISK				1			//虚拟盘方式运行
#define GAME_RUNTYPE_UPDATE				2			//本地更新，通过内网更新到客户机后，再运行
#define GAME_RUNTYPE_DIRECT				3			//直接运行，支持第三方虚拟盘，直接运行客户机的游戏

//快捷方式类型
#define SHORTCUT_TYPE_UNKNOWN			0			//未知
#define SHORTCUT_TYPE_GAME				1			//游戏快捷方式
#define SHORTCUT_TYPE_CLASS				2			//类别快捷方式
#define SHORTCUT_TYPE_TOOL				3			//工具快捷方式
#define SHORTCUT_TYPE_EXE				4			//EXE快捷方式
#define SHORTCUT_TYPE_URL				5			//网页快捷方式

//开机任务的类型
#define BOOT_TASK_UNKNOWN				0			//未知
#define BOOT_TASK_DOWNRUN				1			//下载并且执行
#define BOOT_TASK_RUNCLI				2			//执行客户机的文件

//磁盘分区类型
#define DISK_TYPE_UNKNOWN				0			//未定义
#define DISK_TYPE_GAME					1			//游戏盘
#define DISK_TYPE_WRITE					2			//回写盘
#define DISK_TYPE_IMAGE					3			//镜像象

//客户机状态：在线、离线
#define CLIENT_STATUS_ONLINE			1ul			//表示客户机在线
#define CLIENT_STATUS_OFFLINE			0ul			//表示客户机离线

//客户机通知硬件状态
#define CLIENT_HD_NORMAL				0ul			//表示客户机正常上报（没有变更)
#define CLIENT_HD_NEW					1ul			//表示新的客户机上报
#define CLIENT_HD_CHANGE				2ul			//表示客户机硬件变更

//客户机引导类型
#define BOOT_TYPE_NODISK				0ul			//表示客户机是无盘.
#define BOOT_TYPE_HARDDISK				1ul			//表示客户机是有盘
#define BOOT_TYPE_SSD					2ul			//表示客户机是SSD

//通知控制台日志类型
#define LOG_TYPE_APP					1			//主服务器日志
#define LOG_TYPE_P2P					2			//三层下载日志
#define LOG_TYPE_SYNC					3			//同步日志
#define LOG_TYPE_VDISK					4			//虚拟盘日志
#define LOG_TYPE_CACHE					5			//缓存日志
#define LOG_TYPE_SSD					6			//ssd日志


//游戏同步方式
#define GAME_SYNC_AUTO					0			//自动同步（推荐）
#define GAME_SYNC_DRIVER				1			//盘符同步
#define GAME_SYNC_SINGLE			    2			//单个同步

//软件事件ID
#define MEMORY_LACK						1			//内存不足
#define DRIVER_LOST						2			//盘符丢失
#define DRIVER_LACK						3			//磁盘空间不足
#define ADAPTER_MODIFY					4			//网卡速率发生变化
#define PORT_BEUSED						5			//端口被占用

//节点服务器状态
#define NODE_NORMAL						0			//正常状态(节点服务器)
#define NODE_TAKEOVER					1			//接管状态
#define NODE_ISMAIN						2			//节点服务器是主服务器
#define NODE_SVRIPNOTEXIST				3			//节点设置的主服务器IP与当前服务器不相符
//////////////////////////////////////////////////////////////////////////
//用户选项
#define OPT_U_USERID					TEXT("uuserid")				//用户id.
#define OPT_U_AGENTID					TEXT("uagentid")			//代理商id.
#define OPT_U_USERNAME					TEXT("uusername")			//用户名
#define OPT_U_PASSWORD					TEXT("uupassword")			//密码
#define OPT_U_REGDATE					TEXT("uregdate")			//用户注册日期
#define OPT_U_DATEEND					TEXT("udateend")			//过期日期
#define OPT_U_PROVINCE					TEXT("uuserprovince")		//用户所在省
#define OPT_U_CITY						TEXT("uusercity")			//用户所在城市
#define OPT_U_ERRINFO					TEXT("uErrInfo")			//用于控制台显示登陆的错误信息
#define OPT_U_ERRORID					TEXT("uErrId")				//用验证的错误id
#define OPT_U_RAMDOMNUM					TEXT("uramdomnum")			//用户没登录一次的随机码
#define OPT_U_NETBARNAME				TEXT("unetbarname")			//网吧名称
#define OPT_U_NETBARCONTACT				TEXT("unetbarcontact")		//网吧联系方式
#define OPT_U_NETBARMOBILE				TEXT("unetbarmobile")		//网吧手机
#define OPT_U_NETBARADDRESS				TEXT("unetbaraddress")		//网吧地址
#define OPT_U_AGENTNAME					TEXT("uagentname")			//代理商名称
#define OPT_U_AGENTCONTACT				TEXT("uagentcontact")		//代理商联系方式
#define OPT_U_AGENTMOBILE				TEXT("uagentmobile")		//代理商手机
#define OPT_U_INTERNETIP				TEXT("uinternetip")			//外网IP
#define OPT_U_ISSSD						TEXT("isssd")				//绑定ssd型号
#define OPT_U_SHFLAG1					TEXT("shflag1")				//三层本地参数控制
#define OPT_U_SHFLAG2					TEXT("shflag2")				//三层破解参数控制
#define OPT_U_LASTLOGINTIME				TEXT("lastlogintime")		//最后登录时间

#define OPT_U_BREAKPOINTPRO				TEXT("breakpointproenable")	//是否启用断电保护(1是,0否)


#define OPT_U_NOREMOTECONSOLE			TEXT("unoremoteconsole")	//禁止控制台远程登录(1禁止,0允许)
#define OPT_U_CTLPWD					TEXT("uctrlpassword")		//控制台密码.密码的crc值.

#define OPT_U_REMOTEPWD					TEXT("uctrlpassword")		//远程密码.密码的crc值（远程节点，客户机）.
#define OPT_U_DOWNVIEWPWD				TEXT("downviewpassword")	//下载查看器登录密码.
#define OPT_U_KCONSOLELOGINPWD			TEXT("kconsoleloginpassword")	//控制台登录密码.


//三层选项
#define OPT_D_TEMPDIR					TEXT("dtempdir")			//三层下载:游戏下载临时目录
#define OPT_D_TASKNUM					TEXT("dtasknum")			//三层下载:同时下载的最大任务数 (max:5,default:3)
#define OPT_D_FIBERMODE					TEXT("dfibermode")			//三层下载:下载模式,1表示光纤模式，0表示ADSL模式.
#define OPT_D_SPEED						TEXT("dspeed")				//三层下载:速度限制(n)(bytes/s)
#define OPT_D_TIMESPEED					TEXT("dtimespeed")			//三层下载:是否启动按时段限速.1表示启用。
#define OPT_D_SYNCSPEED					TEXT("dsyncspeed")			//同步限速(bytes/s).
#define OPT_D_PUSHSPEED					TEXT("dpushspeed")			//推送限速(bytes/s).
#define OPT_D_SYNCMODE					TEXT("dsyncmode")			//游戏同步方式(0.根据节点分区大小自动同步;1.优先同步到相同盘符(默认))
#define OPT_D_P2PACCELERATE				TEXT("dp2paccelerate")		//三层下载:是否启动P2P加速.1表示启用。
#define OPT_D_MADSLOPTIMIZE				TEXT("dmadsloptimize")		//三层下载:是否启动多线adsl优化.1表示启用。

#define OPT_D_AUTOGAMECLEAR				TEXT("autogameclear")		//是否自动清理游戏(1.是;0否)
#define OPT_D_RATESPACE					TEXT("ratespace")			//剩余百分之多少空间清理游戏(0表示不启用)

//客户端选项名
#define OPT_M_POWEROFFEABLE				TEXT("meablepoweroffe")		//控制台:断电保护
#define OPT_M_SVRREMOTEEABLE			TEXT("meablesvrremote")		//控制台:允许服务器远程操作.1表示允许
#define OPT_M_LOGINEABLE				TEXT("meablelogin")			//控制台:允许登录.1表示允许
#define OPT_M_CONSOLEPWD				TEXT("mconpwd")				//控制台:客服机开超级密码.(密码的CRC32字符串)
#define OPT_M_CLIPWD					TEXT("mclipwd")				//客户机:高级设置密码(密码的CRC32字符串)
#define OPT_M_GAMEDISK					TEXT("mgamedisk")			//客户机:存放游戏的磁盘.
#define OPT_M_RUNMENU					TEXT("mrunmenu")			//客户机:开机启动菜单.
#define OPT_M_WINDOWMODE				TEXT("mwinmode")			//客户机:开机启动菜单的模式:0全屏，1窗口.
#define OPT_M_HIDEMENU					TEXT("mhidemenu")			//客户机:启动游戏后是否隐藏菜单.1表示隐藏.
#define OPT_M_SYNCTIME					TEXT("msyntime")			//客户机:启动时同步服务器时间(0|1)
#define OPT_M_DELETEINDEXFILE			TEXT("mdeleteindexfile")	//客户机更新前是否删除索引文件
#define OPT_M_SAFECENTER				TEXT("msafecenter")			//客户机:是否启用安全中心(0|1)

#define OPT_M_DISPLAYPNP				TEXT("mdisplaypnp")			//客户机:是否启用显卡PNP(0|1)

#define OPT_M_SHOWEXCEPTIONTEMPERATURE	TEXT("mshowexceptiontemperature")	//客户机:仅显示异常温度的客户机.1表示启用
#define OPT_M_TEMPERATUREENABLE			TEXT("mtemperatureeable")	//客户机:是否启用温度监控.1表示启用
#define OPT_M_DUMPTUREENABLE			TEXT("mdumptureeable")		//客户机:是否启用蓝屏查看.1表示启用
#define OPT_M_HARDWAREENABLE			TEXT("mhardwareenable")		//客户机:是否启用硬件监控.1表示启用
#define OPT_M_SHOWEXCEPTIONCLIENT		TEXT("mshowexceptionclient")//客户机:仅显示异常硬件的客户机.1表示启用

#define OPT_M_SEVENDAYCHECKENABLE		TEXT("sevendaycheckenable")	//客户机:是否7天后检测更新.1表示启用
#define OPT_M_TEMPERATURECPU			TEXT("mtemperaturecpu")		//客户机:CPU最高温度
#define OPT_M_TEMPERATUREBOARD			TEXT("mtemperatureboard")	//客户机:主板最高温度
#define OPT_M_TEMPERATUREDISK			TEXT("mtemperaturedisk")	//客户机:硬盘最高温度
#define OPT_M_TEMPERATUREDISPLAY		TEXT("mtemperaturedisplay")	//客户机:显卡最高温度
#define OPT_M_IEHISTORYENABLE			TEXT("miehistoryenable")	//客户机:是否开启IE历史记录
#define OPT_M_IEHISTORYSAVEDAY			TEXT("miehistorysaveday")	//客户机:IE历史记录保存时间
#define OPT_M_RANDSELMENUBG				TEXT("mrandselmenubg")		//客户机:随机选择菜单皮肤（1.是;0否）
#define OPT_M_MENUBG					TEXT("mmenubg")				//客户机:当前选中菜单皮肤
#define OPT_M_CUSTOMMENUBG				TEXT("mcustommenubg")		//客户机:自定义菜单皮肤（|线分割）
#define OPT_M_QQHISTORYENABLE			TEXT("mqqhistoryenable")	//客户机:是否开启QQ登录记录
#define OPT_M_QQHISTORYSAVEDAY			TEXT("miehistorysaveday")	//客户机:QQ登录记录保存时间
#define OPT_M_CLICKSTARTGAME			TEXT("clickstartgame")		//客户机:是否双击启动游戏(0|1)
#define OPT_M_MENUANIMATION				TEXT("menuanimation")		//客户机:是否启用菜单动画效果

#define OPT_M_MULTISYSTEMDISPTIME		TEXT("multisystemdisptime")	//无盘:多系统等带选择的时间.
#define OPT_M_DHCPTFTPENABLE			TEXT("dhcptftpenable")		//无盘:是否开启DHCP、TFTP服务器(组合值：第0位:0表示人为禁用，1表示人为启用;第一位：1验证成功，0表示验证失败)
#define OPT_M_SHOWWIZARD				TEXT("showwizard")			//是否显示配置向导

#define OPT_M_ENABLEUDISK				TEXT("menableudisk")		//个人磁盘:允许个人磁盘(0,1)
#define OPT_M_UDISKVOL					TEXT("mudiskvol")			//个人磁盘:加载的个人磁盘盘符
#define OPT_M_UDISKSIZE					TEXT("mudisksize")			//个人磁盘:个人磁盘大小(M)
#define OPT_M_ARCHIVECOUNT				TEXT("marchivecount")		//个人磁盘:游戏存档数量
#define OPT_M_UDISKDIR					TEXT("mudiskdir")			//个人磁盘:存放个人磁盘文件的目录
#define OPT_M_ENABLECLEAN				TEXT("menableclean")		//个人磁盘:允许定期清理个人磁盘(0,1)
#define OPT_M_CLEANCYCLE				TEXT("mcleancycle")			//个人磁盘:定期清理的周期(天)

#define OPT_M_WRITEBACKLIMIT			TEXT("mwritebacklimit")		//回写限制,按M计
#define OPT_M_MOUNTDISKATBOOT			TEXT("mmountdiskatboot")	//是否驱动中提前刷盘（1是,0否)


#define OPT_M_PREREADENABLE				TEXT("prereadenable")		//客户机：是否启用预读（0否，1是）
#define OPT_M_USBPNPENABLE				TEXT("usbpnpenable")		//客户机：是否启用USBPNP（0否，1是）
#define OPT_M_VIDEOPNPENABLE			TEXT("videopnpenable")		//客户机：是否启用显卡PNP（0否，1是）
#define OPT_M_DISPLAYPNPENABLE			TEXT("displaypnpenable")	//客户机：是否启用显示器PNP（0否，1是）
#define OPT_M_HDMIFORBIDDEN				TEXT("hdmiforbidden")		//客户机：是否禁用HDMI声卡（0否，1是）

#define OPT_M_SHUTDOWNENABLE			TEXT("shutdownenable")		//是否启用快速关机
#define OPT_M_FORCEDSHUTDOWNENABLE		TEXT("forcedshutdown")		//是否启用强制关机
#define	OPT_M_UPSUPERENABLE				TEXT("upsuperenable")		//是否允许客户机上传系统、开超级
#define OPT_M_ASSISTENABLE				TEXT("assistenable")		//客户机：是否启用防逃费（0否，1是）
#define	OPT_M_VIEWERENABLE				TEXT("viewerenable")		//是否允许游戏下载查看器远程连接（0否，1是）

//ssd选项
#define OPT_S_DEPLOY_SYNGAME			TEXT("deploysyngame")		//施工模式是否同步游戏（0否，1是）
#define OPT_S_NODEPLOY_SYNGAME			TEXT("nodeploysyngame")		//非施工模式是否同步游戏（0否，1是）
#define OPT_S_SSDSYNSPEED				TEXT("ssddspeed")			//ssd同步速度:速度限制(n)(bytes/s)
#define OPT_S_WRITESIZE					TEXT("ssdwritesize")		//ssd回写大小（G）

//网月路由器客户端识别
#define	OPT_M_NETMOONENABLE				TEXT("netmoonenable")		//是否启用网月客户端识别

//桌面秀参数
#define OPT_S_CLOUDDESKTOP				TEXT("sclouddesktop")		//是否使用云桌面(1是,0否)
#define OPT_S_CLOUDTOOLDLG				TEXT("scloudtooldlg")		//是否使用云桌面工具面板(1是,0否)

#define OPT_S_SKIN						TEXT("sskin")				//皮肤
#define OPT_S_LAYER						TEXT("layer")
#define OPT_S_SW_COMNAME				TEXT("sswcomname")			//是否显示计算机名(1是,0否)
#define OPT_S_SW_COMIP					TEXT("sswcomip")			//是否显示计算机ip(1是,0否)
#define OPT_S_COMNAME_TOPMARGIN			TEXT("scomnametopmargin")	//计算机名上边距
#define OPT_S_COMNAME_DIGITONLY			TEXT("scomnamedigitonly")	//只取计算机名中的数字(1是,0否)
#define OPT_S_COMNAME_PREFIX			TEXT("scomnameprefix")		//计算机名前缀
#define OPT_S_COMNAME_SURFIX			TEXT("scomnamesurfix")		//计算机名后缀
#define OPT_S_COMNAME_FONT				TEXT("scomnamefont")		//计算机名字体
#define OPT_S_COMNAME_FONTSIZE			TEXT("scomnamefontsize")	//计算机名字号
#define OPT_S_COMNAME_FONTCOLOR			TEXT("scomnamefontcolor")	//计算机名字颜色
#define OPT_S_COMNAME_FONTSHAPE			TEXT("scomnamefontshape")	//计算机名字体形状(0|0|0:是否粗体,斜体,下画线)

#define OPT_M_NOTIEABLE					TEXT("mnoticeeable")			//是否启用公告
#define OPT_S_NOTICE_TITLE				TEXT("snoticetitle")			//公告标题
#define OPT_S_NOTICE_TITLE_FONTNAME		TEXT("snoticetitlefontname")	//公告标题字体
#define OPT_S_NOTICE_TITLE_FONTSIZE		TEXT("snoticetitlefontsize")	//公告标题字号
#define OPT_S_NOTICE_TITLE_FONTCOLOR	TEXT("snoticetitlefontcolor")	//公告标题字体颜色
#define OPT_S_NOTICE_TITLE_FONTSHAPE	TEXT("snoticetitlefontshape")	//公告标题字体形状


#define OPT_S_SW_WEATHER				TEXT("sswweather")				//是否显示天气(1是,0否)
#define OPT_S_WEATHER_AREAID			TEXT("sweatherareaid")			//区域编码
#define OPT_S_WEATHER_DATA				TEXT("sweatherdata")			//天气预报数据

#define OPT_S_SW_CALENDAR				TEXT("sswcalendar")				//是否显示日历(1是,0否)
#define OPT_S_SW_LUNAR					TEXT("sshowlunar")				//是否显示农历(1是,0否)
#define OPT_S_SW_ZODIAC					TEXT("sshowzodiac")				//是否显示星座(1是,0否)

#define OPT_S_SW_RATE					TEXT("sswrate")					//是否显示费率
#define OPT_S_RATE_TITLE_FONTNAME		TEXT("sratetitlefontname")		//费率标题字体名称
#define OPT_S_RATE_TITLE_FONTSIZE		TEXT("sratetitlefontsize")		//费率标题字号
#define OPT_S_RATE_TITLE_FONTCOLOR		TEXT("sratetitlefontcolor")		//费率标题字体颜色
#define OPT_S_RATE_TITLE_FONTSHAPE		TEXT("sratetitlefontshape")		//费率标题字体形状
#define OPT_S_RATE_FONTNAME				TEXT("sratefontname")			//费率字体名称
#define OPT_S_RATE_FONTSIZE				TEXT("sratefontsize")			//费率字号
#define OPT_S_RATE_FONTCOLOR			TEXT("sratefontcolor")			//费率字体颜色
#define OPT_S_RATE_FONTSHAPE			TEXT("sratefontshape")			//费率字体形状
#define OPT_S_CLIENT_LANGUAGEID			TEXT("sclanguageid")			//客户端语言ID


//控制台控制客户机的命令操作。控制台发命令给主服务器，由主服务器通过udp通知客户机 //(data后面的数据解释)
#define CONTROL_CLINET_INSTPROT			1		//安装并保护指定分区.string(要保护的分区)+string(不保护的分区)
#define CONTROL_CLINET_REMOVEPROT		2		//删除还原
#define CONTROL_CLINET_SHUTDOWN			3		//关机或者重启，dword：为0表示关机，为1表示重启
#define CONTROL_CLINET_VIEWPROCLIST		4		//获取进程列表. send(cmd + xmlstring)
#define CONTROL_CLINET_KILLPROC			5		//kill进程,dword:pid.
#define CONTROL_CLIENT_RUNPROCESS		6		//运行客户机程序。string:进程全路径名.
#define CONTROL_CLIENT_RUNVNC			7		//运行客户机VNC。
#define CONTROL_CLIENT_RAPIDSHUTDOWN	8		//快速关机


//定义通信命令
#define CMD_DB_HEART					0x0101		//心跳命令，上行：无，下行：无.
#define CMD_DB_SQL						0x0102		//执行SQL语句. 上行：sql语句,下行：row, colomn, 记录集
#define CMD_DB_TSQL						0x0103		//按事务方式执行SQL语句组,但不会返回记录集。
#define CMD_DB_ICON						0x0104		//数据库图标操作.

#define CMD_DB_GETDHCPINFO				0x0105      //获取dhcp客户机信息
#define CMD_DB_LOARDCLIENTINFO			0x0106      //获取loader客户机信息


#define ICON_OP_GET						0			//获取游戏的图标数据:gid.返回图标数据(gid + hash + length + data)
#define ICON_OP_SET						1			//设置游戏的图标数据(添加或者更新):gid + length + data.返回成功与否
#define ICON_OP_DEL						2			//删除游戏的图标:count + gid数组。（批量删除图标数据).返回成功与否
#define ICON_OP_FILENAME				3			//根据图标文件名修改图标数据.(gid, string(图标文件名)), 返回成功与否.
#define ICON_OP_GETLIST					4			//取得图标列表(count+gid,hash数组)

#define CMD_USER_REGISTER				0x0201		//个人磁盘用户注册
#define CMD_USER_LOGIN					0x0202		//个人磁盘用户登录
#define CMD_USER_MODIFYPASSWORD			0x0203		//个人磁盘修改密码
#define CMD_USER_DELUDFILE				0x0204		//删除个人磁盘文件
#define CMD_USER_QUERYPWD				0X0205		//查询用户密码,需求来源：项目：2017.6.15 需求编号：256

#define CMD_CLI_HEART					0x0301		//客户机心跳
#define CMD_CLI_CLIENTUPDATEINDEX		0x0302		//客户机查询升级文件列表.
//#define CMD_CLI_NODEUPDATEINDEX			0x0303		//节点查询升级文件列表.
#define CMD_CLI_QUERYDIRINFO			0x0304		//查询目录信息.
#define CMD_CLI_QUERYFILEINFO			0x0305		//客户机查询文件信息
#define CMD_CLI_DOWNFILEBLOCK			0x0306		//客户机下载文件块。
#define CMD_CLI_RPTCLIENTINFO			0x0307		//上报客户机基本信息.
#define CMD_CLI_RPTHARDINFO				0x0308		//上报客户机硬件信息
#define CMD_CLI_RPTTEMPRATURE			0x0309		//上报客户机温度信息
#define CMD_CLI_RPTGAMEMENUCLICK		0x030A		//菜单上报游戏点击数,菜单运行次数
#define CMD_CLI_CONSOLE2CLIENT			0x030B		//服务端转发控制台命令到客户机.  //用于控制客户机还原操作，开关机操作等.
#define CMD_CLI_CLIENT2CONSOLE			0x030C		//服务端转发客户端服务给控制台	 //用于客户机上报信息到控制台.
#define CMD_CLI_RPTGAMEMENUCLICKEX		0x030D		//15版菜单上报游戏点击数,菜单运行次数
#define CMD_CLI_NODEUPDATEINDEXEX		0x030E		//节点查询升级文件列表(拓展)64位版本.

// V2017.6.15版本需求
//从节点上传数据库，用于主节点同步，
//以下命令和CMD_CLI_UPLOADFILE命令配合使用
//用于从节点向主节点上传同步数据
#define CMD_CLI_CHECKUPLOADENABLE		0x030F		//从节点检查是否允许向主节点上传同步数据，主节点不处理
#define CMD_CLI_CHECKUPLOADOVER			0x0310		//客户端检查从节点向主节点上传同步数据是否结束，从节点处理
#define CMD_CLI_SLAVESYNCDB2MASTER		0x0311		//客户端向从节点发起同步数据，从节点使用CMD_CLI_UPLOADFILE命令向主节点发送数据，主节点不处理该命令
//

// 用于从客户机获取客户机信息
#define CONTROL_CLINET_BASEINFO			0x0901		//基本信息查询
#define CONTROL_CLINET_PING				0x0902		//ping
#define CONTROL_CLINET_DRIVERINFO		0x0903

#define CMD_CLI_GETSYSOPTION			0x0401		//获取系统选项列表
#define CMD_CLI_GETSERVERLIST			0x0402		//获取服务器列表
#define CMD_CLI_GETVDISKLIST			0x0403		//获取虚拟盘列表
#define CMD_CLI_GETSERVERTIME			0x0404		//获取服务器时间
#define CMD_CLI_GETGAMELIST				0x0405		//获取游戏列表索引
#define CMD_CLI_GETGAMEINFO				0x0406		//下载游戏
#define CMD_CLI_GETICONLIST				0x0407		//获取图标列表索引
#define CMD_CLI_GETICONINFO				0x0408		//下载图标
#define CMD_CLI_GETSHORTCUT				0x0409		//获取快捷方式
#define CMD_CLI_GETWALLPAPER			0x040A		//获取所在区域墙纸信息
#define CMD_CLI_GETTASKLIST				0x040B		//获取开机任务
#define CMD_CLI_GETIPLIST				0x040C		//获取客户机列表
#define CMD_CLI_SENDDUMP				0x040D		//发送客户端dump
#define CMD_CLI_QUERYRATE				0x040E		//获取费率设置
#define CMD_CLI_QUERYKMEAUXML			0x040F		//获取菜单XML
#define CMD_CLI_QUERYGAMEICOPATH		0x0410		//获取游戏桌面快捷方式图标路径
#define CMD_CLI_QUERYALLGAMESVR			0x0411		//查询所有游戏服务器列表
#define CMD_CLI_QUERYALLGAMESVRDRIVER	0x0412		//查询所有游戏服务器游戏盘信息列表
#define CMD_CLI_QUERYTASKLIST			0x0413		//查询通道插件信息
#define CMD_CLI_DOWNTASKZIP				0x0414		//下载通道插件
#define CMD_CLI_GETREPORTSTYLE			0x0415		//取是否上报日志数据
#define CMD_CLI_GETSSDWORKMODE			0x0416      //取是否进入施工模式
#define CMD_CLI_GETSSDGAMEINFO			0x0417		//取SSD盘游戏信息
#define CMD_CLI_UPLOADFILE				0x0418		//上传文件
#define CMD_CLI_ADDGAMECONFIG			0x0419		//添加游戏配置
#define CMD_CLI_REPORTSSDINFO			0x041A		//SSD施工信息
#define CMD_CLI_MODIFYSSDINIT			0x041B		//修改SSD初始化
#define CMD_CLI_MODIFYSSDDEPLOY			0x041C		//修改SSD施工模式
#define CMD_CLI_QUERYGAMECONFIG			0x041D		//获取游戏配置
#define CMD_CLI_REPORTSSDIMAGE			0x041E		//上报客户机SSD镜像
#define CMD_CLI_MODIFYTOPTION			0x041F		//修改系统选项
#define CMD_CLI_GETSSDGAMEINFOEX		0x0420		//取SSD盘游戏信息扩展
#define CMD_CLI_GETSSDGAMEINFOEXEX		0x0421		//取SSD盘游戏信息扩展(加服务端版本号)
#define CMD_CLI_GETGAMEINFOEX			0x0422		//下载游戏扩展

#define CMD_CLI_LOGTOSVR				0x0422		//客户机向服务端写日志(输入:type(DWORD保留)+level(DWORD)+ MSG(string))
#define CMD_CLI_GETTASKLISTEX			0x0423		//获取开机任务，命令扩展，获取imageid
#define CMD_CLI_QUERYALLGAMESVREXEX		0x0424      //游戏盘均衡
#define CMD_CLI_QUERYKMEAUXMLEX			0x0425		//获取新菜单XML

#define CMD_CON_REFRESHGAMELIST			0x0501		//控制台通知刷新资源列表
#define CMD_CON_QUERYONLINEMACHINE		0x0502		//控制台查询在线客户机
#define CMD_CON_QUERYTEMPRATURE			0x0503		//控制台查询机器温度信息.
#define CMD_CON_NOTIFYONLINE			0x0504		//通知控制台，客户机是否在线
#define CMD_CON_NOTIFYHARDWARE			0x0505		//通知控制台，客户机硬件信息
#define CMD_CON_ACCEPTHARDWARE			0x0506		//控制台确认硬件变更
#define CMD_CON_QUERYSVRINFO			0x0507		//控制台取服务器基本信息
#define CMD_CON_QUERYDISKLIST			0x0508		//控制台取磁盘分区列表信息
#define CMD_CON_NOTIFYLOGINFO			0x0509		//通知控制台，日志信息.
#define CMD_CON_QUERYIPINFO				0x050A		//查询计算机名,ip
#define CMD_CON_QUERYCHECK				0x050B		//查询验证是否成功
#define CMD_CON_QUERYWEATHER			0x050C		//查询天气
#define CMD_CON_REMOTENODESVR			0x050D		//远程节点
#define CMD_CON_QUERYNODESTATUS			0x050E		//查询节点状态(需传主服务器的所有IP)
#define CMD_CON_NOTIFYSYNCDATA			0x050F		//控制台通知刷新(快捷方式，开机任务，桌面墙纸)
#define CMD_CON_QUERYSSDIMAGESYNCINFO	0x0510		//控制获取SSD镜像同步信息
#define CMD_CON_QUERYNODENEEDUP			0x0511      //获取节点是否需要升级
#define CMD_CON_NOTIFYNODEUP			0x0512      //通知节点升级
#define CMD_CON_QUERYMASKGAME			0x0513		//控制台获取游戏黑名单
#define CMD_CON_LOGIN					0x0514		//控制台登录验证
#define CMD_CON_CHECK_VER				0x0515		//控制台获取检查升级
#define CMD_CON_DEL_DUMP_FILE			0x0516		//控台删除dump文件
#define CMD_CON_QUERYONLINEMACHINEEX	0x0517		//控制台查询在线客户机扩展(加上在线时长)
#define CMD_CONSOLE_REPLACE_DATA		0x0518		//控制台请求修改tOption数据

#define CMD_MGR_TAKEOVER				0x0601		//手动接管(无参数)
#define CMD_MGR_CANCELTAKEOVER			0x0602		//手动取消接管(无参数)
#define CMD_MGR_NOTIFYTAKEOVER			0x0603		//通知接管(是否接管标志+服务器名+ip数组)
#define CMD_MGR_QUERYTAKEOVER			0x0604		//查询接管(接管者标志+是否接管标志+服务器名+ip数组)
#define CMD_MGR_ISINSERVERLIST			0x0605		//查询节点是否在服务器列表内(节点服务器名)
#define CMD_MGR_QUERYMAIN				0x0606		//查询主服务器
#define CMD_MGR_UPNODEDISKDRIVER		0x0607		//更新节点盘符信息
#define CMD_MGR_REPORTSVRHARDWARE		0x0608		//上报服务器硬件信息
#define CMD_MGR_GETUPGRADEINFO			0x0609      //获取主服务文件信息，判断是否可以升级
#define CMD_MGR_REPORTSVRIPINFO			0x060A		//上报服务器ip信息
#define CMD_MGR_REPORTSVRIPINFOEX		0x060B		//上报服务器ip，MASK等信息

#define CMD_SVRMONITOR_DISKCPUMEM		0x0701		//获取盘符空闲,CPU占用,内存使用(cpu+盘符个数+数组(盘付+使用率)+内存总大小+可用内存+网卡个数+数组(名字+使用率)))
#define CMD_IEQQ_QUERYNETBARINFO		0x0801		//IE，QQ记录获取网吧ID，名称，机器在线数(ID,NAME,USER,总终端,在线终端)
#define CMD_DOWNLOAD_VIEWER_FILES		0x0901      //客户端请求下载查看器的文件列表
#define CMD_PLDOWNFAILEDCOUNT			0X0902		//PL下载失败的次数

class CLock
{
private:
	CRITICAL_SECTION m_lock;
public:
	CLock()			{ InitializeCriticalSection(&m_lock);	}
	~CLock()		{ DeleteCriticalSection(&m_lock);		}
	void Lock()		{ EnterCriticalSection(&m_lock);		}
	void UnLock()	{ LeaveCriticalSection(&m_lock);		}
};

template <typename T>
class ObjectLock
{
private:
	T& _lock;
public:
	ObjectLock(T& lock): _lock(lock) { _lock.Lock(); }
	~ObjectLock()	{ _lock.UnLock(); }
};

#define OBJECT_LOCK(lock) ObjectLock<CLock> __xlock__(lock)

//return is 64bit operation system.
bool __stdcall AfxIs64BitOpertionSystem();

//generate exception dump file. 
LONG __stdcall AfxUnHandledExceptionFilter(PEXCEPTION_POINTERS pExcept);

inline void AfxSetExptHandle() { SetUnhandledExceptionFilter(AfxUnHandledExceptionFilter); }

//get application root path with "\\"
LPTSTR __stdcall AfxGetAppPath(LPTSTR szPath, DWORD dwSize = MAX_PATH, LPCTSTR szAppend = NULL);

//create directory tree.
void __stdcall AfxCreateDirectory(LPCTSTR szDirectory, size_t offset = 0);

//delete all file and sub dir.
void AfxDeleteDirectory(LPCTSTR szDirectory, HANDLE hExit = NULL);

//delete all match file and sub dir 's file.
void AfxDeleteMatchFile(LPCTSTR szDirectory, LPCTSTR szFilter, HANDLE hExit = NULL);

//get windows system error text.
LPTSTR __stdcall AfxGetWinErrorText(LPTSTR buf, DWORD dwSize, DWORD dwError);

//get file version.x.x.x.x
LPTSTR __stdcall AfxGetFileVersion(LPCTSTR pFileName, LPTSTR pVersion);

//get current user registry key.
LPTSTR __stdcall AfxGetCurUserHKeyRoot(LPTSTR HkeyRoot, DWORD dwSize);

//read application config.
LPTSTR __stdcall AfxGetSysString(LPCTSTR Section, LPCTSTR Key, LPTSTR Value, DWORD dwSize = MAX_PATH, LPCTSTR Default = TEXT(""));
DWORD  __stdcall AfxGetSysLong(LPCTSTR Section, LPCTSTR Key, long Default = 0);

//read application config for language
LPTSTR __stdcall AfxGetLangString(LPCTSTR Section, LPCTSTR Key, LPTSTR Value, 
	DWORD dwSize = MAX_PATH, LPCTSTR Default = TEXT(""));
LPTSTR __stdcall AfxGetLangStringEx(HANDLE hInstance, UINT uID, LPTSTR Value,
	DWORD dwSize, LPCTSTR Default = TEXT(""));

//read application config for language dll path name 
LPTSTR __stdcall AfxGetLangDll(LPTSTR pPath, DWORD dwSize = MAX_PATH);

//get url use CPkgHelper
LPSTR __stdcall AfxGetUrlString(LPSTR lpBuffer, DWORD dwSize, int nUrl);

//set language id
void __stdcall AfxSetLangId(WORD wLangID);

//get idc class name
LPTSTR __stdcall AfxGetIdcClassName(LPCTSTR szGuid, LPTSTR szName, DWORD dwSize = MAX_PATH);

//get idc area name
LPTSTR __stdcall AfxGetIdcAreaName(LPCTSTR szGuid, LPTSTR szName, DWORD dwSize = MAX_PATH);


//write application config.
void   __stdcall AfxSetSysString(LPCTSTR Section, LPCTSTR Key, LPCTSTR Value);
void   __stdcall AfxSetSysLong(LPCTSTR Section, LPCTSTR Key, long Value);

//create a new guid.
LPTSTR __stdcall AfxCreateGuidString(LPTSTR guid);

//format time32_t to date time string.
LPTSTR __stdcall AfxFormatTimeString(DWORD time, LPTSTR buffer, DWORD dwSize);

//format ip(network order) to string.
LPTSTR __stdcall AfxFormatIpString(DWORD ip, LPTSTR buffer, DWORD dwSize);

//convert filetime to time_t.
DWORD  __stdcall AfxFileTime2UtcTime(FILETIME& ft);

//convert time_t to filetime.
void   __stdcall AfxUtcTime2FileTime(DWORD time, FILETIME& ft);

//get process id.
DWORD __stdcall  AfxGetProcIdByName(LPCTSTR szProcName);

//get parent process id.
DWORD __stdcall  AfxGetParentProcessId();

//return process token by process name.
HANDLE __stdcall AfxGetTokenByProcName(LPCTSTR lpName);

//run process by "explore.exe" token.(current user.)
bool __stdcall AfxRunProcess(LPCTSTR lpProcess, LPCTSTR lpParameter = NULL);

//kill process by process name.
bool __stdcall AfxKillProcess(LPCTSTR lpProcess);

//adjust process privilege.
void __stdcall AfxSetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);

//split string.
inline void	AfxSplitString(const tstring& source, std::vector<tstring>& subs, TCHAR chr = TEXT('|'));

//network connect with timeout(second).
SOCKET __stdcall AfxSocketConnect(LPCTSTR ip, WORD port, DWORD timeout = 2);

//socket listen port with bind ip.
SOCKET __stdcall AfxSocketListen(WORD port, LPCTSTR ip = NULL);

//socket communication.(by pkgheader).
DWORD  __stdcall AfxSocketExecute(SOCKET sck, LPCSTR pOutData, int outLength, LPSTR pInData, int inLength);
DWORD  __stdcall AfxSocketExecuteEx(SOCKET sck, LPCSTR pOutData, int outLength, LPSTR pInData, int inLength, int try_max);

//socket close.
void   __stdcall AfxSocketClose(SOCKET& sck);

//send udp data.
bool   __stdcall AfxSendUdpNotify(LPCSTR pdata, DWORD length, LPCTSTR ip, WORD port);

//send log to console.
void   __cdecl	 AfxSendLogToConsole(LPCTSTR ip, DWORD logtype, LPCTSTR format, ...);

//get device name by device class id.
LPCTSTR __stdcall AfxGetFirstDevName(const GUID& devclassguid, LPTSTR szDevName, DWORD dwSize,LPTSTR szDevId = NULL, DWORD dwIdSize = 0);
LPCTSTR __stdcall AfxGetDevNameByWmi(LPCTSTR szWql, LPCTSTR szFiled, LPTSTR szDevName, DWORD dwSize);

//save icon to buffer.
DWORD __stdcall AfxSaveHICONToData(HICON hIcon, LPCSTR data, DWORD length);

//load icon from buffer.
HICON __stdcall	AfxLoadHICONFromData(LPCSTR data, DWORD length);

//determine if a ip is one of local host's ips
bool __stdcall AfxIsLocalIP(DWORD ip, bool b127IsLocalMachine=true);

//broadcast detect takeover server
bool __stdcall AfxDetectTakeover(DWORD bcAddr, LPDWORD lpServerIPs,int tmo = 1, int nMaxRetry = 5);
bool __stdcall AfxDetectTakeoverEx(int main_or_takeover, DWORD bcAddr, LPDWORD lpServerIPs,int tmo = 1, int nMaxRetry = 5);


//service helper function.
//max_path/ERROR_SUCCESS/guid(single instance)	//AfxGetServiceInfo
typedef DWORD (__stdcall* AFXGETSERVICEINFO)(LPTSTR SvrName, LPTSTR SvrDescription, LPTSTR guid);	
//AfxSeviceProc:service main function. service exit if the function exit.
typedef DWORD (__stdcall* AFXSEVICEPROC)(HANDLE hExit);

bool  __stdcall AfxIsServiceExist(LPCTSTR lpszSvrName);
DWORD __stdcall AfxInstallService(LPCTSTR lpszSvrName, LPCTSTR lpszSvrDicription, LPCTSTR path);
DWORD __stdcall AfxRemoveService(LPCTSTR lpszSvrName);
DWORD __stdcall AfxGetServiceState(LPCTSTR lpszSvrName);
DWORD __stdcall AfxStartService(LPCTSTR lpszSvrName);
DWORD __stdcall AfxStopService(LPCTSTR lpszSvrName);
DWORD __stdcall AfxRunAsService(LPCTSTR SvrExePath, int argc, _TCHAR* argv[]);


//encode function.
DWORD  __stdcall AfxCrc32Encode(DWORD crc, const void* data, DWORD ByteLen);
DWORD  __stdcall AfxGetFileCrc32(LPCTSTR szFile);
LPTSTR __stdcall AfxMD5Encode(const void* data, DWORD ByteLen, LPTSTR md5);
void* __stdcall AfxMD5EncodeEx(const void* data, DWORD ByteLen, void *md5);
LPTSTR __stdcall AfxGetFileMd5(LPCTSTR szFile, LPTSTR md5);
LPTSTR __stdcall AfxUriEncode(LPCTSTR source, LPTSTR dest, int nSize);
LPWSTR __stdcall AfxUTF82StrW(LPCSTR  pUTF8, LPWSTR pUnicode, int& nSize);
LPSTR  __stdcall AfxUTF82StrA(LPCSTR  pUTF8, LPSTR pChar, int& nSize);
LPSTR  __stdcall AfxStrW2UTF8(LPCWSTR pUnicode, LPSTR pUtf8, int& nSize);
LPSTR  __stdcall AfxStrA2UTF8(LPCSTR pChar, LPSTR pUtf8, int& nSize);

#ifdef _UNICODE
#define AfxUTF82Str			AfxUTF82StrW
#define AfxStr2UTF8			AfxStrW2UTF8
#else
#define AfxUTF82Str			AfxUTF82StrA
#define AfxStr2UTF8			AfxStrA2UTF8
#endif

//write log function.
HANDLE __stdcall AfxCreateLogger(LPCTSTR szModuleName);
void  __cdecl AfxWriteLogger(HANDLE hLogger, LPCTSTR formater, ...);
void  __stdcall AfxCloseLogger(HANDLE& hLogger);
void __cdecl AfxWriteDbgLog(LPCTSTR formater, ...);

//////////////////////////////////////////////////////////////////////////
//scsi disk driver function.

#pragma pack(push, 1)
typedef struct DisklessInfo
{
	ULONG is_admin;			//admin or not
	ULONG server_ip;		//mirror server ip
	ULONG diskless;			//diskless or not
	ULONG dhcp_server_ip;	//dhcp server ip
	ULONG mode_flag;		//ssd boot or not
	ULONG got_volume_pos;	//$volume file record pos got or not(for cache driver)
	LONGLONG volume_pos;	//$volume file record pos (absolute)(for cache driver)
	ULONG reserved[8];
}DISKLESS_INFO, *PDISKLESS_INFO;
#pragma pack()

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


//////////////////////////////////////////////////////////////////////////
//net function.
#define PKG_VERSION				0x0100		//1.0
#define PKG_SIG					'.pkg'
#define PKG_MAXLENGTH			0x10000
#define PKG_STA_SUCCESS			0
#define PKG_STA_ERROR			1

#pragma warning(disable:4200)
struct PkgHeader
{
	DWORD sig;					//package start flag.	
	DWORD length;				//total length, include package header
	DWORD command;				//command.
	DWORD type;					//object type.OBJECT_
	DWORD version;				//version.
	DWORD status;				//status.
	DWORD reserved[4];			//reserved, fill 0.
	char  data[0];				//data.
};
#pragma warning(default:4200)

#include "kcore.inl"

class CPkgHelper;
struct INetModule;

struct INetCallback
{
	virtual void Release() = 0;

	//disconnect socket if return is not error_success.
	virtual long OnAccept(INetModule* pNetModule, DWORD_PTR Id) = 0;
	virtual long OnSend(INetModule* pNetModule, DWORD_PTR Id) = 0;
	virtual long OnReceive(INetModule* pNetModule, DWORD_PTR Id, LPCSTR pRecvBuf, LPSTR pSendBuf) = 0;

	virtual void OnClose(INetModule* pNetModule, DWORD_PTR Id) = 0;
};

struct INetModule
{
	virtual void Release() = 0;

	virtual DWORD Start(WORD port, DWORD_PTR reserved = 0) = 0;
	virtual void Stop() = 0;

	virtual unsigned short GetListPort() = 0;
	virtual void SetNetCallback(INetCallback* pNetCallback) = 0;

	virtual bool GetPeerInfo(DWORD_PTR Id, SOCKET& sck, DWORD& peerip) = 0;
};

bool __stdcall AfxCreateINetModule(INetModule*& pINetModule);


//memory database support.
//open database.
bool __stdcall AfxOpenSqliteMemory(LPCTSTR szDbFileName, sqlite3*& pSqlite, DWORD_PTR& cookie);

//flush database(save).
void __stdcall AfxFlushSqliteMemory(DWORD_PTR cookie);

//close database.
void __stdcall AfxCloseSqliteMemory(DWORD_PTR cookie);

//////////////////////////////////////////////////////////////////////////
//zip helper function.
DECLARE_HANDLE(HZIP);
typedef DWORD ZRESULT;

//ZIP FILE FLAG
#define ZIP_HANDLE   1
#define ZIP_FILENAME 2
#define ZIP_MEMORY   3
#define ZIP_FOLDER   4

#define ZR_OK         0x00000000     // nb. the pseudo-code zr-recent is never returned,
#define ZR_RECENT     0x00000001     // but can be passed to FormatZipMessage.
#define ZR_GENMASK    0x0000FF00
#define ZR_NODUPH     0x00000100     // couldn't duplicate the handle
#define ZR_NOFILE     0x00000200     // couldn't create/open the file
#define ZR_NOALLOC    0x00000300     // failed to allocate some resource
#define ZR_WRITE      0x00000400     // a general error writing to the file
#define ZR_NOTFOUND   0x00000500     // couldn't find that file in the zip
#define ZR_MORE       0x00000600     // there's still more data to be unzipped
#define ZR_CORRUPT    0x00000700     // the zipfile is corrupt or not a zipfile
#define ZR_READ       0x00000800     // a general error reading the file
#define ZR_CALLERMASK 0x00FF0000
#define ZR_ARGS       0x00010000     // general mistake with the arguments
#define ZR_NOTMMAP    0x00020000     // tried to ZipGetMemory, but that only works on mmap zipfiles, which yours wasn't
#define ZR_MEMSIZE    0x00030000     // the memory size is too small
#define ZR_FAILED     0x00040000     // the thing was already failed when you called this function
#define ZR_ENDED      0x00050000     // the zip creation has already been closed
#define ZR_MISSIZE    0x00060000     // the indicated input file size turned out mistaken
#define ZR_PARTIALUNZ 0x00070000     // the file had already been partially unzipped
#define ZR_ZMODE      0x00080000     // tried to mix creating/opening a zip 
#define ZR_BUGMASK    0xFF000000
#define ZR_NOTINITED  0x01000000     // initialisation didn't work
#define ZR_SEEK       0x02000000     // trying to seek in an unseekable file
#define ZR_NOCHANGE   0x04000000     // changed its mind on storage, but not allowed
#define ZR_FLATE      0x05000000     // an internal error in the de/inflation code

typedef struct
{
	int index;
	char name[MAX_PATH];
	DWORD attr;
	FILETIME atime,ctime,mtime;
	long comp_size;
	long unc_size;
} ZIPENTRYA;

typedef struct
{
	int index;
	WCHAR name[MAX_PATH];
	DWORD attr;
	FILETIME atime,ctime,mtime;
	long comp_size;
	long unc_size;
} ZIPENTRYW;

HZIP	AfxCreateZipZ(void *z, unsigned int len, DWORD flags);
ZRESULT AfxZipAddZ(HZIP hz, const TCHAR *dstzn, void *src, unsigned int len, DWORD flags);
ZRESULT AfxCloseZipZ(HZIP hz);
ZRESULT AfxZipGetMemoryZ(HZIP hz, void **buf, unsigned long *len);
DWORD	AfxFormatZipMessageZ(ZRESULT code, char *buf,unsigned int len);
BOOL	AfxAddFolderContentZ(HZIP hZip, TCHAR* AbsolutePath, TCHAR* DirToAdd);
DWORD	AfxFormatZipMessageZ(ZRESULT code, char *buf, DWORD len);

HZIP	AfxOpenZipU(void *z, unsigned int len, DWORD flags);
ZRESULT AfxGetZipItemA(HZIP hz, int index, ZIPENTRYA *ze);
ZRESULT AfxGetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
ZRESULT AfxFindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYA *ze);
ZRESULT AfxFindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
ZRESULT AfxUnzipItem(HZIP hz, int index, void* dst, unsigned int len, DWORD flags);
ZRESULT AfxCloseZipU(HZIP hz);
DWORD	AfxFormatZipMessageU(ZRESULT code, char *buf,DWORD len);

#ifdef _UNICODE
#define ZIPENTRY	ZIPENTRYW
#define AfxGetZipItem	AfxGetZipItemW
#define AfxFindZipItem	AfxFindZipItemW
#else
#define ZIPENTRY ZIPENTRYA
#define AfxGetZipItem	AfxGetZipItemA
#define AfxFindZipItem	AfxFindZipItemA
#endif // _UNICODE


bool AfxEncryptVnc(char* plain,int len, unsigned char* crypted);
bool AfxDecryptVnc(unsigned char* crypted, char* plain, int len);

//pkey(8个字节) nKeyLength=4
void AfxEncryTea(TCHAR* plain,int nlainlength,TCHAR* pkey,int nKeyLength,TCHAR* pCrypt,int& nCryptLength,int round = 32, bool isNetByte = false);
void AfxDecryTea(TCHAR* pCrypt,int nCryptLength,TCHAR* pkey,int nKeyLength,TCHAR* plain,int nlainlength,int round = 32, bool isNetByte = false);

BOOL __stdcall AfxDisableFsRedirection(PVOID* pOldValue);
BOOL __stdcall AfxRevertFsRedirection(PVOID oldValue);

bool __stdcall AfxOnlineBackupDb(sqlite3* pSrcDb, LPSTR pBackupName);
bool __stdcall AfxRunProcessAsAdmin(LPCTSTR lpProcess, LPCTSTR lpParameter);

// SSD初始化模式
typedef enum {INITALLDISK=1,INITIMAGEDISK,INITGAMEDISK}INITSSD;

//////////////////////////////////////////////////////////////////////////

END_NAMESPACE_KCORE

#endif