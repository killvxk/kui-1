#include "RenderEngine.h"

namespace OpenHMI
{
	#define DASH_LINE_WIDTH		3	//(pixel 3 space 3)

	#define CHECK_DIB_BUFFER	\
	if (m_pbtPixelData == NULL)\
	{\
		return;\
	}

	#define RENDER_8_PIXEL(pPixelDest, btSrc, pixelSrcColor)	\
	{\
		UINT uiResultAlpha = ALPHA_255_TO_256(btSrc);\
		\
		pPixelDest->btB = (((pixelSrcColor.btB - pPixelDest->btB) * uiResultAlpha) >> 8) + pPixelDest->btB;\
		pPixelDest->btG = (((pixelSrcColor.btG - pPixelDest->btG) * uiResultAlpha) >> 8) + pPixelDest->btG;\
		pPixelDest->btR = (((pixelSrcColor.btR - pPixelDest->btR) * uiResultAlpha) >> 8) + pPixelDest->btR;\
		pPixelDest->btA = (((pixelSrcColor.btA - pPixelDest->btA) * uiResultAlpha) >> 8) + pPixelDest->btA;\
	}

 	#define RENDER_32_PIXEL(pPixelDest, pPixelSrc)	\
 	{\
 		UINT uiResultAlpha = ALPHA_255_TO_256(255 - pPixelSrc->btA);\
 	\
 		pPixelDest->btB = pPixelSrc->btB + ((pPixelDest->btB * uiResultAlpha) >> 8);\
 		pPixelDest->btG = pPixelSrc->btG + ((pPixelDest->btG * uiResultAlpha) >> 8);\
 		pPixelDest->btR = pPixelSrc->btR + ((pPixelDest->btR * uiResultAlpha) >> 8);\
 		pPixelDest->btA = pPixelSrc->btA + ((pPixelDest->btA * uiResultAlpha) >> 8);\
 	}

// 	#define REPLACE_32_PIXEL(pPixelDest, pPixelSrc)	\
// 	{\
// 		pPixelDest->btB = pPixelSrc->btB;\
// 		pPixelDest->btG = pPixelSrc->btG;\
// 		pPixelDest->btR = pPixelSrc->btR;\
// 		pPixelDest->btA = pPixelSrc->btA;\
// 	}

	#define REPLACE_32_PIXEL(pPixelDest, pPixelSrc)	\
	{\
		*pPixelDest = *pPixelSrc;\
	}

	//#define RENDER_32_PIXEL(pPixelDest, pPixelSrc) \
	//{\
	//	UINT uiSrcAlpha  = ALPHA_255_TO_256(pPixelSrc->btA);\
	//	UINT uiDestAlpha = ALPHA_255_TO_256(pPixelDest->btA);\
	//	\
	//	pPixelDest->btR = ((pPixelSrc->btR * uiSrcAlpha) >> 8) + (((256 - uiSrcAlpha) * pPixelDest->btR) >> 8);\
	//	pPixelDest->btG = ((pPixelSrc->btG * uiSrcAlpha) >> 8) + (((256 - uiSrcAlpha) * pPixelDest->btG) >> 8);\
	//	pPixelDest->btB = ((pPixelSrc->btB * uiSrcAlpha) >> 8) + (((256 - uiSrcAlpha) * pPixelDest->btB) >> 8);\
	//	\
	//	pPixelDest->btA = (255 * (uiSrcAlpha + uiDestAlpha) - uiSrcAlpha * uiDestAlpha) >> 8;\
	//}

	//	pPixelDest->btA = (pPixelSrc->btA + pPixelDest->btA) + ( (uiSrcAlpha * pPixelDest->btA) >> 8);\

	// 访问一个点的函数，(x,y)坐标可能超出图片边界； 
	// 边界处理模式:边界饱和
	#define BILINEAR_GET_PIXEL(pixelResult, bmp, x, y)	\
	{\
		LONG lX1 = x < 0 ? 0 : (x >= bmp.iWidth ? bmp.iWidth - 1 : x);\
		LONG lY1 = y < 0 ? 0 : (y >= bmp.iHeight ? bmp.iHeight - 1 : y);\
		pixelResult = ((tagPixel*)(bmp.pbtData + bmp.iStride * lY1))[lX1];\
	}	

	CDIB::CDIB()
		: m_pbtPixelData(NULL)
		, m_uiWidth(0)
		, m_uiHeight(0)
		, m_pDIBInfo(NULL)
		, m_eMergeState(MS_ALPHA_BLEND)
	{
		;
	}

	CDIB::~CDIB()
	{
		destroyDIB();
	}

	void CDIB::writeToFile(const String &strFilePath)
	{
#ifdef OS_WIN
		CFile file;
		file.open(strFilePath,  FALSE);

		BITMAPFILEHEADER bf;
		memset(&bf, 0, sizeof(BITMAPFILEHEADER));		
		bf.bfType = CHAR('M') << 8 | 'B';
		bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		bf.bfSize = bf.bfOffBits + m_uiWidth * 4 * m_uiHeight;
		file.writeData((BYTE*)&bf, sizeof(BITMAPFILEHEADER));

		BITMAPINFOHEADER bi;
		memset(&bi, 0, sizeof(BITMAPINFOHEADER));
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biBitCount = 32;
		bi.biWidth = m_uiWidth;
		bi.biHeight = -(INT)m_uiHeight;
		bi.biPlanes = 1;
		bi.biCompression = BI_RGB;
		bi.biXPelsPerMeter = 72;
		bi.biYPelsPerMeter = 72;
		file.writeData((BYTE*)&bi, sizeof(BITMAPINFOHEADER));

		if (m_pbtPixelData != NULL)
		{
			file.writeData(m_pbtPixelData, m_uiWidth * 4 * m_uiHeight);
		}

		file.close();
#endif

#ifdef OS_QNX

#endif

#ifdef OS_LINUX

#endif
	}

	void* CDIB::getDIBInfo()
	{
#ifdef OS_WIN
		if (m_pDIBInfo == NULL)
		{
			m_pDIBInfo = new BITMAPINFOHEADER;
		}

		LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)m_pDIBInfo;
		memset(lpbi, 0, sizeof(BITMAPINFOHEADER));
		
		lpbi->biSize = sizeof(BITMAPINFOHEADER);
		lpbi->biBitCount = 32;
		lpbi->biWidth = m_uiWidth;
		lpbi->biHeight = -(INT)m_uiHeight;
		lpbi->biPlanes = 1;
		lpbi->biCompression = BI_RGB;
		lpbi->biXPelsPerMeter = 72;
		lpbi->biYPelsPerMeter = 72;
#endif

#ifdef OS_QNX

#endif

#ifdef OS_LINUX

#endif

