#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtDouble::CAmtDouble(DOUBLE dValue)
		: m_dValue(dValue)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtDouble::~CAmtDouble()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtDouble::add(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_dValue = m_dValue + ((CAmtDouble*)pData)->m_dValue;
		}
		else
		{
			m_pCloneData->m_dValue = m_dValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtDouble::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_dValue = m_dValue - ((CAmtDouble*)pData)->m_dValue;
		}
		else
		{
			m_pCloneData->m_dValue = m_dValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtDouble::multiply(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_dValue = DOUBLE(m_dValue * fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtDouble::divide(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_dValue = DOUBLE(m_dValue / fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtDouble::cloneData()
	{
		genCloneData();

		m_pCloneData->m_dValue = m_dValue;

		return m_pCloneData;
	}

	void CAmtDouble::set(CBaseAmtData* pData)
	{
		if( pData != NULL )
		{
			m_dValue = ((CAmtDouble*)(pData))->m_dValue;
		}
	}

	void CAmtDouble::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtDouble(m_dValue);;
		}
	}
}