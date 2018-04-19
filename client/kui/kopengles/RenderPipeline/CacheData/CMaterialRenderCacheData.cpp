#include "RenderEngine.h"

namespace OpenHMI
{

	CMaterialRenderCacheData::CMaterialRenderCacheData()
		: m_pMtl(NULL)
	{
		;
	}

	CMaterialRenderCacheData::~CMaterialRenderCacheData()
	{
		m_pMtl = NULL;
	}

	CCacheMaterial* CMaterialRenderCacheData::getMaterial()
	{
		
		return m_pMtl;
	}

	void CMaterialRenderCacheData::flushMaterial()
	{
		;
	}
}