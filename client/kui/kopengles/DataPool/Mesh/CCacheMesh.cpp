#include "RenderEngine.h"

namespace OpenHMI
{
	CCacheMesh::CCacheMesh(EMeshhType eType)
		: m_eType(eType)
		, m_pfVertexes(NULL)
		, m_pusIndexes(NULL)
		, m_uiVertexDataCnt(0)
		, m_uiIdxDataCnt(0)
		, m_uiVertexDataCapacity(0)
		, m_uiIdxDataCapacity(0)
		, m_uiVertexFmt(0)
		, m_uiVertexStride(0)
		, m_ePrimitiveMode(EMESHPM_TRIANGLES)
		, m_iVBO(0)
		, m_iIBO(0)
	{
		;
	}

	CCacheMesh::~CCacheMesh()
	{
		if( m_eType == CCacheMesh::EMESHTYPE_MEMORY )
		{
			if (m_pfVertexes != NULL)
			{
				delete [] m_pfVertexes;
				m_pfVertexes = NULL;
			}

			if (m_pusIndexes != NULL)
			{
				delete [] m_pusIndexes;
				m_pusIndexes = NULL;
			}
		}
		else if( m_eType == CCacheMesh::EMESHTYPE_GPU )
		{
			if( m_iVBO > 0 || m_iIBO > 0 )
			{
				changeRenderContext();
				if ( m_iVBO > 0 )
				{
					glDeleteBuffers(1,&m_iVBO);
					

					GLenum error = glGetError();
					if ( error == GL_INVALID_OPERATION )
					{
						CLog::error2(String(RENDER_ENGINE_NAME),String::format(L"GLERROR CCacheMesh VERTEX GL_INVALID OPERATION %u", m_iVBO));
					}
					else if( error != GL_NO_ERROR )
					{
						CLog::error2(String(RENDER_ENGINE_NAME),String::format(L"GLERROR CCacheMesh VERTEX %u", m_iVBO));
					}

					m_iVBO = 0;
				}

				if ( m_iIBO > 0 )
				{
					glDeleteBuffers(1,&m_iIBO);

					GLenum error = glGetError();
					if ( error == GL_INVALID_OPERATION )
					{
						CLog::error2(String(RENDER_ENGINE_NAME),String::format(L"GLERROR CCacheMesh INDEX GL_INVALID OPERATION %u", m_iIBO));
					}
					else if( error != GL_NO_ERROR )
					{
						CLog::error2(String(RENDER_ENGINE_NAME),String::format(L"GLERROR CCacheMesh VERTEX %u", m_iIBO));
					}

					m_iIBO = 0;
				}
				resetRenderContext();
			}
		}
	}

	void CCacheMesh::setVBO(const FLOAT* pVertexData, UINT uiFmt, UINT uiCount, EMeshVBOUsage eUsage)
	{
		if ( m_iVBO != 0 && m_uiVertexDataCapacity < uiCount )
		{
			glDeleteBuffers(1,&m_iVBO);
			m_iVBO = 0;
			m_uiVertexDataCnt = 0;
			m_uiVertexDataCapacity = 0;
		}

		if ( m_iVBO == 0 )
		{
			m_uiVertexFmt = uiFmt;
			m_uiVertexStride = getVertexStride(uiFmt);
			m_uiVertexDataCapacity = uiCount;

			GLuint id;
			glGenBuffers(1, &id);

			glBindBuffer( GL_ARRAY_BUFFER, id );
			glBufferData( GL_ARRAY_BUFFER, m_uiVertexStride * uiCount * sizeof(FLOAT), pVertexData, (GLenum)eUsage);
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			m_iVBO = id;
		}

		if ( pVertexData != NULL)
		{
			m_uiVertexDataCnt = uiCount;
		}
	}

	void CCacheMesh::setIBO(const USHORT* pIndexData, UINT uiCount, EMeshVBOUsage eUsage)
	{
		if ( m_iIBO != 0 && m_uiIdxDataCapacity < uiCount )
		{
			glDeleteBuffers(1,&m_iIBO);
			m_iIBO = 0;
			m_uiIdxDataCnt = 0;
			m_uiIdxDataCapacity = 0;
		}

		if ( m_iIBO == 0 )
		{
			m_uiIdxDataCapacity = uiCount;

			GLuint id;
			glGenBuffers(1, &id);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id );
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, uiCount*sizeof(USHORT), pIndexData, (GLenum)eUsage);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );

			m_iIBO = id;
		}

		if ( pIndexData != NULL)
		{
			m_uiIdxDataCnt = uiCount;
		}
	}

	void CCacheMesh::fillVBO(const FLOAT* pData, UINT uiCount)
	{
		if ( uiCount > m_uiVertexDataCapacity )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"FILLVBO out count");
			return;
		}

		UINT uiOffset = m_uiVertexDataCnt;
		m_uiVertexDataCnt += uiCount;
		if ( m_uiVertexDataCnt > m_uiVertexDataCapacity )
		{
			reset();
			m_uiVertexDataCnt += uiCount;
			uiOffset = 0;	
		}
#ifdef OS_QNX
	#ifdef CHIPSET_IMX6SOLO
		glBufferData(GL_ARRAY_BUFFER,uiCount*m_uiVertexStride*sizeof(FLOAT), pData,GL_STATIC_DRAW);
	#else
		glBufferSubData(GL_ARRAY_BUFFER,uiOffset * m_uiVertexStride * sizeof(FLOAT), uiCount*m_uiVertexStride*sizeof(FLOAT), pData);
	#endif
