#include "RenderEngine.h"

namespace OpenHMI
{
	CBaseFilter* CFilterCenter::createFilter(const CRenderBaseFilter* pRndFilter)
	{
		switch (pRndFilter->getType())
		{
		case CRenderBaseFilter::FT_REFLECTION:
			{
				CFilterReflection * p = new CFilterReflection();
				return p;
			}
			break;

		case CRenderBaseFilter::FT_GLASS:
			{
				CFilterGlass *p = new CFilterGlass();
				return p;
			}
			break;

		case CRenderBaseFilter::FT_RIPPLE:
			{
				CFilterRipple *p = new CFilterRipple();
				return p;
			}
			break;
		case CRenderBaseFilter::FT_MASK:
			{
				CFilterMask *p = new CFilterMask();
				return p;
			}
			break;
		}
		return NULL;
	}

	void CFilterCenter::refreshFilterGenerateRT(const Array<CBaseFilter>& aryFilter)
	{
		BOOLEAN bNeedRT = FALSE;
		UINT uiFilterCnt = aryFilter.size();
		for (INT j = uiFilterCnt-2; j >= 0; --j)
		{
			CBaseFilter* pFilter = aryFilter[j];
			CBaseFilter* pNextFilter = aryFilter[j+1];
			if (pNextFilter && pNextFilter->isNeedRT() )
			{
				bNeedRT = TRUE;
			}

			if( bNeedRT )
			{
				pFilter->setGenerateRT(TRUE);
			}
		}
	}
}