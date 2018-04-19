/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CAnimationGroup
	*			It is used to make more than one animations to play together.
	*/
	class OPENHMI_API CAnimationGroup : public CBaseAnimation
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CAnimationGroup();
		virtual ~CAnimationGroup();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		INT				getAnimationCount();
		CBaseAnimation*	getAnimation(UINT uiIndex);
		void			addAnimation(CBaseAnimation* pAnimation);
		void			removeAnimation(CBaseAnimation* pAnimation);
		void			clearAllAnimations();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*  Inner method.
		*/
		virtual	void	clearControlStatus();

	protected:// method
		CAnimationGroup(const CAnimationGroup&){}
		CAnimationGroup& operator =(const CAnimationGroup&){return *this;}

		virtual	LONG	getDurationTime();
		virtual	void	updateAnimationData(LONG lOffsetTime);

	private:// method

	protected:// property

	private:// property
		Array2<CBaseAnimation*>	m_aryListAnimation;
	};
}