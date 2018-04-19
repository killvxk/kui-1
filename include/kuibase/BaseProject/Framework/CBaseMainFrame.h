/**
* @attention Copyright
* Copyright 2013-2014 - All Rights Reserved 
* HARMAN Shanghai - HMI Team of Software
* Email: Author Zeki.Yan
*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CBaseMainFrame
	*			This class defines the main structure for a HMI Application.
	*/
	class OPENHMI_API CBaseMainFrame 
		: public CContainer
		, public ITimerListener
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EFreeControlPos
		{
			FCP_BELOW_SCREEN_STACK = 0,
			FCP_BETWEEN_SCREEN_STACK_AND_OVERLAY_STACK,
			FCP_AMONG_OVERLAY_STACK,
			FCP_ABOVE_OVERLAY_STACK,
		};

	private:// embed class or struct or enum

		friend class CMain;

	public:// method		
		static	CBaseMainFrame*	getInstance();	

		DECLARE_EVENT_MAP

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	Make background visible, the background can have more than one image files.
		*
		* @param	uiIndex The index to the image list, start from 1.
		*/
		void			showBackground(UINT uiIndex = 1);

		/**
		* Make background invisible.
		*/
		void			hideBackground();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Add a new screen into screen stack. The main frame manage the life cycle for this screen.
		* This function is used in framework, you should not use it directly in project developping.This will not cause the screen animation.
		*/
		void			addScreen(CBaseScreen *pScreen);

		/**
		* Remove a screen from the screen stack.
		* This will not cause the screen animation.
		*/
		void			removeScreen(CBaseScreen *pScreen);

		/**
		* @brief	Remove screens in the screen stack.
		*			This will not cause the screen animation.
		*			
		* @param	aryScreenID	An array of screen IDs you want to remove from the screen stack.
		*/
		void			removeScreens(const Array2<UINT> &aryScreenID);

		/**
		* Get a screen from the screen stack according to the screen ID.
		*/
		CBaseScreen*	getScreen(UINT uiID);

		/**
		* Get the first visible screen from the screen stack.
		*/
		CBaseScreen*	getTopScreen();
		
		UINT			getScreenCount() { return m_screenContainer.getComponentCount(); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Make home screen visible
		* Remove all screens from the screen stack except the home screen. this will cause the screen animation.
		*/
		void			showHomeScreen();

		/**
		* @brief	Remove all screens from the screen stack except the home screen and the new entering screen. This will cause the screen animation.
		*
		* @param	strClassName	The application name, same with the name you call registerApplication method of CManager_Application.
		* @param	mapCommand		The application start params, please refer to the application's start method comment.
		*/
		void			switchToApplication(const String &strClassName, const Map2<UINT, UINT, tagNumCompare<UINT> > &mapCommand);

		/**
		* @brief	Add the new entering screen to the screen stack without removing old screens. This will cause the screen animation.
		*
		* @param	strClassName	The application name, same with the name you call registerApplication method of CManager_Application.
		* @param	mapCommand		the application start params, please refer to the application's start method comment.
		*/
		void			enterIntoApplication(const String &strClassName, const Map2<UINT, UINT, tagNumCompare<UINT> > &mapCommand);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Add a control to the overlay stack.
		* The life cycle of control is managed by the derived class of the CBaseMainFrame.
		*/
		void			addOverlay(CComponent *pComponent);

		/**
		* Remove a control from the overlay stack.
		*/
		void			removeOverlay(CComponent *pComponent);

		/**
		* Get a control from the control stack according to the control ID.
		*/
		CComponent*		getOverlay(UINT uiID);

		/**
		* Get the first visible control from the control stack, exclude the pointed controls.
		*/
		CComponent*		getTopOverlay(const Array2<CComponent*> &aryExcludedCompont);

		UINT			getOverlayCount() { return m_overlayContainer.getComponentCount(); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* For some specific use case, you maybe need to use free control.
		* The life cycle of free control is managed by the derived class of the CBaseMainFrame.
		*/
		void			showFreeControl(CComponent *pComponent, EFreeControlPos ePos);

		void			hideFreeControl(CComponent *pComponent);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CWgtMessageBox*  showMessageBox(UINT								uiID,
										CWgtMessageBox::tagMessageBoxParam	tagParam);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*  Inner method.
		*/
		CRenderGroup*	buildRenderTree();

		/**
		*  Inner method.
		*/
		void			breakUpRenderTree();


		virtual void	resetLayout() { }
		virtual	void	refresh();
		virtual	void	destroy();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*  Inner method.
		*/
		void					notifySkinFirstPrepared();

		/**
		*  Inner method.
		*/
		void					notifySkinChanged();

		/**
		*  Inner method.
		*/
		void					innerStartScreenAnimation();

		/**
		*  Inner method.
		*/
		void					innerStopScreenAnimation();

		CBaseOperationCenter*	getOperationCenter()	{ return m_pOperationCenter; }
		CBaseScreen*			getHomeScreen()			{ return m_pHomeScreen; }		

		BOOLEAN					getUseGdi() { return m_bUseGdi; }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// implement interfaces
		virtual	void	onTimer(const CTimer *pTimer, UINT uiCurrentCount);
		virtual	void	onTimerComplete(const CTimer *pTimer);		

	protected:// method
		CBaseMainFrame();
		virtual ~CBaseMainFrame();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Register your own OperationCenter to the framework to deal with hard key and global event.
		*/
		void			registerOperationCenter(CBaseOperationCenter *pOperationCenter);

		/**
		* Register your own home screen to framework.
		*/
		void			registerHomeScreen(CBaseScreen *pHomeScreen);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	When the application has startup, will callback this function.
		*			The derived class should override this method.
		*
		* @param	pParser	 A pointer to the project CConfigParser file.
		*/
		virtual	void	onStartup(CConfigParser *pParser) = 0;

		/**
		* Initialize skin info and set data to children controls or set children controls status.
		* The derived class should override this method.
		*/
		virtual	void	pushDataToUI() = 0;

		/**
		* When the skin system if loaded, will callback this function.
		* The derived class should override this method.
		*/
		virtual	void	onSkinResourceReady() = 0;

		/**
		* @brief	When the animation starts, should set the right properties to the leaving and entering screen.
		*			The derived class should override this method.
		*			
		* @param	pScreenToLeave	The leaving screen while animation.
		* @param	pScreenToEnter	The entering screen while animation.
		* @param	uiIntervalTime	The unit is millisecond.
		* @param	uiRepeatCount	The iIntervalTime * iRepeatCount is the total animation time.
		*/
		virtual void	startScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter, UINT uiIntervalTime, UINT uiRepeatCount);

		/**
		* @brief	This method will be called many times during the screen animation. 
		*			You should adjust the two screens' properties according to your animation style.
		*			The derived class should override this method.
		*			
		* @param	pScreenToLeave	The leaving screen while animation.
		* @param	pScreenToEnter	The entering screen while animation.
		* @param	uiCurrentCount	Current repeat count.
		*/
		virtual void	doScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter, UINT uiCurrentCount);

		/**
		* @brief	When the animation complete, should set the right properties to the enter screen.
		*			The derived class should override this method.
		*
		* @param	pScreenToLeave	The leaving screen while animation.
		* @param	pScreenToEnter	The entering screen while animation.
		*/
		virtual void	completeScreenAnimation(CBaseScreen *pScreenToLeave, CBaseScreen *pScreenToEnter);

	private:// method
		BOOLEAN			initialize();
		void			unInitialize();

		virtual	void	buildRenderTree(CRenderGroup &renderGroup);	

		static  void	onMessageBoxDestroy(Object* pParam);

	protected:// property

	private:// property
		static	CBaseMainFrame	*s_pInstance;

		CContainer 				m_screenContainer;
		CContainer 				m_overlayContainer;

		CBaseOperationCenter	*m_pOperationCenter;
		CBaseScreen				*m_pHomeScreen;	

		ObjectPtr<CBaseScreen>	m_ptrRubbishScreen;
		ObjectPtr<CBasePopup>	m_ptrRubbishPopup;

		BOOLEAN					m_bShowBackground;

		CTimer					m_timerScreen;
		CBaseScreen				*m_pScreenToEnter;
		CBaseScreen				*m_pScreenToLeave;
		BOOLEAN 				m_bDestroyLeaveScreen;

		BOOLEAN					m_bUseGdi;
	};
}