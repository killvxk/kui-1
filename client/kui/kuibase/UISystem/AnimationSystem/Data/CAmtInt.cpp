#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtInt::CAmtInt(INT iValue) 
		: m_iValue(iValue)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtInt::~CAmtInt()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtInt::add(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_iValue = m_iValue + ((CAmtInt*)pData)->m_iValue;
		}
		else
		{
			m_pCloneData->m_iValue = m_iValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtInt::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_iValue = m_iValue - ((CAmtInt*)pData)->m_iValue;
		}
		else
		{
			m_pCloneData->m_iValue = m_iValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtInt::multiply(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_iValue = INT(m_iValue * fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtInt::divide(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_iValue = INT(m_iValue / fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtInt::cloneData()
	{
		genCloneData();

		m_pCloneData->m_iValue = m_iValue;

		return m_pCloneData;
	}

	void CAmtInt::set(CBaseAmtData* pData)
	{
		if( pData != NULL )
		{
			m_iValue = ((CAmtInt*)(pData))->m_iValue;
		}
	}

	void CAmtInt::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtInt(m_iValue);;
		}
	}
}