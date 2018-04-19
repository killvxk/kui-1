#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtLong::CAmtLong(LONG lValue) 
		: m_lValue(lValue)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtLong::~CAmtLong()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtLong::add(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_lValue = m_lValue + ((CAmtLong*)pData)->m_lValue;
		}
		else
		{
			m_pCloneData->m_lValue = m_lValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtLong::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_lValue = m_lValue - ((CAmtLong*)pData)->m_lValue;
		}
		else
		{
			m_pCloneData->m_lValue = m_lValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtLong::multiply(FLOAT fValue)
	{
		genCloneData();
		
		m_pCloneData->m_lValue = LONG(m_lValue * fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtLong::divide(FLOAT fValue)
	{
		genCloneData();
		
		m_pCloneData->m_lValue = LONG(m_lValue / fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtLong::cloneData()
	{
		genCloneData();

		m_pCloneData->m_lValue = m_lValue;

		return m_pCloneData;
	}

	void CAmtLong::set(CBaseAmtData* pData)
	{
		if( pData != NULL )
		{
			m_lValue = ((CAmtLong*)(pData))->m_lValue;
		}
	}

	void CAmtLong::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtLong(m_lValue);;
		}
	}
}