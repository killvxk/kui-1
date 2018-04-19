#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CPopTips
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
			RES_ITM_BG									 = 2 ,
			RES_ITM_TIPS								 = 8 ,

		};

	public:// method
		CPopTips(BOOLEAN bModal = TRUE);
		virtual ~CPopTips();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void setContent(const String& str, BOOLEAN bShowBg = TRUE) { m_strTips = str;  m_bShowBg = bShowBg; }
		void setNotCatchKey() { m_funcWgtTips.setNotCatchKey(); }
	protected:// method		
		//virtual void	startShowAnimation(UINT uiIntervalTime, UINT uiRepeatCount);
		//virtual void	doShowAnimation(UINT uiCurrentCount);
		//virtual void	completeShowAnimaiton();

		//virtual void	startHideAnimation(UINT uiIntervalTime, UINT uiRepeatCount);
		//virtual void	doHideAnimation(UINT uiCurrentCount);
		//virtual void	completeHideAnimaiton();

	private:// method

		// TODO: define control event listener here


		// TODO: define logic event listener here
		void onLeavePopTips();

		// TODO: define your normal private method here

	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgBg;
		CFuncWgtTips						m_funcWgtTips;


		// TODO: define normal private property here
		String								m_strTips;
		INT					m_iOrignalX;
		INT					m_iOrignalY;
		BOOLEAN				m_bShowBg;
	};
}