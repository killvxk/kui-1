#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtColor::CAmtColor(UINT uiValue)
		: m_pCloneData(NULL)
	{
		m_btA = (uiValue >> 24) & 0xFF;
		m_btR = (uiValue >> 16) & 0xFF;
		m_btG = (uiValue >> 8) & 0xFF;
		m_btB = uiValue & 0xFF;
	}

	CAmtColor::CAmtColor(BYTE btA, BYTE btR, BYTE btG, BYTE btB) 
		: m_btA(btA)
		, m_btR(btR)
		, m_btG(btG)
		, m_btB(btB)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtColor::~CAmtColor()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtColor::add(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_btA = m_btA + ((CAmtColor*)pData)->m_btA;
			m_pCloneData->m_btR = m_btR + ((CAmtColor*)pData)->m_btR;
			m_pCloneData->m_btG = m_btG + ((CAmtColor*)pData)->m_btG;
			m_pCloneData->m_btB = m_btB + ((CAmtColor*)pData)->m_btB;
		}
		else
		{
			m_pCloneData->m_btA = m_btA;
			m_pCloneData->m_btR = m_btR;
			m_pCloneData->m_btG = m_btG;
			m_pCloneData->m_btB = m_btB;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtColor::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_btA = m_btA - ((CAmtColor*)pData)->m_btA;
			m_pCloneData->m_btR = m_btR - ((CAmtColor*)pData)->m_btR;
			m_pCloneData->m_btG = m_btG - ((CAmtColor*)pData)->m_btG;
			m_pCloneData->m_btB = m_btB - ((CAmtColor*)pData)->m_btB;
		}
		else
		{
			m_pCloneData->m_btA = m_btA;
			m_pCloneData->m_btR = m_btR;
			m_pCloneData->m_btG = m_btG;
			m_pCloneData->m_btB = m_btB;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtColor::multiply(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_btA = (BYTE)(m_btA * fValue);
		m_pCloneData->m_btR = (BYTE)(m_btR * fValue);
		m_pCloneData->m_btG = (BYTE)(m_btG * fValue);
		m_pCloneData->m_btB = (BYTE)(m_btB * fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtColor::divide(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_btA = (BYTE)(m_btA / fValue);
		m_pCloneData->m_btR = (BYTE)(m_btR / fValue);
		m_pCloneData->m_btG = (BYTE)(m_btG / fValue);
		m_pCloneData->m_btB = (BYTE)(m_btB / fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtColor::cloneData()
	{
		genCloneData();

		m_pCloneData->m_btA = m_btA;
		m_pCloneData->m_btR = m_btR;
		m_pCloneData->m_btG = m_btG;
		m_pCloneData->m_btB = m_btB;

		return m_pCloneData;
	}

	void CAmtColor::set(CBaseAmtData* pData)
	{
		if( pData != NULL )
		{
			m_btA = ((CAmtColor*)pData)->m_btA;
			m_btR = ((CAmtColor*)pData)->m_btR;
			m_btG = ((CAmtColor*)pData)->m_btG;
			m_btB = ((CAmtColor*)pData)->m_btB;
		}
	}

	UINT CAmtColor::getValue()
	{
		return ((UINT)m_btA << 24) | ((UINT)m_btR << 16) | ((UINT)m_btG << 8) | ((UINT)m_btB);
	}

	void CAmtColor::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtColor(m_btA, m_btR, m_btG, m_btB);
		}
	}
}