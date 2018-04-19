/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtInt : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method
		CAmtInt(INT iValue);
		virtual ~CAmtInt();		

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtInt(const CAmtInt&){}
		CAmtInt& operator =(const CAmtInt&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		INT		m_iValue;

	protected:// property

	private:// property
		CAmtInt*	m_pCloneData;
	};
}