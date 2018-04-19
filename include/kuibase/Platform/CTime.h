/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	/**
	* @class	CTime	
	*			It is the time class providing common interface.
	*/
	class OPENHMI_API CTime : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		static	CTime	getCurrentTime();

		/**
		* @brief	Get tick count from the system started.
		*
		* @return	The unit is millisecond.
		*/
		static	ULONG	getTickCount();

		CTime();
		CTime(const CTime &src);
		virtual ~CTime();

		USHORT	getYear() const			{ return m_usYear; }
		USHORT	getMonth() const		{ return m_usMonth; }
		USHORT	getDay() const			{ return m_usDay; }
		USHORT	getHour() const			{ return m_usHour; }
		USHORT	getMinute() const		{ return m_usMinute; }
		USHORT	getSecond() const		{ return m_usSecond; }
		USHORT	getMilliseconds() const	{ return m_usMilliseconds; }

		CTime& operator =(const CTime &src);

	protected:// method		
		
	private:// method
		
	protected:// property

	private:// property
		USHORT	m_usYear;
		USHORT	m_usMonth;
		USHORT	m_usDay;
		USHORT	m_usHour;
		USHORT	m_usMinute;
		USHORT	m_usSecond;
		USHORT	m_usMilliseconds;

		static	FLOAT	s_fStartTickCount;
	};
}