#include "OpenHMI.h"

namespace OpenHMI
{
	CPathGroup::CPathGroup()
	{
		;
	}

	CPathGroup::~CPathGroup()
	{
		;
	}

	INT CPathGroup::getPathCount()
	{
		return (INT)m_aryListPath.size();
	}

	CBasePath* CPathGroup::getPath(UINT uiIndex)
	{
		return uiIndex >= m_aryListPath.size() ? NULL : (CBasePath*)m_aryListPath[uiIndex];
	}

	void CPathGroup::addPath(CBasePath* pPath)
	{
		m_aryListPath.addItem(pPath);
	}

	void CPathGroup::removePath(UINT uiIndex)
	{
		m_aryListPath.deleteItemByIndex(uiIndex);
	}

	void CPathGroup::clearAllPaths()
	{
		m_aryListPath.clear();
	}

	FLOAT CPathGroup::getLength()
	{
		FLOAT fLength = 0.0f;
		
		m_aryListPath.resetIterator();
		while (m_aryListPath.hasMore())
		{
			fLength += ((CBasePath*)m_aryListPath.next())->getLength();
		}
		
		return fLength;
	}

	CAmtPoint* CPathGroup::getPointByPercent(FLOAT fPercent)
	{
		if (m_aryListPath.size() == 0)
		{
			return NULL;
		}
		
		if (fPercent > 1.0)
		{
			fPercent -= (INT)fPercent;
		}

		if (fPercent < 0.0)
		{
			fPercent += (INT)(-fPercent) + 1;
		}
		
		if (fPercent <= 0.0)
		{
			// return begin point
			return ((CBasePath*)m_aryListPath[0])->getPointByPercent(0.0f);
		}
		else if (fPercent >= 1.0)
		{
			// return end point
			return ((CBasePath*)m_aryListPath[m_aryListPath.size() - 1])->getPointByPercent(1.0f);
		}
		
		FLOAT fCurLength = getLength() * fPercent;
		
		m_aryListPath.resetIterator();
		while (m_aryListPath.hasMore())
		{
			CBasePath* pPath = (CBasePath*)m_aryListPath.next();
			FLOAT fSubPathLength = pPath->getLength();
			
			if (fCurLength < fSubPathLength)
			{
				return pPath->getPointByPercent(fCurLength / fSubPathLength);
			}
			
			fCurLength -= fSubPathLength;
		}
		
		return ((CBasePath*)m_aryListPath[m_aryListPath.size() - 1])->getPointByPercent(1.0f);
	}

	void CPathGroup::smashToSegments(INT iSegmentCount)
	{
		m_aryListPath.resetIterator();
		while (m_aryListPath.hasMore())
		{
			m_aryListPath.next()->smashToSegments(iSegmentCount);
		}
	}
}