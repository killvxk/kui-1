#include "OpenHMI.h"

namespace OpenHMI
{
	CViewProgressbar::CViewProgressbar()
		: m_uiRegularBarImageID(0)
		, m_uiRegularSliderImageID(0)
		, m_uiIrregularBarImageID(0)
		, m_uiIrregularSliderImageID(0)
	{
		;
	}

	CViewProgressbar::~CViewProgressbar()
	{
		;
	}

	void CViewProgressbar::setSkinRegularBar(UINT uiImageID)
	{
		m_uiRegularBarImageID = uiImageID;
	}

	void CViewProgressbar::setSkinRegularSlider(UINT uiImageID)
	{
		m_uiRegularSliderImageID = uiImageID;
	}

	void CViewProgressbar::setSkinIrregularBar(UINT uiImageID)
	{
		m_uiIrregularBarImageID = uiImageID;
	}

	void CViewProgressbar::setSkinIrregularSlider(UINT uiImageID)
	{
		m_uiIrregularSliderImageID = uiImageID;
	}	

	CMovieClip*	CViewProgressbar::getSkinRegularBar()
	{
		return ENV::getMovieClip(m_uiRegularBarImageID);
	}

	CMovieClip*	CViewProgressbar::getSkinRegularSlider()
	{
		return ENV::getMovieClip(m_uiRegularSliderImageID);
	}

	CMovieClip*	CViewProgressbar::getSkinIrregularBar()
	{
		return ENV::getMovieClip(m_uiIrregularBarImageID);
	}

	CMovieClip*	CViewProgressbar::getSkinIrregularSlider()
	{
		return ENV::getMovieClip(m_uiIrregularSliderImageID);
	}

	void CViewProgressbar::drawSkinSlider(CMovieClip* pMovieClip, BOOLEAN bFocus)
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

	void CViewProgressbar::drawSkinIrregularBar(CMovieClip* pMovieClip, FLOAT fPosRatio)
	{
		if (pMovieClip != NULL)
		{
			INT iFrame = (INT)(pMovieClip->getTotalFrames() * fPosRatio) + 1;
			if (iFrame > (INT)pMovieClip->getTotalFrames())
			{
				iFrame = pMovieClip->getTotalFrames();
			}

			pMovieClip->gotoAndStop(iFrame);
		}
	}
}