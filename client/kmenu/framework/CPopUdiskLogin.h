#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CPopUdiskLogin
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
			RES_ITM_BG									 = 1 ,
			RES_ITM_UDISK_LOGIN							 = 2 ,

		};

	public:// method
		CPopUdiskLogin(BOOLEAN bModal = TRUE);
		virtual ~CPopUdiskLogin();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void setAutoQuit() { m_funcWgtUdiskLogin.setAutoQuit(); }
		void quit() { m_funcWgtUdiskLogin.quit(); }

		//virtual void	startShowAnimation(UINT uiIntervalTime, UINT uiRepeatCount);
		//virtual void	completeShowAnimaiton();

	protected:// method		

	private:// method

		// TODO: define control event listener here


		// TODO: define logic event listener here
		void onLeavePopUdiskLogin();

		// TODO: define your normal private method here

	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgBg;
		CFuncWgtUdiskLogin					m_funcWgtUdiskLogin;


		// TODO: define normal private property here
	};
}