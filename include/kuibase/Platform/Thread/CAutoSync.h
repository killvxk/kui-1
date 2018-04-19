/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CAutoSync	
	*			It is a help class used to protect some data, make it can be accessed by one thread as one time.
	*/
	class OPENHMI_API CAutoSync : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CAutoSync(CSyncObject &syncObj);
		virtual ~CAutoSync();

	protected:// method
		CAutoSync(const CAutoSync& src) : m_syncObj(src.m_syncObj){}
		CAutoSync& operator =(const CAutoSync&){return *this;}

	private:// method		

	protected:// property

	private:// property
		CSyncObject	&m_syncObj;
	};
}