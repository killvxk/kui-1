/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"
#include "String.h"

namespace OpenHMI
{
	/**
	* @class	CacheObject	
	*			It is the base class for cache data, and the inherited class can be put into the CachePool or CachePool2.
	*/
	class OPENHMI_API CacheObject : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method		
		virtual ~CacheObject();

		/**
		*  Internal method.
		*/
		void		increaseFrequency()		{ m_uiFrequency++; }

		/**
		*  Internal method.
		*/
		UINT		getFrequency() const	{ return m_uiFrequency; }

		/**
		*  Internal method.
		*/
		UINT		getTime() const			{ return m_uiTime; }

		void			setName(const String& strName)	{ m_strName = strName;	}
		const String&	getName() const					{ return m_strName;		}

		void			setID(INT	iID)				{ m_iID = iID;		}
		INT				getID() const					{ return m_iID;		}

	protected:// method
		CacheObject();

	private:// method

	protected:// property
		
	private:// property			
		UINT		m_uiFrequency;
		UINT		m_uiTime;
		String		m_strName;
		INT			m_iID;		
	};
}