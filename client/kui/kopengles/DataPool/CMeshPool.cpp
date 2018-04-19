#include "RenderEngine.h"

namespace OpenHMI
{
	CMeshPool* CMeshPool::s_pInstance	= NULL;

	CMeshPool*	CMeshPool::getInstance()		
	{
		if (s_pInstance == NULL)
		{
			s_pInstance = new CMeshPool();
		}

		return s_pInstance;
	}

	void CMeshPool::deleteInstance()
	{
		if (s_pInstance != NULL)
		{
			delete s_pInstance;
			s_pInstance = NULL;
		}
	}

	CMeshPool::CMeshPool()
		: m_ptrDynamicUIMesh(NULL)
	{
		;
	}

	CMeshPool::~CMeshPool()
	{
		unInitialize();
	}

	void CMeshPool::initialize()
	{
		m_cachePool.setPoolSize(CONFIG::getMeshCacheCount());
	}

	void CMeshPool::unInitialize()
	{
		clear();
	}

	void CMeshPool::clear()
	{
		m_cachePool.clear();
	}

	CCacheMesh* CMeshPool::getRectangleMesh(UINT uiWidth,UINT uiHeight,UINT uiColor,BOOLEAN bDashLine)
	{
		String strName = String::format(L"I:Rect%u-%u-%u-%d", uiWidth, uiHeight, uiColor, bDashLine);
		CCacheMesh* pMesh = m_cachePool.getCacheObject(strName);
		if( pMesh == NULL )
		{
			pMesh = new CCacheMesh();
			UINT uiVtxCnt = 0;
			UINT uiIdxCnt = 0;
			FLOAT* pVtxBuf = NULL;
			USHORT* pIdxBuf = NULL;
			CMeshFactory::generateRectangle(uiWidth, uiHeight,uiColor,bDashLine,&pVtxBuf, uiVtxCnt, &pIdxBuf, uiIdxCnt);
			pMesh->setVB(pVtxBuf, CCacheMesh::EMESHFMT_POSITION2|CCacheMesh::EMESHFMT_COLOR4, uiVtxCnt);
			pMesh->setIB(pIdxBuf, uiIdxCnt);
			pMesh->setPrimitiveMode(CCacheMesh::EMESHPM_LINES);
			pMesh->setName(strName);
			delete[](pVtxBuf);
			delete[](pIdxBuf);
			m_cachePool.addCacheObject(strName, pMesh);
		}

		return pMesh;
	}

	CCacheMesh* CMeshPool::getTriangleMesh(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine)
	{
		String strName = String::format(L"I:Triangle%d %d-%d %d-%d %d-%u-%d", p1.getX(),p1.getY(),
																				p2.getX(), p2.getY(),
																				p3.getX(), p3.getY(), uiColor, bDashLine);
		CCacheMesh* pMesh = m_cachePool.getCacheObject( strName);
		if( pMesh == NULL )
		{
			pMesh = new CCacheMesh();
			UINT uiVtxCnt = 0;
			UINT uiIdxCnt = 0;
			FLOAT* pVtxBuf = NULL;
			USHORT* pIdxBuf = NULL;
			CMeshFactory::generateTriangle(p1, p2, p3, uiColor, bDashLine,&pVtxBuf, uiVtxCnt, &pIdxBuf, uiIdxCnt);
			pMesh->setVB(pVtxBuf, CCacheMesh::EMESHFMT_POSITION2|CCacheMesh::EMESHFMT_COLOR4, uiVtxCnt);
			pMesh->setIB(pIdxBuf, uiIdxCnt);
			pMesh->setPrimitiveMode(CCacheMesh::EMESHPM_LINES);
			pMesh->setName(strName);
			delete[](pVtxBuf);
			delete[](pIdxBuf);
			m_cachePool.addCacheObject(strName, pMesh);
		}

		return pMesh;
	}

