#include "RenderEngine.h"

namespace OpenHMI
{
	C2DBatchRender::C2DBatchRender()
		: m_pParent(NULL)
		, m_pMaterial(NULL)
		, m_bIs25D(FALSE)
	{
		m_worldMatrix.identity();
	}

	C2DBatchRender::~C2DBatchRender()
	{
		reset();
	}

	void C2DBatchRender::addRenderItem(CRenderGroup* pRndGroup, BOOLEAN bOpengl)
	{
		tagBatchRenderItem item;
		if ( Math::isZero(pRndGroup->getRotateRadian()) )
		{
			item.m_fLeft    = (FLOAT)pRndGroup->getX();
			item.m_fTop     = (FLOAT)pRndGroup->getY();
			if ( Math::isZero(pRndGroup->getScaleWidth() - 1.0000f) == FALSE )
			{
				item.m_fRight = item.m_fLeft + pRndGroup->getWidth() * pRndGroup->getScaleWidth();
			}
			else
			{
				item.m_fRight = item.m_fLeft + pRndGroup->getWidth();
			}

			if ( Math::isZero(pRndGroup->getScaleHeight() - 1.0000f) == FALSE )
			{
				item.m_fBottom = item.m_fTop + pRndGroup->getHeight() * pRndGroup->getScaleHeight();
			}
			else
			{
				item.m_fBottom = item.m_fTop + pRndGroup->getHeight();
			}
		}
		else
		{
			CRenderGroupCacheData* pCacheData = (CRenderGroupCacheData*)pRndGroup->getCacheData();
			item.m_fLeft = -pCacheData->getAnchorX() * pCacheData->getRealWidth();
			item.m_fTop  = -pCacheData->getAnchorY() * pCacheData->getRealHeight();
			item.m_fRight = item.m_fLeft + pCacheData->getRealWidth();
			item.m_fBottom = item.m_fTop + pCacheData->getRealHeight();
			m_worldMatrix = pCacheData->getRotateZTransMatrix();

			if ( pRndGroup->getRotateAxis() != CRenderGroup::RA_Z )
			{
				m_bIs25D = TRUE;
			}
		}

		if ( bOpengl )
		{
			item.m_fUVLeft   = 0.0f;
			item.m_fUVTop    = 1.0f;
			item.m_fUVRight  = 1.0f;
			item.m_fUVBottom = 0.0f;
		}
		INT nAlpha = pRndGroup->getAlpha();	

		item.m_fAlpha = (FLOAT) ( (FLOAT)(pRndGroup->getAlpha()) / 255.0f );
		m_aryItems.addItem(item);

		if (!m_pMaterial )
		{
			m_pMaterial = getMaterial(pRndGroup);
		}

		onAddRenderItem(CRectangle(0,0,2480,2480));
	}

	void C2DBatchRender::addRenderItem(const CRectangle& rcDest, const CRectangleF& rcTex, CCacheMaterial* pMtl)
	{
		if( rcDest.isEmpty() )
		{
			return;
		}

		tagBatchRenderItem item;
		item.m_fLeft = (FLOAT)rcDest.getX();
		item.m_fTop = (FLOAT)rcDest.getY();
		item.m_fRight = (FLOAT)rcDest.getRight() + 1;
		item.m_fBottom = (FLOAT)rcDest.getBottom() + 1;
		item.m_fAlpha = 1.0f;

		item.m_fUVLeft = rcTex.getX();
		item.m_fUVTop = rcTex.getY();
		item.m_fUVRight = item.m_fUVLeft + rcTex.getWidth();
		item.m_fUVBottom = item.m_fUVTop + rcTex.getHeight();

		m_aryItems.addItem(item);

		if (!m_pMaterial )
		{
			m_pMaterial = pMtl;
		}

		onAddRenderItem(rcDest);
	}

	void C2DBatchRender::setRenderModel(INT iX, INT iY, CCacheMesh* pMesh, CCacheMesh* pVBOMesh, CCacheMaterial* pMaterial)
	{
		m_pMesh = pMesh;
		m_pMaterial = pMaterial;
		m_pVBOMesh = pVBOMesh;
		m_worldMatrix.makeTrans(FLOAT(iX),FLOAT(iY),0.0f);
	}

