/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtVector2 : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CAmtVector2(FLOAT fX, FLOAT fY);
		virtual ~CAmtVector2();

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtVector2(const CAmtVector2&){}
		CAmtVector2& operator =(const CAmtVector2&){return *this;}
		
	protected:// method

	private:// method
		void genCloneData();

	public:// property
		FLOAT		m_fX;
		FLOAT		m_fY;

	protected:// property

	private:// property
		CAmtVector2*	m_pCloneData;
	};
}