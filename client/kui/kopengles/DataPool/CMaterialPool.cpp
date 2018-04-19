#include "RenderEngine.h"

namespace OpenHMI
{
	CMaterialPool* CMaterialPool::s_pInstance	= NULL;

	CMaterialPool*	CMaterialPool::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CMaterialPool();
		}

		return s_pInstance;
	}

	void CMaterialPool::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CMaterialPool::CMaterialPool()
	{
		;
	}

	CMaterialPool::~CMaterialPool()
	{
		unInitialize();
	}

	void CMaterialPool::initialize()
	{
		m_cachePool.setPoolSize(CONFIG::getMaterialCacheCount());
	}

	void CMaterialPool::unInitialize()
	{
		clear();
	}

	void CMaterialPool::clear()
	{
		m_cachePool.clear();
	}

	CCacheMaterial*	CMaterialPool::getInternalMaterial(EMaterialType eType, CCacheTexture* pTex)
	{
		String strName = generateInternalMtlName(eType,(pTex == NULL) ? -1: pTex->getTexID() );
		CCacheMaterialPtr p(m_cachePool.getCacheObject(strName));

		if ( p.isNull() )
		{			
			p = new CCacheMaterial();
			p->setName(strName);
			//CLog::debug2(String(RENDER_ENGINE_NAME),String(L"new material")+strName);

			m_cachePool.addCacheObject(strName,p);
			g_contructMaterial(eType, p);

			if ( pTex )
			{
				p->addCustomTexture(pTex);
			}
		}
		return p;
	}

	CCacheMaterial*	CMaterialPool::getInternalMaterial(EMaterialType eType, const Array2<CCacheTexture*>& aryTex)
	{
		UINT size = aryTex.size();
		if( size == 0 )
		{
			return NULL;
		}

		String strName = generateInternalMtlName(eType,aryTex );
		CCacheMaterial* p = m_cachePool.getCacheObject(strName);

		if ( p== NULL)
		{			
			p = new CCacheMaterial();
			p->setName(strName);
			CLog::debug2(String(RENDER_ENGINE_NAME),String(L"new material")+strName);

			m_cachePool.addCacheObject(strName,p);
			g_contructMaterial(eType, p);
		
			for(UINT i = 0 ; i < size; ++i )
			{
				p->addCustomTexture(aryTex[i]);
			}
		}
		return p;
	}

	CCacheMaterial*	CMaterialPool::getInternalMergeMaterial()
	{
		static UINT idx = 0;
		String strName = String::format(L"megeIdx %u ",idx++);
		CCacheMaterial* p = new CCacheMaterial();
		p->setName(strName);
		g_contructMaterial(EMT_UI_DEFAULT_MERGE, p);

		return p;
	}

	void CMaterialPool::freeMaterial(CCacheMaterial* pMtl)
	{
		if ( pMtl != NULL )
		{
			m_cachePool.deleteCacheObject(pMtl->getName());
		}
	}

	String CMaterialPool::generateInternalMtlName(EMaterialType eType, UINT iTexID)
	{
		return String::format(L"internalMtl-%u-%u-%u", eType, g_uiSessionID, iTexID );
	}

	String CMaterialPool::generateInternalMtlName(EMaterialType eType, const Array2<CCacheTexture*>& aryTex)
	{
		String strName = String::format(L"internalMtl-%u-%u", eType, g_uiSessionID);
		UINT size = aryTex.size();
		for(UINT i = 0 ; i < size; ++i )
		{
			CCacheTexture* pTex = aryTex[i];
			if( pTex )
			{
				UINT id = pTex->getTexID();
				strName.append( L"-");
				strName.append(String::toString(id));
			}
		}
		return strName;
	}
}