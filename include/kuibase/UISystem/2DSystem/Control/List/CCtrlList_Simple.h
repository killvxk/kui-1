/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	/**
	* @class	CCtrlList_Simple	
	*			It is a simple list control.
	*			It can only be controlled by associated scrollbar. 
	*			It doesn't support to show a note bubble.
	*			All of the list items use same component, and the item height is same,and there is no interval distance between two items.
	*			The height of the list control must be multiple the item height.
	*			The list item component is so simple, so CCP focus can't go in to this component from list control.
	*			When user rotates CCP, the CCP focus moves with list item scrolling.
	*
	*			Actually, this simple list control is used for data list, and there are so many items, such as song list.
	*/
	class OPENHMI_API CCtrlList_Simple
		: public CComponent
		, public IEventListener
		, public IEventListenerChain
	{
	public:// const define

	private:// const define
		static const INT DISTANCE_OFFSET	 = 0;

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		struct tagViewState
		{
			Object		*pObjSelectedItem;
			Object		*pObjCCPFocusItem;
			INT			iShowTopIndex;

			tagViewState() 
			{
				;
			}
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

		class tagAmtData_FlingExt : public Object
		{
		public:
			INT						iStartPos;
			INT						iDeltaPos;
		};

	public:// method
		CCtrlList_Simple();
		virtual ~CCtrlList_Simple();

		friend class CBaseListAmt;
		friend class CListAmtElasticity;
		friend class CListAmtFling;

		DECLARE_EVENT_MAP

		void			setModel(Array<Object> *pModel);
		void			setView(IViewList_Simple* pView);

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set current selected item by index.
		*/
		void			setCurSel(INT iValue);
		
		/**
		* Get the index of current selected item.
		*/
		INT				getCurSel()								{ return m_iCurSel; }

		/**
		* Set current selected item by content.
		*/	
		void			setSelectedItem(Object* pObj)			{ m_iCurSel = (m_pModel == NULL ? -1 : m_pModel->getItemIndex(pObj)); }
		
		/**
		* Get the content of current selected item.
		*/
		Object*			getSelectedItem()
		{
			if (m_iCurSel == -1)
			{
				return NULL;
			}
			else
			{
				return m_aryComponent[m_iCurSel];
			}
		}

		/**
		* Get the index of current hover focus item.
		*/
		INT								getCurHoverFocus()							{ return m_iCurHoverFocus; }

		/**
		* Get the content of current hover focus item.
		*/
		Object*							getHoverFocusedItem()
		{
			if (m_iCurHoverFocus == -1)
			{
				return NULL;
			}
			else
			{
				return m_aryComponent[m_iCurHoverFocus];
			}
		}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set current touch focus item by index.
		*/
		void			setCurTouchFocus(INT iValue);

		/**
		* Get the index of current touch focus item.
		*/
		INT				getCurTouchFocus()						{ return m_iCurTouchFocus; }

		/**
		* Set current touch focus item by content.
		*/	
		void			setTouchFocusedItem(Object *pObj)		{ m_iCurTouchFocus = (m_pModel == NULL ? -1 : m_pModel->getItemIndex(pObj)); }

		/**
		* Get the content of current touch focus item.
		*/
		Object*			getTouchFocusedItem()
		{
			if (m_iCurTouchFocus == -1)
			{
				return NULL;
			}
			else
			{
				return m_aryComponent[m_iCurTouchFocus];
			}
		}

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
		void			setCCPFocusedItem(Object* pObj)			{ m_iCurCCPFocus = (m_pModel == NULL ? -1 : m_pModel->getItemIndex(pObj)); }

		/**
		* Get the content of current CCP focus item.
		*/
		Object*			getCCPFocusedItem();

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
		* The item count for each page.
		* Here the page does not mean the page to show, but means a virtual page buffer. 
		* When list is going to show previous or next page, a WANT_PREV_PAGE or WANT_NEXT_PAGE event will be dispatched.
		*/
		void			setItemsPerPage(UINT uiVaue)			{ m_uiItemsPerPage = uiVaue; }

		/**
		* A flag indicates the first item of list is the first of your data or not.
		* If it is FALSE, when list control is going to show previous page items which don't exist in the model list, it will dispatch WANT_PREV_PAGE event.
		* The default value is TRUE.
		*/
		void			setReachToHead(BOOLEAN bValue)			{ m_bReachToHead = bValue; }
		BOOLEAN			getReachToHead() const					{ return m_bReachToHead; }

		/**
		* A flag indicates the last item of list is the last of your data or not.
		* If it is FALSE, when list control is going to show next page items which don't exist in the model list, it will dispatch WANT_NEXT_PAGE event.
		* The default value is TRUE.
		*/
		void			setReachToTail(BOOLEAN bValue)			{ m_bReachToTail = bValue; }
		BOOLEAN			getReachToTail() const					{ return m_bReachToTail; }

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
		* Enable or disable ccp focus change by outer control moving UP/DOWN key. 
		*/
		void			setCanMoveFocusByUpDown(BOOLEAN bValue)		{ m_bCanMoveFocusByUpDown = bValue; }	

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Turn on/off the elasticity animation switch
		*/
		void			setIsElasticityOn(BOOLEAN bValue)			{ m_bIsElasticityOn = bValue; }

		/**
		* Turn on/off the fling animation switch
		*/
		void			setIsFlingOn(BOOLEAN bValue)				{ m_bIsFlingOn = bValue; }

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

		virtual BOOLEAN	moveFocusToLeft();
		virtual BOOLEAN	moveFocusToRight();
		virtual BOOLEAN	moveFocusToUp();
		virtual BOOLEAN	moveFocusToDown();
		virtual BOOLEAN	turnFocusToLeft(UINT uiStep);
		virtual BOOLEAN	turnFocusToRight(UINT uiStep);
		virtual	void	doFocusEnterPress();
		virtual	void	doFocusEnterRelease();
		virtual BOOLEAN	doFocusBack();

		virtual void	onMove(INT iX, INT iY);
		virtual void	onLostHover();
		virtual	void	onWheel(CCursor::EWheelType eType);
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
		CCtrlList_Simple(const CCtrlList_Simple&){}
		CCtrlList_Simple& operator =(const CCtrlList_Simple&){return *this;}

	private:// method		
		void			updateViewForFirstBuild();		
		void			checkNeedMorePage();
		void			showItems(INT iTop);
		void			showItemState();

		void			resetScrollbar(BOOLEAN bMaintainPos = FALSE);

		UINT			getTotalShowItemHeight();
		BOOLEAN			needVerifyShowInfo(INT iPos);
		void			verifyShowInfo();
		void			updateComponentVisibility();
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

		void			startElasticityAnimation(INT iStartValue, INT iEndValue);
		void			stopElasticityAnimation();

		void			startFlingAnimation(INT iStartValue, INT iSpeed);
		void			startFlingAnimationExt(INT iStartPos, INT iDeltaPos);
		void			stopFlingAnimation();

		void			onScrollbarPosChanged(CComponent *pTarget);
		void			onScrollbarPosContinueChangeEnded(CComponent *pTarget);

	protected:// property

	private:// property
		Array<Object>*					m_pModel			;
		CView							m_view				;
		IViewList_Simple*				m_pView				;
		
		CCtrlScrollbar*					m_pScrollbar		;
		BOOLEAN							m_bControlScrollbarVisible;	

		Array<CComponent>				m_aryComponent		;
		BOOLEAN							m_bFirstBuild		;
		UINT							m_uiItemHeight		;
		
		INT								m_iCurSel			;
		INT								m_iCurTouchFocus	;
		INT								m_iCurCCPFocus		;
		INT								m_iCurHoverFocus	;

		INT								m_iShowTopIndex		;
		INT								m_iShowItemIndex	;
		UINT							m_uiShowItemCount	;
		CRectangle						m_scrollRect		;

		UINT							m_uiItemsPerPage	;
		BOOLEAN							m_bReachToHead		;
		BOOLEAN							m_bReachToTail		;		
		BOOLEAN							m_bInAppendPrevPageMode		;
		BOOLEAN							m_bInAppendNextPageMode		;
		
		INT								m_iTouchBeginPosY	;
		INT								m_iShowBeginPosY	;
		INT								m_iShowCurrentPosY	;
		BOOLEAN							m_bDirectionDown	;
		BOOLEAN							m_bCanScrollByTouchMove;
		
		tagViewState					m_viewState		;
		BOOLEAN							m_bOuterChangedData	;
		
		BOOLEAN							m_bCanMoveFocusByUpDown;
		BOOLEAN							m_bIsElasticityOn	;
		BOOLEAN							m_bIsFlingOn		;
		ObjectPtr<CBaseListAmt>			m_pAmtElasticity	;
		ObjectPtr<CBaseListAmt>			m_pAmtFling			;

		CPoint							m_ptCurrentCursor	;
	};
}