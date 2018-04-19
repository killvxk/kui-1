#pragma once

namespace OpenHMI
{
	class CRenderEntry : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CRenderEntry();
		virtual ~CRenderEntry();

		void	beginDraw(const tagScreenContext *pScreenContext);		

		void	drawGroup(CRenderGroup *pRndGroup, INT iOffsetX, INT iOffsetY, CDIB * pDestDib);
		void	drawText(CRenderText *pRndText);
		void	drawImage(CRenderImage *pRndImage);
		void	drawViewport(CRenderViewport* pRndViewport);

		void	drawRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine = FALSE);	
		void	drawTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine = FALSE);
		void	drawLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine = FALSE);
		void	drawCircle(INT iX, INT iY, UINT uiRadius, UINT uiColor, BOOLEAN bDashLine = FALSE);		
		void	fillRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor);

		void	endDraw();
		void	swapBuffer();
		void	reDraw();

		const CRectangle&		getClipRect() const								{ return m_rcClipBox;	}
		void					setClipRect(const CRectangle& rc)				{ m_rcClipBox = rc;	}

		BOOLEAN					isNeedCacheRenderGroup(CRenderGroup* ) const;
		CBatchRenderGroup*		getCurrentBatchRenderGroup()					{ return m_pCurBatchRenderGroup;	}
		C2DBatchRenderGroup*	getCurrent2DBatchRenderGroup();
		C3DBatchRenderGroup*	getCurrent3DBatchRenderGroup();
		void					setCurrentBatchRenderGroup(CBatchRenderGroup* p){ m_pCurBatchRenderGroup = p;		}
		C2DBatchRenderGroup*	getFree2DBatchRenderGroup();
		void					free2DBatchRenderGroup(C2DBatchRenderGroup* p);
		C3DBatchRenderGroup*	getFree3DBatchRenderGroup();
		void					free3DBatchRenderGroup(C3DBatchRenderGroup* p);
		void					addBatchRenderGroup(CBatchRenderGroup* p);

		CRenderState*			getRenderState()								{ return &m_renderState;	}

	protected:// method	
		CRenderEntry(const CRenderEntry &){}
		CRenderEntry &operator = (const CRenderEntry &){return *this;}

	private:// method
		void		initialize();
		void		unInitialize();

		void		drawGroupChildren(CRenderGroup* pRndGroup);
		void		process2Dfilter(CRenderGroup* pRndGroup, C2DBatchRenderGroup* pCurrentBRG);

		void		printRenderTree(CRenderGroup* pRndGroup,const String& str);

	protected:// property

	private:// property
		BOOLEAN				m_bInitialized;	

		UINT				m_uiSessionID;
		BOOLEAN				m_bMultiSession;
		tagScreenContext	m_screenContext;
		tagRenderContext	m_renderContext;

		CRectangle			m_rcClipBox;
		CBatchRenderGroup*	m_pCurBatchRenderGroup;
		
		C2DBatchRenderGroup				m_batchRenderGroup;
		Array2<CBatchRenderGroupPtr>	m_aryBatchRenderGroup;
		
		CPool<C2DBatchRenderGroup>		m_pool2DBatchRenderGroup;
		CPool<C3DBatchRenderGroup>		m_pool3DBatchRenderGroup;

		CRenderState					m_renderState;
		C2DRenderUtility*				m_p2DRenderUtility;

		CDIB							m_dib;
		BOOLEAN							m_bDIBDirty;
	};
}