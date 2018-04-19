#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtByte::CAmtByte(BYTE btValue) 
		: m_btValue(btValue)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtByte::~CAmtByte()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtByte::add(CBaseAmtData* pData)
	{
		genCloneData();

		if ( pData != NULL )
		{
			m_pCloneData->m_btValue = m_btValue + ((CAmtByte*)pData)->m_btValue;
		}
		else
		{
			m_pCloneData->m_btValue = m_btValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtByte::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_btValue = m_btValue - ((CAmtByte*)pData)->m_btValue;
		}
		else
		{
			m_pCloneData->m_btValue = m_btValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtByte::multiply(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_btValue = BYTE(m_btValue * fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtByte::divide(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_btValue = BYTE(m_btValue / fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtByte::cloneData()
	{
		genCloneData();

		m_pCloneData->m_btValue = m_btValue;

		return m_pCloneData;
	}

	void CAmtByte::set(CBaseAmtData* pData)
	{
		if ( pData != NULL )
		{
			m_btValue = ((CAmtByte*)(pData))->m_btValue;
		}
	}

	void CAmtByte::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtByte(m_btValue);
		}
	}
}