#include "OpenHMI.h"

namespace OpenHMI
{
	CCtrlImageField::CCtrlImageField()
		: m_uiShowImage(0)
		, m_uiIntervalTime(0)
	{
		setBaseView(&m_view);
		setCanHitTest(FALSE);
		notAcceptFocus();
	}

	CCtrlImageField::~CCtrlImageField()
	{
		;
	}

	void CCtrlImageField::setUserMovieClip(CMovieClip* pMovieClip)
	{
		if( m_userSkin != pMovieClip )
		{
			m_userSkin = NULL;
			m_userSkin = pMovieClip;
			m_skin = m_userSkin;

			setDirty();
		}
	}

	UINT CCtrlImageField::getSrcImageWidth()
	{
		return m_skin.isNotNull() ? m_skin->getWidth() : 0;
	}

	UINT CCtrlImageField::getSrcImageHeight()
	{
		return m_skin.isNotNull() ? m_skin->getHeight() : 0;
	}

	void CCtrlImageField::adjustPreferredSizeToSrcImageSize()
	{
		if (m_skin.isNotNull() && !m_skin->isEmpty())
		{
			setPreferredSize(m_skin->getWidth(), m_skin->getHeight());
		}
	}

	void CCtrlImageField::showImage(UINT uiIndex)
	{
		if (m_skin.isNotNull())
		{
			m_skin->gotoAndStop(uiIndex);

			setDirty();
		}
		else
		{
			m_uiShowImage = uiIndex;
		}
	}

	void CCtrlImageField::play(UINT uiIntervalTime)
	{
		if (m_skin.isNotNull() && m_skin->getTotalFrames() > 1)
		{
			m_skin->play(uiIntervalTime);
		}
		else
		{
			m_uiIntervalTime =  uiIntervalTime;
		}
	}

	void CCtrlImageField::stop()
	{
		if (m_skin.isNotNull())
		{
			m_skin->stop();
		}

		m_uiShowImage = 0;
		m_uiIntervalTime = 0;
	}

	void CCtrlImageField::setLayout(CRenderImage::ELayout eValue)
	{
		if (m_renderImage.getLayout() != eValue)
		{
			m_renderImage.setLayout(eValue);
			setDirty();
		}
	}

	void CCtrlImageField::setNormalLayoutClipOffsetX(INT iValue)
	{
		if (m_renderImage.getNormalLayoutClipOffsetX() != iValue)
		{
			m_renderImage.setNormalLayoutClipOffsetX(iValue);
			setDirty();
		}
	}

	void CCtrlImageField::setNormalLayoutClipOffsetY(INT iValue)
	{
		if (m_renderImage.getNormalLayoutClipOffsetY() != iValue)
		{
			m_renderImage.setNormalLayoutClipOffsetY(iValue);
			setDirty();
		}
	}

	void CCtrlImageField::refresh()
	{
		CComponent::refresh();

		if( m_userSkin.isNotNull() )
		{
			m_skin = m_userSkin;
		}

		if (m_skin.isNotNull())
		{
			if (m_uiShowImage != 0)
			{
				m_skin->gotoAndStop(m_uiShowImage);
			}

			if (m_uiIntervalTime != 0)
			{
				m_skin->play(m_uiIntervalTime);
			}
		}
	}
}