	CCacheMesh* CMeshPool::getLineMesh(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine)
	{
		String strName = String::format(L"I:Line%d-%d-%d-%d-%u-%d", iX1, iY1, iX2, iY2, uiColor, bDashLine);
		CCacheMesh* pMesh = m_cachePool.getCacheObject(strName);
		if( pMesh == NULL )
		{
			pMesh = new CCacheMesh();
			UINT uiVtxCnt = 0;
			UINT uiIdxCnt = 0;
			FLOAT* pVtxBuf = NULL;
			USHORT* pIdxBuf = NULL;
			CMeshFactory::generateLine(iX1, iY1, iX2, iY2,uiColor,bDashLine,&pVtxBuf, uiVtxCnt, &pIdxBuf, uiIdxCnt);
			pMesh->setVB(pVtxBuf, CCacheMesh::EMESHFMT_POSITION2|CCacheMesh::EMESHFMT_COLOR4, uiVtxCnt);
			pMesh->setIB(pIdxBuf, uiIdxCnt);
			pMesh->setPrimitiveMode(CCacheMesh::EMESHPM_LINES);
			pMesh->setName(strName);
			delete[](pVtxBuf);
			delete[](pIdxBuf);
			m_cachePool.addCacheObject(strName, pMesh);
		}

		return pMesh;
	}

	CCacheMesh* CMeshPool::getCircleMesh(UINT uiRadius, UINT uiColor, BOOLEAN bDashLine)
	{
		String strName = String::format(L"I:Circle%u-%u-%d", uiRadius, uiColor, bDashLine);
		CCacheMesh* pMesh = m_cachePool.getCacheObject(strName);
		if( pMesh == NULL )
		{
			pMesh = new CCacheMesh();
			UINT uiVtxCnt = 0;
			UINT uiIdxCnt = 0;
			FLOAT* pVtxBuf = NULL;
			USHORT* pIdxBuf = NULL;
			CMeshFactory::generateCircle(uiRadius,uiColor,bDashLine,&pVtxBuf, uiVtxCnt, &pIdxBuf, uiIdxCnt);
			pMesh->setVB(pVtxBuf, CCacheMesh::EMESHFMT_POSITION2|CCacheMesh::EMESHFMT_COLOR4, uiVtxCnt);
			pMesh->setIB(pIdxBuf, uiIdxCnt);
			pMesh->setPrimitiveMode(CCacheMesh::EMESHPM_LINES);
			pMesh->setName(strName);
			delete[](pVtxBuf);
			delete[](pIdxBuf);
			m_cachePool.addCacheObject(strName, pMesh);
		}

		return pMesh;
	}

	CCacheMesh* CMeshPool::getSolidRectangleMesh(UINT uiWidth,UINT uiHeight,UINT uiColor)
	{
		String strName = String::format(L"I:SolidRect%u-%u-%u", uiWidth, uiHeight, uiColor);
		CCacheMesh* pMesh = m_cachePool.getCacheObject(strName);
		if( pMesh == NULL )
		{
			pMesh = new CCacheMesh();
			UINT uiVtxCnt = 0;
			UINT uiIdxCnt = 0;
			FLOAT* pVtxBuf = NULL;
			USHORT* pIdxBuf = NULL;
			CMeshFactory::generateSolidRectangle(uiWidth, uiHeight,uiColor,&pVtxBuf, uiVtxCnt, &pIdxBuf, uiIdxCnt);
			pMesh->setVB(pVtxBuf, CCacheMesh::EMESHFMT_POSITION2|CCacheMesh::EMESHFMT_COLOR4, uiVtxCnt);
			pMesh->setIB(pIdxBuf, uiIdxCnt);
			pMesh->setPrimitiveMode(CCacheMesh::EMESHPM_TRIANGLES);
			pMesh->setName(strName);
			delete[](pVtxBuf);
			delete[](pIdxBuf);
			m_cachePool.addCacheObject(strName, pMesh);
		}

		return pMesh;
	}

	CDynamicUIMesh* CMeshPool::getDynamicUIMesh()
	{
		if( m_ptrDynamicUIMesh.isNull() )
		{
			m_ptrDynamicUIMesh = new CDynamicUIMesh();
			m_ptrDynamicUIMesh->setVertexFmt(CCacheMesh::EMESHFMT_POSITION3 | CCacheMesh::EMESHFMT_TEXCOORD0);
			m_ptrDynamicUIMesh->resizeQuad(100);
			m_ptrDynamicUIMesh->reset();
		}

		return m_ptrDynamicUIMesh;
	}

