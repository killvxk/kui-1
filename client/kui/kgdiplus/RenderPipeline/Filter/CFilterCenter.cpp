#include "RenderEngine.h"

namespace OpenHMI
{
	void CFilterCenter::doFilter(CDIB &srcDib, CDIB &bgDib, INT &iBgX, INT &iBgY, const CRenderBaseFilter *pRndFilter)
	{
		if (pRndFilter == NULL)
		{
			return;
		}

		switch (pRndFilter->getType())
		{
		case CRenderBaseFilter::FT_REFLECTION:
			{
				CFltReflection filter;
				filter.doFilter(srcDib, bgDib, iBgX, iBgY, static_cast<const CRenderFltReflection*>(pRndFilter));
			}
			return;

		case CRenderBaseFilter::FT_GLASS:
			{
				;
			}
			return;

		case CRenderBaseFilter::FT_RIPPLE:
			{
				;
			}
			return;

		case CRenderBaseFilter::FT_MASK:
			{
				CFltMask filter;
				filter.doFilter(srcDib, bgDib, iBgX, iBgY, static_cast<const CRenderFltMask*>(pRndFilter));
			}
			return;
		}
	}
}