/**
* @attention Copyright
* Copyright 2013-2014 - All Rights Reserved 
* HARMAN Shanghai - HMI Team of Software
* Email: Author Zeki.Yan
*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CBaseAnimation
	*			It is the base class for any type animations.
	*/
	class OPENHMI_API CBaseAnimation : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method		
		virtual ~CBaseAnimation();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void			setAnimationData(CBaseAmtData* pAmtData);
		CBaseAmtData*	getAnimationData()		{ return m_pAmtData; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set a interpolation algorithm to this animation.
		*/
		void			setInterpolation(CBaseInterpolation* pInterpolation);

		/**
		* @brief	Set delay time to this animation. When this animation starts, it will delay to that time.
		*
		* @param	lValue The value is millisecond.
		*/
		void			setDelayTime(LONG lValue);

		/**
		* @brief	Set duration time to this animation.
		*
		* @param	lValue The value is millisecond and the delay time is not included in the duration time.
		*/
		void			setDurationTime(LONG lValue); 		
		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set a sequent animation to this animation. The sequent animation will be started when this animation ends.
		*/
		void			setSequentAnimation(CBaseAnimation* pAnimation);
		CBaseAnimation*	getSequentAnimation()	{ return m_pSequentAnimation; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*  Inner method.
		*/
		virtual	void	clearControlStatus();

		/**
		*  Inner method.
		*/
		LONG			getAnimationLength();

		/**
		*  Inner method.
		*/
		void			runAnimation(LONG lCurrentTime);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Add a animation listener to response to animation events
		*/
		void			addListener(IAnimationListener* pListener);

		/**
		* Remove a animation listener from animation listener list.
		*/
		void			removeListener(IAnimationListener* pListener);

	protected:// method
		CBaseAnimation();

		virtual	LONG	getDurationTime();
		virtual	void	updateAnimationData(LONG lOffsetTime)	{ }

	private:// method
		void	fireStarted();
		void	fireValueChanged();
		void	fireEnded();

	protected:// property
		ObjectPtr<CBaseAmtData>				m_pAmtData;
		ObjectPtr<CBaseInterpolation>		m_pInterpolation;

	private:// property
		LONG						m_lDelayTime;
		LONG						m_lDurationTime;

		CBaseAnimation*				m_pSequentAnimation;

		Array2<IAnimationListener*>	m_aryListener;

		BOOLEAN						m_bHasStarted;
		BOOLEAN						m_bHasEnded;
	};
}