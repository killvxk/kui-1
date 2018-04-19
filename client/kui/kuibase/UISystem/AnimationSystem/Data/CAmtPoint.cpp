#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtPoint::CAmtPoint(INT iX, INT iY)
		: m_iX(iX)
		, m_iY(iY)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtPoint::~CAmtPoint()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtPoint::add(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_iX = m_iX + ((CAmtPoint*)pData)->m_iX;
			m_pCloneData->m_iY = m_iY + ((CAmtPoint*)pData)->m_iY;
		}
		else
		{
			m_pCloneData->m_iX = m_iX;
			m_pCloneData->m_iY = m_iY;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtPoint::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_iX = m_iX - ((CAmtPoint*)pData)->m_iX;
			m_pCloneData->m_iY = m_iY - ((CAmtPoint*)pData)->m_iY;
		}
		else
		{
			m_pCloneData->m_iX = m_iX;
			m_pCloneData->m_iY = m_iY;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtPoint::multiply(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_iX = (INT)(m_iX * fValue);
		m_pCloneData->m_iY = (INT)(m_iY * fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtPoint::divide(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_iX = (INT)(m_iX / fValue);
		m_pCloneData->m_iY = (INT)(m_iY / fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtPoint::cloneData()
	{
		genCloneData();

		m_pCloneData->m_iX = m_iX;
		m_pCloneData->m_iY = m_iY;

		return m_pCloneData;
	}

	void CAmtPoint::set(CBaseAmtData* pData)
	{
		if( pData != NULL )
		{
			m_iX = ((CAmtPoint*)pData)->m_iX;
			m_iY = ((CAmtPoint*)pData)->m_iY;
		}
	}	

	void CAmtPoint::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtPoint(m_iX, m_iY);
		}
	}
}