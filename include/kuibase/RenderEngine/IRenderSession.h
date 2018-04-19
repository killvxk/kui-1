/*@Author Zeki.Yan*/
#pragma once

#include "../Common.h"

namespace OpenHMI
{
	class OPENHMI_API IRenderSession
	{
	public:// method
		virtual			~IRenderSession(){}

		virtual	void	beginDraw(const tagScreenContext *pScreenContext) = 0;		

		virtual void	drawGroup(CRenderGroup *pRndGroup) = 0;
		virtual	void	drawText(CRenderText *pRndText) = 0;
		virtual	void	drawImage(CRenderImage *pRndImage) = 0;
		virtual void	drawRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine = FALSE) = 0;
		virtual void	drawTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine = FALSE) = 0;
		virtual void	drawLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine = FALSE) = 0;
		virtual void	drawCircle(INT iX, INT iY, UINT iRadius, UINT uiColor, BOOLEAN bDashLine = FALSE) = 0;		
		virtual void	fillRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor) = 0;
		
		virtual	void	endDraw() = 0;
		virtual	void	swapBuffer() = 0;

		virtual void	drawSelectRectangle(void *hdc, INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor) = 0;
		virtual void	drawSelectRectangles(void *hdc, INT *piX, INT *piY, UINT *puiWidth, UINT *puiHeight, UINT uiCount, UINT uiColor) = 0;
	};
}