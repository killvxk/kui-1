/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtChar : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CAmtChar(CHAR cValue);
		virtual ~CAmtChar();

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtChar(const CAmtChar&){}
		CAmtChar& operator =(const CAmtChar&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		CHAR		m_cValue;

	protected:// property

	private:// property
		CAmtChar* m_pCloneData;
	};
}