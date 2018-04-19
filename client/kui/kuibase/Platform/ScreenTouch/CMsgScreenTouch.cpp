#include "OpenHMI.h"

namespace OpenHMI
{
	CMsgScreenTouch::CMsgScreenTouch(
					IScreenTouchListener	*pListener,
					const CCursor			&cursor)
		: m_pListener(pListener)
		, m_cursor(cursor)
	{
		m_eType = CBaseMessage::MT_SCREEN_TOUCH;
		m_uiID = m_eType;
	}

	CMsgScreenTouch::~CMsgScreenTouch()
	{
		;
	}

	void CMsgScreenTouch::handleMessage()
	{
		if (m_pListener == NULL)
		{
			return;
		}

		switch (m_cursor.getTouchStatus())
		{
		case CCursor::TS_LOSTFOCUS:
			m_pListener->onLostFocus();
			break;

		case CCursor::TS_MOVE:
			m_pListener->onCursorMoved(m_cursor);
			break;

		case CCursor::TS_PRESS_DOWN:
			m_pListener->onCursorPressDown(m_cursor);
			break;

		case CCursor::TS_R_PRESS_DOWN:
			m_pListener->onCursorPressDownR(m_cursor);
			break;
		
		case CCursor::TS_PRESS_DRAG:
			m_pListener->onCursorPressDrag(m_cursor);
			break;

		case CCursor::TS_PRESS_UP:
			m_pListener->onCursorPressUp(m_cursor);
			break;

		case CCursor::TS_R_PRESS_UP:
			m_pListener->onCursorPressUpR(m_cursor);
			break;
		
		case CCursor::TS_WHEEL:
			{
				m_pListener->onWheel(m_cursor);
			}
			break;
		}
	} 
}