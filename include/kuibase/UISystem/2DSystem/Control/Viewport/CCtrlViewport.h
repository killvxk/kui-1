/*@Author Zeki.Yan*/
#pragma once

#include "../../../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CEventViewport
	{
	public:
		static const UINT TOUCHED			= 0;
	};

	class OPENHMI_API CCtrlViewport : public CComponent, public ITimerListener
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum ETouchStatus
		{
			TS_UNKNOW					= 0,
			TS_PRESS,					
			TS_PRESS_RELEASE,
			TS_LONG_PRESS,
			TS_CONTINUE_PRESS,
			TS_LONG_PRESS_RELEASE,
			TS_MOVE,
			TS_MOVE_RELEASE,
			TS_DRAG,
			TS_DRAG_RELEASE,
			TS_DOUBLE_PRESS,
			TS_DOUBLE_PRESS_RELEASE,
		};

	private:// embed class or struct or enum

	public:// method
		CCtrlViewport();
		virtual ~CCtrlViewport();

		/**
		*  Inner method.
		*/
		void							setStyle(CRenderViewport::CBaseStyle *pStyle);
		CRenderViewport::CBaseStyle*	getStyle() { return m_ptrSytle; }

		/**
		* If you get style and modify its parameters, you should invoke this method.
		*
		*/
		void							updateStyle();

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

		CCtrlViewport::ETouchStatus getTouchStatus() const	{ return m_eTS; }
		INT							getTouchX() const		{ return m_iTouchX; }
		INT							getTouchY() const		{ return m_iTouchY; }
		INT							getSpeedX() const		{ return m_iSpeedX; }
		INT							getSpeedY() const		{ return m_iSpeedY; }

		virtual	void	destroySkin();

		virtual void	buildRenderTree(CRenderGroup &renderGroup);

		virtual	void	onTimer(const CTimer *pTimer, UINT uiCurrentCount);
		virtual	void	onTimerComplete(const CTimer *pTimer){}

	protected:// method
		CCtrlViewport(const CCtrlViewport&){}
		CCtrlViewport& operator =(const CCtrlViewport&){return *this;}

	private:// method

	protected:// property

	private:// property
		ETouchStatus m_eTS;
		INT			 m_iTouchX;
		INT			 m_iTouchY;
		INT          m_iSpeedX;
		INT			 m_iSpeedY;

		ObjectPtr<CRenderViewport::CBaseStyle>	m_ptrSytle;

		CRenderViewport	m_renderViewport;

		CTimer			m_timer;
	};
}