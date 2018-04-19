/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtBoolean : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		CAmtBoolean(BOOLEAN bValue);
		virtual ~CAmtBoolean();		

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtBoolean(const CAmtBoolean&){}
		CAmtBoolean& operator =(const CAmtBoolean&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		BOOLEAN		m_bValue;

	protected:// property

	private:// property
		CAmtBoolean* m_pCloneData;
	};
}