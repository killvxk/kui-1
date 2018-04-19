#include "OpenHMI.h"

namespace OpenHMI
{
	CViewLabel::CViewLabel()
	{
		;
	}

	CViewLabel::~CViewLabel()
	{
		;
	}

	void CViewLabel::drawSkin(CMovieClip *pMovieClip, BOOLEAN bEnable, BOOLEAN bHover, BOOLEAN bFocus, BOOLEAN bSelected, BOOLEAN bLongFocus)
	{
		if (pMovieClip == NULL)
		{
			return;
		}

		if (pMovieClip->getTotalFrames() >= 5)
		{
			if (!bEnable)
			{
				pMovieClip->setVisible(FALSE);
			}
			else
			{
				if (bHover || bSelected || bFocus || bLongFocus)
				{
					pMovieClip->setVisible(TRUE);
					pMovieClip->play(100);
				}
				else
				{
					pMovieClip->stop();
					pMovieClip->setVisible(FALSE);
				}
			}
		}
		else if (pMovieClip->getTotalFrames() == 4)
		{
			if (!bEnable)
			{
				pMovieClip->gotoAndStop(4);
			}
			else
			{
				if (bFocus || bLongFocus)
				{
					pMovieClip->gotoAndStop(3);
				}
				else if (bHover)
				{
					pMovieClip->gotoAndStop(2);
				}
				else
				{
					pMovieClip->gotoAndStop(1);
				}
			}
		}
		else if (pMovieClip->getTotalFrames() == 3)
		{
			if (!bEnable)
			{
				pMovieClip->setVisible(FALSE);
			}
			else
			{
				if (bSelected || bFocus || bLongFocus)
				{
					pMovieClip->gotoAndStop(3);
				}
				else if (bHover)
				{
					pMovieClip->gotoAndStop(2);
				}
				else
				{
					pMovieClip->gotoAndStop(1);
				}
			}
		}
		else if (pMovieClip->getTotalFrames() == 2)
		{
			if (!bEnable)
			{
				pMovieClip->setVisible(FALSE);
			}
			else
			{
				if (bHover || bSelected || bFocus || bLongFocus)
				{
					pMovieClip->gotoAndStop(2);
				}
				else
				{
					pMovieClip->gotoAndStop(1);
				}
			}
		}
		else
		{
			if (!bEnable)
			{
				;
			}
			else
			{
				if (bHover || bSelected || bFocus || bLongFocus)
				{
					pMovieClip->setVisible(TRUE);
					pMovieClip->gotoAndStop(1);
				}
				else
				{
					pMovieClip->setVisible(FALSE);
				}
			}
		}
	}
}