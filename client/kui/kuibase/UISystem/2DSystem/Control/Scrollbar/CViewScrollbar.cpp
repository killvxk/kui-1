#include "OpenHMI.h"

namespace OpenHMI
{
	CViewScrollbar::CViewScrollbar()
		: m_uiUpArrowImageID(0)
		, m_uiDownArrowImageID(0)
		, m_uiBarImageID(0)
		, m_uiSliderImageID(0)
	{
		;
	}

	CViewScrollbar::~CViewScrollbar()
	{
		;
	}

	void CViewScrollbar::setSkinUpArrow(UINT uiImageID)
	{
		m_uiUpArrowImageID = uiImageID;
	}

	void CViewScrollbar::setSkinDownArrow(UINT uiImageID)
	{
		m_uiDownArrowImageID = uiImageID;
	}

	void CViewScrollbar::setSkinBar(UINT uiImageID)
	{
		m_uiBarImageID = uiImageID;
	}

	void CViewScrollbar::setSkinSlider(UINT uiImageID)
	{
		m_uiSliderImageID = uiImageID;
	}

	CMovieClip* CViewScrollbar::getSkinUpArrow()
	{
		return ENV::getMovieClip(m_uiUpArrowImageID);
	}

	CMovieClip* CViewScrollbar::getSkinDownArrow()
	{
		return ENV::getMovieClip(m_uiDownArrowImageID);
	}

	CMovieClip* CViewScrollbar::getSkinBar()
	{
		return ENV::getMovieClip(m_uiBarImageID);
	}

	CMovieClip* CViewScrollbar::getSkinSlider()
	{
		return ENV::getMovieClip(m_uiSliderImageID);
	}

	void CViewScrollbar::drawSkinUpArrow(CMovieClip * pMovieClip, BOOLEAN bFocus)
	{
		drawSkin(pMovieClip, bFocus);
	}

	void CViewScrollbar::drawSkinDownArrow(CMovieClip * pMovieClip, BOOLEAN bFocus)
	{
		drawSkin(pMovieClip, bFocus);
	}

	void CViewScrollbar::drawSkinBar(CMovieClip * pMovieClip, BOOLEAN bFocus)
	{
		drawSkin(pMovieClip, bFocus);
	}

	void CViewScrollbar::drawSkinSlider(CMovieClip * pMoiveClip, BOOLEAN bFocus)
	{
		drawSkin(pMoiveClip, bFocus);
	}

	// private
	void CViewScrollbar::drawSkin(CMovieClip * pMoiveClip, BOOLEAN bFocus)
	{
		if (pMoiveClip == NULL) 
		{
			return;
		}

		if (bFocus)
		{
			pMoiveClip->gotoAndStop(2);
		}
		else 
		{
			pMoiveClip->gotoAndStop(1);
		}
	}
}