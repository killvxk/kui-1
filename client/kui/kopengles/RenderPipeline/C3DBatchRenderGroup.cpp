#include "RenderEngine.h"

namespace OpenHMI
{
	CPool<C3DBatchRender>	C3DBatchRenderGroup::sm_listFreeBatchRender(10);
	C3DBatchRenderGroup::C3DBatchRenderGroup()
		: m_pCamera(NULL)
	{
		;
	}

	C3DBatchRenderGroup::~C3DBatchRenderGroup()
	{
		if ( m_pCamera )
		{
			delete m_pCamera;
			m_pCamera = NULL;
		}	
	}

	void C3DBatchRenderGroup::add(C3DBatchRender* p)
	{
		m_aryUsedBatchRenderArray.addItem(p);
	}

	void C3DBatchRenderGroup::render()
	{
		UINT cnt = m_aryUsedBatchRenderArray.size();
		
		for ( UINT i = 0; i < cnt; ++i )
		{
			C3DBatchRender* pRender = m_aryUsedBatchRenderArray[i];
			pRender->render();
			
			pRender->reset();
			sm_listFreeBatchRender.release(pRender);
		}
		
		m_aryUsedBatchRenderArray.clear();
	}

	C3DBatchRender* C3DBatchRenderGroup::getFreeBatchRender()
	{
		C3DBatchRender* p = sm_listFreeBatchRender.get();
		p->setParent(this);
		return p;
	}

	void C3DBatchRenderGroup::release()
	{
		if (m_iRef > 0)
		{
			--m_iRef;
			if (m_iRef == 0)
			{
				getOwner()->free3DBatchRenderGroup(this);
			}
		}
	}

	void C3DBatchRenderGroup::releaseBatchRender()
	{
		sm_listFreeBatchRender.release();
	}
}