#else
		glBufferSubData(GL_ARRAY_BUFFER,uiOffset * m_uiVertexStride * sizeof(FLOAT), uiCount*m_uiVertexStride*sizeof(FLOAT), pData);
#endif
		GLenum error = glGetError();
	}

	void CCacheMesh::fillIBO(const USHORT* pData, UINT uiCount)
	{
		if ( uiCount > m_uiIdxDataCapacity )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"FILLIBO out count");
			return;
		}

		UINT uiOffset = m_uiIdxDataCnt;
		m_uiIdxDataCnt += uiCount;
		if ( m_uiIdxDataCnt > m_uiIdxDataCapacity )
		{
			reset();
			m_uiIdxDataCnt += uiCount;
			uiOffset = 0;
		}

		//glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iVBO);
#ifdef OS_QNX
	#ifdef CHIPSET_IMX6SOLO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, uiCount*sizeof(USHORT), pData,GL_STATIC_DRAW);
	#else
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, uiOffset * sizeof(USHORT), uiCount*sizeof(USHORT), pData);
	#endif
#else
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, uiOffset * sizeof(USHORT), uiCount*sizeof(USHORT), pData);
#endif
		//glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	}

	void CCacheMesh::setVB(const FLOAT* pVertexData, UINT uiFmt, UINT uiCount)
	{
		m_uiVertexFmt = uiFmt;
		m_uiVertexStride = getVertexStride(uiFmt);
		m_uiVertexDataCapacity = uiCount;

		m_pfVertexes = new FLOAT[uiCount*m_uiVertexStride];
		if(pVertexData)
		{
			m_uiVertexDataCnt = uiCount;

			memcpy(m_pfVertexes, pVertexData, uiCount * m_uiVertexStride * sizeof(FLOAT));

		}
	}

	void CCacheMesh::setIB(const USHORT* pIndexData, UINT uiCount)
	{
		m_uiIdxDataCapacity = uiCount;

		m_pusIndexes = new USHORT[uiCount];

		if(pIndexData)
		{
			m_uiIdxDataCnt = uiCount;

			memcpy(m_pusIndexes , pIndexData, uiCount * sizeof(USHORT));

		}
	}

	void CCacheMesh::fillVB(const FLOAT* pData, UINT uiCount)
	{
		if ( uiCount > m_uiVertexDataCapacity )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"FILLVBO out count");
			return;
		}

		UINT uiOffset = m_uiVertexDataCnt;
		m_uiVertexDataCnt += uiCount;
		if ( m_uiVertexDataCnt > m_uiVertexDataCapacity )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"FILLVBO out count");
			return;
		}
		
		memcpy(m_pfVertexes + uiOffset * m_uiVertexStride, pData, uiCount * m_uiVertexStride * sizeof(FLOAT));
		
	}

	void CCacheMesh::fillIB(const USHORT*pData, UINT uiCount)
	{
		if ( uiCount > m_uiIdxDataCapacity )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"FILLIBO out count");
			return;
		}

		UINT uiOffset = m_uiIdxDataCnt;
		m_uiIdxDataCnt += uiCount;
		if ( m_uiIdxDataCnt > m_uiIdxDataCapacity )
		{
			CLog::error2(RENDER_ENGINE_NAME,L"FILLIBO out count");
			return;
		}

		memcpy(m_pusIndexes + uiOffset, pData, uiCount * sizeof(USHORT));
		
	}

	void CCacheMesh::lock(FLOAT** ppfVertexes, USHORT** ppusIndexes)
	{
		if( ppfVertexes )
		{
			*ppfVertexes = m_pfVertexes;
		}

		if( ppusIndexes )
		{
			*ppusIndexes = m_pusIndexes;
		}
	}

	void CCacheMesh::unLock()
	{
		;
	}

	BOOLEAN CCacheMesh::isCanAddVB(UINT uiCount) const
	{
		UINT uiOffset = m_uiVertexDataCnt;
		uiOffset += uiCount;
		if ( uiOffset > m_uiVertexDataCapacity )
		{
			return FALSE;
		}
		return TRUE;
	}

	BOOLEAN CCacheMesh::isCanAddIB(UINT uiCount) const
	{
		UINT uiOffset = m_uiIdxDataCnt;
		uiOffset += uiCount;
		if ( uiOffset > m_uiIdxDataCapacity )
		{
			return FALSE;
		}
		return TRUE;
	}

	void CCacheMesh::reset(EMeshDataType eType)
	{
		if ( eType == EMDT_VB )
		{
			m_uiVertexDataCnt = 0;
		}
		else if( eType == EMDT_IB )
		{
			m_uiIdxDataCnt = 0;
		}
		else if( eType == EMDT_ALL )
		{
			m_uiIdxDataCnt = 0;
			m_uiVertexDataCnt = 0;
		}
	}

	void CCacheMesh::setVertexFmt(UINT uiVertexFmt)
	{
		m_uiVertexFmt = uiVertexFmt;	
		m_uiVertexStride = getVertexStride(uiVertexFmt);	
	}

	UINT CCacheMesh::getVertexStride(UINT uiFmt)
	{
		UINT uiStride = 0;
		if( uiFmt & EMESHFMT_POSITION2)
		{
			uiStride +=2;
		}
		else if ( uiFmt & EMESHFMT_POSITION3)
		{
			uiStride +=3;
		}

		if ( uiFmt & EMESHFMT_NORMAL )
		{
			uiStride +=3;
		}

		if ( uiFmt & EMESHFMT_TEXCOORD0)
		{
			uiStride += 2;
		}

		if ( uiFmt & EMESHFMT_COLOR4 )
		{
			uiStride += 4;
		}

		return uiStride;
	}
}