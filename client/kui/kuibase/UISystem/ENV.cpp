#include "OpenHMI.h"

namespace OpenHMI
{
	BOOLEAN					ENV::s_bDirty = FALSE;
	BOOLEAN					ENV::s_bIsLockDirty = FALSE;

	INT						ENV::s_iScreenOffsetX = 0;
	INT						ENV::s_iScreenOffsetY = 0;
	UINT					ENV::s_uiScreenWidth = 800;
	UINT					ENV::s_uiScreenHeight = 480;

	UINT					ENV::s_uiHitOffset = 5;
	UINT					ENV::s_uiWaverDistance = 15;
	UINT					ENV::s_uiMoveWaverDistance = 0;
	UINT					ENV::s_uiLongPressDelay = 1500;
	UINT					ENV::s_uiContinuePressInterval = 100;
	UINT					ENV::s_uiDoubleTapInterval = 300;
	UINT					ENV::s_uiDoubleTapWaverDistance = 15;
	UINT					ENV::s_uiContinueTapInterval = 1000;
	Array2<CCursor>			ENV::s_aryCursor;

	BOOLEAN					ENV::s_bCCPBindTouch = FALSE;
	CContainer				*ENV::s_pTopOverlayer = NULL;
	CContainer				*ENV::s_pTopScreen = NULL;

	IMovieClipGenerator		*ENV::s_pMovieClipGenerator = NULL;

	BOOLEAN					ENV::s_bSoundSwitch = TRUE;
	ISoundGenerator			*ENV::s_pSoundGenerator = NULL;

	IGestureRecognizer		*ENV::s_pGestureRecognizer = NULL;
	Map2<UINT, CComponent*, tagNumCompare<UINT> >	ENV::s_mapGestureGroup;
	UINT					ENV::s_uiGestureGroupID = 0;

	ULONG					ENV::s_ulRenderCounter = 0;

	CRectangle				ENV::s_rcCaption;
	void ENV::setDirty()
	{
		if(!s_bIsLockDirty)
		{
			s_bDirty = TRUE;

			CMain::getInstance()->paintHMI();
		}
	}

	void ENV::addCursor(const CCursor &cursor)
	{
		s_aryCursor.addItem(cursor);
	}

	void ENV::updateCursor(const CCursor &cursor)
	{
		for (UINT i = 0; i < s_aryCursor.size(); i++)
		{
			if (s_aryCursor[i].getID() == cursor.getID())
			{
				s_aryCursor[i] = cursor;
				break;
			}
		}
	}

	void ENV::removeCursor(const CCursor &cursor)
	{
		for (UINT i = 0; i < s_aryCursor.size(); i++)
		{
			if (s_aryCursor[i].getID() == cursor.getID())
			{
				s_aryCursor.deleteItemByIndex(i);
				break;
			}
		}
	}

	void ENV::switchCCPFocus(CComponent *pTouchedComponent)
	{
		if (!s_bCCPBindTouch)
		{
			return;
		}

		switchCCPFocusTo(pTouchedComponent);
	}

	void ENV::switchCCPFocusTo(CComponent *pComponent)
	{
		if (!pComponent->canSetFocus())
		{
			if (pComponent->getTargetComponent() != NULL)
			{
				switchCCPFocusTo(pComponent->getTargetComponent());
			}
			return;
		}

		if (pComponent->hasFocus())
		{
			return;
		}

		if (s_pTopOverlayer != NULL && s_pTopOverlayer->containsComponent(pComponent))
		{
			s_pTopOverlayer->releaseFocus();
			pComponent->setFocus();
			return;
		}

		if (s_pTopScreen != NULL && s_pTopScreen->containsComponent(pComponent))
		{
			s_pTopScreen->releaseFocus();
			pComponent->setFocus();
			return;
		}
	}

	CMovieClip*	ENV::getMovieClip(UINT uiImageID)
	{
		if (s_pMovieClipGenerator != NULL)
		{
			return s_pMovieClipGenerator->createMovieClip(uiImageID);
		}

		return NULL;
	}

	void ENV::playSoundForButtonClicked(CComponent *pComponent)
	{
		if (   s_bSoundSwitch == FALSE
			|| s_pSoundGenerator == NULL
			|| pComponent == NULL
			|| pComponent->getSoundSwitch() == FALSE   )
		{
			return;
		}

		s_pSoundGenerator->playButtonClickSound(pComponent->getSoundType());
	}

	UINT ENV::getGestureGroupID(CComponent* pComponent,BOOLEAN bReleasePrevious )
	{
		if ( pComponent == NULL )
		{
			return 0;
		}
		
		UINT uiID = s_mapGestureGroup.size();

		if ( uiID == 0 )
		{
			s_uiGestureGroupID = s_uiGestureGroupID + 1;
			s_mapGestureGroup.addItem(s_uiGestureGroupID, pComponent);
			return s_uiGestureGroupID;
		}

		for (  ; uiID > 1 ; uiID-- )
		{
			if ( pComponent == s_mapGestureGroup.getValueByIndex( uiID - 1 ) )
			{
				break;
			}
		}

		if ( bReleasePrevious == TRUE )
		{
			for ( UINT ui = s_mapGestureGroup.size() ; ui > 1 ; ui-- )
			{
				if ( pComponent == s_mapGestureGroup.getValueByIndex( ui - 1 ) )
				{
					s_mapGestureGroup.deleteItem( s_mapGestureGroup.getKeyByIndex( ui - 1 ));
				}
			}

			s_uiGestureGroupID = s_uiGestureGroupID + 1;
			s_mapGestureGroup.addItem(s_uiGestureGroupID, pComponent);
			return s_uiGestureGroupID;
		}
		else
		{
			return s_mapGestureGroup.getKeyByIndex( uiID - 1);
		}
	}
}