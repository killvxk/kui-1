#pragma once

#include "OpenHMI.h"

namespace OpenHMI
{
	class CImpRenderSession : public IRenderSession
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CImpRenderSession();	
		virtual ~CImpRenderSession();

		//---start---implement IRenderSession
		virtual	void	beginDraw(const tagScreenContext *pScreenContext);		

		virtual void	drawGroup(CRenderGroup *pRndGroup);
		virtual	void	drawText(CRenderText *pRndText);
		virtual	void	drawImage(CRenderImage *pRndImage);
		virtual void	drawRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine = FALSE);
		virtual void	drawTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine = FALSE);		
		virtual void	drawLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine = FALSE);
		virtual void	drawCircle(INT iX, INT iY, UINT iRadius, UINT uiColor, BOOLEAN bDashLine = FALSE);		
		virtual void	fillRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor);		

		virtual	void	endDraw();
		virtual void	swapBuffer();

		virtual void	drawSelectRectangle(void *hdc, INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor);
		virtual void	drawSelectRectangles(void *hdc, INT *piX, INT *piY, UINT *puiWidth, UINT *puiHeight, UINT uiCount, UINT uiColor);
		//---end---implement IRenderSession

	protected:// method		

	private:// method

	protected:// property

	private:// property
		CRenderEntry	m_renderEntry;
	};
}