		return m_pDIBInfo;
	}

	void CDIB::createDIB(UINT uiWidth, UINT uiHeight, UINT uiColor)
	{		
		if (m_uiWidth != uiWidth || m_uiHeight != uiHeight)
		{
			destroyDIB();

			if (uiWidth == 0 || uiHeight == 0 || uiWidth > 10000 || uiHeight > 10000)
			{
				return;
			}

			m_uiWidth = uiWidth;
			m_uiHeight = uiHeight;

			m_pbtPixelData = new BYTE[m_uiWidth * 4 * m_uiHeight];
		}

		if (m_pbtPixelData == NULL)
		{
			return;
		}
		
		if (uiColor == 0)
		{
			memset(m_pbtPixelData, 0, m_uiWidth * 4 * m_uiHeight);
		}
		else
		{
			UINT *puiPixelData = (UINT *)m_pbtPixelData;
			UINT *puiPixelDataTail = puiPixelData + m_uiWidth * m_uiHeight;
			
			while (puiPixelData < puiPixelDataTail)
			{
				*puiPixelData++ = uiColor;
			}
		}
	}

	void CDIB::createDIB(UINT uiWidth, UINT uiHeight, const BYTE *pbtPixelData, UINT uiPixelCoun)
	{
		createDIB(uiWidth, uiHeight, (UINT)0);

		if (m_pbtPixelData != NULL && pbtPixelData != NULL)
		{
			memcpy(m_pbtPixelData, pbtPixelData, uiPixelCoun == 0 ? m_uiWidth * 4 * m_uiHeight : uiPixelCoun);
		}
	}

	void CDIB::moveDIBFrom(CDIB *pSrcDib)
	{
		destroyDIB();

		if (pSrcDib == NULL)
		{
			return;
		}

		m_pbtPixelData = pSrcDib->m_pbtPixelData;
		m_uiWidth = pSrcDib->m_uiWidth;
		m_uiHeight = pSrcDib->m_uiHeight;
		m_pDIBInfo = pSrcDib->m_pDIBInfo;

		pSrcDib->m_pbtPixelData = NULL;
		pSrcDib->m_uiWidth = 0;
		pSrcDib->m_uiHeight = 0;
		pSrcDib->m_pDIBInfo = NULL;
	}

	void CDIB::copyDIBFrom(CDIB *pSrcDib, INT iSrcX, INT iSrcY, INT iSrcWidth, INT iSrcHeight)
	{
		destroyDIB();

		if (pSrcDib == NULL)
		{
			return;
		}

		CRectangle rcSrc(0, 0, pSrcDib->getWidth(), pSrcDib->getWidth());
		rcSrc = rcSrc.intersect(CRectangle(iSrcX, iSrcY, iSrcWidth, iSrcHeight));
		if (rcSrc.isEmpty())
		{
			return;
		}

		m_uiWidth = rcSrc.getWidth();
		m_uiHeight = rcSrc.getHeight();
		m_pbtPixelData = new BYTE[m_uiWidth * 4 * m_uiHeight];

		BYTE *pbtDestPixelData = m_pbtPixelData;
		const BYTE *pbtSrcPixelData = pSrcDib->getPixelData() + rcSrc.getY() * pSrcDib->getStride() + rcSrc.getX() * 4;

		for (INT i = 0; i < (INT)m_uiHeight; i++)
		{
			memcpy(pbtDestPixelData, pbtSrcPixelData, m_uiWidth * 4);

			pbtDestPixelData += m_uiWidth * 4;
			pbtSrcPixelData += pSrcDib->getStride();
		}
	}

	void CDIB::cutDIBFrom(CDIB *pSrcDib, INT iSrcX, INT iSrcY, INT iSrcWidth, INT iSrcHeight)
	{
		destroyDIB();

		if (pSrcDib == NULL)
		{
			return;
		}

		CRectangle rcSrc(0, 0, pSrcDib->getWidth(), pSrcDib->getWidth());
		rcSrc = rcSrc.intersect(CRectangle(iSrcX, iSrcY, iSrcWidth, iSrcHeight));
		if (rcSrc.isEmpty())
		{
			return;
		}

		m_uiWidth = rcSrc.getWidth();
		m_uiHeight = rcSrc.getHeight();
		m_pbtPixelData = new BYTE[m_uiWidth * 4 * m_uiHeight];

		BYTE *pbtDestPixelData = m_pbtPixelData;
		BYTE *pbtSrcPixelData = (BYTE*)pSrcDib->getPixelData() + rcSrc.getY() * pSrcDib->getStride() + rcSrc.getX() * 4;

		for (INT i = 0; i < (INT)m_uiHeight; i++)
		{
			memcpy(pbtDestPixelData, pbtSrcPixelData, m_uiWidth * 4);
			memset(pbtSrcPixelData, 0, m_uiWidth * 4);

			pbtDestPixelData += m_uiWidth * 4;
			pbtSrcPixelData += pSrcDib->getStride();
		}
	}

	void CDIB::destroyDIB()
	{
		if (m_pbtPixelData != NULL)
		{
			delete [] m_pbtPixelData;
			m_pbtPixelData = NULL;		
		}

		m_uiWidth = 0;
		m_uiHeight = 0;

		if (m_pDIBInfo != NULL)
		{
			delete m_pDIBInfo;
			m_pDIBInfo = NULL;
		}
	}

	void CDIB::merge8DIB(	INT iDestX, INT iDestY,
							const BYTE *pbtSrcData, INT iSrcWidth, INT iSrcHeight, BYTE btSrcOpacity, UINT uiSrcColor)
	{
		CHECK_DIB_BUFFER

		if (pbtSrcData == NULL || iSrcWidth <= 0 || iSrcHeight <= 0 || btSrcOpacity == 0)
		{
			return;
		}

		// 没有缩放的情况，采用真实图片和要求描绘目标区域最小者处理

		// left-top
		INT iX1 = Math::getMax(0, iDestX);
		INT iY1 = Math::getMax(0, iDestY);

		// right-bottom
		INT iX2 = Math::getMin((INT)m_uiWidth, iDestX + iSrcWidth);
		INT iY2 = Math::getMin((INT)m_uiHeight, iDestY + iSrcHeight);

		// 判断目标矩形是否相交
		if (iX1 < iX2 && iY1 < iY2) 
		{
			// 判断src左上的点是否在目标矩形内, 如果在内部即用SRC的(0,0)
			INT iSrcX = 0;
			INT iSrcY = 0;

			// 如果在外部算src的相对坐标
			if (iDestX < 0 || iDestX > (INT)m_uiWidth || iDestY < 0 || iDestY > (INT)m_uiHeight)
			{
				iSrcX = iX1 - iDestX;
				iSrcY = iY1 - iDestY;
			}

			pasteMerge8DIB(iX1, iY1, pbtSrcData, iSrcX, iSrcY, iX2 - iX1, iY2 - iY1, iSrcWidth, btSrcOpacity, uiSrcColor);
		}
	}

	void CDIB::merge32DIB(	INT iDestX, INT iDestY, CRectangle rcClipBox,
							const BYTE *pbtSrcData, INT iSrcWidth, INT iSrcHeight, BYTE btSrcOpacity)
	{
		CHECK_DIB_BUFFER

		rcClipBox = rcClipBox.intersect(CRectangle(0, 0, m_uiWidth, m_uiHeight));
		if (rcClipBox.isEmpty())
		{
			return;
		}

		CRectangle rcDest(iDestX, iDestY, iSrcWidth, iSrcHeight);
		rcDest = rcDest.intersect(rcClipBox);
		if (rcDest.isEmpty())
		{
			return;
		}

		pasteMerge32DIB(rcDest.getX(), rcDest.getY(), pbtSrcData,
						rcDest.getX() - iDestX, rcDest.getY() - iDestY, rcDest.getWidth(), rcDest.getHeight(),
						iSrcWidth * 4, btSrcOpacity);
	}

	void CDIB::merge32DIB(	INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight,
							const BYTE *pbtSrcData, INT iSrcWidth, INT iSrcHeight, BYTE btSrcOpacity,
							EHorizontalLayout eHLayout, EVerticalLayout eVLayout)
	{
		CHECK_DIB_BUFFER

		if (iDestWidth <= 0 || iDestHeight <= 0 || pbtSrcData == NULL || iSrcWidth <= 0 || iSrcHeight <= 0 || btSrcOpacity == 0)
		{
			return;
		}

		// 没有缩放的情况，采用真实图片和要求描绘目标区域最小者处理

		CRectangle rcTarget(iDestX, iDestY, iDestWidth, iDestHeight);

		INT iOffsetX = calcXByLayout(rcTarget.getX(), iSrcWidth, INT(rcTarget.getWidth()), eHLayout);
		INT iOffsetY = calcYByLayout(rcTarget.getY(), iSrcHeight, INT(rcTarget.getHeight()), eVLayout);

		rcTarget = rcTarget.intersect(CRectangle(0, 0, m_uiWidth, m_uiHeight));
		if (rcTarget.isEmpty())
		{
			return;
		}

		OpenHMI::CRectangle rcSrc(iOffsetX, iOffsetY, iSrcWidth, iSrcHeight);		
		OpenHMI::CRectangle rcDest = rcTarget.intersect(rcSrc);
		if (rcDest.isEmpty())
		{
			return;
		}

		pasteMerge32DIB(rcDest.getX(), rcDest.getY(), pbtSrcData,
						rcDest.getX() - rcSrc.getX(), rcDest.getY() - rcSrc.getY(), rcDest.getWidth(), rcDest.getHeight(),
						iSrcWidth * 4, btSrcOpacity);
	}	

	void CDIB::merge32DIB(	INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight,
							const BYTE *pbtSrcData, INT iSrcWidth, INT iSrcHeight, BYTE btSrcOpacity,
							EStretchLayout eSLayout)
	{
		CHECK_DIB_BUFFER

		if (iDestWidth <= 0 || iDestHeight <= 0 || pbtSrcData == NULL || iSrcWidth <= 0 || iSrcHeight <= 0 || btSrcOpacity == 0)
		{
			return;
		}

		if (iDestWidth == iSrcWidth && iDestHeight == iSrcHeight)
		{
			merge32DIB(iDestX, iDestY, iDestWidth, iDestHeight, pbtSrcData, iSrcWidth, iSrcHeight, btSrcOpacity, HL_LEFT, VL_TOP);
			return;
		}		

		// 有缩放的情况下先做缩放，再做相交判断
		INT iStretchedX ,iStrecthedY, iStretchedWidth, iStretchedHeight;

		if (eSLayout == CDIB::SL_CENTER)
		{
			FLOAT fWRatio = (FLOAT)iSrcWidth / (FLOAT)iDestWidth;
			FLOAT fHRatio = (FLOAT)iSrcHeight / (FLOAT)iDestHeight;

			if (fWRatio > fHRatio) 
			{
				iStretchedWidth = iDestWidth;
				iStretchedHeight = (INT)(iSrcHeight / fWRatio);
				iStretchedX = iDestX;
				iStrecthedY = iDestY + (iDestHeight - iStretchedHeight) / 2;
			}
			else 
			{
				iStretchedWidth = (INT)(iSrcWidth / fHRatio);
				iStretchedHeight = iDestHeight;
				iStretchedX = iDestX + (iDestWidth - iStretchedWidth) / 2;
				iStrecthedY = iDestY;
			} 
		}
		else if (eSLayout == SL_FILL) 
		{
			iStretchedX = iDestX;
			iStrecthedY = iDestY;
			iStretchedWidth = iDestWidth;
			iStretchedHeight = iDestHeight;	
		}
		else
		{
			return;
		}

		// left-top
		INT iX1 = Math::getMax(0, iStretchedX);
		INT iY1 = Math::getMax(0, iStrecthedY);

		// right-bottom
		INT iX2 = Math::getMin((INT)m_uiWidth, iStretchedX + iStretchedWidth);
		INT iY2 = Math::getMin((INT)m_uiHeight, iStrecthedY + iStretchedHeight);

		// ROI Width and Heigth
		INT iRoiX = 0;
		INT iRoiY = 0;
		INT iRoiWidth = 0;
		INT iRoiHeight = 0;	

		if (iX1 < iX2 && iY1 < iY2) 
		{
			// src ROI
			FLOAT fWRatio = (FLOAT)(iX2 - iX1) / (FLOAT)iStretchedWidth;
			FLOAT fHRatio = (FLOAT)(iY2 - iY1) / (FLOAT)iStretchedHeight;

			// ROI 长宽
			iRoiWidth =  (INT)(iSrcWidth * fWRatio);
			iRoiHeight = (INT)(iSrcHeight * fHRatio);

			if (iRoiWidth == 0 || iRoiHeight == 0)
			{
				return;
			}

			// ROI 的x, y
			iRoiX = (INT)(iSrcWidth * ((FLOAT)(iX1 - iStretchedX) / (FLOAT)iStretchedWidth));
			iRoiY = (INT)(iSrcHeight * ((FLOAT)(iY1 - iStrecthedY) / (FLOAT)iStretchedHeight));

			stretchMerge32DIB(	iX1, iY1, iX2 - iX1, iY2 - iY1, 
								pbtSrcData, iRoiX, iRoiY, iRoiWidth, iRoiHeight, iSrcWidth * 4, btSrcOpacity);
		}		
	}

	void CDIB::mergeMaskDIB(const BYTE *pbtSrcData, INT iSrcWidth, INT iSrcHeight)
	{
		CHECK_DIB_BUFFER

		if (pbtSrcData == NULL)
		{
			return;
		}

		UINT uiWidth = m_uiWidth < (UINT)iSrcWidth ? m_uiWidth : iSrcWidth;
		UINT uiHeight = m_uiHeight < (UINT)iSrcHeight ? m_uiHeight : iSrcHeight;

		INT uiAlpha255To256 = 256;
		INT uiAlpha255To256_2 = 256;

		BYTE		*pbtDestLine = m_pbtPixelData;
		tagPixel	*pPixelDest = NULL;

		for (UINT i =  0; i < uiHeight; ++i)
		{
			for (UINT j =  0; j < uiWidth; ++j)
			{
				pPixelDest = (tagPixel*)(pbtDestLine + j * 4);

				uiAlpha255To256_2 = ALPHA_255_TO_256(*(pbtSrcData + j * 4 + 3));
				pPixelDest->btA = (pPixelDest->btA * uiAlpha255To256_2) >> 8;

				uiAlpha255To256 = ALPHA_255_TO_256(pPixelDest->btA);

				pPixelDest->btB = (pPixelDest->btB * uiAlpha255To256) >> 8;
				pPixelDest->btG = (pPixelDest->btG * uiAlpha255To256) >> 8;
				pPixelDest->btR = (pPixelDest->btR * uiAlpha255To256) >> 8;			
			}

			pbtDestLine += m_uiWidth * 4;
			pbtSrcData += iSrcWidth * 4;
		}
	}

	void CDIB::replace( INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight, const BYTE* pbtSrcData)
	{
		CHECK_DIB_BUFFER

		if (iDestWidth <= 0 || iDestHeight <= 0 )
		{
			return;
		}

		CRectangle rcDest(iDestX, iDestY, iDestWidth, iDestHeight);
		CRectangle rcClip(0, 0, m_uiWidth, m_uiHeight);
		CRectangle rc = rcClip.intersect(rcDest);
		if ( rc.isEmpty() )
		{
			return;
		}

		INT iX = rc.getX();
		INT iY = rc.getY();
		UINT uiWidth = rc.getWidth();
		UINT uiHeight = rc.getHeight();

		BYTE		*pbtDestLine = m_pbtPixelData + m_uiWidth * 4 * iY + iX * 4;
		const BYTE	*pbtSrcLine = pbtSrcData + iDestWidth * 4 * (iY - iDestY) + (iX - iDestX) * 4;

		tagPixel *pPixelDest = NULL;
		tagPixel *pPixelSrc = NULL;

		INT iSrcStride = uiWidth * 4;

		for (UINT i =  0; i < uiHeight; ++i)
		{
			pPixelDest = (tagPixel *)pbtDestLine;
			pPixelSrc = (tagPixel *)pbtSrcLine;

			for (UINT j =  0; j < uiWidth; ++j)
			{
				REPLACE_32_PIXEL(pPixelDest, pPixelSrc);

				++pPixelDest;
				++pPixelSrc;
			}

			pbtDestLine += m_uiWidth * 4;
			pbtSrcLine += iSrcStride;
		}
	}

	void CDIB::setColor( INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight, UINT uiSrcColor)
	{
		CHECK_DIB_BUFFER

		if (iDestWidth <= 0 || iDestHeight <= 0 )
		{
			return;
		}

		CRectangle rcDest(iDestX,iDestY,iDestWidth,iDestHeight);
		CRectangle rcClip(0,0,m_uiWidth,m_uiHeight);
		CRectangle rc = rcClip.intersect(rcDest);
		if ( rc.isEmpty() )
		{
			return;
		}

		tagPixel *pPixelSrcColor = (tagPixel *)&uiSrcColor;

		INT iX = rc.getX();
		INT iY = rc.getY();
		UINT uiWidth = rc.getWidth();
		UINT uiHeight = rc.getHeight();

		BYTE	*pbtDestLine = m_pbtPixelData + m_uiWidth * 4 * iY + iX * 4;

		tagPixel *pPixelDest = NULL;

		for (UINT i =  0; i < uiHeight; ++i)
		{
			pPixelDest = (tagPixel *)pbtDestLine;

			for (UINT j =  0; j < uiWidth; ++j)
			{
				REPLACE_32_PIXEL(pPixelDest, pPixelSrcColor);

				++pPixelDest;
			}

			pbtDestLine += m_uiWidth * 4;
		}
	}

	void CDIB::flipVertically()
	{
		CHECK_DIB_BUFFER

		BYTE *pbtTemp = new BYTE[m_uiWidth * 4];
		BYTE *pbtHead = m_pbtPixelData;
		BYTE *pbtTail = m_pbtPixelData + (m_uiHeight - 1) * m_uiWidth * 4;

		for (INT i = 0; i < (INT)m_uiHeight / 2; i++)
		{
			memcpy(pbtTemp, pbtHead, m_uiWidth * 4);
			memcpy(pbtHead, pbtTail, m_uiWidth * 4);
			memcpy(pbtTail, pbtTemp, m_uiWidth * 4);

			pbtHead += m_uiWidth * 4;
			pbtTail -= m_uiWidth * 4;
		}

		delete [] pbtTemp;
	}	

	void CDIB::drawRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine)
	{
		CHECK_DIB_BUFFER

		INT iX1 = iX;
		INT iX2 = iX + (INT)uiWidth - 1;
		INT iY1 = iY;
		INT iY2 = iY + (INT)uiHeight - 1;

		drawLine(iX1, iY1, iX2, iY1, uiColor, bDashLine);
		drawLine(iX2, iY1, iX2, iY2, uiColor, bDashLine);
		drawLine(iX2, iY2, iX1, iY2, uiColor, bDashLine);
		drawLine(iX1, iY2, iX1, iY1, uiColor, bDashLine);
	}

	void CDIB::drawTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine)
	{
		CHECK_DIB_BUFFER

		drawLine(p1.getX(), p1.getY(), p2.getX(), p2.getY(), uiColor, bDashLine);
		drawLine(p1.getX(), p1.getY(), p3.getX(), p3.getY(), uiColor, bDashLine);
		drawLine(p3.getX(), p3.getY(), p2.getX(), p2.getY(), uiColor, bDashLine);
	}

	void CDIB::drawLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine)
	{
		CHECK_DIB_BUFFER
		
		INT dx = iX2-iX1;
		INT dy = iY2-iY1;

		if ( dx == 0 )
		{
			tagPixel pixelColor = modifyColor(uiColor);

			INT x = iX1;
			INT y = iY1;
			INT iEndY = iY2;
			INT deltaY = bDashLine ? DASH_LINE_WIDTH : 1;
			if ( iY1 < iY2 )
			{
				;
			}
			else
			{
				y = iY2;
				iEndY = iY1;
			}

			INT delta = 0;
			for(; y < iEndY; )
			{	
				setPixel(x,y,pixelColor);
				if ( delta == deltaY-1 )
				{
					y += deltaY;
					delta = 0;
				}
				else
				{
					++delta;
					y += 1;
				}
			}
		}
		else if ( Math::abs(dx) >= Math::abs(dy) )
		{
			if ( iX1 > iX2 )
			{
				drawLineDDA(iX2, iY2, iX1, iY1, uiColor, bDashLine, TRUE);
			}
			else if( iX1 < iX2 )
			{
				drawLineDDA(iX1, iY1, iX2, iY2, uiColor, bDashLine, TRUE);
			}
		}
		else 
		{
			if ( iY1 > iY2 )
			{
				drawLineDDA(iX2, iY2, iX1, iY1, uiColor, bDashLine, FALSE);
			}
			else if( iY1 < iY2 )
			{
				drawLineDDA(iX1, iY1, iX2, iY2, uiColor, bDashLine, FALSE);
			}
		}
	}

	void CDIB::drawCircle(INT iX, INT iY, UINT uiRadius, UINT uiColor, BOOLEAN bDashLine)
	{
		CHECK_DIB_BUFFER

		tagPixel pixelColor = modifyColor(uiColor);

		FLOAT e = 3 - 2.0f * uiRadius;
		FLOAT d = 0.0f;

		INT x = 0;
		INT y = uiRadius;
		INT x0 = iX;
		INT y0 = iY;

		setPixel(x+x0,y+y0, pixelColor);
		setPixel(x+x0,-y+y0, pixelColor);
		setPixel(y+x0,x+y0, pixelColor);
		setPixel(-y+x0,x+y0, pixelColor);

		if ( bDashLine )
		{
			INT delta = 0;
			while(x<=y)
			{	
				if ( e < 0 ) 
				{
					e = e + 4*x + 6;
					++x;
				}
				else
				{
					e = e + 4*(x-y) + 10; 
					++x;
					--y;
				}
				
				if ( delta == DASH_LINE_WIDTH )
				{
					setPixel(x+x0,y+y0, pixelColor);
					setPixel(-x+x0,y+y0, pixelColor);
					setPixel(-x+x0,-y+y0, pixelColor);
					setPixel(x+x0,-y+y0, pixelColor);
	 				setPixel(y+x0,x+y0, pixelColor);
	 				setPixel(-y+x0,x+y0, pixelColor);
	 				setPixel(-y+x0,-x+y0, pixelColor);
					setPixel(y+x0,-x+y0, pixelColor);
					delta = 0;
				}
				++delta;
			}
		}
		else
		{
			while(x<=y)
			{	
				if ( e < 0 ) 
				{
					e = e + 4*x + 6;
					++x;
				}
				else
				{
					e = e + 4*(x-y) + 10; 
					++x;
					--y;
				}
				
				setPixel(x+x0,y+y0, pixelColor);
				setPixel(-x+x0,y+y0, pixelColor);
				setPixel(-x+x0,-y+y0, pixelColor);
				setPixel(x+x0,-y+y0, pixelColor);
	 			setPixel(y+x0,x+y0, pixelColor);
	 			setPixel(-y+x0,x+y0, pixelColor);
	 			setPixel(-y+x0,-x+y0, pixelColor);
				setPixel(y+x0,-x+y0, pixelColor);
			}
		}

	}	

	void CDIB::fillRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor)
	{
		CHECK_DIB_BUFFER		

		CRectangle rcDest(iX, iY, uiWidth, uiHeight);
		rcDest = rcDest.intersect(CRectangle(0, 0, m_uiWidth, m_uiHeight));
		if (rcDest.isEmpty())
		{
			return;
		}

		tagPixel pixelColor = modifyColor(uiColor);

		tagPixel *pPixelDataHead = (tagPixel *)m_pbtPixelData + rcDest.getY() * m_uiWidth + rcDest.getX();
		tagPixel *pPixelData = NULL;

		for (UINT i = 0; i < rcDest.getHeight(); i++)
		{
			pPixelData = pPixelDataHead;

			for (UINT j = 0; j < rcDest.getWidth(); j++, pPixelData++)
			{
				RENDER_32_PIXEL(pPixelData, (&pixelColor));
			}

			pPixelDataHead += m_uiWidth;
		}
	}


	INT CDIB::calcXByLayout(INT iDestX, INT iSrcWidth, INT iDestWidth, CDIB::EHorizontalLayout eHLayout)
	{
		INT iResult = 0;

		switch (eHLayout)
		{
		case CDIB::HL_LEFT:
			break;

		case CDIB::HL_CENTER:
			iResult = (iDestWidth - iSrcWidth) / 2; 
			break;

		case CDIB::HL_RIGHT:
			iResult = iDestWidth - iSrcWidth;
			break;
		}

		return iDestX + iResult;
	}

	INT CDIB::calcYByLayout(INT iDestY, INT iSrcHeight, INT iDestHeight, CDIB::EVerticalLayout eVLayout)
	{
		INT iResult = 0;

		switch (eVLayout)
		{
		case CDIB::VL_TOP:
			break;

		case CDIB::VL_MIDDLE:
			iResult = (iDestHeight - iSrcHeight) / 2;
			break;

		case CDIB::VL_BOTTOM:
			iResult = iDestHeight - iSrcHeight;
			break;
		}

		return iDestY + iResult;
	}

	CRectangle CDIB::calcDestRect(	INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight, 
											INT iSrcWidth, INT iSrcHeight, CRenderImage::ELayout layout)
	{
		CRectangle rc;
		if (iDestWidth <= 0 || iDestHeight <= 0 || iSrcWidth <= 0 || iSrcHeight <= 0 )
		{
			return rc;
		}

		switch (layout)
		{
		case CRenderImage::L_NORMAL:
			{
				rc.setX(iDestX);
				rc.setY(iDestY);
				rc.setWidth(iSrcWidth);
				rc.setHeight(iSrcHeight);
			}
			break;

		case CRenderImage::L_STRETCH_CENTER:
			{
				if (iDestWidth == iSrcWidth && iDestHeight == iSrcHeight)
				{
					rc.setX(iDestX);
					rc.setY(iDestY);
					rc.setWidth(iSrcWidth);
					rc.setHeight(iSrcHeight);
				}		
				else
				{
					// 有缩放的情况下先做缩放，再做相交判断
					INT iStretchedX ,iStrecthedY, iStretchedWidth, iStretchedHeight;
			
					FLOAT fWRatio = (FLOAT)iSrcWidth / (FLOAT)iDestWidth;
					FLOAT fHRatio = (FLOAT)iSrcHeight / (FLOAT)iDestHeight;

					if (fWRatio > fHRatio) 
					{
						iStretchedWidth = iDestWidth;
						iStretchedHeight = (INT)(iSrcHeight / fWRatio);
						iStretchedX = iDestX;
						iStrecthedY = iDestY + (iDestHeight - iStretchedHeight) / 2;
					}
					else 
					{
						iStretchedWidth = (INT)(iSrcWidth / fHRatio);
						iStretchedHeight = iDestHeight;
						iStretchedX = iDestX + (iDestWidth - iStretchedWidth) / 2;
						iStrecthedY = iDestY;
					} 
					rc.setX(iStretchedX);
					rc.setY(iStrecthedY);
					rc.setWidth(iStretchedWidth);
					rc.setHeight(iStretchedHeight);
				}
			}
			break;

		case CRenderImage::L_STRETCH_FILL:
			{
				rc.setX(iDestX);
				rc.setY(iDestY);
				rc.setWidth(iDestWidth);
				rc.setHeight(iDestHeight);
			}
			break;

		}

		return rc;
	}

	CRectangle CDIB::calcDestRect(	INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight,
											INT iSrcWidth, INT iSrcHeight, 
											CRenderText::EHorizontalLayout eHLayout, CRenderText::EVerticalLayout eVLayout)
	{
		CRectangle rc;
		if (iDestWidth <= 0 || iDestHeight <= 0 || iSrcWidth <= 0 || iSrcHeight <= 0 )
		{
			return rc;
		}

		INT iXResult = 0;

		switch (eHLayout)
		{
		case CRenderText::HL_LEFT:
			break;

		case CRenderText::HL_CENTER:
			iXResult = (iDestWidth - iSrcWidth) / 2; 
			break;

		case CRenderText::HL_RIGHT:
			iXResult = iDestWidth - iSrcWidth;
			break;
		}

		INT iYResult = 0;

		switch (eVLayout)
		{
		case CRenderText::VL_TOP:
			break;

		case CRenderText::VL_MIDDLE:
			iYResult = (iDestHeight - iSrcHeight) / 2;
			break;

		case CRenderText::VL_BOTTOM:
			iYResult = iDestHeight - iSrcHeight;
			break;
		}

		rc.setX(iXResult + iDestX);
		rc.setY(iYResult + iDestY);
		rc.setWidth(iSrcWidth);
		rc.setHeight(iSrcHeight);

		return rc;
	}

	void CDIB::pasteMerge8DIB	(	INT iDestX, INT iDestY,
									const BYTE *pbtSrcData, 
									INT iSrcX, INT iSrcY, INT iSrcWidth, INT iSrcHeight, 
									INT iSrcStride, BYTE btSrcOpacity, UINT uiSrcColor)
	{
		tagPixel pixelSrcColor = *(tagPixel *)&uiSrcColor;

		if (0xFF != btSrcOpacity)
		{
			pixelSrcColor.btB = ((UINT)pixelSrcColor.btB * btSrcOpacity) >> 8;
			pixelSrcColor.btG = ((UINT)pixelSrcColor.btG * btSrcOpacity) >> 8;
			pixelSrcColor.btR = ((UINT)pixelSrcColor.btR * btSrcOpacity) >> 8;
			pixelSrcColor.btA = ((UINT)pixelSrcColor.btA * btSrcOpacity) >> 8;
		}

		if (0 == pixelSrcColor.btA)
		{
			return;
		}

		BYTE		*pbtDestLine = m_pbtPixelData + m_uiWidth * 4 * iDestY + iDestX * 4;
		const BYTE	*pbtSrcLine = pbtSrcData + iSrcStride * iSrcY + iSrcX;

		tagPixel *pPixelDest = NULL;
		BYTE	 *pbtSrc = NULL;

		for (INT i =  0; i < iSrcHeight; i++)
		{
			pPixelDest = (tagPixel *)pbtDestLine;
			pbtSrc = (BYTE *)pbtSrcLine;

			for (INT j =  0; j < iSrcWidth; j++)
			{
				RENDER_8_PIXEL(pPixelDest, *pbtSrc, pixelSrcColor);

				pPixelDest++;
				pbtSrc++;
			}

			pbtDestLine += m_uiWidth * 4;
			pbtSrcLine += iSrcStride;
		}
	}

	void CDIB::pasteMerge32DIB(	INT iDestX, INT iDestY,
								const BYTE *pbtSrcData, 
								INT iSrcX, INT iSrcY, INT iSrcWidth, INT iSrcHeight, 
								INT iSrcStride, BYTE btSrcOpacity)
	{
		BYTE		*pbtDestLine = m_pbtPixelData + m_uiWidth * 4 * iDestY + iDestX * 4;
		const BYTE	*pbtSrcLine = pbtSrcData + iSrcStride * iSrcY + iSrcX * 4;

		tagPixel *pPixelDest = NULL;
		tagPixel *pPixelSrc = NULL;

		for (INT i =  0; i < iSrcHeight; i++)
		{
			pPixelDest = (tagPixel *)pbtDestLine;
			pPixelSrc = (tagPixel *)pbtSrcLine;

			for (INT j =  0; j < iSrcWidth; j++)
			{
				switch (m_eMergeState)
				{
				case MS_ALPHA_BLEND:
					if (pPixelSrc->btA > 0)
					{
						RENDER_32_PIXEL(pPixelDest, pPixelSrc);
					}
					break;

				case MS_ALPHA_PAINT:
					if (pPixelSrc->btA > 0)
					{
						REPLACE_32_PIXEL(pPixelDest, pPixelSrc);
					}					
					break;

				default:
					break;
				}				

				pPixelDest++;
				pPixelSrc++;
			}

			pbtDestLine += m_uiWidth * 4;
			pbtSrcLine += iSrcStride;
		}
	}	

	void CDIB::stretchMerge32DIB(	INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight,
									const BYTE *pbtSrcData, 
									INT iSrcX, INT iSrcY, INT iSrcWidth, INT iSrcHeight, 
									INT iSrcStride, BYTE btSrcOpacity)
	{
		if (iDestWidth == iSrcWidth && iDestHeight == iSrcHeight)
		{
			pasteMerge32DIB(iDestX, iDestY, pbtSrcData, iSrcX, iSrcY, iSrcWidth, iSrcHeight, iSrcStride, btSrcOpacity);
			return;
		}

		// use Bilinear stretch arithmetic, not use NearNeighbor stretch arithmetic.

		BYTE		*pbtDestLine = m_pbtPixelData + m_uiWidth * 4 * iDestY + iDestX * 4;
		//const BYTE	*pbtSrcLine = pbtSrcData + iSrcStride * iSrcY + iSrcX * 4;

		LONG		lXrIntFloat_16 = (iSrcWidth << 16 ) / iDestWidth + 1;
		LONG		lYrIntFloat_16 = (iSrcHeight << 16) / iDestHeight + 1;
		
		const LONG	L_ERROR_X = -(1 << 15) + (lXrIntFloat_16 >> 1);
		const LONG	L_ERROR_Y = -(1 << 15) + (lYrIntFloat_16 >> 1);

		LONG lBorderX0 = -L_ERROR_X / lXrIntFloat_16 + 1;
		if (lBorderX0 >= iDestWidth )
		{
			lBorderX0 = iDestWidth;
		}

		LONG lBorderY0 = -L_ERROR_Y / lYrIntFloat_16 + 1; 
		if (lBorderY0 >= iDestHeight)
		{
			lBorderY0 = iDestHeight;
		}

		LONG lBorderX1 = (((iSrcWidth - 2) << 16) - L_ERROR_X) / lXrIntFloat_16 + 1;
		if (lBorderX1 < lBorderX0)
		{
			lBorderX1 = lBorderX0;
		}

		LONG lBorderY1 = (((iSrcHeight - 2) << 16) - L_ERROR_Y) / lYrIntFloat_16 + 1;
		if (lBorderY1 < lBorderY0)
		{
			lBorderY1 = lBorderY0;
		}		

		tagPixel	*pPixelDest = (tagPixel*)pbtDestLine;
		tagBitmap	bmpSrc;
		bmpSrc.pbtData = (BYTE*)pbtSrcData;
		bmpSrc.iWidth = iSrcWidth;
		bmpSrc.iHeight = iSrcHeight;
		bmpSrc.iStride = iSrcStride;

		LONG lSrcY16 = L_ERROR_Y;
		LONG lY;

		for (LONG lY = 0; lY < lBorderY0; ++lY)
		{
			LONG lSrcX16 = L_ERROR_X;
			LONG lX;

			for (lX = 0; lX < iDestWidth; ++lX)
			{
				bilinearBoder(bmpSrc, lSrcX16, lSrcY16, btSrcOpacity, &pPixelDest[lX]);
				lSrcX16 += lXrIntFloat_16;
			}

			lSrcY16 += lYrIntFloat_16;
			pPixelDest += m_uiWidth;
		}

		for (lY = lBorderY0; lY < lBorderY1; ++lY)
		{
			LONG lSrcX16 = L_ERROR_X;
			LONG lX;

			for (lX = 0; lX < lBorderX0; ++lX)
			{
				bilinearBoder(bmpSrc, lSrcX16, lSrcY16, btSrcOpacity, &pPixelDest[lX]);
				lSrcX16 += lXrIntFloat_16;
			}

			if (lBorderX1 - lBorderX0 > 0) 
			{
				lSrcX16 = bilinearScanline(	(BYTE*)&pPixelDest[lBorderX0],
											pbtSrcData + iSrcStride * (lSrcY16 >> 16),
											lSrcX16,
											lSrcY16,
											lXrIntFloat_16,
											lBorderX1 - lBorderX0,
											iSrcStride,
											btSrcOpacity);
			}

			for (lX = lBorderX1; lX < iDestWidth; ++lX)
			{
				bilinearBoder(bmpSrc, lSrcX16, lSrcY16, btSrcOpacity, &pPixelDest[lX]);
				lSrcX16 += lXrIntFloat_16;
			}

			lSrcY16 += lYrIntFloat_16;
			pPixelDest += m_uiWidth;
		}

		for (lY = lBorderY1; lY < iDestHeight; ++lY)
		{
			LONG lSrcX16=L_ERROR_X;
			LONG lX;

			for (lX = 0; lX < iDestWidth; ++lX)
			{
				bilinearBoder(bmpSrc, lSrcX16, lSrcY16, btSrcOpacity, &pPixelDest[lX]);
				lSrcX16 += lXrIntFloat_16;
			}

			lSrcY16 += lYrIntFloat_16;
			pPixelDest += m_uiWidth;
		}
	}

	UINT CDIB::bilinearScanline(BYTE *pbtDestData, const BYTE *pbtSrcData,
								LONG lSrcX16, LONG lSrcY16, LONG lXrIntFloat_16, LONG lYrStride,
								INT iSrcStride, BYTE btSrcOpacity)
	{
		tagPixel	*pPixelDest = (tagPixel *)pbtDestData;
		tagPixel	*pPixelSrc = (tagPixel *)pbtSrcData;
		tagPixel	*pPixelTemp = NULL;

		ULONG ulV2 = (lSrcY16 & 0xFFFF) >> 14;

		do 
		{
			ULONG ulU2 = (lSrcX16 & 0xFFFF) >> 14;
			BilinearFunc pBilinearFun = g_aBilinearFuncs[(ulV2 << 2) | ulU2];

			UINT *puiColor00 = (UINT*)(pPixelSrc + (lSrcX16 >> 16));
			UINT *puiColor10 = (UINT*)((BYTE*)puiColor00 + iSrcStride);

			UINT uiSrc = pBilinearFun(*puiColor00, *(puiColor00 + 1), *puiColor10, *(puiColor10 + 1));
			pPixelTemp = (tagPixel*)&uiSrc;

			switch (m_eMergeState)
			{
			case MS_ALPHA_BLEND:
				if (pPixelTemp->btA > 0)
				{
					RENDER_32_PIXEL(pPixelDest, pPixelTemp);
				}
				break;

			case MS_ALPHA_PAINT:
				if (pPixelTemp->btA > 0)
				{
					REPLACE_32_PIXEL(pPixelDest, pPixelTemp);
				}
				break;

			default:
				break;
			}

			pPixelDest++;
			lYrStride--;
			lSrcX16 += lXrIntFloat_16;

		}while (lYrStride > 0);

		return lSrcX16;
	}

	void CDIB::bilinearBoder(const tagBitmap &bmp, LONG lX16, LONG lY16, BYTE btOpacity, tagPixel *pPixelResult)
	{
		LONG lX = lX16 >> 16;
		LONG lY = lY16 >> 16;	

		tagPixel pixels[4];
		BILINEAR_GET_PIXEL(pixels[0], bmp, lX, lY);
		BILINEAR_GET_PIXEL(pixels[1], bmp, lX + 1, lY);
		BILINEAR_GET_PIXEL(pixels[2], bmp, lX, lY + 1);
		BILINEAR_GET_PIXEL(pixels[3], bmp, lX + 1, lY + 1);

		ULONG ulU2 = (lX16 & 0xFFFF) >> 14;
		ULONG ulV2 = (lY16 & 0xFFFF) >> 14;

		BilinearFunc pBilinearFun = g_aBilinearFuncs[(ulV2 << 2) | ulU2];
		UINT uiSrc = pBilinearFun(*(UINT*)&pixels[0], *(UINT*)&pixels[1], *(UINT*)&pixels[2], *(UINT*)&pixels[3]);

		tagPixel	*pPixelSrc= (tagPixel *)&uiSrc;

		switch (m_eMergeState)
		{
		case MS_ALPHA_BLEND:
			if (pPixelSrc->btA > 0)
			{
				RENDER_32_PIXEL(pPixelResult, pPixelSrc);
			}
			break;

		case MS_ALPHA_PAINT:
			if (pPixelSrc->btA > 0)
			{
				REPLACE_32_PIXEL(pPixelResult, pPixelSrc);
			}
			break;

		default:
			break;
		}
	}

	CDIB::tagPixel CDIB::modifyColor(UINT uiColor)
	{
		tagPixel pixelColor = *(tagPixel *)&uiColor;
		UINT uiAlpha255To256 = ALPHA_255_TO_256(pixelColor.btA);

		pixelColor.btR = (pixelColor.btR * uiAlpha255To256) >> 8;
		pixelColor.btG = (pixelColor.btG * uiAlpha255To256) >> 8;
		pixelColor.btB = (pixelColor.btB * uiAlpha255To256) >> 8;

		return pixelColor;
	}

	void CDIB::setPixel(INT iX, INT iY, tagPixel& rPixel)
	{
		if ( iX >= 0 && iX < INT(m_uiWidth) && iY < INT(m_uiHeight) && iY >= 0 )
		{
			tagPixel* pCurPixelData = (tagPixel*)m_pbtPixelData + iY * m_uiWidth + iX;	
			
// 			switch (m_eMergeState)
// 			{
// 			case MS_MERGE:
				RENDER_32_PIXEL(pCurPixelData, (&rPixel));
// 				break;
// 
// 			case MS_REPLACE:
// 				REPLACE_32_PIXEL(pCurPixelData, (&rPixel));
// 				break;
// 
// 			default:
// 				break;
// 			}
		}
	}

	void CDIB::drawLineBresenham(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine)
	{
		// iX1 < iX2
		tagPixel pixelColor = modifyColor(uiColor);
		
		if ( iY1 <= iY2 )
		{
			INT x = iX1;
			INT y = iY1;
			INT dx = iX2-iX1;
			INT dy = iY2-iY1;
			if ( dx >= dy )
			{
				FLOAT k = FLOAT(dy)/dx;
				FLOAT e = -0.5f;
				for(INT i = 0; i <= dx; ++i)
				{	
					setPixel(x,y,pixelColor);

					++x;
					e=e+k;

					if( e >= 0 )  
					{ 
						++y; 
						e = e - 1.0f;
					}
				}
			}
			else
			{
				FLOAT k = FLOAT(dx)/dy;
				FLOAT e = -0.5f;
				for(INT i = 0; i <= dy; ++i)
				{	
					setPixel(x,y,pixelColor);

					++y;
					e=e+k;

					if( e >= 0 )  
					{ 
						++x; 
						e = e - 1.0f;
					}
				}
			}
		}
		else
		{
			INT x = iX1;
			INT y = iY1;
			INT dx = iX2-iX1;
			INT dy = iY2-iY1;
			if ( dx >= -dy )
			{
				FLOAT k = FLOAT(dy)/dx;
				FLOAT e = 0.5f;
				for(INT i = 0; i <= dx; ++i)
				{	
					setPixel(x,y,pixelColor);

					++x;
					e=e+k;

					if( e <= 0 )  
					{ 
						--y; 
						e = e + 1.0f;
					}
				}
			}
			else
			{
				FLOAT k = FLOAT(dx)/dy;
				FLOAT e = 0.5f;
				for(INT i = 0; i <= -dy; ++i)
				{	
					setPixel(x,y,pixelColor);

					--y;
					e=e+k;

					if( e >= 0 )  
					{ 
						++x; 
						e = e + 1.0f;
					}
				}
			}
		}
	}

	void CDIB::drawLineDDA(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine, BOOLEAN bIncX )
	{
		tagPixel pixelColor = modifyColor(uiColor);
		INT dx = iX2-iX1;
		INT dy = iY2-iY1;

		if ( bIncX )
		{ // |k| <= 1		
			FLOAT k = FLOAT(dy)/dx;
			FLOAT y= FLOAT(iY1);        

			if( bDashLine )
			{
				INT delta = 0;
				for (INT x = iX1; x <= iX2 ; )
				{
					INT iy = INT(y+0.5f);
					setPixel(x,iy,pixelColor);

					if ( delta == DASH_LINE_WIDTH-1 )
					{
						y += DASH_LINE_WIDTH * k;
						x += DASH_LINE_WIDTH;
						delta = 0;
					}
					else
					{
						++delta;
						y += k;
						x += 1;
					}					
				} 
			}
			else
			{
				for (INT x = iX1; x <= iX2 ; ++x)
				{
					INT iy = INT(y+0.5f);
					setPixel(x,iy,pixelColor);

					y += k;
				} 
			}
		}
		else
		{ // |k| > 1
			FLOAT k = FLOAT(dx)/dy;
			FLOAT x= FLOAT(iX1);    

			if( bDashLine )
			{
				INT delta = 0;
				for (INT y = iY1; y <= iY2 ; )
				{
					INT ix = INT(x+0.5f);
					setPixel(ix,y,pixelColor);

					if ( delta == DASH_LINE_WIDTH-1 )
					{
						x += DASH_LINE_WIDTH * k;
						y += DASH_LINE_WIDTH;
						delta = 0;
					}
					else
					{
						++delta;
						x += k;
						y += 1;
					}
				}
			}
			else
			{
				for (INT y = iY1; y <= iY2 ; ++y)
				{
					INT ix = INT(x+0.5f);
					setPixel(ix,y,pixelColor);

					x+=k;
				}
			}
		}

	}
}