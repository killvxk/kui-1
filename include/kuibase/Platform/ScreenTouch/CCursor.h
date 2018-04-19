/*@Author Zeki.Yan*/
#pragma once

#include "../../Common.h"

namespace OpenHMI
{
	class OPENHMI_API CCursor : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum ETouchStatus
		{
			TS_MOVE = 0,		//鼠标移动
			TS_PRESS_DOWN,		//鼠标按下
			TS_PRESS_DRAG,		//鼠标按下拖动
			TS_PRESS_UP,		//鼠标抬起
			TS_R_PRESS_DOWN,	//鼠标右键按下
			TS_R_PRESS_DRAG,	//鼠标右键按下拖动
			TS_R_PRESS_UP,		//鼠标右键抬起
			TS_WHEEL,			//滚轮滚动
			TS_SETFOCUS,		//获得焦点
			TS_LOSTFOCUS,		//失去焦点
			TS_UNDEFINE,
		};

		enum EWheelType
		{
			WT_UP = 0,		//滚轮向上
			WT_DOWN,		//滚轮向下
			WT_UNDEFINE,
		};

	private:// embed class or struct or enum

	public:// method
		static	UINT	getNewID();

		CCursor();
		CCursor(const CCursor &src);
		virtual ~CCursor();

		void			setID(UINT uiValue)					{ m_uiID = uiValue; }
		UINT			getID() const						{ return m_uiID; }
		void			setX(INT iValue)					{ m_iX = iValue; }
		INT				getX() const						{ return m_iX; }
		void			setY(INT iValue)					{ m_iY = iValue; }
		INT				getY() const						{ return m_iY; }
		void			setXSpeed(INT iValue)				{ m_iXSpeed = iValue; }
		INT				getXSpeed() const					{ return m_iXSpeed; }
		void			setYSpeed(INT iValue)				{ m_iYSpeed = iValue; }
		INT				getYSpeed() const					{ return m_iYSpeed; }
		void			setTouchStatus(ETouchStatus eValue)	{ m_eTouchStatus = eValue; }
		ETouchStatus	getTouchStatus() const				{ return m_eTouchStatus; }
		void			setTimestamp(INT uiValue)			{ m_iTimestamp = uiValue; }
		INT				getTimestamp() const				{ return m_iTimestamp; }
		void			setAlive(BOOLEAN bAlive)			{ m_bIsAlive = bAlive; }
		BOOLEAN			getAlive()							{ return m_bIsAlive; }

		void			setWheelType(EWheelType eValue)					{ m_eWheelType = eValue; }
		EWheelType		getWheelType() const						{ return m_eWheelType; }
		

		void	calcXSpeed(INT preX);
		void	calcYSpeed(INT preY);
		void	reset();

		CCursor& operator =(const CCursor &src);

	protected:// method

	private:// method

	protected:// property

	private:// property
		static UINT 	s_iCursorID;

		UINT			m_uiID;
		INT				m_iX;
		INT				m_iY;
		INT				m_iXSpeed;
		INT				m_iYSpeed;
		ETouchStatus 	m_eTouchStatus;
		INT				m_iTimestamp;
		BOOLEAN			m_bIsAlive;

		EWheelType		m_eWheelType;
	};
}