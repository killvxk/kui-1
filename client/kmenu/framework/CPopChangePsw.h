#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CPopChangePsw
	 : public CBasePopup
	{
	public:// TODO: define const here

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here

	private:// TODO: define embed class or struct or enum here
		enum EResItem
		{
			RES_ITM_UNKNOWN				= 0,

			// TODO: define control ID here
			RES_ITM_CHANGE_PSW							 = 2 ,

		};

	public:// method
		CPopChangePsw(BOOLEAN bModal = TRUE);
		virtual ~CPopChangePsw();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void enter() { m_funcWgtChangePsw.enter(); }
		void quit() { m_funcWgtChangePsw.quit(); }

	protected:// method		

	private:// method

		// TODO: define control event listener here


		// TODO: define logic event listener here
		void onLeaveChangePsw();

		// TODO: define your normal private method here

	protected:// property

	private:// property
		
		// TODO: define control member here
		CFuncWgtChangePsw					m_funcWgtChangePsw;


		// TODO: define normal private property here
	};
}