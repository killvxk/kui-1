#include "RenderEngine.h"

namespace OpenHMI
{

	CDynamicUIMesh::CDynamicUIMesh()
		: m_uiQuadSize(0)
		, m_uiQuadCapacity(0)
	{
		;
	}

	CDynamicUIMesh::~CDynamicUIMesh()
	{
		clear();
	}

	void CDynamicUIMesh::resizeQuad(UINT uisize)
	{
		m_uiQuadSize = uisize;

		if ( m_uiQuadCapacity >= uisize)
		{
			;
		}
		else
		{
			if ( m_pfVertexes ) 
				delete[] m_pfVertexes;

			if ( m_pusIndexes ) 
				delete[] m_pusIndexes;

			this->m_pfVertexes = new GLfloat[uisize*4*5];
			this->m_pusIndexes = new GLushort[uisize*6];

			m_uiQuadCapacity = uisize;
		}

		this->m_uiVertexDataCnt = 0;
		this->m_uiIdxDataCnt = 0;
	}

	void CDynamicUIMesh::fillQuad(UINT uidx,const C2DBatchRender::tagBatchRenderItem& rItem)
	{
		UINT uiVertexidx = uidx * 4 * 5;
		UINT uiIdxidx = uidx * 6;
		UINT cnt = uidx * 4;
		const UINT uiVertexSize = 5;

		m_pusIndexes[uiIdxidx+0] = cnt + 0;
		m_pusIndexes[uiIdxidx+1] = cnt + 1;
		m_pusIndexes[uiIdxidx+2] = cnt + 2;
		m_pusIndexes[uiIdxidx+3] = cnt + 0;
		m_pusIndexes[uiIdxidx+4] = cnt + 2;
		m_pusIndexes[uiIdxidx+5] = cnt + 3;

		m_pfVertexes[cnt*uiVertexSize + 0] = rItem.m_fLeft;
		m_pfVertexes[cnt*uiVertexSize + 1] = rItem.m_fTop;
		m_pfVertexes[cnt*uiVertexSize + 2] = rItem.m_fAlpha;
		m_pfVertexes[cnt*uiVertexSize + 3] = rItem.m_fUVLeft;
		m_pfVertexes[cnt*uiVertexSize + 4] = rItem.m_fUVTop;
		++cnt;

		m_pfVertexes[cnt*uiVertexSize + 0] = rItem.m_fLeft;
		m_pfVertexes[cnt*uiVertexSize + 1] = rItem.m_fBottom;
		m_pfVertexes[cnt*uiVertexSize + 2] = rItem.m_fAlpha;
		m_pfVertexes[cnt*uiVertexSize + 3] = rItem.m_fUVLeft;
		m_pfVertexes[cnt*uiVertexSize + 4] = rItem.m_fUVBottom;
		++cnt;

		m_pfVertexes[cnt*uiVertexSize + 0] = rItem.m_fRight;
		m_pfVertexes[cnt*uiVertexSize + 1] = rItem.m_fBottom;
		m_pfVertexes[cnt*uiVertexSize + 2] = rItem.m_fAlpha;
		m_pfVertexes[cnt*uiVertexSize + 3] = rItem.m_fUVRight;
		m_pfVertexes[cnt*uiVertexSize + 4] = rItem.m_fUVBottom;
		++cnt;

		m_pfVertexes[cnt*uiVertexSize + 0] = rItem.m_fRight;
		m_pfVertexes[cnt*uiVertexSize + 1] = rItem.m_fTop;
		m_pfVertexes[cnt*uiVertexSize + 2] = rItem.m_fAlpha;
		m_pfVertexes[cnt*uiVertexSize + 3] = rItem.m_fUVRight;
		m_pfVertexes[cnt*uiVertexSize + 4] = rItem.m_fUVTop;

		m_uiVertexDataCnt += 4;
		m_uiIdxDataCnt += 6;
	}

