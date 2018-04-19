#pragma once

#include "ClassDeclare.h"
#include "OpenHMI.h"

namespace OpenHMI
{
	class CMainFrame : public CBaseMainFrame
	{
	public:// TODO: define const here

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here

	private:// TODO: define embed class or struct or enum here
		enum EResItem
		{
			RES_ITM_UNKNOWN				= 0,

			// TODO: define control ID here

		};

	public:// method
		static	CMainFrame*	getInstance();

		// TODO: define your public method here
		void showTips(const String& str, BOOLEAN bAutoHideEdit = TRUE, BOOLEAN bShowBg = TRUE);
		void quitTips() { if (m_pPopTips != NULL) m_pPopTips->destroy();}
	protected:// method
		CMainFrame();
		virtual ~CMainFrame();

		DECLARE_EVENT_MAP

		virtual	void	onStartup(CConfigParser *pParser);
		virtual	void	pushDataToUI();
		virtual	void	onSkinResourceReady();
		virtual void	startScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter, UINT uiIntervalTime, UINT uiRepeatCount);
		virtual void	doScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter, UINT uiCurrentCount);
		virtual void	completeScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter);

	private:// method


		// TODO: define control event listener here
		void onPopTipsDsy(CBasePopup *pTarget);

		// TODO: define logic event listener here

		// TODO: define your normal private method here

	protected:// property

	private:// property
		static	CMainFrame	*s_pInstance;

		// TODO: define control member here


		// TODO: define normal private property here
		BOOLEAN		m_bScreenAmtSwitch;

		CPopTips*							m_pPopTips;
		BOOLEAN								m_bAutoHideEdit;
	};
}