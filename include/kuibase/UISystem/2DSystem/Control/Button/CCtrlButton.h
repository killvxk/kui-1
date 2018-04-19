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
	class OPENHMI_API CEventButton
	{
	public:
		static const UINT TOUCH_PRESSED				= 0;		
		static const UINT TOUCH_RELEASED			= 1;
		static const UINT TOUCH_LONG_PRESSED		= 2;
		static const UINT TOUCH_CONTINUE_PRESSED	= 3;
		static const UINT TOUCH_LONG_RELEASED		= 4;
		static const UINT TOUCH_DOUBLE_PRESSED		= 5;
		static const UINT TOUCH_DOUBLE_RELEASED		= 6;
		static const UINT DISABLE_TOUCHED			= 7;
		static const UINT TOUCH_CANCELED			= 8;
		static const UINT HOVER_FOCUSED				= 9;
		static const UINT HOVER_CANCELED			= 10;
		static const UINT TOUCH_PRESSED_R			= 11;		
		static const UINT TOUCH_RELEASED_R			= 12;
	};

	class OPENHMI_API CCtrlButton : public CCtrlLabel
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method
		CCtrlButton();
		virtual ~CCtrlButton();

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual void	doFocusEnterPress();
		virtual void	doFocusEnterRelease();

		virtual	void	onMove(INT iX, INT iY);
		virtual	void	onLostHover();
		virtual	void	onTouchPress(INT iX, INT iY);
		virtual	void	onTouchPressRelease(INT iX, INT iY);
		virtual	void	onTouchPressR(INT iX, INT iY);
		virtual	void	onTouchPressReleaseR(INT iX, INT iY);
		virtual	void	onTouchLongPress(INT iX, INT iY);
		virtual	void	onTouchContinuePress(INT iX, INT iY);
		virtual	void	onTouchLongPressRelease(INT iX, INT iY);;
		virtual	void	onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY);
		virtual	void	onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY);
		virtual	void	onTouchDragRelease(INT iX, INT iY);
		virtual	void	onTouchDoublePress(INT iX, INT iY);
		virtual	void	onTouchDoublePressRelease(INT iX, INT iY);

		void setHandCursor(BOOLEAN bValue) { m_bHandCursor = bValue; }
	protected:// method
		CCtrlButton(const CCtrlButton&){}
		CCtrlButton& operator =(const CCtrlButton&){return *this;}

	private:// method

	protected:// property

	private:// property
		BOOLEAN m_bHandCursor;
	};
}