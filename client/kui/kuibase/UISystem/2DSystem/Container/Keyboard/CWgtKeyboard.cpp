#include "OpenHMI.h"

namespace OpenHMI
{
	CWgtKeyboard::CWgtKeyboard()
		: m_pKeyTable(NULL)
		, m_pCurFocusKey(NULL)
		, m_pNewFocusKey(NULL)
		, m_bFocusChanged(FALSE)
	{
		setType(CONFIG::CONTAINER_TYPE_CTRLWIDGET);
	}

	CWgtKeyboard::~CWgtKeyboard()
	{
		;
	}

	BEGIN_EVENT_MAP(CWgtKeyboard)
		CONTROL_EVENT_HANDLER_CHAIN(this, CEventButton::TOUCH_RELEASED, onBtnKeyClick)
		CONTROL_EVENT_HANDLER_CHAIN(this, CEventButton::TOUCH_LONG_RELEASED, onBtnKeyLongClick)
	END_EVENT_MAP(CContainer)

	// TPDO register Event 
	void CWgtKeyboard::setKeyEnable(const String& strKey, BOOLEAN bEnable)
	{
		CWgtKeyboard_Key *pkey = m_mapKey[strKey];
		if ( pkey != NULL ) 
		{
			pkey->btnKey.setEnable(bEnable);
			pkey->btnKey.littleRefresh();
		}
	}

	void CWgtKeyboard::setKeyEnable(UINT uiKeyCode, BOOLEAN bEnable)
	{
		CWgtKeyboard_Key *pKey = NULL;

		m_mapKey.resetIterator();
		while (m_mapKey.hasMore())
		{
			pKey = m_mapKey.next();

			if ( pKey->aryKeyCode.isExist(uiKeyCode) )
			{
				pKey->btnKey.setEnable(bEnable);
				pKey->btnKey.littleRefresh();

				return;
			}
		}
	}

	void CWgtKeyboard::setKeyTouchFocus(const String& strKey, BOOLEAN bFocus)
	{
		CWgtKeyboard_Key *pkey = m_mapKey[strKey];
		if ( pkey != NULL ) 
		{
			pkey->btnKey.setTouchFocus(bFocus);
			pkey->btnKey.littleRefresh();
		}
	}

	void CWgtKeyboard::setKeyTouchFocus(UINT uiKeyCode, BOOLEAN bFocus)
	{
		CWgtKeyboard_Key *pKey = NULL;

		m_mapKey.resetIterator();
		while (m_mapKey.hasMore())
		{
			pKey = m_mapKey.next();

			if ( pKey->aryKeyCode.isExist(uiKeyCode) )
			{
				pKey->btnKey.setTouchFocus(bFocus);
				pKey->btnKey.littleRefresh();

				return;
			}
		}
	}

	void CWgtKeyboard::setKeyVisible(const String& strKey, BOOLEAN bVisible)
	{
		CWgtKeyboard_Key *pkey = m_mapKey[strKey];
		if ( pkey != NULL ) 
		{
			pkey->btnKey.setVisible(bVisible);
			pkey->btnKey.littleRefresh();
		}
	}

	void CWgtKeyboard::setKeyVisible(UINT uiKeyCode, BOOLEAN bVisible)
	{
		CWgtKeyboard_Key *pKey = NULL;

		m_mapKey.resetIterator();
		while (m_mapKey.hasMore())
		{
			pKey = m_mapKey.next();

			if ( pKey->aryKeyCode.isExist(uiKeyCode) )
			{
				pKey->btnKey.setVisible(bVisible);
				pKey->btnKey.littleRefresh();

				return;
			}
		}
	}

	void CWgtKeyboard::setAllKeysVisible(BOOLEAN bVisible)
	{
		CWgtKeyboard_Key *pKey = NULL;

		m_mapKey.resetIterator();
		while (m_mapKey.hasMore())
		{
			pKey = m_mapKey.next();
			pKey->btnKey.setVisible(bVisible);
			pKey->btnKey.littleRefresh();
		}
	}

	CCtrlButton* CWgtKeyboard::getButton(const String& strKey)
	{
		CWgtKeyboard_Key* pKey = m_mapKey[strKey];
		if (NULL == pKey)
		{
			return NULL;
		}

		return &(pKey->btnKey);
	}