	void CDynamicUIMesh::fillQuad(UINT uidx,const C2DBatchRender::tagBatchRenderItem& rItem,const C3DMatrix& matrix)
	{
		UINT uiIdxidx = uidx * 6;
		UINT cnt = uidx * 4;
		const UINT uiVertexSize = 5;

		m_pusIndexes[uiIdxidx+0] = cnt + 0;
		m_pusIndexes[uiIdxidx+1] = cnt + 1;
		m_pusIndexes[uiIdxidx+2] = cnt + 2;
		m_pusIndexes[uiIdxidx+3] = cnt + 0;
		m_pusIndexes[uiIdxidx+4] = cnt + 2;
		m_pusIndexes[uiIdxidx+5] = cnt + 3;

		m_pfVertexes[cnt*uiVertexSize + 0] = rItem.m_fLeft;
		m_pfVertexes[cnt*uiVertexSize + 1] = rItem.m_fTop;
		m_pfVertexes[cnt*uiVertexSize + 2] = 2.0f;
		{
		C3DVector4 vec(m_pfVertexes[cnt*uiVertexSize + 0],m_pfVertexes[cnt*uiVertexSize + 1],m_pfVertexes[cnt*uiVertexSize + 2],1.0f);
		vec = matrix * vec;
		m_pfVertexes[cnt*uiVertexSize + 0] = vec.getX();
		m_pfVertexes[cnt*uiVertexSize + 1] = vec.getY();
		m_pfVertexes[cnt*uiVertexSize + 2] = vec.getZ();
		}

		m_pfVertexes[cnt*uiVertexSize + 3] = rItem.m_fUVLeft;
		m_pfVertexes[cnt*uiVertexSize + 4] = rItem.m_fUVTop;
		++cnt;

		m_pfVertexes[cnt*uiVertexSize + 0] = rItem.m_fLeft;
		m_pfVertexes[cnt*uiVertexSize + 1] = rItem.m_fBottom;
		m_pfVertexes[cnt*uiVertexSize + 2] = 2.0f;
				{
		C3DVector4 vec(m_pfVertexes[cnt*uiVertexSize + 0],m_pfVertexes[cnt*uiVertexSize + 1],m_pfVertexes[cnt*uiVertexSize + 2],1.0f);
		vec = matrix * vec;
		m_pfVertexes[cnt*uiVertexSize + 0] = vec.getX();
		m_pfVertexes[cnt*uiVertexSize + 1] = vec.getY();
		m_pfVertexes[cnt*uiVertexSize + 2] = vec.getZ();
		}
		m_pfVertexes[cnt*uiVertexSize + 3] = rItem.m_fUVLeft;
		m_pfVertexes[cnt*uiVertexSize + 4] = rItem.m_fUVBottom;
		++cnt;

		m_pfVertexes[cnt*uiVertexSize + 0] = rItem.m_fRight;
		m_pfVertexes[cnt*uiVertexSize + 1] = rItem.m_fBottom;
		m_pfVertexes[cnt*uiVertexSize + 2] = 2.0f;
				{
		C3DVector4 vec(m_pfVertexes[cnt*uiVertexSize + 0],m_pfVertexes[cnt*uiVertexSize + 1],m_pfVertexes[cnt*uiVertexSize + 2],1.0f);
		vec = matrix * vec;
		m_pfVertexes[cnt*uiVertexSize + 0] = vec.getX();
		m_pfVertexes[cnt*uiVertexSize + 1] = vec.getY();
		m_pfVertexes[cnt*uiVertexSize + 2] = vec.getZ();
		}
		m_pfVertexes[cnt*uiVertexSize + 3] = rItem.m_fUVRight;
		m_pfVertexes[cnt*uiVertexSize + 4] = rItem.m_fUVBottom;
		++cnt;

		m_pfVertexes[cnt*uiVertexSize + 0] = rItem.m_fRight;
		m_pfVertexes[cnt*uiVertexSize + 1] = rItem.m_fTop;
		m_pfVertexes[cnt*uiVertexSize + 2] = 2.0f;
				{
		C3DVector4 vec(m_pfVertexes[cnt*uiVertexSize + 0],m_pfVertexes[cnt*uiVertexSize + 1],m_pfVertexes[cnt*uiVertexSize + 2],1.0f);
		vec = matrix * vec;
		m_pfVertexes[cnt*uiVertexSize + 0] = vec.getX();
		m_pfVertexes[cnt*uiVertexSize + 1] = vec.getY();
		m_pfVertexes[cnt*uiVertexSize + 2] = vec.getZ();
		}
		m_pfVertexes[cnt*uiVertexSize + 3] = rItem.m_fUVRight;
		m_pfVertexes[cnt*uiVertexSize + 4] = rItem.m_fUVTop;

		m_uiVertexDataCnt += 4;
		m_uiIdxDataCnt += 6;
	}

	void CDynamicUIMesh::reset()
	{
		m_uiQuadSize = 0;
		m_uiVertexDataCnt = 0;
		m_uiIdxDataCnt = 0;
	}

	void CDynamicUIMesh::clear()
	{
		if ( m_pfVertexes ) 
		{
			delete[] m_pfVertexes;
			m_pfVertexes = NULL;
		}

		if ( m_pusIndexes ) 
		{
			delete[] m_pusIndexes;
			m_pusIndexes = NULL;
		}
	}
}