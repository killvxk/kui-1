/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventPopup
	{
	public:
		static const UINT DESTROYED		= 0;
	};

	/**
	* @class	CBasePopup
	*			This class is the base class for all the project Popups.
	*/
	class OPENHMI_API CBasePopup 
		: public CContainer
		, public ICommandListener
		, public ITimerListener
		//, public CCommandDispatcher
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		virtual ~CBasePopup();

		DECLARE_EVENT_MAP
		DECLARE_CMD_MAP

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	After this Popup created, it should be initialized using this function.
		*			The derived class should override this method.
		*			
		* @return	TRUE	Means this Popup is ready to enter.
		*			FALSE	Means this Popup is not able to enter.
		*/
		virtual BOOLEAN	initialize() = 0;

		/**
		* Before this Popup is destroyed, it should be uninitialized using this function.
		* The derived class should override this method.
		*/
		virtual void	unInitialize() = 0;

		/**
		* Initialize skin info and set data to children controls or set children controls status according to the logic data.
		* After calling this method, the refresh method will be called automatically.
		* The derived class should override this method.
		*/
		virtual	void	pushDataToUI() = 0;		

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		void			setRealX(UINT uiValue)		{ m_iRealX = (INT)uiValue; }
		UINT			getRealX() const			{ return m_iRealX; }

		void			setRealY(UINT uiValue)		{ m_iRealY = (INT)uiValue; }
		UINT			getRealY() const			{ return m_iRealY; }

		void			setRealWidth(UINT uiValue)	{ m_uiRealWidth = uiValue; }
		UINT			getRealWidth() const		{ return m_uiRealWidth; }

		void			setRealHeight(UINT uiValue)	{ m_uiRealHeight = uiValue; }
		UINT			getRealHeight()	const		{ return m_uiRealHeight; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	To show this Popup.
		*
		* @return	TRUE: Success to show this Popup. 
		* 			FALSE: Fail to show this Popup and this Popup has been destroyed.
		*/
		BOOLEAN			doModal();		

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		virtual	void			refresh();
		virtual	void			destroy();
		virtual BOOLEAN			doFocusBack();

		virtual	ICursorTracker*	probe(INT iX, INT iY);	

		virtual	void			buildRenderTree(CRenderGroup &renderGroup);

		// implement interfaces
		virtual	void	onTimer(const CTimer *pTimer, UINT uiCurrentCount);
		virtual	void	onTimerComplete(const CTimer *pTimer);	

	protected:// method
		/**
		* @brief	The constructor of the class.
		*
		* @param	bModal			TRUE modal Popup. FALSE modeless Popup.
		* @param	bAutoDestroy	If the Popup is modeless and this property is set to TRUE, 
		* 							the Popup will be destroyed when click outside the Popup;
		* 							otherwise you must destroy the popup by manual.
		*/
		CBasePopup(BOOLEAN bModal = TRUE, BOOLEAN bAutoDestroy = TRUE);

		virtual	void	buildRenderTreeFromBackground(CRenderGroup &renderGroup);

		/**
		* @brief	Animation system for Popup comes shown.
		*			The derived class should override this method if you want to have an animation while the Popup come shown.
		*			
		* @param	iIntervalTime	The unit is millisecond
		* @param	iRepeatCount	The iIntervalTime * iRepeatCount is the total animation time.
		*/
		virtual void	startShowAnimation(UINT uiIntervalTime, UINT uiRepeatCount);

		/**
		* This method will be called many times during the animation. 
		* The derived class should override this method and change the properties of Popup to make an animation if you want to have an animation while the Popup comes shown.
		*/
		virtual void	doShowAnimation(UINT uiCurrentCount);

		/**
		* When the animation complete, should set the correct properties to the Popup.
		* The derived class should override this method if you want to have an animation while the Popup comes shown.
		*/
		virtual void	completeShowAnimaiton();

		/**
		* @brief	Animation system for Popup come hidden.
		*			The derived class should override this method if you want to have an animation while the Popup come hidden.
		*			
		* @param	iIntervalTime	The unit is millisecond
		* @param	iRepeatCount	The iIntervalTime * iRepeatCount is the total animation time.
		*/
		virtual void	startHideAnimation(UINT uiIntervalTime, UINT uiRepeatCount);

		/**
		* This method will be called many times during the animation. 
		* The derived class should override this method and change the properties of Popup to make an animation if you want to have an animation while the Popup comes hidden.
		*/
		virtual void	doHideAnimation(UINT uiCurrentCount);

		/**
		* When the animation complete, should set the correct properties to the Popup.
		* The derived class should override this method if you want to have an animation while the Popup comes hidden.
		*/
		virtual void	completeHideAnimaiton();
		
	private:// method

	protected:// property

	private:// property
		BOOLEAN		m_bModal;
		BOOLEAN		m_bAutoDestroy;
		INT			m_iRealX;
		INT			m_iRealY;
		UINT		m_uiRealWidth;
		UINT		m_uiRealHeight;
		INT			m_iOriginalX;
		INT			m_iOriginalY;

		CTimer		m_timerAnimation;
		BOOLEAN		m_bIsShowAnimationRunning;
	};
}