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
	* @class	CComponent
	*			It is the base class for all controls.
	*/
	class OPENHMI_API CComponent : 
		public Object, 
		public CEventDispatcher, 
		public IRenderable,
		public ICanvas,
		public ICursorTracker
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum ECCPOperation
		{
			CCP_MOVE_LEFT = 0,
			CCP_MOVE_RIGHT,
			CCP_MOVE_UP,
			CCP_MOVE_DOWN,
			CCP_ROTATE_LEFT,
			CCP_ROTATE_RIGHT,
		};

	private:// embed class or struct or enum
		enum ETouchStatus
		{
			TS_INIT = 0,
			TS_PRESS,
			TS_MOVE,
			TS_LONG_PRESS,
			TS_CONTINUE_PRESS,
			TS_DRAG,
			TS_DOUBLE_PRESS,
			TS_DISABLED_PRESS,
		};

	public:// method
		virtual ~CComponent();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*  Inner method.
		*/
		void			setParent(CComponent *pValue)			{ m_pParent = pValue; }
		CComponent*		getParent()								{ return m_pParent; }

		/**
		*  Inner method.
		*/
		virtual	BOOLEAN	contains(CComponent *pComponent);

		/**
		*  Inner method.
		*/
		void			setBaseView(CView* pValue)				{ m_pBaseView = pValue; }

		/**
		*  Inner method.
		*/
		void			asContainer()							{ m_bIsContainer = TRUE; }
		BOOLEAN			isContainer() const						{ return m_bIsContainer; }

		/**
		*  Inner method.
		*/
		void			setID(UINT uiValue)						{ m_uiID = uiValue; }
		UINT			getID()	const							{ return m_uiID; }

		/**
		*  Inner method.
		*/
		void			setName(const String &strValue)			{ m_strName = strValue; }
		const String &	getName() const							{ return m_strName; }

		/**
		*  Inner method.
		*/
		void			setHasInitFromResource(BOOLEAN bValue)	{ m_bHasInitFromResource = bValue; }
		BOOLEAN			getHasInitFromResource()				{ return m_bHasInitFromResource; }	

		/**
		* Any type of user data inherited from Object, this is used for user extension.
		*/
		void			setUserData(Object	*pData)				{ m_ptrUserData = pData;}
		Object*			getUserData()							{ return m_ptrUserData; }	

		/**
		* Lock this component to make no response to screen touch and ccp.
		*/
		virtual	void	lock();
		virtual	void	unlock();
		BOOLEAN			isLock() const							{ return m_bLock; }		

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* This position is relative to its parent control. And unit is pixel.
		*/
		void			setX(INT iValue);
		INT				getX() const							{ return m_iX; }

		/**
		* This position is relative to its parent control. And unit is pixel.
		*/
		void			setY(INT iValue);
		INT				getY() const							{ return m_iY; }

		/**
		* Change global coordinate into local coordinate. The global origin is left-top of display. 
		*/
		CPoint			globalToLocal(INT iX, INT iY) const;
		CPoint			globalToLocal(const CPoint &pt) const;

		/**
		* Change local coordinate into global coordinate. The global origin is left-top of display.	
		*/
		CPoint			localToGlobal(INT iX, INT iY) const;
		CPoint			localToGlobal(const CPoint &pt) const;

		/**
		* Set the component's preferred width and height.
		* The preferred size is used to decide the hit test area of this component.
		* The hit test area is a rectangle starts from point(X, Y) with width = PreferredSize.x, height = PreferredSize.y.
		*/
		void			setPreferredSize(const CSize &szValue);
		void			setPreferredSize(UINT uiWidth, UINT uiHeight);		

		CSize			getPreferredSize() const				{ return m_szPreferredSize; }

		INT				getCenterPointX() const					{ return m_iX + m_szPreferredSize.getX() / 2; }
		INT				getCenterPointY() const					{ return m_iY + m_szPreferredSize.getY() / 2; }

		/**
		* @brief	Set the rotate information.
		*			Anchor point means the point of intersection between axis and the component.		
		*			This point will be used to place the axis. It is local coordinates refer to its left-top point.
		*			As a limitation,the anchor point must be restricted in the area of the component.
		*
		* @param	fValue		The rotate angle, it is a angle value, not a radian value
		* @param	iAnchorX	The x coordinate of the anchor point. 
		* @param	iAnchorY	The y coordinate of the anchor point.
		* @param	eRA			The axis around which the component rotates.
		*/
		void			setRotateAngle(FLOAT fValue, INT iAnchorX, INT iAnchorY, CRenderGroup::ERotateAxis eRA);

		FLOAT			getRotateAngle() const					{ return m_fRotateAngle; }

		CRenderGroup::ERotateAxis	getRotateAxis() const		{ return m_eRotateAxis; }
		CPoint						getAnchorPoint() const		{ return CPoint(m_iAnchorX, m_iAnchorY); }

		/**
		* Indicate the horizontal scale rate of this component, its value range is over 0(>0).
		*/
		void			setScaleWidth(FLOAT fValue);
		FLOAT			getScaleWidth() const					{ return m_fScaleWidth; }		

		/**
		* Indicate the vertical scale rate of this component, its value range is over 0(>0).
		*/
		void			setScaleHeight(FLOAT fValue);
		FLOAT			getScaleHeight() const					{ return m_fScaleHeight; }
		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*  Inner method.
		*/
		void			setSkin(UINT uiImageID);		

		/**
		* If user are doing animation for this component, user can set this flag, then it may improve render performance.
		*/
		void			setFrequentChangeFlag(BOOLEAN bValue)	{ m_renderGroup.setFrequentChangeFlag(bValue); }

		/**
		* Indicate the opacity of this component, its value range is [0-100]
		*/
		void			setOpacity(UINT uiValue);
		UINT			getOpacity() const						{ return m_uiOpacity; }		
		
		/**
		* Indicate the visibility of this component
		*/
		virtual void	setVisible(BOOLEAN bValue);
		BOOLEAN			getVisible() const						{ return m_bVisible; }		

		/**
		* Indicate this component is enabled or disabled. 
		* This state can maintain for any time.
		* The normal, pressed (touch focus), selected and disable states are mutually exclusive each other.
		*/
		virtual	void	setEnable(BOOLEAN bValue);
		BOOLEAN			getEnable() const						{ return m_bEnable; }


		/**
		* Indicate this component has Hover focus (Hover state) or not.
		* This is a instantaneous state
		* The normal, pressed (touch focus), selected and disable states are mutually exclusive each other.
		*/
		void			setHoverFocus(BOOLEAN bValue);
		BOOLEAN			getHoverFocus() const					{ return m_bHoverFocus; }

		/**
		* Indicate this component has touch focus (touch pressed state) or not.
		* This is a instantaneous state
		* The normal, pressed (touch focus), selected and disable states are mutually exclusive each other.
		*/
		void			setTouchFocus(BOOLEAN bValue);
		BOOLEAN			getTouchFocus() const					{ return m_bTouchFocus; }

		/**
		* Indicate this component has been selected or not.
		* This state can maintain for any time.
		* The normal, pressed (touch focus), selected and disable states are mutually exclusive each other.
		*/
		void			setSelected(BOOLEAN bValue);
		BOOLEAN			getSelected() const						{ return m_bSelected; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* This function is used to refresh this component's appearance.
		* When the control's data changed, you should call this method, or call the notifyModelChanged method of the related model.
		*/
		virtual	void	refresh();

		/**
		* This function is used to little refresh this component's appearance.
		* If only the control's status changed, you should call this method. 
		*/
		virtual	void	littleRefresh();

		/**
		*  Inner method.
		*/
		virtual	void	destroySkin();

		/**
		*  Inner method.
		*/
		virtual	void	destroy();	

		/**
		*  Inner method.
		*/
		BOOLEAN			isDestroyed() const						{ return m_bDestroyed; }

		/**
		*  Inner method.
		*/
		void			setDirty();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// implement interface: IRenderable
		/**
		*  Inner method.
		*/
		virtual	void	buildRenderTree(CRenderGroup &renderGroup);	

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		UINT				getFilterCount() const				{ return m_aryFilter.size(); }


		CRenderBaseFilter*	getFilterAt(UINT uiIndex)			{ return m_aryFilter[uiIndex]; }

		/**
		*  Inner method.
		*/
		void				addFilter(CRenderBaseFilter *pFilter);

		/**
		* If you get a filter and modify its parameters, you should invoke this method.
		*
		*/
		void				updateFilter();

		/**
		* Play all the filters of this component.
		*/
		void				playFilter();

		/**
		* Stop playing all the filters of this component.
		*/
		void				stopFilter();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* The switch of the component to phonate or not when touched.
		*/
		void			setSoundSwitch(BOOLEAN bValue);
		BOOLEAN			getSoundSwitch() const					{ return m_bSoundSwitch; }

		/**
		* The component's sound type.
		* The value means the beep type used in your own project,the framework doesn't concern what it is.
		*/
		void			setSoundType(INT iValue);
		INT				getSoundType() const					{ return m_iSoundType; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Make this component can not get CCP focus.
		*/
		void			notAcceptFocus()						{ m_bCanSetFocus = FALSE; }

		/**
		* Make this component can get CCP focus.
		*/
		void			setAcceptFocus()						{ m_bCanSetFocus = TRUE; }

		/**
		* Make CCP focus can not move out from this component.
		* For container style component, by default, you can move CCP focus to outer.
		* If this type component don't want to move CCP focus outer, should call this method.
		* The "move" here means move left/right/up/down and turning, not including "back".
		*/
		void			notMoveFocusOut()						{ m_bCanMoveFocusOut = FALSE; }
		BOOLEAN			canMoveFocusOut()						{ return m_bCanMoveFocusOut; }

		/**
		* For container style component, by default, you can use Move-Left, Move-Right, Move-Up, Move-Down of CCP to move CCP focus.
		* But if you want to stop this operation, should call this method.
		* But, the Rotate-Left, Rotate-Right of CCP is still effect.	
		*/
		void			notMoveFocus()							{ m_bCanMoveFocus = FALSE; }
		BOOLEAN			canMoveFocus()							{ return m_bCanMoveFocus; }

		/**
		*  Inner method.
		*/
		void			setFocusSkin(UINT uiImageID)			{ m_CCPView.setSkin(uiImageID); }		

		/**
		*  Inner method.
		*/
		void			setCCPBelowComponent(BOOLEAN bValue)	{ m_bCCPBelowComponent = bValue; }
		BOOLEAN			getCCPBelowComponent()					{ return m_bCCPBelowComponent; }				

		/**
		* Set a target component to this component.
		* If this component not accept CCP focus, you can set a target component, when you click this component, the CCP focus will move to the target component.
		*/
		void			setTargetComponent(CComponent *pValue)	{ m_pTargetComponent = pValue; }
		CComponent*		getTargetComponent() const				{ return m_pTargetComponent; }

		/**
		* Set a superior component to this component.
		* If this component and the superiorComponent lie in the same container, we can setup superior and junior relationship between them.
		* When do CCP focus back operation, will move CCP focus from junior component to the superior component.
		* If this component hasn't superior component, doing back CCP focus operation will move CCP focus to its parent container or higher.
		*/
		void			setSuperiorComponent(CComponent *pValue){ m_pSuperiorComponent = pValue; }
		CComponent*		getSuperiorComponent() const			{ return m_pSuperiorComponent; }

		/**
		* Set a junior component to this component.
		* If this component and the superiorComponent lie in the same container, we can setup superior and junior relationship between them.
		* When do CCP focus enter in operation, will move CCP focus from superior component to the junior component.
		* If this component hasn't junior component, do nothing.
		*/
		void			setJuniorComponent(CComponent *pValue)	{ m_pJuniorComponent = pValue; }
		CComponent*		getJuniorComponent() const				{ return m_pJuniorComponent; }

		/**
		*  Inner method.
		*/
		virtual	BOOLEAN	canSetFocus();

		/**
		* Make this component to get CCP focus, and it will release CCP focus from other component.	
		* This CCP focus status can maintain for any time.
		* CCP focus state is attached state, you can attach the CCP focus state to one the normal, pressed (touch focus) and selected states.
		* There is no relationship between pressed state and CCP focus state. They are self-governed states. 
		* But if you don't set CCPSkin to a control, it will use pressed state to replace the CCP focus state.
		* (Notice that only means the appearance, not refer the TouchFocus and CCP Focus properties.)
		*/
		virtual	void	setUniqueFocus();

		/**
		*  Inner method.
		*/
		virtual	void	setFocus(ECCPOperation eCCPOperation = CCP_ROTATE_RIGHT);

		/**
		*  Inner method.
		*/
		virtual	void	releaseFocus();

		/**
		*  Inner method.
		*/
		virtual	BOOLEAN	hasFocus()								{ return m_bCCPFocus; }

		virtual BOOLEAN	moveFocusToLeft()						{ return FALSE; }
		virtual BOOLEAN	moveFocusToRight()						{ return FALSE; }
		virtual BOOLEAN	moveFocusToUp()							{ return FALSE; }
		virtual BOOLEAN	moveFocusToDown()						{ return FALSE; }
		virtual BOOLEAN	turnFocusToLeft(UINT uiStep)			{ return FALSE; }
		virtual BOOLEAN	turnFocusToRight(UINT uiStep)			{ return FALSE; }
		virtual	void	doFocusEnterPress()						{ }
		virtual	void	doFocusEnterRelease()					{ }
		virtual void	doFocusEnterIn();
		virtual BOOLEAN	doFocusBack();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Indicate this component can be hit or not.
		*/
		void			setCanHitTest(BOOLEAN bValue)			{ m_bCanHitTest = bValue; }
		BOOLEAN			getCanHitTest() const					{ return m_bCanHitTest; }

		/**
		* Indicate the component can be touched while it is disabled.
		*/
		void			setCanDisabledTouch(BOOLEAN bValue)		{ m_bCanDisabledTouch = bValue; }
		BOOLEAN			getCanDisabledTouch()					{ return m_bCanDisabledTouch; }

		/**
		* Indicate this component support multi component touched or not when other component be touched.
		*/
		void			setSupportMultiComponentTouched(BOOLEAN bValue)	{ m_bSupportMultiComponentTouched = bValue; }
		BOOLEAN			getSupportMultiComponentTouched() const			{ return m_bSupportMultiComponentTouched; }

		/**
		* Indicate this component can be dragged or not.
		*/
		void			setCanDrag(BOOLEAN bValue)				{ m_bCanDrag = bValue; }
		BOOLEAN			getCanDrag() const						{ return m_bCanDrag; }

		/**
		*  Inner method.
		*/
		virtual	void	setParentMoving(BOOLEAN bValue)			{ m_bIsParentMoving = bValue; }
		BOOLEAN			getParentMoving() const					{ return m_bIsParentMoving; }

		/**
		*  Inner method.
		*/
		UINT			getCursorCnt()							{ return m_aryCursor.size(); }

		/**
		*  Inner method.
		*/
		CCursor&		getCursorByIndex(UINT uiIndex)			{ return m_aryCursor[uiIndex]; }	

		/**
		*  Inner method.
		*/
		void			addTouchCursor(const CCursor& cursor);

		/**
		*  Inner method.
		*/
		void			removeTouchCursor(const CCursor& cursor);

		/**
		*  Inner method.
		*/
		void			addHitTestArea( CBaseHitArea* pHitArea ){ m_aryHitArea.addItem(pHitArea); }

		/**
		* @brief	used to estimate whether a point is in this component's area.
		*
		* @param	iX	X position in screen
		* @param	iY	Y position in screen
		* @return	TRUE	This component is hit.
		*			FALSE	This component is not hit.
		*/
		virtual	BOOLEAN hitTest(INT iX, INT iY);		

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// implement interface: ICanvas
		/**
		*  Inner method.
		*/
		virtual	void move(INT iX, INT iY);
		virtual	void hoverCheck(INT iX, INT iY);
		virtual void			lostHover();
		virtual	ICursorTracker*	probe(INT iX, INT iY);
		virtual	void wheel(CCursor::EWheelType eType);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// implement interface: ICursorTracker
		/**
		*  Inner method.
		*/
		virtual	void	addCursor(const CCursor &cursor);
		virtual void	addCursorR(const CCursor &cursor);

		/**
		*  Inner method.
		*/
		virtual	void	updateCursor(const CCursor &cursor);

		/**
		*  Inner method.
		*/
		virtual	void	removeCursor(const CCursor &cursor);
		virtual void	removeCursorR(const CCursor &cursor);

		/**
		*  Inner method.
		*/
		virtual	BOOLEAN	isSupportMultiComponentTouched()		{ return m_bSupportMultiComponentTouched; }

		/**
		*  Inner method.
		*/
		virtual BOOLEAN	isDraging()								{ return m_eTouch1Status == TS_DRAG ? TRUE : FALSE; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual	void	onMove(INT iX, INT iY)											{ }
		virtual	void	onLostHover()													{ }
		virtual	void	onWheel(CCursor::EWheelType eType)								{ }
		virtual	void	onTouchPress(INT iX, INT iY)									{ }
		virtual	void	onTouchPressRelease(INT iX, INT iY)								{ }
		virtual	void	onTouchPressR(INT iX, INT iY)									{ }
		virtual	void	onTouchPressReleaseR(INT iX, INT iY)							{ }
		virtual	void	onTouchLongPress(INT iX, INT iY)								{ }
		virtual	void	onTouchContinuePress(INT iX, INT iY)							{ }
		virtual	void	onTouchLongPressRelease(INT iX, INT iY)							{ }
		virtual	void	onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)			{ }
		virtual	void	onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)	{ }
		virtual	void	onTouchDrag(INT iX, INT iY)										{ }
		virtual	void	onTouchDragRelease(INT iX, INT iY)								{ }
		virtual	void	onTouchDoublePress(INT iX, INT iY)								{ }
		virtual	void	onTouchDoublePressRelease(INT iX, INT iY)						{ }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		* Indicate this component has private gesture or not.
		* The true value means this component has its own gesture, and it will catch gesture input.
		*/
		void			setCanCatchGesture(BOOLEAN bValue)		{ m_bCanCatchGesture = bValue; }
		BOOLEAN			getCanCatchGesture()					{ return m_bCanCatchGesture; }

		/**
		*  Inner method.
		*/
		void			setGestureGroupID(UINT uiID)			{ m_uiGestureGroupID = uiID; }

		/**
		*  Inner method.
		*/
		UINT			getGestureGroupID()						{ return m_uiGestureGroupID; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	protected:// method
		CComponent();		

		void			innerSetFocus(BOOLEAN bValue)			{ m_bCCPFocus = bValue; }
		BOOLEAN			innerGetFocus()							{ return m_bCCPFocus; }
		
		void			dispatchEvent(UINT uiEventID);

		virtual	void	buildRenderTreeFromBackground(CRenderGroup &renderGroup);
		virtual	void	buildRenderTreeFromCCP(CRenderGroup &renderGroup, INT iCCPPos = -1);

	private:// method
		BOOLEAN			checkMove(const CCursor &cursor, const CPoint &ptLastPoint, INT iDistance);
		
		void			refreshCCPSkin();
		void			drawCCPFocs();
		void			destroyCCPSkin();

		CComponent*		getGestureCatcher();

	protected:// property
		ObjectPtr<CMovieClip>		m_skin;
		ObjectPtr<CMovieClip>		m_CCPSkin;

		CRenderImage				m_renderImage;
		CRenderImage				m_renderImageCCP;
		CRenderGroup				m_renderGroup;

		Array<CBaseHitArea>			m_aryHitArea;

	private:// property	
		//---start---basic property
		CComponent					*m_pParent;
		CView						*m_pBaseView;
		BOOLEAN						m_bIsContainer;

		UINT						m_uiID;
		String						m_strName;
		
		BOOLEAN						m_bHasInitFromResource;
		ObjectPtr<Object>			m_ptrUserData;

		BOOLEAN						m_bLock;
		BOOLEAN						m_bDestroyed;
		//---end---basic property

		//---start---used for transform
		INT							m_iX;
		INT							m_iY;
		CSize						m_szPreferredSize;		
		FLOAT						m_fRotateAngle;
		CRenderGroup::ERotateAxis	m_eRotateAxis;
		INT							m_iAnchorX;
		INT							m_iAnchorY; 
		FLOAT						m_fScaleWidth;
		FLOAT						m_fScaleHeight;
		//---end---used for transform

		//---start---used for render
		UINT						m_uiOpacity;
		BOOLEAN						m_bVisible;
		BOOLEAN						m_bEnable;
		BOOLEAN						m_bHoverFocus;
		BOOLEAN						m_bTouchFocus;
		BOOLEAN						m_bSelected;
		//---end---used for render

		//---start---used for filter
		Array<CRenderBaseFilter>	m_aryFilter;
		//---end---used for filter

		//---start---used for sound
		BOOLEAN						m_bSoundSwitch;
		INT							m_iSoundType;
		//---end---used for sound				

		//---start---used for CCP	
		BOOLEAN						m_bCanSetFocus;
		BOOLEAN						m_bCanMoveFocusOut;
		BOOLEAN						m_bCanMoveFocus;

		BOOLEAN						m_bCCPFocus;
		BOOLEAN						m_bCCPBelowComponent;
		CView						m_CCPView;

		CComponent					*m_pTargetComponent;
		CComponent					*m_pSuperiorComponent;
		CComponent					*m_pJuniorComponent;
		//---end---used for CCP	

		//---start---used for touch
		BOOLEAN						m_bCanHitTest;				
		BOOLEAN						m_bCanDisabledTouch;		
		BOOLEAN						m_bSupportMultiComponentTouched;
		BOOLEAN						m_bCanDrag;
		BOOLEAN						m_bIsParentMoving;

		UINT						m_uiTouch1ID;
		ETouchStatus				m_eTouch1Status;
		CPoint						m_ptTouch1LastPoint;
		UINT						m_uiTouch1Time;	
		
		Array2<CCursor>				m_aryCursor;
		UINT						m_uiActiveCursor;
		//---end---used for touch

		//---start---used for gesture
		BOOLEAN						m_bCanCatchGesture;
		UINT						m_uiGestureGroupID;
		//---end---used for gesture
	};
}