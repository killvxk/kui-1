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
	* @class	CContainer	
	*			It is the base class for container type control. As the class name showing, it can have children controls.
	*/
	class OPENHMI_API CContainer 
		: public CComponent
		, public IEventListener
	{
	public:// const define

	private:// const define
		static	const	INT		DISTANCE_OFFSET = 10;

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method		
		CContainer();
		virtual ~CContainer();

		DECLARE_EVENT_MAP

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*  Inner method.
		*/
		void			setType(UINT uiValue)	{ m_uiType = uiValue; }
		UINT			getType()				{ return m_uiType; }

		UINT			getComponentCount();
		CComponent*		getComponentAt(UINT uiIndex);
		CComponent*		getComponentByID(UINT uiID);
		INT				getComponentPos(CComponent *pComponent);
		void			addComponent(CComponent *pComponent);
		void			insertComponent(CComponent *pComponent, UINT uiIndex);
		void			removeComponentAt(UINT uiIndex);
		void			removeComponentByID(UINT uiID);
		void			removeComponent(CComponent *pComponent);
		void			removeAllComponents();
		BOOLEAN			containsComponent(CComponent *pComponent);

		virtual	void	hideComponents();
		virtual void	showComponents();
		virtual void	resetLayout();

		virtual void	lock();
		virtual void	unlock();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Once called this method, the ccp move will obey the add order instead of the order computed by position relationship.
		*/
		void			moveFocusInAddOrder()	{ m_bMoveCCPFocusInAddOrder = TRUE; }

		virtual	BOOLEAN	canSetFocus();
		virtual	void	setFocus(CComponent::ECCPOperation eCCPOperation = CComponent::CCP_ROTATE_RIGHT);
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
		virtual void	doFocusEnterIn();
		virtual BOOLEAN	doFocusBack();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual	void	setParentMoving(BOOLEAN bValue);
		virtual	BOOLEAN hitTest(INT iX, INT iY);
		virtual	BOOLEAN	contains(CComponent *pComponent);
		virtual	void	refresh();
		virtual	void	littleRefresh();
		virtual	void	destroySkin();
		virtual	void	destroy();	

		// implement interfaces
		virtual	void			buildRenderTree(CRenderGroup &renderGroup);
		virtual void			hoverCheck(INT iX, INT iY);
		virtual void			lostHover();
		virtual	void			wheel(CCursor::EWheelType eType);
		virtual	void			move(INT iX, INT iY);	
		virtual	ICursorTracker*	probe(INT iX, INT iY);		

		virtual	void	onTouchPress(INT iX, INT iY);
		virtual	void	onTouchPressRelease(INT iX, INT iY);
		virtual	void	onTouchLongPress(INT iX, INT iY);
		virtual	void	onTouchContinuePress(INT iX, INT iY);
		virtual	void	onTouchLongPressRelease(INT iX, INT iY);
		virtual	void	onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY);
		virtual	void	onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY);
		virtual	void	onTouchDrag(INT iX, INT iY);
		virtual	void	onTouchDragRelease(INT iX, INT iY);
		virtual	void	onTouchDoublePress(INT iX, INT iY);
		virtual	void	onTouchDoublePressRelease(INT iX, INT iY);

	protected:// method
		CContainer(const CContainer&){}
		CContainer& operator =(const CContainer&){return *this;}

		void			buildRenderTreeFromChildren(CRenderGroup &renderGroup);

		CComponent*		getFocusComponent();		

	private:// method
		CComponent*		getFirstComponentByPos();
		CComponent*		getLastComponentByPos();
		CComponent*		getFirstComponentByAddOrder();
		CComponent*		getLastComponentByAddOrder();
		INT				getComponentDistance(CComponent* pComponent1, CComponent* pComponent2);

	protected:// property		

	private:// property
		UINT				m_uiType;
		CView				m_view;		
		Array<CComponent>	m_aryChildren;

		BOOLEAN				m_bMoveCCPFocusInAddOrder;

		CComponent*			m_pHittedComponent;
		CComponent*			m_pLastHittedComponent;
	};
}