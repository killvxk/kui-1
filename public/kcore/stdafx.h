#pragma once

#ifndef WINVER
#define WINVER 0x0600
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0600
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#define WIN32_LEAN_AND_MEAN

#include "kcore.h"
#include <windows.h>
#include <stdlib.h>
#include <comutil.h>
#include <Shlwapi.h>
#include "localconfig/localconfigfun.h"

#pragma comment(lib, "shlwapi")
