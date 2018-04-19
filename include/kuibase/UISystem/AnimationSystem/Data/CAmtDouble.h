/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtDouble : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CAmtDouble(DOUBLE dValue);
		virtual ~CAmtDouble();		

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtDouble(const CAmtDouble&){}
		CAmtDouble& operator =(const CAmtDouble&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		DOUBLE		m_dValue;

	protected:// property

	private:// property
		CAmtDouble* m_pCloneData;
	};
}