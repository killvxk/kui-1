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

		void	drawGroup(CRenderGroup *pRndGroup, INT iOffsetX, INT iOffsetY, CDIB * pDestDib, BOOLEAN *pbHasContent);
		void	drawText(CRenderText *pRndText, INT iOffsetX, INT iOffsetY, CDIB * pDestDib, BOOLEAN *pbHasContent);
		void	drawImage(CRenderImage *pRndImage, INT iOffsetX, INT iOffsetY, CDIB * pDestDib, BOOLEAN *pbHasContent);
		void	drawViewport(CRenderViewport* pRndViewport, INT iOffsetX, INT iOffsetY, CDIB* pDestDib, BOOLEAN *pbHasContent);

		void	drawRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine = FALSE);	
		void	drawTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine = FALSE);
		void	drawLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine = FALSE);
		void	drawCircle(INT iX, INT iY, UINT iRadius, UINT uiColor, BOOLEAN bDashLine = FALSE);		
		void	fillRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor);
	
		void	endDraw();
		void	swapBuffer();
		void	reDraw();

	protected:// method	
		CRenderEntry(const CRenderEntry &){}
		CRenderEntry &operator = (const CRenderEntry &){return *this;}

	private:// method
		void	initialize();
		void	unInitialize();

		void	drawGroupChildren(CRenderGroup *pRndGroup, INT iOffsetX, INT iOffsetY, CDIB * pDestDib, BOOLEAN *pbHasContent);

	protected:// property

	private:// property
		BOOLEAN				m_bInitialized;
		
		BOOLEAN				m_bMultiSession;
		tagScreenContext	m_screenContext;
		tagRenderContext	m_renderContext;		

		CDIB				m_dib;
	};
}