/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CAmtRectangle : public CBaseAmtData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		CAmtRectangle(INT iLeft, INT iTop, INT iWidth, INT iHeight);
		virtual ~CAmtRectangle();

		virtual CBaseAmtData*	add(CBaseAmtData* pData);
		virtual CBaseAmtData*	subtract(CBaseAmtData* pData);
		virtual CBaseAmtData*	multiply(FLOAT fValue);
		virtual CBaseAmtData*	divide(FLOAT fValue);	
		virtual CBaseAmtData*	cloneData();
		virtual void			set(CBaseAmtData* pData);

	protected:// method
		CAmtRectangle(const CAmtRectangle&){}
		CAmtRectangle& operator =(const CAmtRectangle&){return *this;}

	private:// method
		void genCloneData();

	public:// property
		INT		m_iLeft;
		INT		m_iTop;
		INT		m_iWidth;
		INT		m_iHeight;

	protected:// property

	private:// property
		CAmtRectangle*	m_pCloneData;
	};
}