#include "OpenHMI.h"

namespace OpenHMI
{
	CAnimationPlayer::CAnimationPlayer() 
		: m_pAnimation(NULL)
		, m_iFPS(20)
		, m_ePlayStatus(PS_STOPPED)
		, m_lPlayStartTime(0)
		, m_lPauseStartTime(0)
		, m_lPausedDurationTime(0)
		, m_bIsCircle(FALSE)
	{
		m_timer.setListener(this);
	}

	CAnimationPlayer::~CAnimationPlayer()
	{
		m_timer.setListener(NULL);
	}

	void CAnimationPlayer::setFPS(INT iValue)
	{
		m_iFPS = iValue < 10 ? 10 : (iValue > 60 ? 60: iValue);
	}

	void CAnimationPlayer::play(CBaseAnimation* pAnimation, BOOLEAN bIsCircle)
	{
		stop();		

		if ( pAnimation != NULL )
		{
			m_bIsCircle = bIsCircle;

			this->m_pAnimation = pAnimation;
			
			CBaseAnimation* pLastAnimation = pAnimation;
			while (pLastAnimation->getSequentAnimation() != NULL)
			{
				pLastAnimation = pLastAnimation->getSequentAnimation();
			}
			
			pLastAnimation->addListener(this);	

			innerPlay();
		}
	}	

	void CAnimationPlayer::pause()
	{
		if (m_ePlayStatus == PS_PLAYING)
		{
			m_lPauseStartTime = CTime::getTickCount();
			m_ePlayStatus = PS_PAUSED;
		}
	}

	void CAnimationPlayer::resume()
	{
		if (m_ePlayStatus == PS_PAUSED)
		{
			m_lPausedDurationTime += CTime::getTickCount() - m_lPauseStartTime;
			m_ePlayStatus = PS_PLAYING;
		}
	}

	void CAnimationPlayer::stop()
	{
		if (m_ePlayStatus == PS_STOPPED)
		{
			return;
		}

		m_timer.stop();
		
		m_ePlayStatus = PS_STOPPED;
	}	

	void CAnimationPlayer::onTimer(const CTimer *pTimer, UINT uiCurrentCount)
	{
		if (m_ePlayStatus != PS_PLAYING)
		{
			return;
		}
		
		long lCurrentTime = CTime::getTickCount() - m_lPlayStartTime - m_lPausedDurationTime;
		
		m_pAnimation->runAnimation(lCurrentTime);
	}

	void CAnimationPlayer::onTimerComplete(const CTimer *pTimer)
	{
		;
	}

	void CAnimationPlayer::onAnimationEnded(CBaseAnimation* pSource)
	{
		if( !m_bIsCircle )
		{
			stop();
		}
		else
		{
			innerPlay();
		}
	}

	void CAnimationPlayer::innerPlay()
	{
		m_pAnimation->clearControlStatus();

		m_lPausedDurationTime = 0;
		m_lPlayStartTime = CTime::getTickCount();
		m_ePlayStatus = PS_PLAYING;

		m_timer.stop();
		m_timer.setDelay( 1000 / m_iFPS );
		m_timer.setRepeatCount(0);
		m_timer.start();
	}
}