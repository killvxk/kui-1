/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtLong : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CAmtLong(LONG lValue);
		virtual ~CAmtLong();		

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtLong(const CAmtLong&){}
		CAmtLong& operator =(const CAmtLong&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		LONG		m_lValue;

	protected:// property

	private:// property
		CAmtLong*	m_pCloneData;
	};
}