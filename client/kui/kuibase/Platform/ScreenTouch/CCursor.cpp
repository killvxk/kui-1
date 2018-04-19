#include "OpenHMI.h"


namespace OpenHMI
{
	UINT CCursor::s_iCursorID = 0;

	UINT CCursor::getNewID()
	{
		s_iCursorID++;

		if (s_iCursorID == 0)
		{
			s_iCursorID = 1;
		}

		return s_iCursorID;
	}

	CCursor::CCursor() 
		: m_uiID(0)
		, m_iX(-1)
		, m_iY(-1)
		, m_iXSpeed(0)
		, m_iYSpeed(0)
		, m_eTouchStatus(TS_UNDEFINE)
		, m_iTimestamp(0)
		, m_bIsAlive(FALSE)
		, m_eWheelType(WT_UNDEFINE)
	{
		;
	}

	CCursor::CCursor(const CCursor &src) 
		: m_uiID(src.m_uiID)
		, m_iX(src.m_iX)
		, m_iY(src.m_iY)
		, m_iXSpeed(src.m_iXSpeed)
		, m_iYSpeed(src.m_iYSpeed)
		, m_eTouchStatus(src.m_eTouchStatus)
		, m_iTimestamp(src.m_iTimestamp)
		, m_eWheelType(src.m_eWheelType)
		, m_bIsAlive(FALSE)
	{
		;
	}

	CCursor::~CCursor()
	{
		;
	}	

	void CCursor::calcXSpeed(INT preX)
	{
		if (m_iTimestamp == 0)
		{
			if (preX != 0)
			{
				m_iXSpeed = 1;
			}
			else
			{
				m_iXSpeed = 0;
			}
		}
		else
		{
			m_iXSpeed = (m_iX - preX) / m_iTimestamp;
		}
	}

	void CCursor::calcYSpeed(INT preY)
	{
		if (m_iTimestamp == 0)
		{
			if (preY != 0)
			{
				m_iYSpeed = 1;
			}
			else
			{
				m_iYSpeed = 0;
			}
		}
		else
		{
			m_iYSpeed = (m_iY - preY) / m_iTimestamp;
		}
	}

	void CCursor::reset()
	{
		m_uiID = 0;
		m_iX = 0;
		m_iY = 0;
		m_iXSpeed = 0;
		m_iYSpeed = 0;
		m_eTouchStatus = TS_UNDEFINE;
		m_iTimestamp = 0;
		m_bIsAlive = FALSE;
	}

	CCursor& CCursor::operator =(const CCursor &src)
	{
		m_uiID = src.m_uiID;
		m_iX = src.m_iX;
		m_iY = src.m_iY;
		m_iXSpeed = src.m_iXSpeed;
		m_iYSpeed = src.m_iYSpeed;
		m_eTouchStatus = src.m_eTouchStatus;
		m_iTimestamp = src.m_iTimestamp;
		m_bIsAlive = src.m_bIsAlive;

		return *this;
	}
}