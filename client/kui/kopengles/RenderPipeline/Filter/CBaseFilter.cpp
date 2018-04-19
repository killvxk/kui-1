#include "RenderEngine.h"

namespace OpenHMI
{
	BOOLEAN CBaseFilter::fromRenderFilter(CRenderGroupCacheData* pRndGroupCacheData, const CRenderBaseFilter* pFilter)
	{
		return TRUE;
	}

	void CBaseFilter::doFilter(C2DBatchRenderGroup* pIn, C2DBatchRenderGroup* pbg, Array2<C2DBatchRenderGroup*>& aryOut)
	{
		;
	}

	void CBaseFilter::doFilter(C3DBatchRenderGroup* pIn, C3DBatchRenderGroup* pbg, Array2<C3DBatchRenderGroup*>& aryOut)
	{
		;
	}
}