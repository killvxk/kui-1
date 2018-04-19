/**
* @attention Copyright
* Copyright 2013-2014 - All Rights Reserved 
* HARMAN Shanghai - HMI Team of Software
* Email: Author Zeki.Yan
*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventList
	{
	public:
		static const UINT SELECT_CHANGED				= 0;
		static const UINT CCP_FOCUS_CHANGED				= 1;
		static const UINT ITEM_DRAGGING					= 2;
		static const UINT ITEM_DROPPED					= 3;
		static const UINT ITEM_SCROLLING				= 4;
		static const UINT ITEM_SCROLL_STOP				= 5;
		static const UINT WANT_PREV_PAGE				= 6;
		static const UINT WANT_NEXT_PAGE				= 7;
		static const UINT ITEM_HOVER_CANCELED			= 8;
		static const UINT ITEM_HOVER_FOCUSED			= 9;
		static const UINT HOVER_FOCUSED					= 10;
		static const UINT HOVER_CANCELED				= 11;
	};

	/**
	* @class	CCtrlList	
	*			It is a powerful list control.
	*			It can be controlled by associated scrollbar or index keyboard (not supported yet). 
	*			It can show note bubble over it or other place.
	*			User can use different component for different list item and the items height can be different.
	*			Maybe there is interval distance between two items.
	*			The list item component can be complex enough, so CCP focus can go in to this component from list control, can back to list control.
	*			When user rotates CCP, the CCP focus can be fixed and scroll list item, or move CCP focus with list item scrolling.
	*/
	class OPENHMI_API CCtrlList
		: public CComponent
		, public IEventListener
		, public IEventListenerChain
	{
	public:// const define

	private:// const define
		static const INT DISTANCE_OFFSET	 = 10;

	public:// embed class or struct or enum
		enum EScrollStyle
		{
			LIST_ITEM_SCROLL = 0,
			CCP_FOCUS_SCROLL,
		};

		enum ETouchAnimation
		{
			TOUCH_SHAKE = 0,
		};

	private:// embed class or struct or enum
		struct tagShowItem
		{
			ObjectPtr<Object>		pObject;
			ObjectPtr<CComponent>	pComponent;

			tagShowItem(Object* pObj, CComponent* pCom) 
				: pObject(pObj)
				, pComponent(pCom)
			{
				;
			}
		};

		struct tagShowComponent
		{
			CComponent		*pComponent;
			INT				iShowX;
			INT				iShowY;

			tagShowComponent(CComponent* pCom) 
				: pComponent(pCom)
			{
				if (pCom != NULL)
				{
					iShowX = pCom->getX();
					iShowY = pCom->getY();
				}
			}
		};

		struct tagViewState
		{
			ObjectPtr<Object>		pObjSelectedItem;
			ObjectPtr<Object>		pObjCCPFocusItem;
			INT						iShowTopIndex;

			tagViewState() 
			{
				;
			}
		};

		class tagAmtData_Enter : public Object
		{
	
		};

		class tagAmtData_Elasiticity : public Object
		{
		public:
			INT						iStartValue;
			INT						iEndValue;
		};

		class tagAmtData_Fling : public Object
		{
		public:
			INT						iStartValue;
			INT						iSpeed;
		};

		class tagAmtData_Touch : public Object
		{
		public:
			ObjectPtr<CComponent>	pComponent;
		};

	public:// method
		CCtrlList();
		virtual ~CCtrlList();

		friend class CBaseListAmt;
		friend class CListAmtEnterInterdigitation;
		friend class CListAmtElasticity;
		friend class CListAmtFling;
		friend class CListAmtTouchShake;

		DECLARE_EVENT_MAP

		CModelList*		getModel();
		void			setView(IViewList* pView);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* @brief	Associate scroll bar with this control.
		*			Usually scrollbar is always shown if the control can be scrolled, and it is hidden if the control can't be scrolled.
		*
		* @param	bControlScrollbarVisible		TRUE, the scroll bar is only shown while this control is being scrolled.
		*											FALSE, the scroll bar is always shown.
		* @param	bMovePageWhenClickUpDownArrow	If it is TRUE, when clicking the up or down arrow of the scroll bar, it will turn over the total height of the control.
		*											If it is FALSE, when clicking the up or down arrow of the scroll bar, it will turn over one item height of the control.
		*/
		void			setAssociateScrollbar(CCtrlScrollbar* pScrollBar, BOOLEAN bControlScrollbarVisible = FALSE, BOOLEAN bMovePageWhenClickUpDownArrow = TRUE);

		/**
		* Associate bubble with list.
		* Usually bubble is shown when list scrolling and hide when list stay static.
		* You can make the bubble alway show if the bControlBubbleVisible is FALSE.
		*/
		void			setAssociateBubble(CComponent* pBubble, BOOLEAN bControlBubbleVisible = TRUE);

//		void			setAssociateKeyboard(CCtrlIndexKeyboard* pIndexKeyboard);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set current selected item by index.
		*/
		void			setCurSel(INT iValue);
		
		/**
		* Get the index of current selected item.
		*/
		INT				getCurSel()									{ return m_iCurSel; }

		/**
		* Set current selected item by content.
		*/	
		void			setSelectedItem(Object* pObj)				{ m_iCurSel = m_model.getItemIndex(pObj); }
		
		/**
		* Get the content of current selected item.
		*/
		Object*			getSelectedItem()							{ return m_iCurSel == -1 ? NULL : m_model.getItemAt(m_iCurSel); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set current touch focus item by index.
		*/
		void			setCurTouchFocus(INT iValue);

		/**
		* Get the index of current touch focus item.
		*/
		INT				getCurTouchFocus()							{ return m_iCurTouchFocus; }

		/**
		* Set current touch focus item by content.
		*/	
		void			setTouchFocusedItem(Object *pObj)			{ m_iCurTouchFocus = m_model.getItemIndex(pObj); }

		/**
		* Get the content of current touch focus item.
		*/
		Object*			getTouchFocusedItem()						{ return m_iCurTouchFocus == -1 ? NULL : m_model.getItemAt(m_iCurTouchFocus); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set current CCP focus item by index.
		*/
		void			setCurCCPFocus(INT iValue);

		/**
		* Get the index of current CCP focus item.
		*/
		INT				getCurCCPFocus();

		/**
		* Set current CCP focus item by content.
		*/	
		void			setCCPFocusedItem(Object* pObj);

		/**
		* Get the content of current CCP focus item.
		*/
		Object*			getCCPFocusedItem();

		/**
		* @brief	Set a position where the CCP focus on
		*			This is used for CCP.
		*			When the list's scroll type is LIST_ITEM_SCROLL, at preset, we bind bubble position with this CCP focus fix position.
		*			It means, the CCP focus fix position must be set to the list's middle item index.
		*
		* @param	uiValue		range: [0, the total number of per page - 1]
		*/
		void			setCCPFocusFixPos(UINT uiValue)			{ m_uiCCPFocusFixPos = uiValue; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Get dragging cursor while dragging the list item. It is a global point.
		*/
		CPoint			getDraggingCursor()						{ return m_ptCurrentCursor; }

		/**
		* Get the content of current dragged item.
		*/
		Object*			getDraggedItem();

		/**
		* @brief	Locate the item index using the specified point.
		*			This is usually used in the situation of dragging list item and insert it into the new position.
		*			You can get the new position by this interface.
		*
		* @return	The index that located by the point specified. If it is less than 0, can't insert to this list.
		*/
		INT				getItemInsertToPos(CPoint pt);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set the distance between two list items.
		*
		*/
		void			setItemSpace(INT iValue)					{ m_iItemSpace = iValue; }
		INT				getItemSpace()								{ return m_iItemSpace; }

		/**
		* The item count for each page.
		* Here the page does not mean the page to show, but means a virtual page buffer. 
		* When list is going to show previous or next page, a WANT_PREV_PAGE or WANT_NEXT_PAGE event will be dispatched.
		*/
		void			setItemsPerPage(UINT uiVaue)				{ m_uiItemsPerPage = uiVaue; }


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Get the component add into list by its content.
		*/
		CComponent*		getItemComponent(Object* pObj);

		/**
		* Get the top item index.
		*/
		INT				getCurrentTopItem()						{ return m_iShowTopIndex; }

		/**
		* Show the item specified by index as top item.
		*/
		void			showTopItem(UINT uiIndex);		

		/**
		* @brief	This is a lightweight refresh for appending previous page items. It is usually used in the situation of response function for WANT_PREV_PAGE event.
		*
		* @param	uiItemCount The number of inserted item to head.
		*/
		void			refreshForAppendPrevPageItem(INT iItemCount);

		/**
		* This is a lightweight refresh for appending next page items. It is usually used in the situation of response function for WANT_NEXT_PAGE event.
		*/
		void			refreshForAppendNextPageItem();

		CRectangle		getScrollRectangle() const				{ return m_scrollRect; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Back up view state.
		* The state includes selected item, ccp focus item,and top index.
		* It must be used with interface restoreViewState
		*/
		void			backupViewState();

		/**
		* Restore view state.
		* The state includes selected item, ccp focus item,and top index.
		* It must be used with interface backupViewState
		*/
		void			restoreViewState();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set whether list control can scroll by touch move or not.
		*/
		void			setCanScrollByTouchMove(BOOLEAN	bValue)		{ m_bCanScrollByTouchMove = bValue; }
		BOOLEAN			getCanScrollByTouchMove() const				{ return m_bCanScrollByTouchMove; }

		/**
		* @brief	Set scroll type to list, the scroll type decides the list's scroll behavior.
		*
		* @param	eValue
		*				LIST_ITEM_SCROLL	Items scroll with CCP fixed in fixed position.
		*				CCP_FOCUS_SCROLL	CCPp scrolls.
		*/
		void			setScrollType(EScrollStyle eValue)			{ m_eScrollType = eValue; }

		/**
		* Enable or disable ccp focus change by outer control moving UP/DOWN key. 
		*/
		void			setCanMoveFocusByUpDown(BOOLEAN bValue)		{ m_bCanMoveFocusByUpDown = bValue; }	

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Turn on/off the enter animation switch
		*/
		void			setIsEnterAnimationOn(BOOLEAN bValue)		{ m_bIsEnterAnimationOn = bValue; }

		/**
		* Play enter animation manually
		*/
		void			playEnterAnimation();

		/**
		* Turn on/off the elasticity animation switch
		*/
		void			setIsElasticityOn(BOOLEAN bValue)			{ m_bIsElasticityOn = bValue; }

		/**
		* Turn on/off the fling animation switch
		*/
		void			setIsFlingOn(BOOLEAN bValue)				{ m_bIsFlingOn = bValue; }

		/**
		* Turn on/off the touch animation switch
		*/
		void			setIsTouchAnimationOn(BOOLEAN bValue)		{ m_bIsTouchAnimationOn = bValue; }

		/**
		* Set the touch animation type
		*/
		void			setTouchAnimationType(ETouchAnimation eValue);
		
		virtual	void	lock();
		virtual	void	unlock();
		virtual	void	refresh();
		virtual	void	littleRefresh();
		virtual	void	destroySkin();
		virtual	void	destroy();
		virtual void	buildRenderTree(CRenderGroup &renderGroup);

		virtual BOOLEAN	canSetFocus();	
		virtual	void	setFocus(ECCPOperation eCCPOperation = CCP_ROTATE_RIGHT);
		virtual	void	releaseFocus();
		virtual	BOOLEAN	hasFocus();

		virtual BOOLEAN	moveFocusToLeft();
		virtual BOOLEAN	moveFocusToRight();
		virtual BOOLEAN	moveFocusToUp();
		virtual BOOLEAN	moveFocusToDown();
		virtual BOOLEAN	turnFocusToLeft(UINT uiStep);
		virtual BOOLEAN	turnFocusToRight(UINT uiStep);
		virtual	void	doFocusEnterPress();
		virtual	void	doFocusEnterRelease();
		virtual BOOLEAN	doFocusBack();

		virtual	void	onTouchPress(INT iX, INT iY);
		virtual	void	onTouchPressRelease(INT iX, INT iY);
		virtual	void	onTouchLongPress(INT iX, INT iY);
		virtual	void	onTouchContinuePress(INT iX, INT iY);
		virtual	void	onTouchLongPressRelease(INT iX, INT iY);
		virtual	void	onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY);
		virtual	void	onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY);
		virtual	void	onTouchDrag(INT iX, INT iY);
		virtual	void	onTouchDragRelease(INT iX, INT iY);

		// implement IEventListenerChain interface
		virtual BOOLEAN	contains(Object *pTarget);

	protected:// method
		CCtrlList(const CCtrlList&){}
		CCtrlList& operator =(const CCtrlList&){return *this;}

	private:// method
		void			updateViewForFirstBuild();
		void			buildShowItemList();
		void			appendShowItemForPrevPage(INT iItemCount);
		void			appendShowItemForNextPage();
		INT				insertShowItem(INT iIndex, Object *pObj, INT iPosY);
		void			destroyShowItemList();
		void			checkNeedMorePage();
		void			showItems(INT iTop);
		void			showItemState();

		void			showBubble();
		void			hideBubble();
		void			resetScrollbar(BOOLEAN bMaintainPos = FALSE);

		UINT			getTotalShowItemHeight();
		BOOLEAN			needVerifyShowInfo(INT iPos);
		void			verifyShowInfo();
		UINT			calculateShowItemIndex(INT iPos);

		void			checkSelectChanged();

		INT				getFirstCCPItemIndex();
		INT				getLastCCPItemIndex();
		void			adjustCCPFocusDisplayPos();
		void			drawListCCPFocs();

		CComponent*		getCCPFocusComponent();

		BOOLEAN			moveCCPFocusIndexDecrease(ECCPOperation eCCPOperation,UINT uiStep = 0);
		BOOLEAN			moveCCPFocusIndexIncrease(ECCPOperation eCCPOperation,UINT uiStep = 0);
		void			switchCCPFocusForTouch();

		void			startEnterAnimation();
		void			stopEnterAnimation();

		void			startElasticityAnimation(INT iStartValue, INT iEndValue);
		void			stopElasticityAnimation();

		void			startFlingAnimation(INT iStartValue, INT iSpeed);
		void			stopFlingAnimation();

		void			startTouchAnimation(CComponent* pCom);
		void			stopTouchAnimation();

		void			onScrollbarPosChanged(CComponent *pTarget);
		void			onScrollbarPosContinueChangeEnded(CComponent *pTarget);

	protected:// property

	private:// property
		CModelList						m_model				;
		CView							m_view				;
		IViewList*						m_pView				;

		CCtrlScrollbar*					m_pScrollbar		;
		BOOLEAN							m_bControlScrollbarVisible;		
		CComponent*						m_pBubble			;		
		BOOLEAN							m_bControlBubbleVisible;
		BOOLEAN							m_bShowBubble		;
		//CCtrlIndexKeyboard*			m_pKeyboard			;

		Array<tagShowItem>				m_aryShowItem		;
		Array<tagShowComponent>			m_aryShowComponent	;
		BOOLEAN							m_bFirstBuild		;		
		
		INT								m_iCurSel			;
		INT								m_iCurTouchFocus	;
		INT								m_iCurBubble		;
		INT								m_iCurCCPFocus		;

		INT								m_iShowTopIndex		;
		INT								m_iShowItemIndex	;
		UINT							m_uiShowItemCount	;
		CRectangle						m_scrollRect		;

		INT								m_iLeftSpace		;
		INT								m_iItemSpace		;
		UINT							m_uiItemsPerPage	;
		BOOLEAN							m_bInAppendPrevPageMode		;
		BOOLEAN							m_bInAppendNextPageMode		;		
		
		INT								m_iTouchBeginPosY	;
		INT								m_iShowBeginPosY	;
		INT								m_iShowCurrentPosY	;
		BOOLEAN							m_bDirectionDown	;
		BOOLEAN							m_bCanScrollByTouchMove;
		
		tagViewState					m_viewState		;
		BOOLEAN							m_bOuterChangedData	;
		
		EScrollStyle					m_eScrollType		;
		UINT							m_uiCCPFocusFixPos	;
		
		BOOLEAN							m_bCanMoveFocusByUpDown;

		BOOLEAN							m_bIsEnterAnimationOn			;
		BOOLEAN							m_bIsElasticityOn				;
		BOOLEAN							m_bIsFlingOn					;
		BOOLEAN							m_bIsTouchAnimationOn			;
		ObjectPtr<CBaseListAmt>			m_pAmtEnter						;
		ObjectPtr<CBaseListAmt>			m_pAmtElasticity				;
		ObjectPtr<CBaseListAmt>			m_pAmtFling						;
		ObjectPtr<CBaseListAmt>			m_pAmtTouch						;		

		CPoint							m_ptCurrentCursor				;
	};
}