	void CWgtKeyboard::setEnable(BOOLEAN bValue)
	{
		CContainer::setEnable(bValue);
		
		m_mapKey.resetIterator();
		while( m_mapKey.hasMore() )
		{
			m_mapKey.next()->btnKey.setEnable(bValue);
		}
	}

	void CWgtKeyboard::destroy()
	{
		if (isDestroyed())
		{
			return;
		}

		m_pKeyTable = NULL;
		
		m_mapKey.resetIterator();
		while (m_mapKey.hasMore())
		{
			m_mapKey.next()->btnKey.removeEventListener(this);
		}

		m_pCurFocusKey = NULL;
		m_pNewFocusKey = NULL;

		CContainer::destroy();
	}

	ICursorTracker* CWgtKeyboard::probe(INT iX, INT iY)
	{
		if (getVisible() == FALSE)
		{
			return NULL;
		}

		if (CContainer::hitTest(iX, iY))
		{
			return this;
		}

		return NULL;
	}

	void CWgtKeyboard::doFocusEnterIn()
	{
		CComponent::doFocusEnterIn();
	}

	void CWgtKeyboard::onTouchPress(INT iX, INT iY)
	{
		CWgtKeyboard_Key *pKey = NULL;

		m_mapKey.resetIterator();
		while( m_mapKey.hasMore() )
		{
			pKey = m_mapKey.next();
			
			if ( pKey->btnKey.hitTest(iX, iY) )
			{
				m_pCurFocusKey = pKey;
				m_pCurFocusKey->btnKey.onTouchPress(iX, iY);
				onKeyPressed( m_pCurFocusKey->btnKey.getName() );
				return;
			}
		}
	}

	void CWgtKeyboard::onTouchPressRelease(INT iX, INT iY)
	{
		if (m_pCurFocusKey != NULL)
		{
			m_pCurFocusKey->btnKey.onTouchPressRelease(iX, iY);
			onKeyReleased();

			m_pCurFocusKey = NULL;
		}
	}

	void CWgtKeyboard::onTouchLongPress(INT iX, INT iY)
	{
		if (m_pCurFocusKey != NULL)
		{
			m_pCurFocusKey->btnKey.onTouchLongPress(iX, iY);
			onKeyLongPressed( m_pCurFocusKey->btnKey.getName() );
		}
	}

	void CWgtKeyboard::onTouchLongPressRelease(INT iX, INT iY)
	{
		if (m_pCurFocusKey != NULL)
		{
			m_pCurFocusKey->btnKey.onTouchLongPressRelease(iX, iY);
			onKeyLongReleased();

			m_pCurFocusKey = NULL;
		}
	}

	void CWgtKeyboard::onTouchMove(INT iX, INT iY, INT iDeltaX, INT iDeltaY)
	{
		if (m_bFocusChanged == FALSE)
		{
			return;
		}

		if (m_pCurFocusKey == NULL)
		{
			return;
		}

		if (!m_pCurFocusKey->btnKey.hitTest(iX, iY))
		{
			CWgtKeyboard_Key *pKey = NULL;

			m_mapKey.resetIterator();
			while (m_mapKey.hasMore())
			{
				pKey = m_mapKey.next();

				if (pKey->btnKey.getEnable() && pKey->btnKey.hitTest(iX, iY))
				{
					if (m_pNewFocusKey != pKey)
					{
						m_pNewFocusKey = pKey;
						m_pNewFocusKey->btnKey.onTouchMove(iX, iY, iDeltaX, iDeltaY);
						onKeyFocusChanged( m_pNewFocusKey->btnKey.getName() );
					}
					else
					{
						m_pNewFocusKey->btnKey.onTouchMove(iX, iY, iDeltaX, iDeltaY);
					}
				}
			}

			if (m_pNewFocusKey != NULL)
			{
				m_pNewFocusKey = NULL;
				onKeyFocusChanged(L"");
			}
		}
		else
		{
			m_pCurFocusKey->btnKey.onTouchMove(iX, iY, iDeltaX, iDeltaY);

			if (m_pNewFocusKey != NULL)
			{
				m_pNewFocusKey = NULL;
				onKeyFocusChanged( m_pCurFocusKey->btnKey.getName() );
			}
		}
	}

