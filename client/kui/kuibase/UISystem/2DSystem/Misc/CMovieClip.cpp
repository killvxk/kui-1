#include "OpenHMI.h"

namespace OpenHMI
{
	CMovieClip::CMovieClip() 
		: m_uiCurrent(0)
		, m_iX(0)
		, m_iY(0)
		, m_uiScaleWidth(0)
		, m_uiScaleHeight(0)
		, m_bVisible(TRUE)
		, m_eType(TYPE_FILE)
	{
		m_timer.setListener(this);
	}

	UINT CMovieClip::getWidth()
	{
		return getCurrentImageSize().getX();
	}	

	UINT CMovieClip::getHeight()
	{
		return getCurrentImageSize().getY();
	}

	BOOLEAN CMovieClip::hitTest(INT iX, INT iY)
	{
		INT iWidth = 0;
		INT iHeight = 0;

		if (m_uiScaleWidth > 0)
		{
			iWidth = (INT)m_uiScaleWidth;
		}
		else 
		{
			iWidth = (INT)getWidth();
		}

		if (m_uiScaleHeight > 0)
		{
			iHeight = (INT)m_uiScaleHeight;
		}
		else
		{
			iHeight = (INT)getHeight();
		}

		CPoint ptLeftTop(m_iX, m_iY);

		ptLeftTop.offset(-(INT)ENV::getHitOffset(), -(INT)ENV::getHitOffset());

		if (   ptLeftTop.getX() <= iX && iX <= ptLeftTop.getX() + iWidth + (INT)ENV::getHitOffset() * 2
			&& ptLeftTop.getY() <= iY && iY <= ptLeftTop.getY() + iHeight + (INT)ENV::getHitOffset() * 2   )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	BOOLEAN CMovieClip::isEmpty()
	{
		if (getWidth() == 0 || getHeight() == 0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	CMovieClip::~CMovieClip()
	{
		if (m_timer.isRunning())
		{
			m_timer.stop();
		}

		m_timer.setListener(NULL);
		m_aryImage.clear();
		m_aryImageBitmap.clear();
	}

	void CMovieClip::addImage(const String &strFileName)
	{
		m_aryImage.addItem(strFileName);
		m_arySize.addItem(CSize());

		m_uiCurrent = 1;
		m_eType = TYPE_FILE;
	}

	void CMovieClip::addImage(const tagBitmapInfo& bitmap)
	{
		m_aryImageBitmap.addItem(bitmap);
		m_arySize.addItem(CSize());

		m_uiCurrent = 1;
		m_eType = TYPE_DATA;
	}

	const String& CMovieClip::getCurrentImage()
	{
		if (m_aryImage.isEmpty())
		{
			return STRING_NULL;
		}
		else
		{
			return m_aryImage[m_uiCurrent - 1];
		}
	}

	BOOLEAN CMovieClip::getBitmap(String& strID, Gdiplus::Bitmap** pBitmap)
	{
		if (m_aryImageBitmap.isEmpty())
		{
			return FALSE;
		}

		strID = m_aryImageBitmap[m_uiCurrent - 1].strID;
		*pBitmap = m_aryImageBitmap[m_uiCurrent - 1].pImage;

		if (strID.length() <= 0)
			return FALSE;
		return TRUE;
	}

	UINT CMovieClip::getTotalFrames() const
	{
		if (m_eType == TYPE_FILE)
		{
			return m_aryImage.size();
		}
		else
			return m_aryImageBitmap.size();
	}

	void CMovieClip::play(UINT uiIntervalTime)
	{
		m_timer.setDelay(uiIntervalTime);
		m_timer.start();
	}

	void CMovieClip::stop()
	{
		m_timer.stop();
	}

	void CMovieClip::gotoAndPlay(UINT uiIndex, UINT uiIntervalTime)
	{
		if (m_eType == TYPE_FILE)
		{
			if (m_aryImage.isEmpty())
			{
				return;
			}

			if (uiIndex > m_aryImage.size())
			{
				m_uiCurrent = m_aryImage.size();
			}
			else if (uiIndex == 0)
			{
				m_uiCurrent = 1;
			}
			else
			{
				m_uiCurrent = uiIndex;
			}
		}
		else
		{
			if (m_aryImageBitmap.isEmpty())
			{
				return;
			}

			if (uiIndex > m_aryImageBitmap.size())
			{
				m_uiCurrent = m_aryImageBitmap.size();
			}
			else if (uiIndex == 0)
			{
				m_uiCurrent = 1;
			}
			else
			{
				m_uiCurrent = uiIndex;
			}
		}
		play(uiIntervalTime);
	}

	void CMovieClip::gotoAndStop(UINT uiIndex)
	{
		if (m_eType == TYPE_FILE)
		{
			if (m_aryImage.isEmpty())
			{
				return;
			}

			if (uiIndex > m_aryImage.size())
			{
				m_uiCurrent = m_aryImage.size();
			}
			else if (uiIndex == 0)
			{
				m_uiCurrent = 1;
			}
			else
			{
				m_uiCurrent = uiIndex;
			}
		}
		else
		{
			if (m_aryImageBitmap.isEmpty())
			{
				return;
			}

			if (uiIndex > m_aryImageBitmap.size())
			{
				m_uiCurrent = m_aryImageBitmap.size();
			}
			else if (uiIndex == 0)
			{
				m_uiCurrent = 1;
			}
			else
			{
				m_uiCurrent = uiIndex;
			}
		}
	}

	void CMovieClip::nextFrame()
	{
		if (m_eType == TYPE_FILE)
		{
			if (m_aryImage.isEmpty())
			{
				return;
			}

			m_uiCurrent++;

			if (m_uiCurrent > m_aryImage.size())
			{
				m_uiCurrent = 1;
			}
		}
		else
		{
			if (m_aryImageBitmap.isEmpty())
			{
				return;
			}

			m_uiCurrent++;

			if (m_uiCurrent > m_aryImageBitmap.size())
			{
				m_uiCurrent = 1;
			}
		}
	}

	void CMovieClip::prevFrame()
	{
		if (m_eType == TYPE_FILE)
		{
			if (m_aryImage.isEmpty())
			{
				return;
			}

			m_uiCurrent--;

			if (m_uiCurrent == 0)
			{
				m_uiCurrent = m_aryImage.size();
			}
		}
		else
		{
			if (m_aryImageBitmap.isEmpty())
			{
				return;
			}

			m_uiCurrent--;

			if (m_uiCurrent == 0)
			{
				m_uiCurrent = m_aryImageBitmap.size();
			}
		}
	}

	void CMovieClip::onTimer(const CTimer *pTimer, UINT uiCurrentCount)
	{
		nextFrame();

		ENV::setDirty();
	}

	void CMovieClip::onTimerComplete(const CTimer *pTimer)
	{
		;
	}

	CSize CMovieClip::getCurrentImageSize()
	{
		CSize sz(0, 0);

		if (!m_aryImage.isEmpty() || !m_aryImageBitmap.isEmpty())
		{
			sz = m_arySize[m_uiCurrent - 1];

			if (sz.getX() == 0 || sz.getY() == 0)
			{
				UINT uiWidth = 0;
				UINT uiHeight = 0;

				if(m_eType == TYPE_FILE)
				{
					CRenderEngine::getInstance()->getImageInfo(m_aryImage[m_uiCurrent - 1], &uiWidth, &uiHeight);
				}
				else
				{
					CRenderEngine::getInstance()->getImageInfo(m_aryImageBitmap[m_uiCurrent - 1].strID, m_aryImageBitmap[m_uiCurrent - 1].pImage, &uiWidth, &uiHeight);
				}

				sz.setX(uiWidth);
				sz.setY(uiHeight);

				m_arySize[m_uiCurrent - 1] = sz;
			}
		}

		return sz;
	}
}