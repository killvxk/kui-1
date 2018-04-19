#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CPopChangeSkin
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
			RES_ITM_CHANGE_SKIN							 = 9 ,

		};

	public:// method
		CPopChangeSkin(BOOLEAN bModal = TRUE);
		virtual ~CPopChangeSkin();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here

	protected:// method		

	private:// method

		// TODO: define control event listener here


		// TODO: define logic event listener here
		void onLeaveChangeSkin();

		// TODO: define your normal private method here

	protected:// property
#if 0
		virtual void	startShowAnimation(UINT uiIntervalTime, UINT uiRepeatCount);
		virtual void	doShowAnimation(UINT uiCurrentCount);
		virtual void	completeShowAnimaiton();

		virtual void	startHideAnimation(UINT uiIntervalTime, UINT uiRepeatCount);
		virtual void	doHideAnimation(UINT uiCurrentCount);
		virtual void	completeHideAnimaiton();
#endif

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgBg;
		CFuncWgtChangeSkin					m_funcWgtChangeSkin;


		// TODO: define normal private property here
		INT					m_iOrignalX;
		INT					m_iOrignalY;
	};
}