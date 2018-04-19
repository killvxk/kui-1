#include "OpenHMI.h"

namespace OpenHMI
{ 
#define	ERROR_		L"[Error]["
#define	WARNING		L"[Warning]["
#define	INFO		L"[Info]["
#define	DEBUGEX		L"[Debug]["

	void CLog::error2(const String &strTab, const String &strMsg)
	{
		return;

		if( CONFIG::getOutputErrorLog() )
		{
			writeLog(ERROR_, strTab, strMsg);
		}
	}

	void CLog::error2(const CHAR *pcTab, const CHAR *pcMsg)
	{
		return;

		if( CONFIG::getOutputErrorLog() )
		{
			writeLog(ERROR_, pcTab, pcMsg);
		}
	}
	
	void CLog::warning2(const String &strTab, const String &strMsg)
	{
		return;

		if( CONFIG::getOutputWarningLog() )
		{
			writeLog(WARNING, strTab, strMsg);
		}
	}

	void CLog::warning2(const CHAR *pcTab, const CHAR *pcMsg)
	{
		return;

		if( CONFIG::getOutputWarningLog() )
		{
			writeLog(WARNING, pcTab, pcMsg);
		}
	}
	
	void CLog::info(const String &strTab, const String &strMsg)
	{
		return;

		if( CONFIG::getOutputInfoLog() )
		{
			writeLog(INFO, strTab, strMsg);
		}
	}

	void CLog::info(const CHAR *pcTab, const CHAR *pcMsg)
	{
		return;

		if( CONFIG::getOutputInfoLog() )
		{
			writeLog(INFO, pcTab, pcMsg);
		}
	}
	
	void CLog::debug2(const String &strTab, const String &strMsg)
	{
		return;

		if( CONFIG::getOutputDebugLog() )
		{
			writeLog(DEBUGEX, strTab, strMsg);
		}
	}

	void CLog::debug2(const CHAR *pcTab, const CHAR *pcMsg)
	{
		return;

		if( CONFIG::getOutputDebugLog() )
		{
			writeLog(DEBUGEX, pcTab, pcMsg);
		}
	}
	
	void CLog::tempCodePos(const CHAR *pcClassName, const CHAR *pcFunctionName)
	{
		String strMessage = String::format(L"[TempCode][-----%ls-----][----%ls-----]\r\n", pcClassName, pcFunctionName);

		//String::printf(strMessage);

#ifdef OS_WIN
	#ifdef _DEBUG
		OutputDebugString(strMessage);
	#endif
#endif

#ifdef OS_QNX
		
#endif

#ifdef OS_LINUX
	
#endif
	}

	void CLog::writeLog(const CHAR *pcType, const String &strTab, const String &strMsg)
	{
		String strMessage = String::format(L"%ls%ls] %ls\r\n", pcType, (const CHAR*)strTab, (const CHAR*)strMsg);

		//String::printf(strMessage);

#ifdef OS_WIN
	#ifdef _DEBUG
		//OutputDebugString(strMessage);
	#endif
#endif

#ifdef OS_QNX

#endif

#ifdef OS_LINUX

#endif
	}

	void CLog::writeLog(const CHAR *pcType, const CHAR *pcTab, const CHAR *pcMsg)
	{
		//String strMessage = String::format(L"%ls%ls] %ls\r\n", pcType, pcTab, pcMsg);

		//String::printf(strMessage);

#ifdef OS_WIN
	#ifdef _DEBUG
		//OutputDebugString(strMessage);
	#endif
#endif
	}
}