/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtFloat : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		CAmtFloat(FLOAT fValue);
		virtual ~CAmtFloat();		

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtFloat(const CAmtFloat&){}
		CAmtFloat& operator =(const CAmtFloat&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		FLOAT		m_fValue;

	protected:// property

	private:// property
		CAmtFloat*	m_pCloneData;
	};
}