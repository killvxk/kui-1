#include "RenderEngine.h"

namespace OpenHMI
{

	C3DBatchRender::C3DBatchRender()
		: m_pModel(NULL)
		, m_pParent(NULL)
	{
		;
	}

	C3DBatchRender::~C3DBatchRender()
	{
		reset();
	}

	void C3DBatchRender::addItem(CRenderModelCacheData* pModel)
	{
		m_pModel = pModel;
	}

	void C3DBatchRender::render()
	{
		if ( !m_pModel )	
		{
			return;
		}

		CCacheMaterial* pMtl = m_pModel->getMaterial();
		if ( pMtl )
		{
			bindAttributes();
			
			pMtl->begin();
			CRenderEntry* pRE = m_pParent->getOwner();
			pMtl->apply(this, pRE->getRenderState() );
			draw();
			pMtl->end();

			unBindAttributes();
		}
	}

	void C3DBatchRender::reset()
	{
		m_pModel = NULL;
		m_pParent = NULL;
	}

	const C3DMatrix& C3DBatchRender::getModelMatrix()
	{
		return m_pModel->getModelMatrix();
	}

	void C3DBatchRender::bindAttributes()
	{
		CCacheMesh* pMesh = m_pModel->getMesh();
		UINT uiVertexSize = pMesh->getVertexStride();
		UINT uiOffset = 0;
		UINT uiFmt = pMesh->getVertexFmt();
		UINT uiVAAIndex = 0;
		
		if ( pMesh->useVBO() )
		{
			glBindBuffer(GL_ARRAY_BUFFER, pMesh->getVBO());
		}
			
		if ( uiFmt & CCacheMesh::EMESHFMT_POSITION3 )
		{
			glEnableVertexAttribArray(uiVAAIndex);
			glVertexAttribPointer(uiVAAIndex, 3 , GL_FLOAT, GL_FALSE, uiVertexSize * sizeof(FLOAT) , (void*)uiOffset);
			
			uiOffset+= 3* sizeof(FLOAT);
			++uiVAAIndex;
		}

		if ( uiFmt & CCacheMesh::EMESHFMT_NORMAL )
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

			uiOffset+= 2* sizeof(FLOAT);
			++uiVAAIndex;
		}

		if ( pMesh->useIBO() )
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->getIBO() );
		}
	}

	void C3DBatchRender::unBindAttributes()
	{
		CCacheMesh* pMesh = m_pModel->getMesh();

		UINT uiVAAIndex = 0;
		UINT uiFmt = pMesh->getVertexFmt();

		if ( uiFmt & CCacheMesh::EMESHFMT_POSITION3 )
		{
			glDisableVertexAttribArray( uiVAAIndex );
			++uiVAAIndex;
		}

		if ( uiFmt & CCacheMesh::EMESHFMT_NORMAL )
		{
			glDisableVertexAttribArray(uiVAAIndex);
			++uiVAAIndex;
		}

		if ( uiFmt & CCacheMesh::EMESHFMT_TEXCOORD0 )
		{
			glDisableVertexAttribArray( uiVAAIndex );
			++uiVAAIndex;
		}

		if ( pMesh->useVBO() )
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		
		if ( pMesh->useIBO() )
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	void C3DBatchRender::draw()
	{
		CCacheMesh* pMesh = m_pModel->getMesh();
		UINT cnt = pMesh->getIndexDataCount();

		if ( pMesh->useVBO() )
		{
			glDrawElements( GL_TRIANGLES, cnt, GL_UNSIGNED_SHORT, (void*)0 );
		}
		else
		{
			glDrawElements( GL_TRIANGLES, cnt, GL_UNSIGNED_SHORT, pMesh->getIndexes() );
		}
	}
}