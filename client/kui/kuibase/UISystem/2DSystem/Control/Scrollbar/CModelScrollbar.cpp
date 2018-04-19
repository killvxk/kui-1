#include "OpenHMI.h"

namespace OpenHMI
{
	CModelScrollbar::CModelScrollbar()
		: m_iMin(0)
		, m_iMax(0)
		, m_iPage(0)
		, m_iPreviousStep(0)
		, m_iNextStep(0)
		, m_iPos(0)
	{
		;
	}

	CModelScrollbar::~CModelScrollbar()
	{
		;
	}

	void CModelScrollbar::setScrollInfo(INT iMin, INT iMax, INT iPos, INT iPage)
	{
		if (iMin > iMax)
		{
			iMax =  iMin;
		}

		m_iMin = iMin;
		m_iMax = iMax;

		INT iRange = m_iMax - m_iMin + 1;

		m_iPage = iPage > iRange ? iRange : (iPage <= 0 ? 1 : iPage);

		if (iPos < m_iMin)
		{
			m_iPos = m_iMin;
		}
		else if (iPos > getMaxPosition())
		{
			m_iPos = getMaxPosition();
		}
		else
		{
			m_iPos = iPos;
		}

		setPreviousStep(m_iPreviousStep);
		setNextStep(m_iNextStep);
	}

	void CModelScrollbar::setRange(INT iMin, INT iMax)
	{
		setScrollInfo(iMin, iMax, m_iPos, m_iPage);
	}

	void CModelScrollbar::setPosition(INT iPos)
	{
		setScrollInfo(m_iMin, m_iMax, iPos, m_iPage);
	}

	void CModelScrollbar::setPreviousStep(INT iValue)
	{
		m_iPreviousStep = iValue > m_iPage ? m_iPage : (iValue <= 0 ? 1 : iValue);
	}

	void CModelScrollbar::setNextStep(INT iValue)
	{
		m_iNextStep = iValue > m_iPage ? m_iPage : (iValue <= 0 ? 1 : iValue);
	}

	INT CModelScrollbar::getMinimum()
	{
		return m_iMin;
	}

	INT CModelScrollbar::getMaximum()
	{
		return m_iMax;
	}

	INT CModelScrollbar::getPage()
	{
		return m_iPage;
	}

	INT CModelScrollbar::getRange()
	{
		return m_iMax - m_iMin + 1;
	}

	INT CModelScrollbar::getPosition()
	{
		return m_iPos;
	}

	INT CModelScrollbar::getMaxPosition()
	{
		return m_iMax - m_iPage + 1;
	}


	FLOAT CModelScrollbar::getPosRatio()
	{
		if (getRange() != 0)
		{
			return FLOAT(m_iPos - m_iMin) / FLOAT(getRange());
		}
		else 
		{
			return 0;
		}
	}

	INT CModelScrollbar::moveStepPrevious()
	{
		m_iPos = m_iPos - m_iPreviousStep;
		if (m_iPos < m_iMin)
		{
			m_iPos = m_iMin;
		}

		return m_iPos;
	}

	INT CModelScrollbar::moveStepNext()
	{
		m_iPos = m_iPos + m_iNextStep;
		if (m_iPos > getMaxPosition()) 
		{
			m_iPos = getMaxPosition();
		}

		return m_iPos;
	}

	INT CModelScrollbar::movePagePrevious()
	{
		m_iPos = m_iPos - m_iPage;
		if (m_iPos < m_iMin)
		{
			m_iPos = m_iMin;
		}

		return m_iPos;
	}

	INT CModelScrollbar::movePageNext()
	{
		m_iPos = m_iPos + m_iPage;
		if (m_iPos > getMaxPosition())
		{
			m_iPos = getMaxPosition();
		}

		return m_iPos;
	}
}