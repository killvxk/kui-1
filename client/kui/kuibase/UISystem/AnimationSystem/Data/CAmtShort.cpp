#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtShort::CAmtShort(SHORT sValue) 
		: m_sValue(sValue)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtShort::~CAmtShort()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtShort::add(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_sValue = m_sValue + ((CAmtShort*)pData)->m_sValue;
		}
		else
		{
			m_pCloneData->m_sValue = m_sValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtShort::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_sValue = m_sValue - ((CAmtShort*)pData)->m_sValue;
		}
		else
		{
			m_pCloneData->m_sValue = m_sValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtShort::multiply(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_sValue = SHORT(m_sValue * fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtShort::divide(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_sValue = SHORT(m_sValue / fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtShort::cloneData()
	{
		genCloneData();

		m_pCloneData->m_sValue = m_sValue;

		return m_pCloneData;
	}

	void CAmtShort::set(CBaseAmtData* pData)
	{
		if( pData != NULL )
		{
			m_sValue = ((CAmtShort*)(pData))->m_sValue;
		}
	}

	void CAmtShort::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtShort(m_sValue);;
		}
	}
}