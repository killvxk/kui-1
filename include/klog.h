#pragma once
#include "kcore.h"

START_NAMESPACE_KCORE
// server
bool __stdcall klog_svr_start();
void __stdcall klog_svr_stop();

//client
typedef void *klog_client_t;
typedef void(__stdcall *sink_t)(void *, const wchar_t *, unsigned __int32);
klog_client_t __stdcall klog_client_create(sink_t, void *);
bool __stdcall klog_client_start(klog_client_t, const char *ip, unsigned __int16 port);
void __stdcall klog_client_write(klog_client_t, unsigned __int8 level, const wchar_t *text, unsigned __int32 size);
void __stdcall klog_client_stop(klog_client_t);
void __stdcall klog_client_destroy(klog_client_t);

END_NAMESPACE_KCORE