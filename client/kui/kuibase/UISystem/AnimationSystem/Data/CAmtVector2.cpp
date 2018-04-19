#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtVector2::CAmtVector2(FLOAT fX, FLOAT fY) 
		: m_fX(fX)
		, m_fY(fY)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtVector2::~CAmtVector2()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtVector2::add(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_fX = m_fX + ((CAmtVector2*)pData)->m_fX;
			m_pCloneData->m_fY = m_fY + ((CAmtVector2*)pData)->m_fY;
		}
		else
		{
			m_pCloneData->m_fX = m_fX;
			m_pCloneData->m_fY = m_fY;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtVector2::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_fX = FLOAT(m_fX - ((CAmtVector2*)pData)->m_fX);
			m_pCloneData->m_fY = FLOAT(m_fY - ((CAmtVector2*)pData)->m_fY);
		}
		else
		{
			m_pCloneData->m_fX = m_fX;
			m_pCloneData->m_fY = m_fY;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtVector2::multiply(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_fX = FLOAT(m_fX * fValue);
		m_pCloneData->m_fY = FLOAT(m_fY * fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtVector2::divide(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_fX = FLOAT(m_fX / fValue);
		m_pCloneData->m_fY = FLOAT(m_fY / fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtVector2::cloneData()
	{
		genCloneData();

		m_pCloneData->m_fX = m_fX;
		m_pCloneData->m_fY = m_fY;

		return m_pCloneData;
	}

	void CAmtVector2::set(CBaseAmtData* pData)
	{
		if( pData != NULL )
		{
			m_fX = ((CAmtVector2*)pData)->m_fX;
			m_fY = ((CAmtVector2*)pData)->m_fY;
		}
	}
	
	void CAmtVector2::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtVector2(m_fX, m_fY);
		}
	}
}