	void CWgtKeyboard::onTouchMoveRelease(INT iX, INT iY, INT iSpeedX, INT iSpeedY)
	{
		if (m_pCurFocusKey == NULL)
		{
			return;
		}

		if (m_pNewFocusKey != NULL && m_bFocusChanged)
		{
			dispatchKeyboardEvent(CEventKeyboard::KEY_CLICKED, m_pNewFocusKey->aryKeyType[0], m_pNewFocusKey->aryKeyCode[0]);
		}

		m_pCurFocusKey->btnKey.onTouchMoveRelease(iX, iY, iSpeedX, iSpeedY);
		onKeyReleased();
		
		m_pCurFocusKey = NULL;
		m_pNewFocusKey = NULL;
	}


	void CWgtKeyboard::onTouchDrag(INT iX, INT iY)
	{
		onTouchMove(iX, iY, 0, 0);
	}

	void CWgtKeyboard::onTouchDragRelease(INT iX, INT iY)
	{
		onTouchMoveRelease(iX, iY, 0, 0);
	}

	BOOLEAN CWgtKeyboard::contains(Object *pTarget)
	{
		return TRUE;
	}
	
	void CWgtKeyboard::registerKeyTable(CKeyTable* pKeyTable)
	{
		m_pKeyTable = pKeyTable;
	}

	void CWgtKeyboard::addKey(const tagKey* pKeyTable, UINT uiKeyTableLength)
	{
		if (uiKeyTableLength == 0)
		{
			return;
		}

		const tagKey		*pKeySource = NULL;
		CWgtKeyboard_Key	*pKey = NULL;
	
		for (UINT i = 0; i < uiKeyTableLength; i++)
		{
			pKeySource = pKeyTable + i;

			pKey = NULL;
			pKey = new CWgtKeyboard_Key();
			
			if (pKey != NULL)
			{
				pKey->strKey = pKeySource->strKey;

				for (UINT j = 0; j < pKeySource->uiLen;  j++)
				{
					pKey->aryKeyType.addItem( pKeySource->auiKeyType[j] );
					pKey->aryKeyCode.addItem( pKeySource->auiKeyCode[j] );
				}

				pKey->btnKey.setName(pKey->strKey);
				pKey->btnKey.setVisible(FALSE);
				pKey->btnKey.addEventListener(this);
				addComponent(&(pKey->btnKey));

				pKey->iTouchTime = 0;
				pKey->iCurIndex = 0;

				m_mapKey.addItem(pKey->strKey, pKey);
			}			
		}
	}

	void CWgtKeyboard::onBtnKeyClick(CComponent *pTarget)
	{
		CWgtKeyboard_Key* pKey = m_mapKey[pTarget->getName()];
		
		if (pKey->aryKeyCode.size() > 1)
		{
			if (CTime::getTickCount() - pKey->iTouchTime < ENV::getContinueTapInterval())
			{
				pKey->iCurIndex++;
				if ( pKey->iCurIndex >= (INT)(pKey->aryKeyCode.size()) )
				{
					pKey->iCurIndex = 0;
				}

				pKey->iTouchTime = CTime::getTickCount();

				dispatchKeyboardEvent(CEventKeyboard::KEY_CONTINUE_CLICKED, pKey->aryKeyType[pKey->iCurIndex], pKey->aryKeyCode[pKey->iCurIndex]);

			}
			else
			{
				pKey->iCurIndex  = 0;
				pKey->iTouchTime = CTime::getTickCount();


				dispatchKeyboardEvent(CEventKeyboard::KEY_CLICKED, pKey->aryKeyType[0], pKey->aryKeyCode[0]);
			}

		}
		else
		{
			dispatchKeyboardEvent(CEventKeyboard::KEY_CLICKED, pKey->aryKeyType[0], pKey->aryKeyCode[0]);
		}
	}

	void CWgtKeyboard::onBtnKeyLongClick(CComponent *pTarget)
	{
		CWgtKeyboard_Key* pKey = m_mapKey[ pTarget->getName() ];

		dispatchKeyboardEvent(CEventKeyboard::KEY_LONG_CLICKED, pKey->aryKeyType[0], pKey->aryKeyCode[0]);
	}

	void CWgtKeyboard::dispatchKeyboardEvent(UINT uiEventID, UINT uiKeyType, UINT uiKeyCode)
	{
		if (m_pKeyTable == NULL)
		{
			return;
		}

		m_pKeyTable->setKeyType(uiKeyType);
		m_pKeyTable->setKeyCode(uiKeyCode);

		dispatchEvent(uiEventID);
	}

}