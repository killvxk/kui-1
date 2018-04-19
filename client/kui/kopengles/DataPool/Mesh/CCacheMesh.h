#pragma once

namespace OpenHMI
{
	class CCacheMesh : public CacheGPUObject
	{
	public:// const define
		enum EMeshhType
		{
			EMESHTYPE_MEMORY	= 0X0000001,
			EMESHTYPE_GPU		= 0X0000002
		};

		enum EMeshFmt
		{
			EMESHFMT_POSITION2	= 0X00000001,
			EMESHFMT_POSITION3	= 0X00000002,
			EMESHFMT_NORMAL		= 0X00000004,
			EMESHFMT_TEXCOORD0	= 0X00000008,
			EMESHFMT_COLOR4		= 0X00000010,
		};

		enum EMeshVBOUsage
		{
			EMESHVU_STATIC		= GL_STATIC_DRAW,
			EMESHVU_DYNAMIC		= GL_DYNAMIC_DRAW,
		};

		enum EMeshPrimitiveMode
		{
			EMESHPM_TRIANGLES		= GL_TRIANGLES,
			EMESHPM_TRIANGLEFAN		= GL_TRIANGLE_FAN,
			EMESHPM_TRIANGLETRIP	= GL_TRIANGLE_STRIP,
			EMESHPM_LINES			= GL_LINES,
			EMESHPM_LINESTRIP		= GL_LINE_STRIP,
			EMESHPM_LINELOOP		= GL_LINE_LOOP,
			EMESHPM_POINTS			= GL_POINTS
		};

		enum EMeshDataType
		{
			EMDT_VB,
			EMDT_IB,
			EMDT_ALL,
		};

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CCacheMesh(EMeshhType eType = EMESHTYPE_MEMORY);
		//CCacheMesh();
		virtual ~CCacheMesh();

		EMeshhType		getType()	const				{ return m_eType;			}
		const GLfloat*	getVertixes() const				{ return m_pfVertexes;		}
		const GLushort*	getIndexes() const				{ return m_pusIndexes;		}
		UINT			getVertexDataCount() const		{ return m_uiVertexDataCnt; }
		UINT			getIndexDataCount() const		{ return m_uiIdxDataCnt;	}

		void	setVBO(const FLOAT* pVertexData, UINT uiFmt, UINT uiCount, EMeshVBOUsage eUsage = EMESHVU_STATIC);
		void	setIBO(const USHORT* pIndexData, UINT uiCount, EMeshVBOUsage eUsage = EMESHVU_STATIC);
		void	fillVBO(const FLOAT* pData, UINT uiCount);	
		void	fillIBO(const USHORT* pData, UINT uiCount);

		BOOLEAN	useVBO() const							{ return (m_iVBO > 0);	}
		BOOLEAN useIBO() const							{ return (m_iIBO > 0);	}

		UINT	getVBO() const							{ return m_iVBO;		}
		UINT	getIBO() const							{ return m_iIBO;		}

		void	setVB(const FLOAT* pVertexData, UINT uiFmt, UINT uiCount);
		void	setIB(const USHORT* pIndexData, UINT uiCount);
		void	fillVB(const FLOAT* pData, UINT uiCount);
		void	fillIB(const USHORT*pData, UINT uiCount);

		void	lock(FLOAT** ppVtxData, USHORT** ppIdxData = NULL);
		void	unLock();

		BOOLEAN isCanAddVB(UINT uiCount) const;
		BOOLEAN isCanAddIB(UINT uiCount) const;
		void	reset(EMeshDataType eType = EMDT_ALL);

		void	setVertexFmt(UINT uiVertexFmt);
		UINT	getVertexFmt() const					{ return m_uiVertexFmt;				}
		UINT	getVertexStride() const					{ return m_uiVertexStride;			}
		UINT	getVertexCapacity() const				{ return m_uiVertexDataCapacity;	}
		UINT	getIdxCapacity() const					{ return m_uiIdxDataCapacity;		}

		void				setPrimitiveMode(EMeshPrimitiveMode eMode)		{ m_ePrimitiveMode = eMode;	}
		EMeshPrimitiveMode	getPrimitiveMode() const						{ return m_ePrimitiveMode;	}

	protected:// method
		CCacheMesh(const CCacheMesh &){}
		CCacheMesh &operator = (const CCacheMesh &){return *this;}	

		static UINT getVertexStride(UINT uiFmt);

	private:// method
		

	protected:// property
		EMeshhType			m_eType;
		GLfloat				*m_pfVertexes;
		GLushort			*m_pusIndexes;
		UINT				m_uiVertexDataCnt;
		UINT				m_uiIdxDataCnt;
		UINT				m_uiVertexDataCapacity;
		UINT				m_uiIdxDataCapacity;
		UINT				m_uiVertexFmt;
		UINT				m_uiVertexStride;
		UINT				m_iVBO;
		UINT				m_iIBO;
		EMeshPrimitiveMode	m_ePrimitiveMode;

	private:// property	
		
	};

	typedef ObjectPtr<CCacheMesh> CCacheMeshPtr;
}