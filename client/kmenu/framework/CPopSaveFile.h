#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CPopSaveFile
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
			RES_ITM_SAVE_FILE							 = 2 ,

		};

	public:// method
		CPopSaveFile(BOOLEAN bModal = TRUE);
		virtual ~CPopSaveFile();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();

		// TODO: define your public method here
		void saveFile(DWORD gid) { m_funcWgtSaveFile.saveFile(gid); }
	
	protected:// method		

	private:// method

		// TODO: define control event listener here


		// TODO: define logic event listener here
		void onLeaveFile();
		// TODO: define your normal private method here

	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlImageField						m_imgBg;
		CFuncWgtSaveFile					m_funcWgtSaveFile;


		// TODO: define normal private property here
	};
}