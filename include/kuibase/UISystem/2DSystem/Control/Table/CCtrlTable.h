/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventTable
	{
	public:
		static const UINT SELECT_CHANGED				= 0;
		static const UINT CCP_FOCUS_CHANGED				= 1;
		static const UINT ITEM_DRAGGING					= 2;
		static const UINT ITEM_DROPPED					= 3;
		static const UINT ITEM_SCROLLING				= 4;
		static const UINT ITEM_SCROLL_STOP				= 5;
		static const UINT WANT_NEXT_PAGE				= 6;
		static const UINT ITEM_HOVER_CANCELED			= 7;
		static const UINT ITEM_HOVER_FOCUSED			= 8;
		static const UINT HOVER_FOCUSED					= 9;
		static const UINT HOVER_CANCELED				= 10;
		static const UINT WHEEL_START					= 11;
		static const UINT WHEEL_END						= 12;
	};

	class OPENHMI_API CCtrlTable
		: public CComponent
		, public IEventListener
		, public IEventListenerChain
	{
	public:// const define

	private:// const define
		static const INT SHOW_ITEM_OFFSET	 = 1;

		static const INT DISTANCE_OFFSET	 = 0;

	public:// embed class or struct or enum
		enum ETouchAnimation
		{
			TOUCH_SHAKE = 0,
		};
		
	private:// embed class or struct or enum
		struct tagShowItem
		{
			ObjectPtr<Object>		pObject;
			ObjectPtr<CComponent>	pComponent;
			CRectangle				rcCell;

			tagShowItem(Object* pObj, CComponent* pCom) 
				: pObject(pObj)
				, pComponent(pCom)
			{
				;
			}
		};

		struct tagShowComponent
		{
			ObjectPtr<CComponent>	pComponent;
			INT						iShowX;
			INT						iShowY;

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

		class tagAmtData_FlingExt : public Object
		{
		public:
			INT						iStartPos;
			INT						iDeltaPos;
		};

		class tagAmtData_Touch : public Object
		{
		public:
			ObjectPtr<CComponent>	pComponent;
		};

	public:// method
		CCtrlTable();
		virtual ~CCtrlTable();

		friend class CBaseTableAmt;
		friend class CTableAmtEnterInterdigitation;
		friend class CTableAmtElasticity;
		friend class CTableAmtFling;
		friend class CTableAmtTouchShake;

		DECLARE_EVENT_MAP

		CModelTable*					getModel();
		void							setView(IViewTable* pView);

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
		void							setAssociateScrollbar(CCtrlScrollbar* pScrollBar, BOOLEAN bControlScrollbarVisible = FALSE, BOOLEAN bMovePageWhenClickUpDownArrow = TRUE);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		UINT							getCellWidth()	 const						{ return m_uiCellWidth; }
		void							setCellWidth(UINT uiValue)					{ m_uiCellWidth = uiValue < 10 ? 10 : uiValue; }

		UINT							getCellHeight()	const						{ return m_uiCellHeight; }
		void							setCellHeight(UINT uiValue)					{ m_uiCellHeight = uiValue < 10 ? 10 : uiValue; }

		/**
		* After changing preferred size or cell size, should call this method..
		*
		*/
		void							resetTableLayout();


		/**
		* The item count for each page.
		* Here the page does not mean the page to show, but means a virtual page buffer. 
		* When table scrolling or page up or page down, if the buffered item count is less than this value, a WANT_NEXT_PAGE event will be dispatched.
		*/
		void							setItemsPerPage(UINT uiVaue)				{ m_uiItemsPerPage = uiVaue; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set current selected item by index.
		*/
		void							setCurSel(INT iValue);
		
		/**
		* Get the index of current selected item.
		*/
		INT								getCurSel()									{ return m_iCurSel; }

		/**
		* Set current selected item by content.
		*/		
		void							setSelectedItem(Object* pObj)				{ m_iCurSel = m_model.getItemIndex(pObj); }

		/**
		* Get the content of current selected item.
		*/
		Object*							getSelectedItem()
		{
			if (m_iCurSel == -1)
			{
				return NULL;
			}
			else
			{
				return m_aryShowItem[m_iCurSel]->pComponent;
			}
		}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		/**
		* Get the index of current hover focus item.
		*/
		INT								getCurHoverFocus()							{ return m_iCurHoverFocus; }

		/**
		* Set current hover focus item by content.
		*/	
		void							setHoverFocusedItem(Object *pObj)			{ m_iCurHoverFocus = m_model.getItemIndex(pObj); }

		/**
		* Get the content of current hover focus item.
		*/
		Object*							getHoverFocusedItem();

		/**
		* Set current hover focus item by index.
		*/
		void							setCurHoverFocus(INT iValue);

		/**
		* Set current touch focus item by index.
		*/
		void							setCurTouchFocus(INT iValue);

		/**
		* Get the index of current touch focus item.
		*/
		INT								getCurTouchFocus()							{ return m_iCurTouchFocus; }
		INT								getCurTouchFocusR()							{ return m_iCurTouchFocusR; }

		/**
		* Set current touch focus item by content.
		*/	
		void							setTouchFocusedItem(Object *pObj)			{ m_iCurTouchFocus = m_model.getItemIndex(pObj); }

		/**
		* Get the content of current touch focus item.
		*/
		Object*							getTouchFocusedItem();
		Object*							getTouchFocusedItemR();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set current CCP focus item by index.
		*/
		void							setCurCCPFocus(INT iValue);

		/**
		* Get the index of current CCP focus item.
		*/
		INT								getCurCCPFocus();

		/**
		* Set current CCP focus item by content.
		*/	
		void							setCCPFocusedItem(Object* pObj);

		/**
		* Get the content of current CCP focus item.
		*/
		Object*							getCCPFocusedItem();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Get dragging cursor while dragging the table item. It is a global point.
		*/
		CPoint							getDraggingCursor()							{ return m_ptCurrentCursor; }

		/**
		* Get the content of current dragged item.
		*/
		Object*							getDraggedItem();

		/**
		* @brief	Locate the item index using the specified point.
		*			This is usually used in the situation of dragging table item and insert it into the new position.
		*			You can get the new position by this interface.
		*
		* @return	The index that located by the point specified. If it is less than 0, can't insert to this table.
		*/
		INT								getItemInsertToPos(CPoint *pt);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Get the component add into table by its content.
		*/
		CComponent*						getItemComponent(Object* pObj);

		/**
		* Get the top item index.
		*/
		INT								getCurrentTopItem()							{ return m_iShowTopIndex; }

		/**
		* Show the item specified by index as top item.
		*/
		void							showTopItem(UINT uiIndex);
		
		CRectangle						getScrollRectangle() const					{ return m_scrollRect; }

		/**
		* This is a lightweight refresh for appending items. It is usually used in the situation of response function for WANT_NEXT_PAGE event.
		*/
		void							refreshForAppendItem();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Back up view state.
		* The state includes selected item, ccp focus item,and top index.
		* It must be used with interface restoreViewState
		*/
		void							backupViewState();

		/**
		* Restore view state.
		* The state includes selected item, ccp focus item,and top index.
		* It must be used with interface backupViewState
		*/
		void							restoreViewState();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Enable or disable ccp focus change by outer control moving UP/DOWN key. 
		*/
		void							setCanMoveFocusByUpDown(BOOLEAN bValue)		{ m_bCanMoveFocusByUpDown = bValue; }	

		/**
		* Enable or disable ccp focus change by outer control moving LEFT/RIGHT key. 
		*/
		void							setCanMoveFocusByLeftRight(BOOLEAN bValue)	{ m_bCanMoveFocusByLeftRight = bValue; }	

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Turn on/off the enter animation switch
		*/
		void							setIsEnterAnimationOn(BOOLEAN bValue)		{ m_bIsEnterAnimationOn = bValue; }

		/**
		* Play enter animation manually
		*/
		void							playEnterAnimation();

		/**
		* Turn on/off the elasticity animation switch
		*/
		void							setIsElasticityOn(BOOLEAN bValue)			{ m_bIsElasticityOn = bValue; }

		/**
		* Turn on/off the fling animation switch
		*/
		void							setIsFlingOn(BOOLEAN bValue)				{ m_bIsFlingOn = bValue; }

		
		/**
		* Turn on/off the touch animation switch
		*/
		void							setIsTouchAnimationOn(BOOLEAN bValue)		{ m_bIsTouchAnimationOn = bValue; }
		
		/**
		* Set the touch animation type
		*/
		void							setTouchAnimationType(ETouchAnimation eValue);

		void							setCanWheel(BOOLEAN bValue) { m_bCanWheel = bValue; }
		
		void							destroyShowItemList();

		void							getRowAndClomn(UINT& row, UINT& clomn) { row = m_uiRow; clomn = m_uiColumn; }
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

		virtual void	onMove(INT iX, INT iY);
		virtual void	onLostHover();
		virtual	void	onWheel(CCursor::EWheelType eType);
		virtual	void	onTouchPress(INT iX, INT iY);
		virtual	void	onTouchPressRelease(INT iX, INT iY);
		virtual	void	onTouchPressR(INT iX, INT iY);
		virtual	void	onTouchPressReleaseR(INT iX, INT iY);
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
		CCtrlTable(const CCtrlTable&){}
		CCtrlTable& operator =(const CCtrlTable&){return *this;}

	private:// method
		void			buildShowItemList();
		void			appendShowItem(UINT uiIndex);
		void			resetShowItemPos();		
		void			showItems(INT iTop);
		void			showItemState();
		
		void			resetScrollbar(BOOLEAN bMaintainPos = FALSE);

		UINT			getTotalShowItemHeight();
		BOOLEAN			needVerifyShowInfo(INT iPos);
		void			verifyShowInfo();
		UINT			calculateShowItemIndex(INT iPos);

		void			checkSelectChanged();
		BOOLEAN			checkCanRollToTop();

		INT				getFirstCCPItemIndex();
		INT				getLastCCPItemIndex();
		void			adjustCCPFocusDisplayPos();
		void			drawTableCCPFocs();

		CComponent*		getCCPFocusComponent();

		BOOLEAN			moveCCPFocusIndexDecrease(ECCPOperation eCCPOperation,UINT uiStep=0);
		BOOLEAN			moveCCPFocusIndexIncrease(ECCPOperation eCCPOperation,UINT uiStep=0);
		void			switchCCPFocusForTouch();

		void			startEnterAnimation();
		void			stopEnterAnimation();

		void			startElasticityAnimation(INT iStartValue, INT iEndValue);
		void			stopElasticityAnimation();

		void			startFlingAnimation(INT iStartValue, INT iSpeed);
		void			startFlingAnimationExt(INT iStartPos, INT iDeltaPos);
		void			stopFlingAnimation();

		void			startTouchAnimation(CComponent* pCom);
		void			stopTouchAnimation();

		void			onScrollbarPosChanged(CComponent *pTarget);
		void			onScrollbarPosContinueChangeEnded(CComponent *pTarget);

	protected:// property

	private:// property
		CModelTable							m_model	;
		CView								m_view;
		IViewTable*							m_pView;

		UINT								m_uiCellWidth;
		UINT								m_uiCellHeight;
		UINT								m_uiRow;
		UINT								m_uiColumn;
		
		Array<tagShowItem>					m_aryShowItem;
		Array<tagShowComponent>				m_aryShowComponent;		
		
		CCtrlScrollbar*						m_pScrollbar;
		BOOLEAN								m_bControlScrollbarVisible;		
		
		INT									m_iCurSel;
		INT									m_iCurTouchFocus;
		INT									m_iCurCCPFocus;
		INT									m_iCurHoverFocus;
		INT									m_iCurTouchFocusR;

		INT									m_iShowTopIndex;
		INT									m_iShowItemIndex;
		UINT								m_uiShowItemCount;
		CRectangle							m_scrollRect;
		UINT								m_uiItemsPerPage;
		BOOLEAN								m_bInAppendMode;
		
		INT									m_iTouchBeginPosY;
		INT									m_iShowBeginPosY;
		INT									m_iShowCurrentPosY;
		BOOLEAN								m_bDirectionDown;
		
		tagViewState						m_viewState;
		BOOLEAN								m_bOuterChangedData;
		
		BOOLEAN								m_bCanMoveFocusByUpDown;
		BOOLEAN								m_bCanMoveFocusByLeftRight;

		BOOLEAN								m_bIsEnterAnimationOn;
		BOOLEAN								m_bIsElasticityOn;
		BOOLEAN								m_bIsFlingOn;
		BOOLEAN								m_bIsTouchAnimationOn;

		BOOLEAN								m_bCanWheel;
		ObjectPtr<CBaseTableAmt>			m_pAmtEnter;
		ObjectPtr<CBaseTableAmt>			m_pAmtElasticity;
		ObjectPtr<CBaseTableAmt>			m_pAmtFling;
		ObjectPtr<CBaseTableAmt>			m_pAmtTouch;		

		CPoint								m_ptCurrentCursor;	
	};
}