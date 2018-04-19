#include "OpenHMI.h"

namespace OpenHMI
{
	CPoliLine::CPoliLine()
	{
		;
	}

	CPoliLine::~CPoliLine()
	{
		
	}

	void CPoliLine::addPoint(INT iX, INT iY)
	{
		m_aryPoints.addItem(new CAmtPoint(iX, iY));
	}
	
	void CPoliLine::addPoint(CAmtPoint* pAmtp)
	{
		m_aryPoints.addItem(new CAmtPoint(pAmtp->m_iX, pAmtp->m_iY));
	}
		
	void CPoliLine::smashToSegments(INT iSegmentCount)
	{
		// smash to point array
		
		CAmtPoint* pAmtp;
		for (UINT ui = 0; ui < m_aryPoints.size(); ui++)
		{
			pAmtp = (CAmtPoint*)m_aryPoints[ui];
			m_aryX.addItem(pAmtp->m_iX);
			m_aryY.addItem(pAmtp->m_iY);
		}
		
		calculateEdgeLength();
	}
}