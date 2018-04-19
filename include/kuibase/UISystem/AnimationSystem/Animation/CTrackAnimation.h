/*@Author Zeki.Yan*/
#pragma once

#include "../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CTrackAnimation : public CBaseAnimation
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum				

	private:// embed class or struct or enum

	public:// method	
		CTrackAnimation();
		virtual ~CTrackAnimation();

		void setAnimationData(CAmtPoint* pAmtpData);
		void setPath(CBasePath* pPath);
		void setStartPercent(FLOAT fValue);
		void setEndPercent(FLOAT fValue);

		void setParams( CAmtPoint*				pAmtPoint, 
						LONG					lDurationTime,
						CBasePath*				pPath,
						FLOAT					fStartPercent, 
						FLOAT					fEndPercent,
						IAnimationListener*		pListener);
	
	protected:// method
		CTrackAnimation(const CTrackAnimation&){}
		CTrackAnimation& operator =(const CTrackAnimation&){return *this;}

		virtual void	updateAnimationData(LONG lOffsetTime);

	private:// method

	protected:// property

	private:// property
		ObjectPtr<CBasePath>	m_pPath;
		ObjectPtr<CAmtFloat>	m_pAmtStartPercent;
		ObjectPtr<CAmtFloat>	m_pAmtEndPercent;
	};
}