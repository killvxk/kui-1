#include "OpenHMI.h"

namespace OpenHMI
{
	CHitAreaTriangle::CHitAreaTriangle()
		: m_iX1(0)
		, m_iY1(0)
		, m_iX2(0)
		, m_iY2(0)
		, m_iX3(0)
		, m_iY3(0)
	{
		m_eHTAType = HTA_TRIANGLE;
	}

	CHitAreaTriangle::~CHitAreaTriangle()
	{
		;
	}

	void CHitAreaTriangle::setVertex1(INT iX, INT iY)
	{
		m_iX1 = iX;
		m_iY1 = iY;
	}

	void CHitAreaTriangle::setVertex2(INT iX, INT iY)
	{
		m_iX2 = iX;
		m_iY2 = iY;
	}

	void CHitAreaTriangle::setVertex3(INT iX, INT iY)
	{
		m_iX3 = iX;
		m_iY3 = iY;
	}

	BOOLEAN CHitAreaTriangle::changeVertexOrder()
	{
		// the three point in the same line
		if( (m_iX1 - m_iX2) * (m_iY3 - m_iY2) == (m_iX3 - m_iX2) * (m_iY1 - m_iY2) )
		{
			return FALSE;
		}

		// change the vertex order.if we walk around the triangle clockwise, we will reach the vertex with a order of ptLeft,ptRight,ptMiddle
		CPoint ptLeft,ptRight,ptMiddle;
		
		if( m_iX1 < m_iX2 )
		{
			if( m_iX1 < m_iX3 )
			{
				ptLeft.setX(m_iX1);
				ptLeft.setY(m_iY1);
				if( m_iX2 < m_iX3 )
				{
					ptRight.setX(m_iX3);
					ptRight.setY(m_iY3);
					ptMiddle.setX(m_iX2);
					ptMiddle.setY(m_iY2);
				}
				else if(m_iX2 == m_iX3)
				{
					if( m_iY2 < m_iY3 )
					{
						ptRight.setX(m_iX2);
						ptRight.setY(m_iY2);
						ptMiddle.setX(m_iX3);
						ptMiddle.setY(m_iY3);
					}
					else
					{
						ptRight.setX(m_iX2);
						ptRight.setY(m_iY2);
						ptMiddle.setX(m_iX3);
						ptMiddle.setY(m_iY3);
					}
				}
				else
				{
					ptRight.setX(m_iX3);
					ptRight.setY(m_iY3);
					ptMiddle.setX(m_iX2);
					ptMiddle.setY(m_iY2);
				}
			}
			else if( m_iX1 == m_iX3 )
			{
				if( m_iY1 > m_iY3 )
				{
					ptLeft.setX(m_iX3);
					ptLeft.setY(m_iY3);
					ptRight.setX(m_iX2);
					ptRight.setY(m_iY2);
					ptMiddle.setX(m_iX1);
					ptMiddle.setY(m_iY1);
				}
				else
				{
					ptLeft.setX(m_iX1);
					ptLeft.setY(m_iY1);
					ptRight.setX(m_iX2);
					ptRight.setY(m_iY2);
					ptMiddle.setX(m_iX3);
					ptMiddle.setY(m_iY3);
				}
			}
			else
			{
					ptLeft.setX(m_iX3);
					ptLeft.setY(m_iY3);
					ptRight.setX(m_iX2);
					ptRight.setY(m_iY2);
					ptMiddle.setX(m_iX1);
					ptMiddle.setY(m_iY1);
			}
		}
		else if( m_iX1 == m_iX2 )
		{
			if( m_iX1 < m_iX3 )
			{
				ptRight.setX(m_iX3);
				ptRight.setY(m_iY3);
				if( m_iY1 < m_iY2 )
				{
					ptLeft.setX(m_iX1);
					ptLeft.setY(m_iY1);
					ptMiddle.setX(m_iX2);
					ptMiddle.setY(m_iY2);
				}
				else
				{
					ptLeft.setX(m_iX2);
					ptLeft.setY(m_iY2);
					ptMiddle.setX(m_iX1);
					ptMiddle.setY(m_iY1);
				}
			}
			else
			{
				ptLeft.setX(m_iX3);
				ptLeft.setY(m_iY3);
				if( m_iY1 < m_iY2 )
				{
					ptRight.setX(m_iX1);
					ptRight.setY(m_iY1);
					ptMiddle.setX(m_iX2);
					ptMiddle.setY(m_iY2);
				}
				else
				{
					ptRight.setX(m_iX2);
					ptRight.setY(m_iY2);
					ptMiddle.setX(m_iX1);
					ptMiddle.setY(m_iY1);
				}
			}
		}
		else
		{
			if( m_iX1 > m_iX3 )
			{
				ptRight.setX(m_iX1);
				ptRight.setY(m_iY1);
				if( m_iX2 < m_iX3 )
				{
					ptLeft.setX(m_iX2);
					ptLeft.setY(m_iY2);
					ptMiddle.setX(m_iX3);
					ptMiddle.setY(m_iY3);
				}
				else if( m_iX2 == m_iX3 )
				{
					if( m_iY2 < m_iY3 )
					{
						ptLeft.setX(m_iX2);
						ptLeft.setY(m_iY2);
						ptMiddle.setX(m_iX3);
						ptMiddle.setY(m_iY3);
					}
					else
					{
						ptLeft.setX(m_iX3);
						ptLeft.setY(m_iY3);
						ptMiddle.setX(m_iX2);
						ptMiddle.setY(m_iY2);
					}
				}
				else
				{
					ptLeft.setX(m_iX3);
					ptLeft.setY(m_iY3);
					ptMiddle.setX(m_iX2);
					ptMiddle.setY(m_iY2);
				}
			}
			else if( m_iX1 == m_iX3 )
			{
				ptLeft.setX(m_iX2);
				ptLeft.setY(m_iY2);

				if( m_iY1 < m_iY3 )
				{
					ptRight.setX(m_iX1);
					ptRight.setY(m_iY1);
					ptMiddle.setX(m_iX3);
					ptMiddle.setY(m_iY3);
				}
				else
				{
					ptRight.setX(m_iX3);
					ptRight.setY(m_iY3);
					ptMiddle.setX(m_iX1);
					ptMiddle.setY(m_iY1);
				}
			}
			else
			{
				ptLeft.setX(m_iX2);
				ptLeft.setY(m_iY2);
				ptRight.setX(m_iX3);
				ptRight.setY(m_iY3);
				ptMiddle.setX(m_iX1);
				ptMiddle.setY(m_iY1);
			}
		}

		if( ptRight.getX() == ptLeft.getX() )
		{
			return FALSE;
		}

		if( ptMiddle.getY() > ptLeft.getY() + (ptMiddle.getX() - ptLeft.getX()) / (ptRight.getX() - ptLeft.getX()) * (ptRight.getY() - ptLeft.getY()))
		{
			CPoint temp = ptMiddle;
			ptMiddle.setX(ptRight.getX());
			ptMiddle.setY(ptRight.getY());
			ptRight.setX(temp.getX());
			ptRight.setY(temp.getY());
		}

		m_iX1 = ptLeft.getX();
		m_iY1 = ptLeft.getY();
		m_iX2 = ptRight.getX();
		m_iY2 = ptRight.getY();
		m_iX3 = ptMiddle.getX();
		m_iY3 = ptMiddle.getY();

		return TRUE;
	}

