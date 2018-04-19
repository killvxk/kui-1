/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtString : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		CAmtString(const CHAR* pcValue);
		CAmtString(const String &strValue);
		virtual ~CAmtString();

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtString(const CAmtString&){}
		CAmtString& operator =(const CAmtString&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		String		m_strValue;

	protected:// property

	private:// property
		CAmtString*	m_pCloneData;
	};
}