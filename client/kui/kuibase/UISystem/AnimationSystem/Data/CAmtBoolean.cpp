#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtBoolean::CAmtBoolean(BOOLEAN bValue) 		
		: m_bValue(bValue)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtBoolean::~CAmtBoolean()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtBoolean::add(CBaseAmtData* pData)
	{
		genCloneData();

		return m_pCloneData;
	}

	CBaseAmtData* CAmtBoolean::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		return m_pCloneData;
	}

	CBaseAmtData* CAmtBoolean::multiply(FLOAT fValue)
	{
		genCloneData();

		return m_pCloneData;
	}

	CBaseAmtData* CAmtBoolean::divide(FLOAT fValue)
	{
		genCloneData();

		return m_pCloneData;
	}

	CBaseAmtData* CAmtBoolean::cloneData()
	{
		genCloneData();

		m_pCloneData->m_bValue = m_bValue;

		return m_pCloneData;
	}

	void CAmtBoolean::set(CBaseAmtData* pData)
	{
		if ( pData != NULL )
		{
			m_bValue = ((CAmtBoolean*)(pData))->m_bValue;
		}
	}

	void CAmtBoolean::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtBoolean(m_bValue);;
		}
	}
}