	BOOLEAN CHitAreaTriangle::isSameSide(C3DVector vA, C3DVector vB, C3DVector vC, C3DVector vP)
	{
		C3DVector AB = vB - vA ;
		C3DVector AC = vC - vA ;
		C3DVector AP = vP - vA ;

		C3DVector v1 = AB.crossProduct(AC) ;
		C3DVector v2 = AB.crossProduct(AP) ;

		// v1 and v2 should point to the same direction
		return v1.dotProduct(v2) >= 0 ;
	}

	BOOLEAN CHitAreaTriangle::hitTest(INT iX, INT iY)
	{
		if( !changeVertexOrder() )
		{
			return FALSE;
		}

		C3DVector vA = C3DVector((FLOAT)m_iX1,(FLOAT)m_iY1, 0.0f);
		C3DVector vB = C3DVector((FLOAT)m_iX2,(FLOAT)m_iY2, 0.0f);
		C3DVector vC = C3DVector((FLOAT)m_iX3,(FLOAT)m_iY3, 0.0f);
		C3DVector vP = C3DVector((FLOAT)iX,(FLOAT)iY, 0.0f);

		return  isSameSide(vA, vB, vC, vP) &&
				isSameSide(vB, vC, vA, vP) && 
				isSameSide(vC, vA, vB, vP) ;
	}
}