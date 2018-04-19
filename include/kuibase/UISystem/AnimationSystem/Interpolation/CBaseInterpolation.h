/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CBaseInterpolation : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		virtual ~CBaseInterpolation();

		void		setValue1(LONG lTime, CBaseAmtData* pAmtValue);
		void		setValue2(LONG lTime, CBaseAmtData* pAmtValue);
		BOOLEAN		isInitialized();
		void		setInitialized(BOOLEAN bValue=TRUE);

		virtual CBaseAmtData* getInterpolatedValue(LONG lTime){ return NULL; };

	protected:// method
		CBaseInterpolation();

	private:// method

	protected:// property
		LONG					m_lTime1;
		ObjectPtr<CBaseAmtData>	m_pAmtValue1;

		LONG					m_lTime2;
		ObjectPtr<CBaseAmtData>	m_pAmtValue2;

	private:// property
		BOOLEAN			m_bInitialized;
	};
}