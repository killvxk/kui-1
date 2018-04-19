/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"
#include "../../BaseProject/Framework/CBaseMessage.h"

namespace OpenHMI
{
	class OPENHMI_API CMsgTimer : public CBaseMessage
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// CMsgTimer
		CMsgTimer(	CTimer					*pTimer,
					ITimerListener			*pListener,
					CTimer::ETimerStatus	eStatus,
					UINT					uiCurrentCount);
		virtual ~CMsgTimer();

		virtual void	handleMessage();

	protected:// method
		CMsgTimer(const CMsgTimer&){}
		CMsgTimer& operator =(const CMsgTimer&){return *this;}

	private:// method

	protected:// property

	private:// property
		CTimer					*m_pTimer;
		ITimerListener			*m_pListener;
		CTimer::ETimerStatus	m_eStatus;
		UINT					m_uiCurrentCount;
	};
}