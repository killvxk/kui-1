#include "RenderEngine.h"

namespace OpenHMI
{

	CCacheData::CCacheData()
		: m_pDib(NULL)
		, m_uiLineHeight(0)
	{
		;
	}

	CCacheData::~CCacheData()
	{
		freeDIB();
	}

	void CCacheData::notifySizePropertyChanged()
	{
		freeDIB();
	}

	void CCacheData::notifyMergePropertyChanged()
	{
		//freeDIB();
	}

	void CCacheData::notifyContentPropertyChanged()
	{
		freeDIB();
	}

	void CCacheData::notifyChildrenChanged()
	{
		freeDIB();
	}

	void CCacheData::setDIB(CDIB *pDib)
	{
		if (m_pDib == pDib)
		{
			return;
		}

		freeDIB();

		m_pDib = pDib;
	}

	void CCacheData::freeDIB()
	{
		if (m_pDib != NULL)
		{
			delete m_pDib;
			m_pDib = NULL;
		}

		m_uiLineHeight = 0;
	}
}