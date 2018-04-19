/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CValueAnimation : public CBaseAnimation
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CValueAnimation();
		virtual ~CValueAnimation();

		void setStartValue(CBaseAmtData* pAmtValue)	{ m_pAmtStartValue = pAmtValue; };
		void setEndValue(CBaseAmtData* pAmtValue)	{ m_pAmtEndValue = pAmtValue; };

		void setParams( CBaseAmtData*			pAmtData, 
						LONG					lDurationTime, 
						CBaseAmtData*			pAmtStartValue, 
						CBaseAmtData*			pAmtEndValue,
						IAnimationListener*		pListener);

	protected:// method
		CValueAnimation(const CValueAnimation&){}
		CValueAnimation& operator =(const CValueAnimation&){return *this;}

		virtual void	updateAnimationData(LONG lOffsetTime);

	private:// method

	protected:// property

	private:// property
		ObjectPtr<CBaseAmtData>	m_pAmtStartValue;
		ObjectPtr<CBaseAmtData>	m_pAmtEndValue;
	};
}