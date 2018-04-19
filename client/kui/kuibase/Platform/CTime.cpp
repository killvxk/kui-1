#include "OpenHMI.h"

namespace OpenHMI
{
	FLOAT	CTime::s_fStartTickCount	= 0;

#ifdef OS_WIN
	CTime CTime::getCurrentTime()
	{
		CTime ctime;

		SYSTEMTIME st;
		::GetLocalTime(&st);

		ctime.m_usYear = st.wYear;
		ctime.m_usMonth = st.wMonth;
		ctime.m_usDay = st.wDay;
		ctime.m_usHour = st.wHour;
		ctime.m_usMinute = st.wMinute;
		ctime.m_usSecond = st.wSecond;
		ctime.m_usMilliseconds = st.wMilliseconds;

		return ctime;
	}

	ULONG CTime::getTickCount()
	{
		if (Math::isZero(s_fStartTickCount))
		{
			s_fStartTickCount = (FLOAT)::GetTickCount();
		}

		return (ULONG)(::GetTickCount() - s_fStartTickCount);
	}
#endif

#ifdef OS_QNX
	CTime CTime::getCurrentTime()
	{
		CTime ctime;

		struct timeval timevalBefore;
		struct timeval timevalAfter;
		gettimeofday(&timevalBefore, NULL);
		time_t timeBefore = time(NULL);
		gettimeofday(&timevalAfter, NULL);
		time_t timeAfter = time(NULL);

		struct tm *ptm;

		if ((timevalAfter.tv_sec - timevalBefore.tv_sec) != 0)
		{
			ptm = localtime(&timeAfter);

			ctime.m_usMilliseconds = timevalAfter.tv_usec / 1000;
		}
		else
		{
			ptm = localtime(&timeBefore);

			ctime.m_usMilliseconds = timevalBefore.tv_usec / 1000;
		}

		ctime.m_usYear = ptm->tm_year + 1900;
		ctime.m_usMonth = ptm->tm_mon + 1;
		ctime.m_usDay = ptm->tm_mday;
		ctime.m_usHour = ptm->tm_hour;
		ctime.m_usMinute = ptm->tm_min;
		ctime.m_usSecond = ptm->tm_sec;

		return ctime;
	}

	ULONG CTime::getTickCount()
	{
		if (Math::isZero(s_fStartTickCount))
		{
			tms tm;
			s_fStartTickCount = (FLOAT)times(&tm);
		}

		tms tm;
		return (ULONG)((FLOAT)(times(&tm) - s_fStartTickCount) / sysconf(_SC_CLK_TCK) * 1000);
	}
#endif

#ifdef OS_LINUX
	CTime CTime::getCurrentTime()
	{
		CTime ctime;

		struct timeval timevalBefore;
		struct timeval timevalAfter;
		gettimeofday(&timevalBefore, NULL);
		time_t timeBefore = time(NULL);
		gettimeofday(&timevalAfter, NULL);
		time_t timeAfter = time(NULL);

		struct tm *ptm;

		if ((timevalAfter.tv_sec - timevalBefore.tv_sec) != 0)
		{
			ptm = localtime(&timeAfter);

			ctime.m_usMilliseconds = timevalAfter.tv_usec / 1000;
		}
		else
		{
			ptm = localtime(&timeBefore);

			ctime.m_usMilliseconds = timevalBefore.tv_usec / 1000;
		}

		ctime.m_usYear = ptm->tm_year + 1900;
		ctime.m_usMonth = ptm->tm_mon + 1;
		ctime.m_usDay = ptm->tm_mday;
		ctime.m_usHour = ptm->tm_hour;
		ctime.m_usMinute = ptm->tm_min;
		ctime.m_usSecond = ptm->tm_sec;

		return ctime;
	}

	ULONG CTime::getTickCount()
	{
		if (Math::isZero(s_fStartTickCount))
		{
			tms tm;
			s_fStartTickCount = (FLOAT)times(&tm);
		}

		tms tm;		
		return (ULONG)((FLOAT)(times(&tm) - s_fStartTickCount) / sysconf(_SC_CLK_TCK) * 1000);
	}
#endif

	CTime::CTime()
		: m_usYear(0)
		, m_usMonth(0)
		, m_usDay(0)
		, m_usHour(0)
		, m_usMinute(0)
		, m_usSecond(0)
		, m_usMilliseconds(0)
	{
		;
	}

	CTime::CTime(const CTime &src)
		: m_usYear(src.m_usYear)
		, m_usMonth(src.m_usMonth)
		, m_usDay(src.m_usDay)
		, m_usHour(src.m_usHour)
		, m_usMinute(src.m_usMinute)
		, m_usSecond(src.m_usSecond)
		, m_usMilliseconds(src.m_usMilliseconds)
	{
		;
	}

	CTime::~CTime()
	{
		;
	}	

	CTime& CTime::operator =(const CTime &src)
	{
		m_usYear = src.m_usYear;
		m_usMonth = src.m_usMonth;
		m_usDay = src.m_usDay;
		m_usHour = src.m_usHour;
		m_usMinute = src.m_usMinute;
		m_usSecond = src.m_usSecond;
		m_usMilliseconds = src.m_usMilliseconds;

		return *this;
	}
}
