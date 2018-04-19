#include "OpenHMI.h"

namespace OpenHMI
{
	CModelProgressbar::CModelProgressbar()
		: m_iMin(0)
		, m_iMax(0)
		, m_iPos(0)
		, m_iStep(0)
	{
		;
	}

	CModelProgressbar::~CModelProgressbar()
	{
		;
	}

	void CModelProgressbar::setProgressInfo(INT iMin, INT iMax, INT iPos, INT iStep)
	{
		if (iMin > iMax)
		{
			iMax =  iMin;
		}

		m_iMin = iMin;
		m_iMax = iMax;
		
		if (iPos < m_iMin)
		{
			m_iPos = m_iMin;
		}
		else if (iPos > m_iMax)
		{
			m_iPos = m_iMax;
		}
		else
		{
			m_iPos = iPos;
		}
		
		m_iStep = iStep;
	}

	void CModelProgressbar::setPos(INT iPos)
	{
		setProgressInfo(m_iMin, m_iMax, iPos, m_iStep);
	}

	FLOAT CModelProgressbar::getPosRatio() const
	{
		if (m_iMax - m_iMin != 0)
		{
			return FLOAT(m_iPos - m_iMin) / FLOAT(m_iMax - m_iMin);
		}
		else 
		{
			return 0.0f;
		}
	}
}