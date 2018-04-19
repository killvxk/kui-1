/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtPoint : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		CAmtPoint(INT iX, INT iY);
		virtual ~CAmtPoint();		

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtPoint(const CAmtPoint&){}
		CAmtPoint& operator =(const CAmtPoint&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		INT		m_iX;
		INT		m_iY;

	protected:// property

	private:// property
		CAmtPoint*	m_pCloneData;
	};
}