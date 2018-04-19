#pragma once

#include "OpenHMI.h"

namespace OpenHMI
{
	class CPrjBaseScreen : public CBaseScreen
	{
	public:// TODO: define const here

	private:// TODO: define const here

	public:// TODO: define embed class or struct or enum here

	private:// TODO: define embed class or struct or enum here

	public:// method	
		virtual ~CPrjBaseScreen();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP
	
		virtual BOOLEAN	initialize();
		virtual void	unInitialize();
		virtual	void	pushDataToUI();
		virtual BOOLEAN	onHardkey(UINT uiKeyCode, INT iKeyStatus);

		virtual void	show();
		virtual void	hide();
		virtual	void	DestroyUserDlg() {}
		virtual void	UserDlgLoginSuccess(const CString& id, const CString& name, const CString& url, const CString& fcm) {}
		virtual void	SetUserDlgAttr(const CString& title, int width, int height){}
		virtual void	OpenUserDlgWin(const CString& url, const CString& title, int with, int height) {}
		virtual void	PopToMenu(const CString& url) {}
		virtual void	UserDlgLoginOut(const CString& id) {}

		// TODO: define your public method here

	protected:// method
		CPrjBaseScreen();

		// TODO: define your protected method here
		void	onBtnBackTchRel(CComponent *pTarget);

	private:// method

		// TODO: define control event listener here

		// TODO: define logic event listener here

		// TODO: define your normal private method here

	protected:// property

		// TODO: define protected property here

	private:// property
		
		// TODO: define control member here

		// TODO: define normal private property here
	};
}