#include "OpenHMI.h"

namespace OpenHMI
{
	CScreenTouchManager	*CScreenTouchManager::s_pInstance	= NULL;

	CScreenTouchManager*	CScreenTouchManager::getInstance()
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CScreenTouchManager();
		}

		return s_pInstance;
	}

	void CScreenTouchManager::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CScreenTouchManager::CScreenTouchManager()
		: m_listCanvas(FALSE)
		, m_mapTracker(FALSE)
	{
		;
	}

	CScreenTouchManager::~CScreenTouchManager()
	{
		;
	}

	void CScreenTouchManager::addCanvas(ICanvas *pCanvas)
	{
		if (pCanvas != NULL)
		{
			m_listCanvas.addFirst(pCanvas);
		}
	}

	void CScreenTouchManager::removeCanvas(ICanvas *pCanvas)
	{
		if (pCanvas != NULL)
		{
			m_listCanvas.deleteItem(pCanvas);
		}
	}

	void CScreenTouchManager::removeCursorTracker(ICursorTracker* pCursorTracker)
	{
		for (UINT ui = 0; ui < m_mapTracker.size(); )
		{
			if ( pCursorTracker == m_mapTracker.getValueByIndex(ui) )
			{
				m_mapTracker.deleteItem(m_mapTracker.getKeyByIndex(ui));
				ui = 0;
			}
			else
			{
				ui++;
			}
		}
	}

	void CScreenTouchManager::onCursorMoved(const CCursor &cursor)
	{		
		m_listCanvas.resetIterator();
		while (m_listCanvas.hasMore())
		{
			m_listCanvas.next()->hoverCheck(cursor.getX(), cursor.getY());
		}
	}

	void CScreenTouchManager::onCursorPressDown(const CCursor &cursor)
	{
		ICursorTracker	*pFirstTracker = NULL;
		ICursorTracker	*pDraggingTracker = NULL;

		for ( UINT ui = 0; ui <  m_mapTracker.size(); ui++ )
		{
			ICursorTracker* pTemp = m_mapTracker.getValueByIndex(ui);

			if ( ui == 0)
			{// maybe there is an error for the first cursor tracker.
				pFirstTracker = pTemp;
			}

			if ( pTemp->isDraging() )
			{
				pDraggingTracker = pTemp;
				break;
			}
		}

		if ( pDraggingTracker != NULL )
		{
			m_mapTracker.addItem(cursor.getID(), pDraggingTracker);
			ENV::addCursor(cursor);
			pDraggingTracker->addCursor(cursor);

			return;
		}

		ICursorTracker	*pCurrentTracker = NULL;
		if (m_mapTracker[cursor.getID()] == NULL)
		{
			m_listCanvas.resetIterator();
			while (m_listCanvas.hasMore())
			{
				pCurrentTracker = m_listCanvas.next()->probe(cursor.getX(), cursor.getY());
				if (pCurrentTracker != NULL)
				{
					break;
				}
			}
		}

		if ( pFirstTracker != NULL )
		{
			if ( pCurrentTracker == pFirstTracker )
			{
				m_mapTracker.addItem(cursor.getID(), pCurrentTracker);
				ENV::addCursor(cursor);
				pCurrentTracker->addCursor(cursor);
			}
			else
			{
				if (pCurrentTracker == NULL)
				{
					return;
				}

				if (   pFirstTracker->isSupportMultiComponentTouched() 
					&& pCurrentTracker->isSupportMultiComponentTouched() )
				{
					m_mapTracker.addItem(cursor.getID(), pCurrentTracker);
					ENV::addCursor(cursor);
					pCurrentTracker->addCursor(cursor);
				}
				else
				{
					m_mapTracker.addItem(cursor.getID(), pFirstTracker);
					ENV::addCursor(cursor);
					pCurrentTracker->addCursor(cursor);
				}
			}
		}
		else
		{
			if (pCurrentTracker != NULL)
			{
				m_mapTracker.addItem(cursor.getID(), pCurrentTracker);
				ENV::addCursor(cursor);
				pCurrentTracker->addCursor(cursor);
			}
		}
	}

	void CScreenTouchManager::onCursorPressDownR(const CCursor &cursor)
	{
		ICursorTracker	*pCurrentTracker = NULL;
		if (m_mapTracker[cursor.getID()] == NULL)
		{
			m_listCanvas.resetIterator();
			while (m_listCanvas.hasMore())
			{
				pCurrentTracker = m_listCanvas.next()->probe(cursor.getX(), cursor.getY());
				if (pCurrentTracker != NULL)
				{
					break;
				}
			}
		}

		if (pCurrentTracker != NULL)
		{
			m_mapTracker.addItem(cursor.getID(), pCurrentTracker);
			pCurrentTracker->addCursorR(cursor);
		}
	}

	void CScreenTouchManager::onCursorPressDrag(const CCursor &cursor)
	{
		ICursorTracker	*pCurrentTracker = m_mapTracker[cursor.getID()];
		if (pCurrentTracker != NULL)
		{
			ENV::updateCursor(cursor);
			pCurrentTracker->updateCursor(cursor);
		}
	}

	void CScreenTouchManager::onCursorPressUp(const CCursor &cursor)
	{
		ICursorTracker	*pCurrentTracker = m_mapTracker[cursor.getID()];

		if (pCurrentTracker != NULL)
		{
			ENV::removeCursor(cursor);
			pCurrentTracker->removeCursor(cursor);
			m_mapTracker.deleteItem(cursor.getID());
		}
	}

	void CScreenTouchManager::onCursorPressUpR(const CCursor &cursor)
	{
		ICursorTracker	*pCurrentTracker = m_mapTracker[cursor.getID()];

		if (pCurrentTracker != NULL)
		{
			pCurrentTracker->removeCursorR(cursor);
			m_mapTracker.deleteItem(cursor.getID());
		}
	}

	void CScreenTouchManager::onWheel(const CCursor &cursor)
	{
		m_listCanvas.resetIterator();
		while (m_listCanvas.hasMore())
		{
			m_listCanvas.next()->wheel(cursor.getWheelType());
		}
	}

	void CScreenTouchManager::onLostFocus()
	{
		m_listCanvas.resetIterator();
		while (m_listCanvas.hasMore())
		{
			m_listCanvas.next()->lostHover();
		}
	}
}