#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtFloat::CAmtFloat(FLOAT fValue)
		: m_fValue(fValue)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtFloat::~CAmtFloat()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtFloat::add(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_fValue = m_fValue + ((CAmtFloat*)pData)->m_fValue;
		}
		else
		{
			m_pCloneData->m_fValue = m_fValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtFloat::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_fValue = m_fValue - ((CAmtFloat*)pData)->m_fValue;
		}
		else
		{
			m_pCloneData->m_fValue = m_fValue;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtFloat::multiply(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_fValue = FLOAT(m_fValue * fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtFloat::divide(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_fValue = FLOAT(m_fValue / fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtFloat::cloneData()
	{
		genCloneData();

		m_pCloneData->m_fValue = m_fValue;

		return m_pCloneData;
	}

	void CAmtFloat::set(CBaseAmtData* pData)
	{
		if ( pData != NULL )
		{
			m_fValue = ((CAmtFloat*)(pData))->m_fValue;
		}
	}

	void CAmtFloat::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtFloat(m_fValue);;
		}
	}
}