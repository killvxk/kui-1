#include "OpenHMI.h"

namespace OpenHMI
{
	CBasePath::CBasePath() 
		: m_fLength(0.0f)
		, m_pAmtPoint(NULL)
	{
		;
	}

	CBasePath::~CBasePath()
	{
		if ( m_pAmtPoint != NULL )
		{
			delete m_pAmtPoint;
		}
	}

	FLOAT CBasePath::getLength()
	{
		return m_fLength;
	}

	CAmtPoint* CBasePath::getPointByPercent(FLOAT fPercent)
	{
		if (m_aryX.size() == 0)
		{
			return NULL;
		}
		else if (m_aryX.size() == 1)
		{
			if( m_pAmtPoint == NULL )
			{
				m_pAmtPoint = new CAmtPoint(m_aryX[0], m_aryY[0]);
			}
			m_pAmtPoint->m_iX = m_aryX[0];
			m_pAmtPoint->m_iY = m_aryY[0];

			return m_pAmtPoint;
		}

		if (fPercent > 1.0)
		{
			fPercent -= (INT)fPercent;
		}

		if (fPercent < 0.0)
		{
			fPercent += (INT)(-fPercent) + 1;
		}

		if (fPercent <= 0.0)
		{
			// return begin point
			if( m_pAmtPoint == NULL )
			{
				m_pAmtPoint = new CAmtPoint(m_aryX[0], m_aryY[0]);
			}
			m_pAmtPoint->m_iX = m_aryX[0];
			m_pAmtPoint->m_iY = m_aryY[0];

			return m_pAmtPoint;
		}
		else if (fPercent >= 1.0)
		{
			// return end point
			if( m_pAmtPoint == NULL )
			{
				m_pAmtPoint = new CAmtPoint(m_aryX[m_aryX.size()-1], m_aryY[m_aryY.size()-1]);
			}
			m_pAmtPoint->m_iX = m_aryX[m_aryX.size()-1];
			m_pAmtPoint->m_iY = m_aryY[m_aryY.size()-1];
			return m_pAmtPoint;
		}

		FLOAT fCurPos = m_fLength * fPercent;		
		UINT	  uiPos = 0;

		{// get the edge index of the point according to the percent
			for ( ; uiPos < m_aryEdge.size(); uiPos++)
			{
				if (fCurPos < m_aryEdge[uiPos])
				{
					break;
				}

				fCurPos -= m_aryEdge[uiPos];
			}

			if (uiPos == m_aryEdge.size())
			{
				if( m_pAmtPoint == NULL )
				{
					m_pAmtPoint = new CAmtPoint(m_aryX[m_aryX.size()-1], m_aryY[m_aryY.size()-1]);
				}
				m_pAmtPoint->m_iX = m_aryX[m_aryX.size()-1];
				m_pAmtPoint->m_iY = m_aryY[m_aryY.size()-1];
				return m_pAmtPoint;
			}
		}

		CAmtVector2* pAmtv1 = new CAmtVector2((FLOAT)m_aryX[uiPos], (FLOAT)m_aryY[uiPos]);
		CAmtVector2* pAmtv2 = new CAmtVector2((FLOAT)m_aryX[uiPos + 1], (FLOAT)m_aryY[uiPos + 1]);
		
		CAmtVector2* pAmtvPoint = (CAmtVector2*)pAmtv1->add(pAmtv2->subtract(pAmtv1)->multiply(fCurPos / m_aryEdge[uiPos]));
		
		if( m_pAmtPoint == NULL )
		{
			m_pAmtPoint = new CAmtPoint((INT)pAmtvPoint->m_fX, (INT)pAmtvPoint->m_fY);
		}
		m_pAmtPoint->m_iX = (INT)pAmtvPoint->m_fX;
		m_pAmtPoint->m_iY = (INT)pAmtvPoint->m_fY;

		delete pAmtv1;
		delete pAmtv2;

		return m_pAmtPoint;
	}

	void CBasePath::calculateEdgeLength()
	{
		m_aryEdge.clear();
		m_fLength = 0;				
		
		if (m_aryX.size() < 2)
		{
			return;
		}

		FLOAT fEdgeLength;
		for (UINT ui = 1; ui < m_aryX.size(); ui++)
		{
			fEdgeLength = (FLOAT)Math::sqrt(  ((FLOAT)m_aryX[ui] - m_aryX[ui - 1]) * (m_aryX[ui] - m_aryX[ui - 1])
													 + ((FLOAT)m_aryY[ui] - m_aryY[ui - 1]) * (m_aryY[ui] - m_aryY[ui - 1]));
			m_aryEdge.addItem(fEdgeLength);
			m_fLength += fEdgeLength;
		}
	}
}