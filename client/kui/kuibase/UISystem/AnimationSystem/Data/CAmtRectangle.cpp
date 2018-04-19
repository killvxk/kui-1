#include "OpenHMI.h"

namespace OpenHMI
{
	CAmtRectangle::CAmtRectangle(INT iLeft, INT iTop, INT iWidth, INT iHeight) 
		: m_iLeft(iLeft)
		, m_iTop(iTop)
		, m_iWidth(iWidth)
		, m_iHeight(iHeight)
		, m_pCloneData(NULL)
	{
		;
	}

	CAmtRectangle::~CAmtRectangle()
	{
		if ( m_pCloneData != NULL )
		{
			delete m_pCloneData;
		}
	}

	CBaseAmtData* CAmtRectangle::add(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_iLeft = m_iLeft + ((CAmtRectangle*)pData)->m_iLeft;
			m_pCloneData->m_iTop = m_iTop + ((CAmtRectangle*)pData)->m_iTop;
			m_pCloneData->m_iWidth = m_iWidth + ((CAmtRectangle*)pData)->m_iWidth;
			m_pCloneData->m_iHeight = m_iHeight + ((CAmtRectangle*)pData)->m_iHeight;
		}
		else
		{
			m_pCloneData->m_iLeft = m_iLeft;
			m_pCloneData->m_iTop = m_iTop;
			m_pCloneData->m_iWidth = m_iWidth;
			m_pCloneData->m_iHeight = m_iHeight;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtRectangle::subtract(CBaseAmtData* pData)
	{
		genCloneData();

		if( pData != NULL )
		{
			m_pCloneData->m_iLeft = m_iLeft - ((CAmtRectangle*)pData)->m_iLeft;
			m_pCloneData->m_iTop = m_iTop - ((CAmtRectangle*)pData)->m_iTop;
			m_pCloneData->m_iWidth = m_iWidth - ((CAmtRectangle*)pData)->m_iWidth;
			m_pCloneData->m_iHeight = m_iHeight - ((CAmtRectangle*)pData)->m_iHeight;
		}
		else
		{
			m_pCloneData->m_iLeft = m_iLeft;
			m_pCloneData->m_iTop = m_iTop;
			m_pCloneData->m_iWidth = m_iWidth;
			m_pCloneData->m_iHeight = m_iHeight;
		}

		return m_pCloneData;
	}

	CBaseAmtData* CAmtRectangle::multiply(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_iLeft = (INT)(m_iLeft * fValue);
		m_pCloneData->m_iTop = (INT)(m_iTop * fValue);
		m_pCloneData->m_iWidth = (INT)(m_iWidth * fValue);
		m_pCloneData->m_iHeight = (INT)(m_iHeight * fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtRectangle::divide(FLOAT fValue)
	{
		genCloneData();

		m_pCloneData->m_iLeft = (INT)(m_iLeft / fValue);
		m_pCloneData->m_iTop = (INT)(m_iTop / fValue);
		m_pCloneData->m_iWidth = (INT)(m_iWidth / fValue);
		m_pCloneData->m_iHeight = (INT)(m_iHeight / fValue);

		return m_pCloneData;
	}

	CBaseAmtData* CAmtRectangle::cloneData()
	{
		genCloneData();

		m_pCloneData->m_iLeft = m_iLeft;
		m_pCloneData->m_iTop = m_iTop;
		m_pCloneData->m_iWidth = m_iWidth;
		m_pCloneData->m_iHeight = m_iHeight;

		return m_pCloneData;
	}

	void CAmtRectangle::set(CBaseAmtData* pData)
	{
		if ( pData != NULL )
		{
			m_iLeft = ((CAmtRectangle*)pData)->m_iLeft;
			m_iTop = ((CAmtRectangle*)pData)->m_iTop;
			m_iWidth = ((CAmtRectangle*)pData)->m_iWidth;
			m_iHeight = ((CAmtRectangle*)pData)->m_iHeight;
		}
	}

	void CAmtRectangle::genCloneData()
	{
		if (  m_pCloneData == NULL )
		{
			m_pCloneData = new CAmtRectangle(m_iLeft, m_iTop, m_iWidth, m_iHeight);
		}
	}
}