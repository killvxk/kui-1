#include "RenderEngine.h"

namespace OpenHMI
{
	CImpRenderSession::CImpRenderSession()
	{
		;
	}

	CImpRenderSession::~CImpRenderSession()
	{
		;
	}

	void CImpRenderSession::beginDraw(const tagScreenContext *pScreenContext)
	{
		m_renderEntry.beginDraw(pScreenContext);
	}

	void CImpRenderSession::drawGroup(CRenderGroup *pRndGroup)
	{
		m_renderEntry.drawGroup(pRndGroup, 0, 0, NULL, NULL);
	}

	void CImpRenderSession::drawText(CRenderText *pRndText)
	{
		m_renderEntry.drawText(pRndText, 0, 0, NULL, NULL);
	}

	void CImpRenderSession::drawImage(CRenderImage *pRndImage)
    {  
        m_renderEntry.drawImage(pRndImage, 0, 0, NULL, NULL);
	}

	void CImpRenderSession::drawRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine)
	{
        m_renderEntry.drawRectangle(iX, iY, uiWidth, uiHeight, uiColor, bDashLine);
	}	

	void CImpRenderSession::drawTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine)
	{
		m_renderEntry.drawTriangle(p1, p2, p3,uiColor, bDashLine);
	}

	void CImpRenderSession::drawLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine)
	{
		 m_renderEntry.drawLine(iX1, iY1, iX2, iY2, uiColor, bDashLine);
	}

	void CImpRenderSession::drawCircle(INT iX, INT iY, UINT uiRadius, UINT uiColor, BOOLEAN bDashLine)
	{
		m_renderEntry.drawCircle(iX, iY, uiRadius, uiColor, bDashLine);
	}

	void CImpRenderSession::fillRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor)
	{
		m_renderEntry.fillRectangle(iX, iY, uiWidth, uiHeight, uiColor);
	}

	void CImpRenderSession::endDraw()
	{
		m_renderEntry.endDraw();
	}

	void CImpRenderSession::swapBuffer()
	{
		m_renderEntry.swapBuffer();
	}

	void CImpRenderSession::drawSelectRectangle(void *hdc, INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor)
	{
#ifdef OS_WIN

		m_renderEntry.reDraw();

		//int iOld = ::SetROP2((HDC)hdc, R2_XORPEN);

		HPEN hNewPen = ::CreatePen(PS_DOT, 1, RGB((uiColor >> 16) & 0xFF, (uiColor >> 8) & 0xFF, uiColor & 0xFF));
		HPEN hOldPen =(HPEN) ::SelectObject((HDC)hdc, hNewPen);

		::MoveToEx((HDC)hdc, iX, iY, NULL);
		::LineTo((HDC)hdc, iX + uiWidth - 1, iY);
		::LineTo((HDC)hdc, iX + uiWidth - 1, iY + uiHeight - 1);
		::LineTo((HDC)hdc, iX, iY + uiHeight - 1);
		::LineTo((HDC)hdc, iX, iY);

		::SelectObject((HDC)hdc, hOldPen);
		//::SetROP2((HDC)hdc, iOld);

		::DeleteObject(hNewPen);
#endif
	}

	void CImpRenderSession::drawSelectRectangles(void *hdc, INT *piX, INT *piY, UINT *puiWidth, UINT *puiHeight, UINT uiCount, UINT uiColor)
	{
#ifdef OS_WIN

		m_renderEntry.reDraw();

		//int iOld = ::SetROP2((HDC)hdc, R2_XORPEN);

		HPEN hNewPen = ::CreatePen(PS_DOT, 1, RGB((uiColor >> 16) & 0xFF, (uiColor >> 8) & 0xFF, uiColor & 0xFF));
		HPEN hOldPen =(HPEN) ::SelectObject((HDC)hdc, hNewPen);

		for (UINT i = 0; i < uiCount; i++)
		{
			::MoveToEx((HDC)hdc, piX[i], piY[i], NULL);
			::LineTo((HDC)hdc, piX[i] + puiWidth[i] - 1, piY[i]);
			::LineTo((HDC)hdc, piX[i] + puiWidth[i] - 1, piY[i] + puiHeight[i] - 1);
			::LineTo((HDC)hdc, piX[i], piY[i] + puiHeight[i] - 1);
			::LineTo((HDC)hdc, piX[i], piY[i]);
		}		

		::SelectObject((HDC)hdc, hOldPen);
		//::SetROP2((HDC)hdc, iOld);

		::DeleteObject(hNewPen);
#endif
	}
}