#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CPopAdmin
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
			RES_ITM_ADMIN								 = 2 ,

		};

	public:// method
		CPopAdmin(BOOLEAN bModal = TRUE);
		virtual ~CPopAdmin();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void enter() { m_funcWgtAdmin.enter(); }
		void quit() { m_funcWgtAdmin.quit(); }
		void showBg(BOOLEAN bShowBg = TRUE) { m_bShowBg = bShowBg; }
	protected:// method		

	private:// method

		// TODO: define control event listener here


		// TODO: define logic event listener here
		void onLeaveAdmin();

		// TODO: define your normal private method here

	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgBg;
		CFuncWgtAdmin						m_funcWgtAdmin;


		// TODO: define normal private property here
		BOOLEAN							m_bShowBg;
	};
}