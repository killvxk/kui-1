/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventSelector
	{
		public:	
			static const UINT CCP_FOCUS_CHANGED			= 0;
	};

	class OPENHMI_API CCtrlSelector 
		: public CComponent
		, public IEventListenerChain
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum		
		enum ELayoutType
		{
			LT_LANDSCRAPE			= 0,
			LT_PROTRAIT				= 1,
		};

	private:// embed class or struct or enum
		
	public:// method
		CCtrlSelector();
		virtual ~CCtrlSelector();
		
		/**
		 * @brief	Set an item into one group.
		 *
		 * @param	strName		All items are built into the selector resource in OpenHMIArtist. And, each item has a unique name.
		 * @param	iGroupID	If the iGroupID > 0, the item will belong to a item group identified by the ID. 
		 * 						All of the items in one item group is mutex, that's means that only one item can be selected.
		 */
		void			setItemGroup(const String& strName, INT iGroupID);

		/**
		 * @brief	Make a item to be selected or not.
		 *
		 * @param	strName The item name defined in the OpenHMIArtist.
		 * @param	bSelected Indicate selected or not.
		 * @param	iGroupID The group ID this item belong to.
		 */
		void			setItemSelect(const String& strName, BOOLEAN bSelected, INT iGroupID = 0);
		
		/**
		 * @brief	Clear the selected item in specified group.
		 *
		 * @param	iGroupID 	If it is 0, clear all selected items.
		 * 						If it is bigger than 0, clear the selected item in this group.
		 */
		void			clearSelectedItem(INT iGroupID = 0);
	
		void			setItemEnable(const String& strName, BOOLEAN bEnable);
		void			setItemVisible(const String& strName, BOOLEAN bVisible);

		/**
		 * Get the button control for the item by the item name defined in the OpenHMIArtist.
		 */
		CCtrlButton*	getButton(const String& strName);
		
		/**
		 * The items are composed in horizontal style or vertical style.
		 */
		void			setLayout(ELayoutType eValue) { m_eLayout = eValue; }

		void			addEventListener(IEventListener *pListener);

		/**
		 * @brief	Make a item to has CCP focus.
		 *
		 * @param	strName The item name defined in the OpenHMIArtist.
		 * @param	bAutoRefresh If it is true, this will cause to call setUniqueFocus() function.
		 */
		void			setCCPFocusItem(const String& strName, BOOLEAN bAutoRefresh = TRUE);
		const String&	getCCPFocusItem();
		const String&	getCCPFocusLastItem();

		virtual void	refresh();
		virtual void	littleRefresh();	
		virtual void	destroySkin();
		virtual void	destroy();
		virtual void	buildRenderTree(CRenderGroup &renderGroup);

		virtual BOOLEAN canSetFocus();
		virtual void	setFocus(ECCPOperation eCCPOperation = CComponent::CCP_ROTATE_RIGHT);
		virtual void	releaseFocus();
		virtual BOOLEAN moveFocusToLeft();
		virtual BOOLEAN	moveFocusToRight();
		virtual BOOLEAN	moveFocusToUp();
		virtual BOOLEAN	moveFocusToDown();
		virtual BOOLEAN turnFocusToLeft(UINT uiStep);
		virtual BOOLEAN turnFocusToRight(UINT uiStep);
		virtual void	doFocusEnterPress();
		virtual void    doFocusEnterRelease();

		virtual void	onMove(INT iX, INT iY);
		virtual void	onTouchPress(INT iX, INT iY);
		virtual void	onTouchPressRelease(INT iX, INT iY);
		virtual void	onTouchLongPress(INT iX, INT iY);
		virtual void	onTouchContinuePress(INT iX, INT iY);
		virtual void	onTouchLongPressRelease(INT iX, INT iY);
		virtual void	onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY);
		virtual void	onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY);
		virtual void	onTouchDrag(INT iX, INT iY);
		virtual void	onTouchDragRelease(INT iX, INT iY);

		// implement IEventListenerChain interface
		virtual BOOLEAN	contains(Object *pTarget);

	protected:// method
		CCtrlSelector(const CCtrlSelector&){}
		CCtrlSelector& operator =(const CCtrlSelector&){return *this;}

	private:// method
		void			drawItemCCPFocs();
		BOOLEAN			moveFocusToPrevious();
		BOOLEAN			moveFocusToNext();
		CCtrlButton*	getCCPFocusButton();
		void			releaseButtonFocus();
		void			switchCCPFocusForTouch();
		BOOLEAN			isSameButton(INT iIndex, const String& strName);
		INT				getFirstCanSetFocusItem();
		INT				getLastCanSetFocusItem();
		INT				getPreviousCanSetFocusItem(INT iIndex);
		INT				getNextCanSetFocusItem(INT iIndex);
		
	protected:// property
		
	private:// property
		CView			m_view;

		Map<String, CCtrlButton,  tagStringCompare>		m_mapButton;
		Array2<CCtrlButton*>							m_aryButton;

		Map<INT,    CButtonGroup, tagNumCompare<INT> >	m_mapGroup;

		String			m_strFocusSel;
		INT				m_iCurCCPFocus;
		ELayoutType		m_eLayout;

		IEventListener	*m_pEventListenr;
	};
}