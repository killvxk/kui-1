/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CTimer	
	*			It is the timer class. First you declare an instance in your class, the class should implement the ITimerListenr interface, then start it in somewhere. 
	*/
	class OPENHMI_API CTimer : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum ETimerStatus
		{
			TS_TIMER = 0,
			TS_TIMER_COMPLETE,
		};

	private:// embed class or struct or enum
		friend class CTimerManager;

	public:// method
		CTimer();
		CTimer(UINT uiDelay, UINT uiRepeatCount = 0);
		virtual ~CTimer();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	Set a listener to response to the timer event.
		*
		* @param	pListener	A class implemented ITimerListener to response to the timer event.
		*/
		void	setListener(ITimerListener *pListener) { m_pListener = pListener; }

		/**
		* @brief	Set delay time to the timer.
		*
		* @param	uiValue	The unit is millisecond.
		*/
		void	setDelay(UINT uiValue);

		UINT	getDelay() const		{ return m_uiDelay; }

		/**
		* @brief	Set repeat count to the timer.
		*
		* @param	uiValue	If it is 0, the timer runs all the while.
		*/
		void	setRepeatCount(UINT uiValue);

		UINT	getRepeatCount() const	{ return m_uiRepeatCount; }

		UINT	getCurrentCount() const	{ return m_uiCurrentCount; }	

		BOOLEAN	isRunning() const		{ return m_bRunning; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Reset the timer, and the current count will be set to 0.
		*			
		*/
		void	reset();

		/**
		* Start the timer again from the last current count.
		*
		*/
		void	start();

		/**
		*  the timer without reseting current count to 0.
		*
		*/
		void	stop();		

	protected:// method
		CTimer(const CTimer&){}
		CTimer& operator =(const CTimer&){return *this;}

	private:// method
		
	protected:// property
		void	increaseCurrentCount();
		BOOLEAN	isTimeOut(UINT uiTickets);
		
	private:// property
		ITimerListener	*m_pListener;

		UINT			m_uiDelay;
		UINT			m_uiRepeatCount;
		UINT			m_uiCurrentCount;
		BOOLEAN			m_bRunning;
		UINT			m_uiInterval;
	};
}