#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CPopUdiskReg
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
			RES_ITM_UDISK_REG							 = 2 ,

		};

	public:// method
		CPopUdiskReg(BOOLEAN bModal = TRUE);
		virtual ~CPopUdiskReg();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void enter() { m_funcWgtUdiskReg.enter(); }
		void quit() { m_funcWgtUdiskReg.quit(); }

	protected:// method		

	private:// method

		// TODO: define control event listener here


		// TODO: define logic event listener here
		void onLeavePopUdiskReg();

		// TODO: define your normal private method here

	protected:// property

	private:// property
		
		// TODO: define control member here
		CFuncWgtUdiskReg					m_funcWgtUdiskReg;


		// TODO: define normal private property here
	};
}