#include "OpenHMI.h"

namespace OpenHMI
{
	CRenderBaseFilter::CRenderBaseFilter()
		: m_eType(FT_NONE)	
		, m_pAssociateComponent(NULL)
		, m_bDynamic(FALSE)
	{
		m_timer.setListener(this);
	}

	CRenderBaseFilter::~CRenderBaseFilter()
	{
		m_timer.setListener(NULL);
	}

	void CRenderBaseFilter::onTimer(const CTimer *pTimer, UINT uiCurrentCount)
	{
		//if( m_pAssociateComponent )
		//{
		//	m_pAssociateComponent->updateFilter();
		//	//ENV::setDirty();
		//}

		if (m_pAssociateComponent != NULL)
		{
			m_pAssociateComponent->setDirty();
		}
	}

	void CRenderBaseFilter::onTimerComplete(const CTimer *pTimer)
	{
		;
	}

	void CRenderBaseFilter::innerPlay()
	{
		if (m_timer.isRunning())
		{
			return;
		}
		
		m_timer.stop();
		m_timer.setDelay(30);
		m_timer.setRepeatCount(0);
		m_timer.start();
	}

	void CRenderBaseFilter::innerStop()
	{
		m_timer.stop();
	}

////////////////////////////////////////////////////////////////////////////////////////////
	CRenderFltReflection::CRenderFltReflection()
		: m_uiHeight(100)
	{
		m_eType = FT_REFLECTION;
	}

////////////////////////////////////////////////////////////////////////////////////////////
	CRenderFltGlass::CRenderFltGlass()
		: m_uiBlurSize(0)
	{
		m_eType = FT_GLASS;
	}

////////////////////////////////////////////////////////////////////////////////////////////
	CRenderFltRipple::CRenderFltRipple()
		: m_uiTouchRadius(3)
		, m_uiDamping(32)
	{
		m_eType = FT_RIPPLE;
		m_bDynamic = TRUE;
	}

	UINT CRenderFltRipple::getCursorCnt()
	{
		if (m_pAssociateComponent != NULL)
		{
			return m_pAssociateComponent->getCursorCnt();
		}
		else
		{
			return ENV::getCursorCount();
		}
	}
	
	CCursor CRenderFltRipple::getCursorByIndex(UINT uiIndex)
	{
		if (m_pAssociateComponent != NULL)
		{
			return m_pAssociateComponent->getCursorByIndex(uiIndex); 
		}
		else
		{
			return ENV::getCursor(uiIndex);
		}
	}

////////////////////////////////////////////////////////////////////////////////////////////
	CRenderFltMask::CRenderFltMask()
	{
		m_eType = FT_MASK;
	}
}