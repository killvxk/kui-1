/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtByte : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method
		CAmtByte(BYTE btValue);
		virtual ~CAmtByte();		

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtByte(const CAmtByte&){}
		CAmtByte& operator =(const CAmtByte&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		BYTE		m_btValue;

	protected:// property

	private:// property
		CAmtByte* m_pCloneData;
	};
}