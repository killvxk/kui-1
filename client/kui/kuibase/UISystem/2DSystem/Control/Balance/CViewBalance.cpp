#include "OpenHMI.h"

namespace OpenHMI
{
	CViewBalance::CViewBalance()
		: m_uiVLineImageID(0)
		, m_uiHLineImageID(0)
		, m_uiSliderImageID(0)
	{
		;
	}

	CViewBalance::~CViewBalance()
	{
		;
	}	

	void CViewBalance::setSkinVerticalLine(UINT uiImageID)
	{
		m_uiVLineImageID = uiImageID;		
	}

	void CViewBalance::setSkinHorizontalLine(UINT uiImageID)
	{
		m_uiHLineImageID = uiImageID;
	}

	void CViewBalance::setSkinSlider(UINT uiImageID)
	{
		m_uiSliderImageID = uiImageID;
	}

	CMovieClip* CViewBalance::getSkinVerticalLine()
	{
		return ENV::getMovieClip(m_uiVLineImageID);
	}

	CMovieClip* CViewBalance::getSkinHorizontalLine()
	{
		return ENV::getMovieClip(m_uiHLineImageID);
	}

	CMovieClip* CViewBalance::getSkinSlider()
	{
		return ENV::getMovieClip(m_uiSliderImageID);
	}

	void CViewBalance::drawSkin(CMovieClip *pMovieClip, BOOLEAN bFocus)
	{
		if (pMovieClip != NULL) 
		{
			if (bFocus == TRUE) 
			{
				pMovieClip->gotoAndStop(2);
			}
			else 
			{
				pMovieClip->gotoAndStop(1);
			}
		}
	}
}