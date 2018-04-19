/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CAnimationPlayer
	*			It is used to control animation play status.
	*/
	class OPENHMI_API CAnimationPlayer 
		: public Object
		, public ITimerListener
		, public CAdapterAnimationListener
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EPlayStatus
		{
			PS_PLAYING = 0,
			PS_PAUSED,
			PS_STOPPED,
		};

	private:// embed class or struct or enum

	public:// method	
		CAnimationPlayer();
		virtual ~CAnimationPlayer();		

		/**
		* Set a frame value per second to control animation playing speed.		
		*/
		void			setFPS(INT iValue);

		EPlayStatus		getPlayStatus()	{ return m_ePlayStatus; }

		void	play(CBaseAnimation* pAnimation, BOOLEAN bIsCircle = FALSE);		
		void	pause();
		void	resume();
		void	stop();

		// implement timer interface
		virtual	void	onTimer(const CTimer *pTimer, UINT uiCurrentCount);
		virtual	void	onTimerComplete(const CTimer *pTimer);

		// override CAdapterAnimationListener's function
		void	onAnimationEnded(CBaseAnimation* pSource);

	protected:// method
		CAnimationPlayer(const CAnimationPlayer&){}
		CAnimationPlayer& operator =(const CAnimationPlayer&){return *this;}

	private:// method
		void	innerPlay();

	protected:// property

	private:// property
		CBaseAnimation*	m_pAnimation;

		INT				m_iFPS;
		EPlayStatus		m_ePlayStatus;
		LONG			m_lPlayStartTime;
		LONG			m_lPauseStartTime;
		LONG			m_lPausedDurationTime;		
		BOOLEAN			m_bIsCircle;

		CTimer			m_timer;
	};
}