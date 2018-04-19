/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CTimerManager : public CThread
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum
	
	public:// method		
		static	CTimerManager*	getInstance();
		static	void			deleteInstance();

		void			addTimer(CTimer *pTimer);
		void			removeTimer(CTimer *pTimer);

		virtual void	stop();

		virtual	void	run();

	protected:// method
		CTimerManager();
		virtual ~CTimerManager();

	private:// method
		
	protected:// property

	private:// property
		static	CTimerManager		*s_pInstance;

		Array<CTimer>				m_aryTimer;

		UINT						m_uiInterval;
		CWaitObject					m_waitTimer;
	};
}