	CCacheMesh* CMeshPool::getWaverMesh(UINT uiWidth, UINT uiHeight, UINT uiRow, UINT uiCol)
	{
		String strName = String::format(L"I:Waver%u-%u-%u-%u", uiWidth, uiHeight, uiRow,uiCol);
		CCacheMesh* pMesh = m_cachePool.getCacheObject(String::toString(g_uiSessionID) + strName);
		if( pMesh == NULL )
		{
			pMesh = new CCacheMesh(CCacheMesh::EMESHTYPE_GPU);
			UINT uiVtxCnt = 0;
			UINT uiIdxCnt = 0;
			FLOAT* pVtxBuf = NULL;
			USHORT* pIdxBuf = NULL;
			CMeshFactory::generatePlaneMeshH(uiWidth,uiHeight,uiRow,uiCol, NULL, &pVtxBuf, uiVtxCnt, &pIdxBuf, uiIdxCnt);
			pMesh->setName(String::toString(g_uiSessionID) + strName);
			pMesh->setVBO(pVtxBuf, CCacheMesh::EMESHFMT_POSITION3 | CCacheMesh::EMESHFMT_NORMAL | CCacheMesh::EMESHFMT_TEXCOORD0, uiVtxCnt);
			pMesh->setIBO(pIdxBuf, uiIdxCnt);
			
			delete[](pVtxBuf);
			delete[](pIdxBuf);

			m_cachePool.addCacheObject(String::toString(g_uiSessionID) + strName, pMesh);
		}

		return pMesh;
	}

	CCacheMesh* CMeshPool::getRippleMesh(UINT uiWidth, UINT uiHeight, UINT uiRow, UINT uiCol)
	{
		String strName = String::format(L"I:Waver%u-%u-%u-%u", uiWidth, uiHeight, uiRow,uiCol);
		CCacheMesh* pMesh = m_cachePool.getCacheObject(strName);
		if( pMesh == NULL )
		{
			pMesh = new CCacheMesh();
			UINT uiVtxCnt = 0;
			UINT uiIdxCnt = 0;
			FLOAT* pVtxBuf = NULL;
			USHORT* pIdxBuf = NULL;
			CMeshFactory::generatePlaneMeshV(uiWidth,uiHeight,uiRow,uiCol,&pVtxBuf, uiVtxCnt, &pIdxBuf, uiIdxCnt);
			
			pMesh->setName(strName);
			pMesh->setVB(pVtxBuf, CCacheMesh::EMESHFMT_POSITION3 | CCacheMesh::EMESHFMT_TEXCOORD0, uiVtxCnt);
			pMesh->setIB(pIdxBuf, uiIdxCnt);
			
			delete[](pVtxBuf);
			delete[](pIdxBuf);
			m_cachePool.addCacheObject(strName, pMesh);
		}

		return pMesh;
	}

	CCacheMesh* CMeshPool::getDynamicUIGPUMesh(UINT uiFmt,UINT uiVtxCount,UINT uiIdxCount)
	{
		String strName = String::format(L"I:DynUIGPUMesh%u-%u-%u", uiFmt, uiVtxCount, uiIdxCount);
		CCacheMesh* pMesh = m_cachePool.getCacheObject(String::toString(g_uiSessionID) + strName);
		if( pMesh == NULL )
		{
			pMesh = new CCacheMesh(CCacheMesh::EMESHTYPE_GPU);
			pMesh->setVBO(NULL,uiFmt,uiVtxCount,CCacheMesh::EMESHVU_DYNAMIC);
			pMesh->setIBO(NULL,uiIdxCount);

			pMesh->setName(String::toString(g_uiSessionID) + strName);
			m_cachePool.addCacheObject(String::toString(g_uiSessionID) + strName, pMesh);
		}

		return pMesh;
	}
	
	void CMeshPool::freeMesh(CCacheMesh* pMesh)
	{
		if ( pMesh != NULL )
		{
			m_cachePool.deleteCacheObject(pMesh->getName());
		}
	}
}