// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 SSD_DEPLOYMENT_LIB_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// SSD_DEPLOYMENT_LIB_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once


#ifdef SSD_DEPLOYMENT_LIB_EXPORTS
#define SSD_DEPLOYMENT_LIB_API __declspec(dllexport)
#else
#define SSD_DEPLOYMENT_LIB_API __declspec(dllimport)
#endif

typedef void *ssd_handle_t;

// 日志回调接口，回调参数/内容
typedef void (__stdcall *log_handler_t)(void *, const wchar_t *msg, unsigned __int32 msg_len);

// 下载总大小/进度回调，回调参数/大小/速度(kb/m)
typedef void (__stdcall *total_size_handler_t)(void *, unsigned __int64 size);
typedef void (__stdcall *progress_handler_t)(void *, unsigned __int64 size, unsigned __int32 speed);

// 施工完成
typedef void(__stdcall *deployment_complete_handler_t)(void *);

// SSD游戏盘符
typedef void(__stdcall *ssd_game_disk_handler_t)(void *, wchar_t);



struct callback_t
{
	log_handler_t log_handler_;

	total_size_handler_t total_size_handler_;
	progress_handler_t progress_handler_;

	deployment_complete_handler_t complete_handler_;
	ssd_game_disk_handler_t ssd_game_disk_handler_;
};

enum class initssd_t
{
	init_alldisk = 1,
	init_imagedisk,
	init_gamedisk
};

struct param_t
{
	bool is_deployment_;
	int  force_init_type;
	bool is_create_process_;

	bool is_deploy_mode_need_update_game_;
	bool is_none_deploy_mode_need_update_game_;
	bool is_check_vendor_;
	unsigned __int32 none_deploy_mode_limit_update_speed_;
	unsigned __int64 writecallback_disk_size_;
};

// 创建ssd组件, 回调接口/回调参数
SSD_DEPLOYMENT_LIB_API ssd_handle_t __stdcall create_ssd_handle(const callback_t &, void *param);
SSD_DEPLOYMENT_LIB_API void __stdcall destroy_ssd_handle(ssd_handle_t handle);

// 启动ssd组件
SSD_DEPLOYMENT_LIB_API bool __stdcall start_ssd(ssd_handle_t handle, const param_t &);

// 停止ssd组件
SSD_DEPLOYMENT_LIB_API void __stdcall stop_ssd(ssd_handle_t handle);




