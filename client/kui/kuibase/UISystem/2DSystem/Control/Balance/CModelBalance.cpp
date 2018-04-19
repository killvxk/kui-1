#include "OpenHMI.h"

namespace OpenHMI
{
	CModelBalance::CModelBalance()
		: m_iXMin(0)
		, m_iXMax(0)
		, m_iXPos(0)
		, m_iXStep(0)
		, m_iYMin(0)		
		, m_iYMax(0)		
		, m_iYPos(0)		
		, m_iYStep(0)
	{
		;
	}

	CModelBalance::~CModelBalance()
	{
		;
	}

	void CModelBalance::setXProgressInfo(INT iMin, INT iMax, INT iPos, INT iStep)
	{
		if (iMin > iMax)
		{
			iMax =  iMin;
		}

		m_iXMin = iMin;
		m_iXMax = iMax;

		if (iPos < m_iXMin)
		{
			m_iXPos = m_iXMin;
		}
		else if (iPos > m_iXMax)
		{
			m_iXPos = m_iXMax;
		}
		else
		{
			m_iXPos = iPos;
		}

		m_iXStep = iStep;

	}

	void CModelBalance::setXPos(INT iValue)
	{
		setXProgressInfo(m_iXMin, m_iXMax, iValue, m_iXStep);
	}

	void CModelBalance::setYProgressInfo(INT iMin, INT iMax, INT iPos, INT iStep)
	{
		if (iMin > iMax)
		{
			iMax =  iMin;
		}

		m_iYMin = iMin;
		m_iYMax = iMax;

		if (iPos < m_iYMin)
		{
			m_iYPos = m_iYMin;
		}
		else if (iPos > m_iYMax)
		{
			m_iYPos = m_iYMax;
		}
		else
		{
			m_iYPos = iPos;
		}

		m_iYStep = iStep;
	}
	

	void CModelBalance::setYPos(INT iValue)
	{
		setYProgressInfo(m_iYMin, m_iYMax, iValue, m_iYStep);
	}

	INT CModelBalance::getXRange()
	{
		return m_iXMax - m_iXMin;
	}

	FLOAT CModelBalance::getXPosRatio()
	{
		if (m_iXMax - m_iXMin != 0)
		{
			return FLOAT(m_iXPos - m_iXMin) / FLOAT(getXRange());
		}
		else 
		{
			return 0;
		}
	}

	INT CModelBalance:: getYRange()
	{
		return m_iYMax - m_iYMin;
	}

	FLOAT CModelBalance::getYPosRatio()
	{
		if (m_iYMax - m_iYMin != 0)
		{
			return FLOAT(m_iYPos - m_iYMin) / FLOAT(getYRange());
		}
		else 
		{
			return 0;
		}
	}
}