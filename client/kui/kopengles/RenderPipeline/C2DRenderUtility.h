#pragma once

namespace OpenHMI
{
	class C2DRenderUtility : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		C2DRenderUtility(CRenderEntry& rOwner);
		virtual ~C2DRenderUtility();

		void	initialize();
		void	unInitialize();

		void	drawText(CRenderText *pRndText, C2DBatchRenderGroup* p2DBRG);
		void	drawImage(CRenderImage *pRndImage, C2DBatchRenderGroup* p2DBRG);
		void	drawViewport(CRenderViewport* pRndViewport, C2DBatchRenderGroup* p2DBRG,UINT uiViewportSiblingCnt);

		void	drawRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine, C2DBatchRenderGroup* p2DBRG);	
		void	drawTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine, C2DBatchRenderGroup* p2DBRG);
		void	drawLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine, C2DBatchRenderGroup* p2DBRG);
		void	drawCircle(INT iX, INT iY, UINT uiRadius, UINT uiColor, BOOLEAN bDashLine, C2DBatchRenderGroup* p2DBRG);		
		void	fillRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, C2DBatchRenderGroup* p2DBRG);

	protected:// method	
		//C2DVisitor(const C2DVisitor &){}
		//C2DVisitor &operator = (const C2DVisitor &){return *this;}

	private:// method
		void	calUseMergeTexTextRoi(CRenderTextCacheData* pCacheData,CRectangleF& rcTex);

	protected:// property

	private:// property
		CRenderEntry&			m_rRenderEntry;
		CCacheMeshPtr			m_pOpenArtistMesh;
	};
}