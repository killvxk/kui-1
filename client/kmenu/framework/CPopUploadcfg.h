#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CPopUploadcfg
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
			RES_ITM_UPLOADCFG							 = 1 ,

		};

	public:// method
		CPopUploadcfg(BOOLEAN bModal = TRUE);
		virtual ~CPopUploadcfg();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void showCfg(DWORD gid) { m_funcWgtUploadcfg.showCfg(gid); }
		void quit() { m_funcWgtUploadcfg.quit(); }

	protected:// method		

	private:// method

		// TODO: define control event listener here


		// TODO: define logic event listener here
		void onLeaveCfg();

		// TODO: define your normal private method here

	protected:// property

	private:// property
		
		// TODO: define control member here
		CFuncWgtUploadcfg					m_funcWgtUploadcfg;
		CCtrlImageField						m_imgBg;


		// TODO: define normal private property here
	};
}