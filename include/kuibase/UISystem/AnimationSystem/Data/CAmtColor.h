/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtColor : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CAmtColor(UINT uiValue);
		CAmtColor(BYTE btA, BYTE btR, BYTE btG, BYTE btB);
		virtual ~CAmtColor();

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

		UINT					getValue();

	protected:// method
		CAmtColor(const CAmtColor&){}
		CAmtColor& operator =(const CAmtColor&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		BYTE		m_btA;
		BYTE		m_btR;
		BYTE		m_btG;
		BYTE		m_btB;

	protected:// property

	private:// property
		CAmtColor* m_pCloneData;
	};
}