#pragma once

namespace OpenHMI
{
#define ALPHA_255_TO_256(alpha) ((UINT)(alpha) + ((UINT)(alpha) >> 7));

	class CDIB : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		enum EHorizontalLayout
		{
			HL_LEFT			= 0,
			HL_CENTER		= 1,
			HL_RIGHT		= 2,
		};

		enum EVerticalLayout
		{
			VL_TOP			= 0,
			VL_MIDDLE		= 1,
			VL_BOTTOM		= 2,
		};

		enum EStretchLayout
		{
			SL_CENTER		= 0,
			SL_FILL			= 1,
		};

		enum EMergeState
		{
			MS_ALPHA_BLEND		= 0,
			MS_ALPHA_PAINT		= 1, // if src alpha > 0, replace dest, else use dest
		};

	private:// embed class or struct or enum
		// these color values order accord with the bitmap format,
		// which is B,G,R,A in memory
		struct tagPixel
		{
			BYTE	btB;
			BYTE	btG;
			BYTE	btR;
			BYTE	btA;
		};

		struct tagBitmap
		{
			BYTE	*pbtData;
			INT		iWidth;
			INT		iHeight;
			INT		iStride;
		};

	public:// method
		CDIB();
		virtual ~CDIB();

		UINT		getWidth() const		{ return m_uiWidth; }
		UINT		getHeight() const		{ return m_uiHeight; }
		UINT		getStride() const		{ return m_uiWidth * 4; }
		const BYTE*	getPixelData() const	{ return m_pbtPixelData; }

		void	writeToFile(const String &strFilePath);
		void*	getDIBInfo();

		void	createDIB(UINT uiWidth, UINT uiHeight, UINT uiColor = 0);
		void	createDIB(UINT uiWidth, UINT uiHeight, const BYTE *pbtPixelData, UINT uiPixelCount = 0);
		void	moveDIBFrom(CDIB *pSrcDib);
		void	copyDIBFrom(CDIB *pSrcDib, INT iSrcX, INT iSrcY, INT iSrcWidth, INT iSrcHeight);
		void	cutDIBFrom(CDIB *pSrcDib, INT iSrcX, INT iSrcY, INT iSrcWidth, INT iSrcHeight);
		void	destroyDIB();

		// this method is only used for rendering text glyph.
		void	merge8DIB(	INT iDestX, INT iDestY,
							const BYTE *pbtSrcData, INT iSrcWidth, INT iSrcHeight, BYTE btSrcOpacity, UINT uiSrcColor);

		void			setMergeState(EMergeState eValue)	{ m_eMergeState = eValue; }
		EMergeState		getMergeState() const				{ return m_eMergeState; }

		void	merge32DIB(	INT iDestX, INT iDestY, CRectangle rcClipBox,
							const BYTE *pbtSrcData, INT iSrcWidth, INT iSrcHeight, BYTE btSrcOpacity);

		void	merge32DIB(	INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight,
							const BYTE *pbtSrcData, INT iSrcWidth, INT iSrcHeight, BYTE btSrcOpacity,
							EHorizontalLayout eHLayout, EVerticalLayout eVLayout);		

		void	merge32DIB(	INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight,
							const BYTE *pbtSrcData, INT iSrcWidth, INT iSrcHeight, BYTE btSrcOpacity,
							EStretchLayout eSLayout);

		void	mergeMaskDIB(const BYTE *pbtSrcData, INT iSrcWidth, INT iSrcHeight);

		void	replace( INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight, const BYTE* ptSrcData);
		void	setColor( INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight, UINT uiSrcColor);
		void	flipVertically();

		void	drawRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine);
		void	drawTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine);
		void	drawLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine);
		void	drawCircle(INT iX, INT iY, UINT uiRadius, UINT uiColor, BOOLEAN bDashLine);		
		void	fillRectangle(INT iX, INT iY, UINT uiWidth, UINT uiHeight, UINT uiColor);

		static INT	calcXByLayout(INT iDestX, INT iSrcWidth, INT iDestWidth, CDIB::EHorizontalLayout eHoriLayout);
		static INT	calcYByLayout(INT iDestY, INT iSrcHeight, INT iDestHeight, CDIB::EVerticalLayout eVectLayout);

		static CRectangle	calcDestRect(INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight,
									INT iSrcWidth, INT iSrcHeight, CRenderImage::ELayout layout);

		static CRectangle	calcDestRect(INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight,
									INT iSrcWidth, INT iSrcHeight, 
									CRenderText::EHorizontalLayout eHLayout, CRenderText::EVerticalLayout eVLayout);
	protected:// method	
		CDIB(const CDIB &){}
		CDIB &operator = (const CDIB &){return *this;}

	private:// method
		void	pasteMerge8DIB	(	INT iDestX, INT iDestY,
									const BYTE *pbtSrcData, 
									INT iSrcX, INT iSrcY, INT iSrcWidth, INT iSrcHeight, 
									INT iSrcStride, BYTE btSrcOpacity, UINT uiSrcColor);

		void	pasteMerge32DIB(	INT iDestX, INT iDestY,
									const BYTE *pbtSrcData, 
									INT iSrcX, INT iSrcY, INT iSrcWidth, INT iSrcHeight, 
									INT iSrcStride, BYTE btSrcOpacity);		

		void	stretchMerge32DIB(	INT iDestX, INT iDestY, INT iDestWidth, INT iDestHeight,
									const BYTE *pbtSrcData, 
									INT iSrcX, INT iSrcY, INT iSrcWidth, INT iSrcHeight, 
									INT iSrcStride, BYTE btSrcOpacity);

		UINT	bilinearScanline(	BYTE *pbtDestData, const BYTE *pbtSrcData,
									LONG lSrcX16, LONG lSrcY16, LONG lXrIntFloat_16, LONG lYrStride,
									INT iSrcStride, BYTE btSrcOpacity);

		void	bilinearBoder(const tagBitmap &bmp, LONG lX16, LONG lY16, BYTE btOpacity, tagPixel *pPixelResult);

		tagPixel	modifyColor(UINT uiColor);

		void	setPixel(INT iX, INT iY, tagPixel& rPixel);

		void	drawLineBresenham(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine);
		void	drawLineDDA(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine, BOOLEAN bIncX = TRUE);

	protected:// property

	private:// property
		BYTE	*m_pbtPixelData;
		UINT	m_uiWidth;
		UINT	m_uiHeight;

		void*	m_pDIBInfo;

		EMergeState	m_eMergeState;
	};
}