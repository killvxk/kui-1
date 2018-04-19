#include "OpenHMI.h"

namespace OpenHMI
{
	CKeyFrame::CKeyFrame(LONG lTime, CBaseAmtData* pAmtData)
		: m_lTime(lTime)
		, m_pAmtValue(pAmtData)
	{
		;
	}

	CKeyFrame::~CKeyFrame()
	{
		m_pAmtValue = NULL;
	}

	LONG CKeyFrame::getTime()
	{
		return m_lTime;
	}
	
	CBaseAmtData* CKeyFrame::getValue()
	{
		return m_pAmtValue;
	}
}