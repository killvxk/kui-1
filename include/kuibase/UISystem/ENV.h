/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"
#include "./UISystem/2DSystem/Misc/CRectangle.h"
namespace OpenHMI
{
	class OPENHMI_API ENV
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		////////////////---dirty info---/////////////////////////////
		static	BOOLEAN isDirty()								{ return s_bDirty; }
		static	void	setDirty();
		static	void	clearDirty()							{ s_bDirty = FALSE; }
		static	void	lockDirty()								{ s_bIsLockDirty = TRUE; }
		static	void	unlockDirty()							{ s_bIsLockDirty = FALSE; }

		////////////////---screen info---/////////////////////////////
		static	void	setScreenOffsetX(INT iValue)			{ s_iScreenOffsetX = iValue; }
		static	INT		getScreenOffsetX()						{ return s_iScreenOffsetX; }
		static	void	setScreenOffsetY(INT iValue)			{ s_iScreenOffsetY = iValue; }
		static	INT		getScreenOffsetY()						{ return s_iScreenOffsetY; }
		static	void	setScreenWidth(UINT uiValue)			{ s_uiScreenWidth = uiValue; }
		static	UINT	getScreenWidth()						{ return s_uiScreenWidth; }
		static	void	setScreenHeight(UINT uiValue)			{ s_uiScreenHeight = uiValue; }
		static	UINT	getScreenHeight()						{ return s_uiScreenHeight; }
		static	void	setScreenCaptionRect(CRectangle& rc)	{s_rcCaption = rc;}
		static	CRectangle&	getScreenCaptionRect()				{ return s_rcCaption; }

		////////////////---touch info---/////////////////////////////
		static	void	setHitOffset(UINT uiValue)				{ s_uiHitOffset = uiValue; }
		static	UINT	getHitOffset()							{ return s_uiHitOffset; }
		static	void	setWaverDistance(UINT uiValue)			{ s_uiWaverDistance = uiValue; }
		static	UINT	getWaverDistance()						{ return s_uiWaverDistance; }
		static	void	setMoveWaverDistance(UINT uiValue)		{ s_uiMoveWaverDistance = uiValue; }
		static	UINT	getMoveWaverDistance()					{ return s_uiMoveWaverDistance; }
		static	void	setLongPressDelay(UINT uiValue)			{ s_uiLongPressDelay = uiValue; }
		static	UINT	getLongPressDelay()						{ return s_uiLongPressDelay; }
		static	void	setContinuePressInterval(UINT uiValue)	{ s_uiContinuePressInterval = uiValue; }
		static	UINT	getContinuePressInterval()				{ return s_uiContinuePressInterval; }
		static	void	setDoubleTapInterval(UINT uiValue)		{ s_uiDoubleTapInterval = uiValue; }
		static	UINT	getDoubleTapInterval()					{ return s_uiDoubleTapInterval; }
		static	void	setDoubleTapWaverDistance(UINT uiValue)	{ s_uiDoubleTapWaverDistance = uiValue; }
		static	UINT	getDoubleTapWaverDistance()				{ return s_uiDoubleTapWaverDistance; }
		static	void	setContinueTapInterval(UINT uiValue)	{ s_uiContinueTapInterval = uiValue; }
		static	UINT	getContinueTapInterval()				{ return s_uiContinueTapInterval; }

		static	UINT	getCursorCount()						{ return s_aryCursor.size(); }
		static	CCursor	getCursor(UINT uiIndex = 0)				{ return s_aryCursor[uiIndex]; }
		static	void	addCursor(const CCursor &cursor);
		static	void	updateCursor(const CCursor &cursor);
		static	void	removeCursor(const CCursor &cursor);

		////////////////---CCP focus info---/////////////////////////////
		static	void		setCCPBindTouch(BOOLEAN bValue)		{ s_bCCPBindTouch = bValue; }
		static	BOOLEAN		getCCPBindTouch()					{ return s_bCCPBindTouch; }
		
		static	void		setTopOverlay(CContainer *pValue)	{ s_pTopOverlayer = pValue; }
		static	CContainer*	getTopOverlay()						{ return s_pTopOverlayer; }
		static	void		setTopScreen(CContainer *pValue)	{ s_pTopScreen = pValue; }
		static	CContainer*	getTopScreen()						{ return s_pTopScreen; }

		static	void		switchCCPFocus(CComponent *pTouchedComponent);
		static	void		switchCCPFocusTo(CComponent *pComponent);

		////////////////---skin info---/////////////////////////////
		static	void		setMovieClipGenerator(IMovieClipGenerator *pValue)	{ s_pMovieClipGenerator = pValue; }
		static	CMovieClip*	getMovieClip(UINT uiImageID);

		////////////////---sound info---/////////////////////////////
		static	void		setSoundSwitch(BOOLEAN bValue)				{ s_bSoundSwitch = bValue; }
		static	BOOLEAN		getSoundSwitch()							{ return s_bSoundSwitch; }

		static	void		setSoundGenerator(ISoundGenerator *pValue)	{ s_pSoundGenerator = pValue; }
		static	void		playSoundForButtonClicked(CComponent *pComponent);

		///////////////---gesture info---////////////////////////////
		static	void					setGestureRecognizer(IGestureRecognizer *pValue)	{ s_pGestureRecognizer = pValue; }
		static	IGestureRecognizer*		getGestureRecognizer()								{ return s_pGestureRecognizer; }
		static	UINT					getGestureGroupID(CComponent* pComponent, BOOLEAN bReleasePrevious = FALSE);
		static	CComponent*				getGestureCatcherByGroupID(UINT uiID)				{ return s_mapGestureGroup.getValueByKey(uiID); }
		static	void					deleteGestureCatcher(CComponent* pComponent)		{ s_mapGestureGroup.deleteItem(getGestureGroupID(pComponent)); }

		//////////////---other---///////////////////////////////////
		static	void					incRenderCounter()				{ s_ulRenderCounter++; }
		static	ULONG					getRenderCounter()				{ return s_ulRenderCounter; }

	protected:// method
		ENV(){}
		ENV(const ENV&){}
		virtual ~ENV(){}		
		ENV& operator =(const ENV&){return *this;}

	private:// method

	protected:// property

	private:// property
		static	 BOOLEAN				s_bDirty;
		static	 BOOLEAN				s_bIsLockDirty;

		static	INT						s_iScreenOffsetX;
		static	INT						s_iScreenOffsetY;
		static	UINT					s_uiScreenWidth;
		static	UINT					s_uiScreenHeight;

		static	CRectangle				s_rcCaption;
		static	UINT					s_uiHitOffset;
		static	UINT					s_uiWaverDistance;
		static	UINT					s_uiMoveWaverDistance;
		static	UINT					s_uiLongPressDelay;
		static	UINT					s_uiContinuePressInterval;
		static	UINT					s_uiDoubleTapInterval;
		static	UINT					s_uiDoubleTapWaverDistance;
		static	UINT					s_uiContinueTapInterval;
		static	Array2<CCursor>			s_aryCursor;

		static	BOOLEAN					s_bCCPBindTouch;
		static	CContainer				*s_pTopOverlayer;
		static	CContainer				*s_pTopScreen;

		static	IMovieClipGenerator		*s_pMovieClipGenerator;

		static	BOOLEAN					s_bSoundSwitch;
		static	ISoundGenerator			*s_pSoundGenerator;

		static	IGestureRecognizer		*s_pGestureRecognizer;
		static	Map2<UINT, CComponent*, tagNumCompare<UINT> >	s_mapGestureGroup;
		static	UINT					s_uiGestureGroupID;

		static	ULONG					s_ulRenderCounter;
	};
}