	void C2DBatchRender::merge(C2DBatchRender* pRender, CCacheMaterial* pMtl)
	{
		if ( m_pMaterial == NULL )
		{
			m_pMaterial = pMtl;
		}

		UINT cnt = pRender->m_aryItems.size();
		for ( UINT i = 0; i < cnt; ++i )
		{
			tagBatchRenderItem item = pRender->m_aryItems[i];
			CCacheMaterial* pRenderMtl = pRender->getMaterial();

			if ( m_pMaterial )
			{
				UINT uidx = m_pMaterial->merge(pRenderMtl);
				item.m_fAlpha = (FLOAT)uidx;
			}
			m_aryItems.addItem(item);
		}
	}

	void C2DBatchRender::render(CDynamicUIMesh* pMesh, CCacheMesh* pVBOMesh)
	{
		if ( m_aryItems.isEmpty() && m_pMesh.isNull() )	
		{
			return;
		}

		CCacheMesh* pCurMesh = pMesh;
		CCacheMesh* pCurVBOMesh = pVBOMesh;
		if ( m_pMesh.isNull() )
		{
			genMesh(pMesh);
		}
		else
		{
			pCurMesh = m_pMesh;
			pCurVBOMesh = m_pVBOMesh;
			pCurVBOMesh->reset();
		}

		CCacheMaterial* pMtl = getMaterial();
		if ( pMtl == NULL ||  pCurVBOMesh == NULL || pCurMesh == NULL)
		{
			return ;
		}

		pMtl->begin();
		bindAttributes(pCurMesh,pCurVBOMesh);
		
		CRenderEntry* pRE = m_pParent->getOwner();
		pMtl->apply(this, pRE->getRenderState() );
		draw(pCurMesh, pCurVBOMesh);
	
		unBindAttributes(pCurMesh);

		pMtl->end();
	}

	void C2DBatchRender::reset()
	{
		m_pMesh = NULL;
		m_pMaterial = NULL;
		m_pVBOMesh = NULL;
		m_pParent = NULL;
		m_aryItems.clear();
		
		m_worldMatrix.identity();

		m_bIs25D = FALSE;
		m_rcbbox.clear();
	}

	CRenderEntry* C2DBatchRender::getRenderEntry()	
	{ 
		return m_pParent->getOwner(); 
	}

	BOOLEAN C2DBatchRender::getCanMerge(CRenderItem* pRndItem) const
	{
		CCacheMaterial* pMtl = getMaterial(pRndItem);
		CCacheMaterial* pMtlDest = getMaterial();
		
		if( pMtlDest == pMtl && pMtl != NULL )
		{
			return TRUE;
		}
		
		return FALSE;
	}

	BOOLEAN	C2DBatchRender::getCanMerge(CCacheMaterial* pMtl) const
	{
		CCacheMaterial* pMtlDest = getMaterial();
		
		if( pMtlDest == pMtl && pMtl != NULL )
		{
			return TRUE;
		}
		
		return FALSE;
	}

	void C2DBatchRender::clone(C2DBatchRender* pSrc)
	{
		UINT cnt = pSrc->m_aryItems.size();
		for ( UINT i = 0; i < cnt; ++i )
		{
			m_aryItems.addItem(pSrc->m_aryItems[i]);
		}
		m_pMaterial = pSrc->getMaterial();
	}

	void C2DBatchRender::genMesh(CDynamicUIMesh* pMesh)
	{
		pMesh->reset();
		UINT cnt = m_aryItems.size();
		pMesh->resizeQuad(cnt);	//4*4 vertex
		for ( UINT i = 0; i < cnt; ++i )
		{
			const tagBatchRenderItem& rItem = m_aryItems[i];
			pMesh->fillQuad(i,rItem);
		}

		// UINT uiOldCnt = pMesh->getVertexcount();
		// pMesh->reserveVertex(cnt*4 + uiOldCnt);
		//
		// 
		//for ( UINT i = 0; i < cnt; ++i )
		//{
		//	const tagBatchRenderItem& rItem = m_aryItems[i];
		//	pMesh->fillQuad(rItem);
		//}
	}

