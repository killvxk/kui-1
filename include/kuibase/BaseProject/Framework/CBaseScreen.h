/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CBaseScreen
	*			This class is the base class for all the project screens.
	*/
	class OPENHMI_API CBaseScreen 
		: public CContainer
		, public ICommandListener
		//, public CCommandDispatcher
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		virtual ~CBaseScreen();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	After this Screen created, it should be initialized using this function.
		*			The derived class should override this method.
		*			
		* @return	TRUE	Means this Screen is ready to enter.
		*			FALSE	Means this Screen is not able to enter.
		*/
		virtual BOOLEAN	initialize() = 0;

		/**
		* Before this Screen is destroyed, it should be uninitialized using this function.
		* The derived class should override this method.
		*/
		virtual void	unInitialize() = 0;

		/**
		* Initialize skin info and set data to children controls or set children controls status according to the logic data.
		* After calling this method, the refresh method will be called automatically.
		* The derived class should override this method.
		*/
		virtual	void	pushDataToUI() = 0;

		/**
		* @brief	Deal with the hard key event.
		*			The derived class should override this method.
		*			
		* @param	uiKeyCode	The key code for hard key, it is defined by general projects.
		* @param	iKeyStatus	The key status for hard key, it is defined by general projects.
		* @return	TRUE	Means the Screen has deal with this hard key..
		*			FALSE	Means the Screen has not deal with this hard key.
		*/
		virtual BOOLEAN	onHardkey(UINT uiKeyCode, INT iKeyStatus) = 0;		

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* When screen becomes visible, the system will call this method.
		* Maybe the derived class should override this method.
		*/
		virtual void	show();

		/**
		* When screen becomes unVisible, the system will call this method.
		* Maybe the derived class should override this method.
		*/
		virtual void	hide();		

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Enter into this screen. 
		* This will cause the screen animation.The framework will add this screen to the screen stack.
		*/
		void			enter();

		/**
		* Leave from this screen. 
		* This screen will be destroyed. this will cause the screen animation.
		* The framework will remove this screen from the screen stack.	
		*/
		void			leave();

		/**
		* Indicates whether this screen is transparent or not. 
		*/
		BOOLEAN			isTransparent() const	{ return m_bIsTransparent; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual	void	refresh();
		virtual	void	destroy();

		virtual BOOLEAN	doFocusBack();

		virtual	void	DestroyUserDlg() {}
		virtual void	UserDlgLoginSuccess(const CString& id, const CString& name, const CString& url, const CString& fcm){}
		virtual void	SetUserDlgAttr(const CString& title, int width, int height) {}
		virtual void	OpenUserDlgWin(const CString& url, const CString& title, int with, int height) {}
		virtual void	PopToMenu(const CString& url) {}
		virtual void	UserDlgLoginOut(const CString& id) {}

	protected:// method
		CBaseScreen();

		void			setTransparent()		{ m_bIsTransparent = TRUE; }

		/**
		* Go to previous screen. 
		*/
		virtual	void	doBack();

		/**
		* Go to home screen. 
		*/
		virtual	void	doHome();

	private:// method

	protected:// property

	private:// property
		BOOLEAN		m_bIsTransparent;
	};
}