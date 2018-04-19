#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtString::CAmtString(const CHAR* pcValue) 
		: m_strValue(pcValue)
		, m_pCloneData(NULL)
	{
		;
	}
	
	CAmtString::CAmtString(const String &strValue)
		: m_strValue(strValue)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtString::~CAmtString()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtString::add(CBaseAmtData* pData)
	{
		genCloneData();

		return m_pCloneData;
	}

	CBaseAmtData* CAmtString::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		return m_pCloneData;
	}

	CBaseAmtData* CAmtString::multiply(FLOAT fValue)
	{
		genCloneData();

		return m_pCloneData;
	}

	CBaseAmtData* CAmtString::divide(FLOAT fValue)
	{
		genCloneData();

		return m_pCloneData;
	}

	CBaseAmtData* CAmtString::cloneData()
	{
		genCloneData();

		m_pCloneData->m_strValue = m_strValue;

		return m_pCloneData;
	}

	void CAmtString::set(CBaseAmtData* pData)
	{
		if ( pData != NULL )
		{
			m_strValue = ((CAmtString*)(pData))->m_strValue;
		}
	}

	void CAmtString::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtString(m_strValue);
		}
	}
}