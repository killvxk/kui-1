#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CFuncWgtOpMenu
		: public CBaseFuncWidget
	{
	public:// TODO: define const here

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here

	private:// TODO: define embed class or struct or enum here
		enum EResItem
		{
			RES_ITM_UNKNOWN				= 0,

			// TODO: define control ID here
			RES_ITM_BTN_RUN								 = 1 ,
			RES_ITM_BTN_OPEN							 = 2 ,
			RES_ITM_BTN_INTRO							 = 3 ,
			RES_ITM_BTN_SAVE_PROGRESS					 = 5 ,
			RES_ITM_BTN_READ_PROGRESS					 = 6 ,
			RES_ITM_BTN_UPLOAD_CONFIG					 = 7 ,
			RES_ITM_JIANGE								 = 8 ,

		};

	public:// method
		CFuncWgtOpMenu();
		virtual ~CFuncWgtOpMenu();

		DECLARE_EVENT_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();
		virtual	ICursorTracker*	probe(INT iX, INT iY);

		// TODO: define your public method here
		void showGameMenu(INT gid, const String& name, const String& url);

	protected:// method		

	private:// method
		
		// TODO: define control event listener here
		void	onBtnRunTchRel(CComponent *pTarget);
		void	onBtnOpenTchRel(CComponent *pTarget);
		void	onBtnIntroTchRel(CComponent *pTarget);
		void	onBtnSaveProgressTchRel(CComponent *pTarget);
		void	onBtnReadProgressTchRel(CComponent *pTarget);
		void	onBtnUploadConfigTchRel(CComponent *pTarget);


		// TODO: define logic event listener here

		// TODO: define your normal private method here
		
	protected:// property

	private:// property
		
		// TODO: define control member here
		CCtrlButton							m_btnRun;
		CCtrlButton							m_btnOpen;
		CCtrlButton							m_btnIntro;
		CCtrlButton							m_btnSaveProgress;
		CCtrlButton							m_btnReadProgress;
		CCtrlButton							m_btnUploadConfig;
		CCtrlImageField						m_imgJiange;


		// TODO: define normal private property here
		INT									m_iGid;
		String								m_strName;
		String								m_strUrl;
	};
}
