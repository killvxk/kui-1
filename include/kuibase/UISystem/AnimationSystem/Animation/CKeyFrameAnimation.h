/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CKeyFrameAnimation : public CBaseAnimation
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CKeyFrameAnimation();
		virtual ~CKeyFrameAnimation();
		
		UINT			getKeyFrameCount();
		CKeyFrame*		getKeyFrame(UINT uiIndex);
		void			addKeyFrame(CKeyFrame* pFrame);
		void			removeKeyFrame(UINT uiIndex);
		void			clearAllKeyFrames();
		void			setParams(CBaseAmtData* pAmtData,IAnimationListener* pListener);

	protected:// method
		CKeyFrameAnimation(const CKeyFrameAnimation&){}
		CKeyFrameAnimation& operator =(const CKeyFrameAnimation&){return *this;}

		virtual void	updateAnimationData(LONG lOffsetTime);
		virtual LONG	getDurationTime();

	private:// method

	protected:// property

	private:// property
		Array<CKeyFrame>	m_aryKeyFrame;
	};
}