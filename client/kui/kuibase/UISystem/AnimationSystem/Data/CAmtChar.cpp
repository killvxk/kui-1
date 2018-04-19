#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtChar::CAmtChar(CHAR cValue)
		: m_cValue(cValue)
		, m_pCloneData(NULL)
	{
		m_cValue = cValue;
	}

	CAmtChar::~CAmtChar()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtChar::add(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_cValue = m_cValue + ((CAmtChar*)pData)->m_cValue;
		}
		else
		{
			m_pCloneData->m_cValue = m_cValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtChar::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_cValue = m_cValue - ((CAmtChar*)pData)->m_cValue;
		}
		else
		{
			m_pCloneData->m_cValue = m_cValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtChar::multiply(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_cValue = CHAR(m_cValue * fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtChar::divide(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_cValue = CHAR(m_cValue / fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtChar::cloneData()
	{
		genCloneData();

		m_pCloneData->m_cValue = m_cValue;

		return m_pCloneData;
	}

	void CAmtChar::set(CBaseAmtData* pData)
	{
		if( pData != NULL )
		{
			m_cValue = ((CAmtChar*)(pData))->m_cValue;
		}
	}

	void CAmtChar::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtChar(m_cValue);;
		}
	}
}