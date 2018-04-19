/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtShort : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CAmtShort(SHORT sValue);
		virtual ~CAmtShort();		

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtShort(const CAmtShort&){}
		CAmtShort& operator =(const CAmtShort&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		SHORT		m_sValue;

	protected:// property

	private:// property
		CAmtShort*	m_pCloneData;
	};
}