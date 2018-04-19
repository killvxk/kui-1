/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CUniformRectilinearMotionAnimation : public CBaseMotionAnimation
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		CUniformRectilinearMotionAnimation();
		virtual 	~CUniformRectilinearMotionAnimation();
		
		void		setStartPoint(CAmtVector2* pAmtvValue);
		void		setSpeed(CAmtVector2* pAmtvValue);
		void		setParams(	CAmtPoint*		pAmtpData, 
								LONG			lDurationTime, 
								CAmtVector2*	pAmtvStartPoint, 
								CAmtVector2*	pAmtvSpeed,
								IAnimationListener*	pListener);
								
		virtual void updateAnimationData(LONG lOffsetTime);

	protected:// method
		CUniformRectilinearMotionAnimation(const CUniformRectilinearMotionAnimation&){}
		CUniformRectilinearMotionAnimation& operator =(const CUniformRectilinearMotionAnimation&){return *this;}
		
	private:// method

	protected:// property

	private:// property
		ObjectPtr<CAmtVector2>	m_pAmtvStartPoint;
		ObjectPtr<CAmtVector2>	m_pAmtvSpeed;
	};
}