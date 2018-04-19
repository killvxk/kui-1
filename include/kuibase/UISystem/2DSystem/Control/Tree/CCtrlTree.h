/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventTree
	{
	public:
		static const UINT SELECT_CHANGED				= 0;
		static const UINT CCP_FOCUS_CHANGED				= 1;
	};

	/**
	* @class	CCtrlTree
	*			It is a powerful tree control.
	*			It can be controlled by associated scrollbar. 
	*			User can use different component for different tree item and the items height can be different.
	*			The tree item component can be complex enough, so CCP focus can go in to this component from tree control, can back to tree control.
	*			When user rotates CCP, the CCP focus moves with tree item scrolling.
	*/
	class OPENHMI_API CCtrlTree
		: public CComponent
		, public IEventListener
		, public IEventListenerChain
	{
	public:// const define

	private:// const define
		static const INT DISTANCE_OFFSET	 = 10;

	public:// embed class or struct or enum

	private:// embed class or struct or enum
		struct tagShowItem
		{
			ObjectPtr<Object>		pObject;
			ObjectPtr<CComponent>	pComponent;
			UINT					uiLevel;

			BOOLEAN					bExpand;

			UINT					uiRawWidth;
			UINT					uiRawHeight;

			tagShowItem(Object* pObj, CComponent* pCom, UINT uiLev) 
				: pObject(pObj)
				, pComponent(pCom)
				, uiLevel(uiLev)
				, bExpand(TRUE)
			{
				uiRawWidth = pComponent->getPreferredSize().getX();
				uiRawHeight = pComponent->getPreferredSize().getY();
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

	public:// method
		CCtrlTree();
		virtual ~CCtrlTree();

		DECLARE_EVENT_MAP

		CModelTree*		getModel();
		void			setView(IViewTree* pView);

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
		void			setCCPFocusedItem(Object* pObj)				{ m_iCurCCPFocus = m_model.getItemIndex(pObj); }

		/**
		* Get the content of current CCP focus item.
		*/
		Object*			getCCPFocusedItem();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Set the distance between two list items.
		*
		*/
		void			setLeftSpace(UINT uiValue)					{ m_iLeftSpace = (INT)uiValue; }
		UINT			getLeftSpace()								{ return m_iLeftSpace; }

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
		* Show those items which level are not more than the level you pointed out.
		*/
		void			showItemToLevel(UINT uiLevel);

		CRectangle		getScrollRectangle() const				{ return m_scrollRect; }

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
		CCtrlTree(const CCtrlTree&){}
		CCtrlTree& operator =(const CCtrlTree&){return *this;}

	private:// method
		void			updateViewForFirstBuild();

		void			buildShowItemList();
		INT				insertShowItem(INT iIndex, CModelTree::tagItemNode *pItemNode, INT iPosY);
		void			destroyShowItemList();
		void			expandShowItem(INT iIndex);
		void			expandShowItemToLevel(UINT uiLevel);
		void			collapseShowItem(INT iIndex);
		void			resetShowItemPos();
		void			showItems(INT iTop);
		void			showItemState();

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

		void			onScrollbarPosChanged(CComponent *pTarget);
		void			onScrollbarPosContinueChangeEnded(CComponent *pTarget);

	protected:// property

	private:// property
		CModelTree						m_model				;
		CView							m_view				;
		IViewTree*						m_pView				;

		CCtrlScrollbar*					m_pScrollbar		;
		BOOLEAN							m_bControlScrollbarVisible;		

		Array<tagShowItem>				m_aryShowItem		;
		Array<tagShowComponent>			m_aryShowComponent	;
		BOOLEAN							m_bFirstBuild		;		
		
		INT								m_iCurSel			;
		INT								m_iCurTouchFocus	;
		INT								m_iCurCCPFocus		;

		INT								m_iShowTopIndex		;
		INT								m_iShowItemIndex	;
		UINT							m_uiShowItemCount	;
		CRectangle						m_scrollRect		;

		INT								m_iLeftSpace		;
		INT								m_iShowLevel		;
		
		INT								m_iTouchBeginPosY	;
		INT								m_iShowBeginPosY	;
		INT								m_iShowCurrentPosY	;
		BOOLEAN							m_bDirectionDown	;
		BOOLEAN							m_bCanScrollByTouchMove;		
		BOOLEAN							m_bCanMoveFocusByUpDown;

		BOOLEAN							m_bOuterChangedData	;
	};
}