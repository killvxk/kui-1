#ifndef __ksafecenter_h__
#define __ksafecenter_h__

#pragma once


#define OPT_SAFE_PCPROEABLE									_T("pcproeable")//是否启用客户机防护(0|1)
#define OPT_SAFE_LANPROEABLE								_T("lanproeable")//是否启用局域网防护(0|1)


#define OPT_M_PROCESSSTOPEABLE								_T("processstopeable")//是否启用非法进程拦截(0|1)
#define OPT_M_PROCESSSPROEABLE								_T("processsproeable")//是否启用重要进程保护(0|1)
#define OPT_M_ARPPROEABLE									_T("arpproeable")//是否启用ARP防护(0|1)
#define OPT_M_AUTOBINDSVREABLE								_T("autobindsvreable")//是否启用自动绑定网关主从服务器(0|1)
#define OPT_M_DRIVERSTOPEABLE								_T("driverstopeable")//是否启用非法驱动拦截(0|1)
#define OPT_M_UNDLGEABLE									_T("undlgeable")//是否启用非法窗口拦截(0|1)
#define OPT_M_IPSTOPEABLE									_T("ipstopeable")
#define OPT_M_NETPROEABLE									_T("netproeable")
#define OPT_M_NETPORTEABLE									_T("netporteable")


#define PROCESS_CLOSE_RESTART								0//关闭后重启进程
#define PROCESS_CLOSE_REBOOT								1//关闭后重启客户机

#define PROCESS_FROM_USER									0//保护进程用户添加	
#define PROCESS_FROM_IDC									1//保护进程来源中心	
#define PROCESS_FROM_AGENT									2//保护进程来源代理商

#define UNPROCESS_WHITE										0//非法进程白名单	
#define UNPROCESS_BLACK										1//非法进程黑名单	

#define UNPROCESS_TYPE_TRUST								0//非法进程信任
#define UNPROCESS_TYPE_STOP									1//非法进程拦截	

#define ARP_ATTACK_PASSIVITY								0//ARP被动攻击	
#define ARP_ATTACK_INITIATIVE								1//ARP主动攻击	



#define CMD_SAFE_DB_HEART										0x0301		//心跳命令，上行：无，下行：无.
#define CMD_SAFE_DB_SQL											0x0302		//执行SQL语句. 上行：sql语句,下行：row, colomn, 记录集
#define CMD_SAFE_DB_TSQL										0x0303		//按事务方式执行SQL语句组,但不会返回记录集。

#define LOG_TYPE_ARPPRO											0x01		//arp日志
#define LOG_TYPE_PROFILTER										0x02		//进程过滤日志
#define LOG_TYPE_PROPROTECT										0x03		//进程保护日志
#define LOG_TYPE_DRIVER											0x04        //驱动防护日志
#define LOG_TYPE_UNDLG											0x05		//非法窗口拦截
#define LOG_TYPE_IPSTOP											0x06		//IP地址拦截
#define LOG_TYPE_NETSTOP										0x07		//网络防护
#define LOG_TYPE_NETPORT										0x08		//网络端口




//安全中心通讯命令
#define CMD_SC_NOTIFYIPMACBIND			0x0702		//通知IP和MAC地址绑定信息
#define CMD_SC_NOTIFYPROTECTINFO		0x0704		//通知进程保护信息
#define CMD_SC_NOTIFYPEFILTERINFO		0x0707		//通知进程过滤信息

#define CMD_SC_QUERYIPMACBIND			0x0901		//获取IP和MAC地址绑定信息
#define CMD_SC_QUERYPROTECTINFO			0x0903		//获取进程保护信息
#define CMD_SC_QUERYPROTECTINFOEX		0x0904		//获取进程保护信息扩展
#define CMD_SC_QUERYPEFILTERBLACKINFO	0x0905		//获取进程过滤黑名单信息
#define CMD_SC_QUERYPEFILTERWHITEINFO	0x0906		//获取进程过滤白名单信息
#define CMD_SC_REPORTINFO				0x0908		//上报安全中心过滤信息



#define CMD_SC_QUERYDRIVERBLACK			0x090A		//获取驱动黑名单
#define CMD_SC_QUERYDRIVERWHITE			0x090B		//获取驱动白名单
#define CMD_SC_QUERYUNDLG				0x090C		//获取非法窗口
#define CMD_SC_QUERYNETPRO				0x090D		//获取网络防护
#define CMD_SC_QUERYNETPORT				0x090E		//获取网络端口
#define CMD_SC_QUERYIPSTOP				0x090F		//获取IP拦截
#define CMD_SC_QUERYIPWHITE				0x0910		//获取网络防护白名单


#define CMD_SC_QUERYPEFILTERBLACKINFOEX	0x0912		//获取进程过滤黑名单信息扩展
#define CMD_SC_QUERYPEFILTERWHITEINFOEX	0x0913		//获取进程过滤白名单信息扩展
#define CMD_SC_QUERYDRIVERBLACKEX		0x0914		//获取驱动黑名单信息扩展
#define CMD_SC_QUERYDRIVERWHITEEX		0x0915		//获取驱动白名单信息扩展







#endif