	void C2DBatchRender::bindAttributes(CCacheMesh*	pMesh, CCacheMesh* pVBOMesh)
	{
		UINT uiVertexSize = pMesh->getVertexStride();
		UINT uiOffset = 0;
		UINT uiFmt = pMesh->getVertexFmt();
		UINT uiVAAIndex = 0;

		glBindBuffer(GL_ARRAY_BUFFER, pVBOMesh->getVBO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pVBOMesh->getIBO() );	

		
		if ( uiFmt & CCacheMesh::EMESHFMT_POSITION2 )
		{
			glEnableVertexAttribArray(uiVAAIndex);
			glVertexAttribPointer(uiVAAIndex, 2 , GL_FLOAT, GL_FALSE, uiVertexSize * sizeof(FLOAT) , (void*)uiOffset);
			
			uiOffset+= 2* sizeof(FLOAT);
			++uiVAAIndex;
		}
		else if ( uiFmt & CCacheMesh::EMESHFMT_POSITION3 )
		{
			glEnableVertexAttribArray(uiVAAIndex);
			glVertexAttribPointer(uiVAAIndex, 3 , GL_FLOAT, GL_FALSE, uiVertexSize * sizeof(FLOAT) , (void*)uiOffset);
			
			uiOffset+= 3* sizeof(FLOAT);
			++uiVAAIndex;
		}

		if ( uiFmt & CCacheMesh::EMESHFMT_TEXCOORD0 )
		{
			glEnableVertexAttribArray(uiVAAIndex);
			glVertexAttribPointer(uiVAAIndex, 2, GL_FLOAT, GL_FALSE, uiVertexSize * sizeof(FLOAT), (void*)uiOffset);
			++uiVAAIndex;
			uiOffset+= 2* sizeof(FLOAT);
		}

		if ( uiFmt & CCacheMesh::EMESHFMT_COLOR4 )
		{
			glEnableVertexAttribArray(uiVAAIndex);
			glVertexAttribPointer(uiVAAIndex, 4, GL_FLOAT, GL_FALSE, uiVertexSize * sizeof(FLOAT), (void*)uiOffset);
			++uiVAAIndex;
			uiOffset+= 4* sizeof(FLOAT);
		}

	
	}

	void C2DBatchRender::unBindAttributes(CCacheMesh*	pMesh)
	{
		UINT uiVAAIndex = 0;
		UINT uiFmt = pMesh->getVertexFmt();

		if ( uiFmt & CCacheMesh::EMESHFMT_POSITION2 )
		{
			glDisableVertexAttribArray( uiVAAIndex );
			++uiVAAIndex;
		}
		else if ( uiFmt & CCacheMesh::EMESHFMT_POSITION3 )
		{
			glDisableVertexAttribArray( uiVAAIndex );
			++uiVAAIndex;
		}
		if ( uiFmt & CCacheMesh::EMESHFMT_TEXCOORD0 )
		{
			glDisableVertexAttribArray( uiVAAIndex );
			++uiVAAIndex;
		}
		if ( uiFmt & CCacheMesh::EMESHFMT_COLOR4 )
		{
			glDisableVertexAttribArray( uiVAAIndex );
			++uiVAAIndex;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void C2DBatchRender::draw(CCacheMesh* pMesh, CCacheMesh* pVBOMesh)
	{
		pVBOMesh->reset();
		UINT cnt = pMesh->getIndexDataCount();

		pVBOMesh->fillVBO(pMesh->getVertixes(), pMesh->getVertexDataCount());
#ifdef _USEDEBUG
		GLenum error = glGetError();
		if ( error == GL_OUT_OF_MEMORY )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"GLERROR Out of Memory before");
		}
		else if( error != GL_NO_ERROR )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"GLERROR other error before");
		}
#endif
		pVBOMesh->fillIBO(pMesh->getIndexes(), pMesh->getIndexDataCount());

#ifdef _USEDEBUG
		error = glGetError();
		if ( error == GL_OUT_OF_MEMORY )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"GLERROR Out of Memory before");
		}
		else if( error != GL_NO_ERROR )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"GLERROR other error before");
		}
#endif
		

		glDrawElements( pMesh->getPrimitiveMode(), cnt, GL_UNSIGNED_SHORT, (void*)(0) );

#if 1
		GLenum error = glGetError();
		if ( error == GL_OUT_OF_MEMORY )
		{
			CMain::getInstance()->quitHMI();
		}
		else if( error != GL_NO_ERROR )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"GLERROR other error");
		}
#endif
	}

	CCacheMaterial* C2DBatchRender::getMaterial(CRenderItem* pRndItem) const
	{
		if ( pRndItem )
		{
			CMaterialRenderCacheData * pCacheData = (CMaterialRenderCacheData*)(pRndItem->getCacheData());
			if ( pCacheData )
			{
				return pCacheData->getMaterial();
			}
			else
			{
				CLog::info(L"", L"pCacheData == NULL");
			}
		}
		return NULL;
	}

	void C2DBatchRender::onAddRenderItem(const CRectangle& rcDest)
	{
		m_rcbbox.add(rcDest);
	}

}