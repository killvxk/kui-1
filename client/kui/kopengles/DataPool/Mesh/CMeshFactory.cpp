#include "RenderEngine.h"

namespace OpenHMI
{

	static const INT DASH_LINE_WIDTH = 3;

	void CMeshFactory::generateLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine, 
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt)
	{
		uiVtxCnt = getLineVtxCnt(iX1,iY1,iX2,iY2,bDashLine);
		uiIdxCnt = bDashLine ? ( uiVtxCnt/2 * 2) : 2; 
		const UINT uiVtxSize = 6;
		FLOAT* pVtxBuf = new FLOAT[uiVtxCnt * uiVtxSize];
		USHORT* pIdxBuf = new USHORT[uiIdxCnt];
		if( ppVtxBuf )
		{
			*ppVtxBuf = pVtxBuf;
		}

		if( ppIdxBuf )
		{
			*ppIdxBuf = pIdxBuf;
		}

		constructLine(pVtxBuf,pIdxBuf, 0, 0,	 iX1, iY1, iX2, iY2, uiColor, uiVtxCnt != 2);
	}

	void CMeshFactory::generateRectangle(UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine, 
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt)
	{
		INT iX1 = 0;
		INT iX2 = 0 + (INT)uiWidth - 1;
		INT iY1 = 0;
		INT iY2 = 0 + (INT)uiHeight - 1;

		UINT uiVtxNum1 = getLineVtxCnt( iX1, iY1, iX2, iY1,bDashLine);
		UINT uiIdxNum1 = bDashLine ? ( uiVtxNum1/2 * 2) : 2; 

		UINT uiVtxNum2 = getLineVtxCnt(iX2, iY1, iX2, iY2,bDashLine);
		UINT uiIdxNum2 = bDashLine ? ( uiVtxNum2/2 * 2) : 2; 

		UINT uiVtxNum3 = getLineVtxCnt(iX2, iY2, iX1, iY2,bDashLine);
		UINT uiIdxNum3 = bDashLine ? ( uiVtxNum3/2 * 2) : 2; 

		UINT uiVtxNum4 = getLineVtxCnt(iX1, iY2, iX1, iY1,bDashLine);
		UINT uiIdxNum4 = bDashLine ? ( uiVtxNum4/2 * 2) : 2; 

		UINT uiVtxSize = 6;

		uiVtxCnt = uiVtxNum1 + uiVtxNum2 + uiVtxNum3 + uiVtxNum4;
		uiIdxCnt = uiIdxNum1 + uiIdxNum2 + uiIdxNum3 + uiIdxNum4; 
		
		FLOAT* pVtxBuf = new FLOAT[uiVtxCnt * uiVtxSize];
		USHORT* pIdxBuf = new USHORT[uiIdxCnt];

		if( ppVtxBuf )
		{
			*ppVtxBuf = pVtxBuf;
		}

		if( ppIdxBuf )
		{
			*ppIdxBuf = pIdxBuf;
		}

		constructLine(pVtxBuf, pIdxBuf, 0,								0,								iX1, iY1, iX2, iY1, uiColor,	uiVtxNum1 != 2);
		constructLine(pVtxBuf, pIdxBuf, uiVtxNum1,						uiIdxNum1,						iX2, iY1, iX2, iY2, uiColor,	uiVtxNum2 != 2);
		constructLine(pVtxBuf, pIdxBuf, uiVtxNum1+uiVtxNum2,			uiIdxNum1+uiIdxNum2,			iX2, iY2, iX1, iY2, uiColor,	uiVtxNum3 != 2);
		constructLine(pVtxBuf, pIdxBuf, uiVtxNum1+uiVtxNum2+uiVtxNum3,	uiIdxNum1+uiIdxNum2+uiIdxNum3,	iX1, iY2, iX1, iY1, uiColor,	uiVtxNum4 != 2);
	}

	void CMeshFactory::generateTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine,
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt)
	{
		UINT uiVtxNum1 = getLineVtxCnt(p1.getX(), p1.getY(), p2.getX(), p2.getY(),bDashLine);
		UINT uiIdxNum1 = bDashLine ? ( uiVtxNum1/2 * 2) : 2; 

		UINT uiVtxNum2 = getLineVtxCnt(p1.getX(), p1.getY(), p3.getX(), p3.getY(),bDashLine);
		UINT uiIdxNum2 = bDashLine ? ( uiVtxNum2/2 * 2) : 2; 

		UINT uiVtxNum3 = getLineVtxCnt(p3.getX(), p3.getY(), p2.getX(), p2.getY(),bDashLine);
		UINT uiIdxNum3 = bDashLine ? ( uiVtxNum3/2 * 2) : 2; 

		uiVtxCnt = uiVtxNum1 + uiVtxNum2 + uiVtxNum3;
		uiIdxCnt = uiIdxNum1 + uiIdxNum2 + uiIdxNum3; 
		
		UINT uiVtxSize = 6;
		FLOAT* pVtxBuf = new FLOAT[uiVtxCnt * uiVtxSize];
		USHORT* pIdxBuf = new USHORT[uiIdxCnt];

		if( ppVtxBuf )
		{
			*ppVtxBuf = pVtxBuf;
		}

		if( ppIdxBuf )
		{
			*ppIdxBuf = pIdxBuf;
		}

		constructLine(pVtxBuf, pIdxBuf, 0, 0,										p1.getX(), p1.getY(), p2.getX(), p2.getY(), uiColor, uiVtxNum1 != 2);
		constructLine(pVtxBuf, pIdxBuf, uiVtxNum1, uiIdxNum1,						p1.getX(), p1.getY(), p3.getX(), p3.getY(), uiColor, uiVtxNum2 != 2);
		constructLine(pVtxBuf, pIdxBuf, uiVtxNum1+uiVtxNum2, uiIdxNum1+uiIdxNum2,	p3.getX(), p3.getY(), p2.getX(), p2.getY(), uiColor, uiVtxNum3 != 2);
	}

	void CMeshFactory::generateCircle(UINT uiRadius, UINT uiColor, BOOLEAN bDashLine, 
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt)
	{
		UINT uiVtxidx = 0;
		UINT uiIdxidx = 0;
		UINT cnt = 0;
		const UINT uiVtxSize = 6;
		C3DColor color(uiColor);

		const FLOAT pi = 3.1415f;
		FLOAT x = 0.0f;
		FLOAT y = 0.0f;
		FLOAT step = pi/90.0f ;
		if ( bDashLine )
		{
			step *= 4;
		}
		
		uiVtxCnt = UINT((2 * pi + step)/step) + 1;
		uiIdxCnt = bDashLine ? (uiVtxCnt/2 * 2) : 2 * (uiVtxCnt-1);

		FLOAT* pVtxBuf = new FLOAT[uiVtxCnt * uiVtxSize];
		USHORT* pIdxBuf = new USHORT[uiIdxCnt];

		if( ppVtxBuf )
		{
			*ppVtxBuf = pVtxBuf;
		}

		if( ppIdxBuf )
		{
			*ppIdxBuf = pIdxBuf;
		}
		
		for(FLOAT angle = 0.0f; angle <= 2 * pi; angle += step )   
        {   
			x   = FLOAT(uiRadius * Math::sin(angle));   
            y   = FLOAT(uiRadius * Math::cos(angle));   

            pVtxBuf[cnt*uiVtxSize + 0] = x ;
			pVtxBuf[cnt*uiVtxSize + 1] = y ;
			pVtxBuf[cnt*uiVtxSize + 2] = color.getR();
			pVtxBuf[cnt*uiVtxSize + 3] = color.getG();
			pVtxBuf[cnt*uiVtxSize + 4] = color.getB();
			pVtxBuf[cnt*uiVtxSize + 5] = color.getA();
			++cnt;
        }  

		if ( bDashLine )
		{
			UINT uiAdjustVtxCnt = (cnt/2)*2;
			for( UINT i = 0; i < uiAdjustVtxCnt ; i+=2 )
			{
				pIdxBuf[uiIdxidx++] = uiVtxidx + i;
				pIdxBuf[uiIdxidx++] = uiVtxidx + i+1;
			}
			if ( uiAdjustVtxCnt != cnt )
			{
				pIdxBuf[uiIdxidx-1] = cnt-1;
			}
		}
		else
		{
			for( UINT i = 0; i < cnt-1 ; ++i )
			{
				pIdxBuf[uiIdxidx++] = i;
				pIdxBuf[uiIdxidx++] = i+1;
			}
		}
	}

	void CMeshFactory::generateSolidRectangle(UINT uiWidth, UINT uiHeight, UINT uiColor,
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt)
	{
		uiVtxCnt = 4;
		uiIdxCnt = 6;
		UINT uiVtxidx = 0;
		UINT uiIdxidx = 0;
		const UINT uiVtxSize = 6;
		UINT cnt = 0;

		FLOAT* pVtxBuf = new FLOAT[uiVtxCnt * uiVtxSize];
		USHORT* pIdxBuf = new USHORT[uiIdxCnt];

		if( ppVtxBuf )
		{
			*ppVtxBuf = pVtxBuf;
		}

		if( ppIdxBuf )
		{
			*ppIdxBuf = pIdxBuf;
		}

		C3DColor color(uiColor);
		CRectangleF rectf;
		rectf.set(FLOAT(0),FLOAT(0), FLOAT(uiWidth-1), FLOAT(uiHeight-1)); 

		pIdxBuf[uiIdxidx+0] = uiVtxidx + 0;
		pIdxBuf[uiIdxidx+1] = uiVtxidx + 1;
		pIdxBuf[uiIdxidx+2] = uiVtxidx + 2;
		pIdxBuf[uiIdxidx+3] = uiVtxidx + 0;
		pIdxBuf[uiIdxidx+4] = uiVtxidx + 2;
		pIdxBuf[uiIdxidx+5] = uiVtxidx + 3;

		pVtxBuf[cnt*uiVtxSize + 0] = rectf.getX();
		pVtxBuf[cnt*uiVtxSize + 1] = rectf.getY();
		pVtxBuf[cnt*uiVtxSize + 2] = color.getR();
		pVtxBuf[cnt*uiVtxSize + 3] = color.getG();
		pVtxBuf[cnt*uiVtxSize + 4] = color.getB();
		pVtxBuf[cnt*uiVtxSize + 5] = color.getA();
		++cnt;

		pVtxBuf[cnt*uiVtxSize + 0] = rectf.getX();
		pVtxBuf[cnt*uiVtxSize + 1] = rectf.getBottom() + 1;
		pVtxBuf[cnt*uiVtxSize + 2] = color.getR();
		pVtxBuf[cnt*uiVtxSize + 3] = color.getG();
		pVtxBuf[cnt*uiVtxSize + 4] = color.getB();
		pVtxBuf[cnt*uiVtxSize + 5] = color.getA();
		++cnt;

		pVtxBuf[cnt*uiVtxSize + 0] = rectf.getRight() + 1;
		pVtxBuf[cnt*uiVtxSize + 1] = rectf.getBottom() + 1;
		pVtxBuf[cnt*uiVtxSize + 2] = color.getR();
		pVtxBuf[cnt*uiVtxSize + 3] = color.getG();
		pVtxBuf[cnt*uiVtxSize + 4] = color.getB();
		pVtxBuf[cnt*uiVtxSize + 5] = color.getA();
		++cnt;

		pVtxBuf[cnt*uiVtxSize + 0] = rectf.getRight() + 1;
		pVtxBuf[cnt*uiVtxSize + 1] = rectf.getY();
		pVtxBuf[cnt*uiVtxSize + 2] = color.getR();
		pVtxBuf[cnt*uiVtxSize + 3] = color.getG();
		pVtxBuf[cnt*uiVtxSize + 4] = color.getB();
		pVtxBuf[cnt*uiVtxSize + 5] = color.getA();
		++cnt;
	}
	
	void CMeshFactory::generatePlaneMeshH(UINT sx, UINT sy, INT dx, INT dy, CRectangleF* pRoi,
											FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt)
	{
	#define PlaneHeight 0.0f

		uiVtxCnt = dx*dy;
		uiIdxCnt = (dx-1)*(dy-1)* 6 ;
		
		const UINT uiVtxSize = 8;
		FLOAT* PlaneVBO = new FLOAT[uiVtxCnt * uiVtxSize];
		USHORT* PlaneIBO = new USHORT[uiIdxCnt];
		
		if ( ppVtxBuf )	
		{
			*ppVtxBuf = PlaneVBO;
		}

		if ( ppIdxBuf ) 
		{
			*ppIdxBuf = PlaneIBO;
		}

		INT x, y;

		// UV
		CRectangleF uv;
		if (pRoi) 
		{
			uv = *pRoi;
		}
		else 
		{
			uv.set(0.0f,0.0f,1.0f,1.0f);
		}

		INT slotId=0;
		for (y=0; y<dy; y++) {
			for (x=0; x<dx; x++) {
				PlaneVBO[slotId+0] = (((FLOAT)x)/(dx-1)-0.5f)*2*sx;
				PlaneVBO[slotId+1] = PlaneHeight;
				PlaneVBO[slotId+2] = (((FLOAT)y)/(dy-1)-0.5f)*2*sy;
				
				PlaneVBO[slotId+3] = 0.0f;
				PlaneVBO[slotId+4] = 1.0f;
				PlaneVBO[slotId+5] = 0.0f;
				
				PlaneVBO[slotId+6] = uv.getX() + ((FLOAT)x)/(dx-1)*uv.getWidth();
				PlaneVBO[slotId+7] = uv.getY() + ((FLOAT)y)/(dy-1)*uv.getHeight();

				slotId+=8;
			}
		}

		slotId = 0;

		for (y=0; y<dy-1; y++) {
			for (x=0; x<dx-1; x++) {
				PlaneIBO[slotId] = y*dx+x;
				PlaneIBO[slotId+1] = y*dx+x+1;
				PlaneIBO[slotId+2] = (y+1)*dx+x;
				PlaneIBO[slotId+3] = y*dx+x+1;
				PlaneIBO[slotId+4] = (y+1)*dx+x+1;
				PlaneIBO[slotId+5] = (y+1)*dx+x;
				slotId+=6;
			}
		}
		
		return;
	}

	void CMeshFactory::generatePlaneMeshV(UINT sx, UINT sy, INT dx, INT dy,
									  FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt)
	{
		#define PlaneZ 1.0f

		uiVtxCnt = dx*dy;
		uiIdxCnt = (dx-1)*(dy-1)* 6 ;
		
		FLOAT* pfPlaneVBO = new FLOAT[uiVtxCnt * 5];
		if (pfPlaneVBO == NULL)
		{
			return;
		}
		
		USHORT* pusPlaneIBO = new USHORT[uiIdxCnt];
		if (pusPlaneIBO == NULL)
		{
			free(pfPlaneVBO);
			return;
		}

		if ( ppVtxBuf )	
		{
			*ppVtxBuf = pfPlaneVBO;
		}

		if ( ppIdxBuf ) 
		{
			*ppIdxBuf = pusPlaneIBO;
		}

		INT x, y;
		INT slotId = 0;
		for (y = 0; y < dy; y++) \
		{
			for (x = 0; x < dx; x++) 
			{
				pfPlaneVBO[slotId + 0] = ((FLOAT)x)/(dx - 1)*sx;
				pfPlaneVBO[slotId + 1] = ((FLOAT)y)/(dy - 1)*sy;
				pfPlaneVBO[slotId + 2] = PlaneZ;

				// UV 0 - 1
				pfPlaneVBO[slotId + 3] = ((FLOAT)x)/(dx - 1);
				pfPlaneVBO[slotId + 4] = 1.0f - ((FLOAT)y)/(dy - 1);

				slotId += 5;
			}
		}

		slotId = 0;

		for (y = 0; y < dy-1; y++) 
		{
			for (x = 0; x < dx-1; x++) 
			{
				pusPlaneIBO[slotId + 0] = y*dx+x;        //0
				pusPlaneIBO[slotId + 1] = (y+1)*dx+x;    //4
				pusPlaneIBO[slotId + 2] = y*dx+x+1;      //1
				pusPlaneIBO[slotId + 3] = y*dx+x+1;      //1
				pusPlaneIBO[slotId + 4] = (y+1)*dx+x;    //4
				pusPlaneIBO[slotId + 5] = (y+1)*dx+x+1;  //5

				slotId += 6;
			}
		}
	}

	UINT CMeshFactory::getLineVtxCnt(INT iX1, INT iY1, INT iX2, INT iY2, BOOLEAN bDashLine)
	{
		if ( !bDashLine )
		{
			return 2;
		}
		else
		{
			UINT uiVtxCnt = 0;
			INT step = DASH_LINE_WIDTH ;
			INT dx = iX2-iX1;
			INT dy = iY2-iY1;

			if ( dx == 0 )//for(INT  y = startY; y <= endY; y += step )   
			{
				INT startY = iY1;
				INT endY = iY2;
				if ( iY1 > iY2 )
				{
					startY = iY2;
					endY = iY1;
				} 

				uiVtxCnt = (endY - startY + 1 + step-1)/step;
			}
			else if ( Math::abs(dx) >= Math::abs(dy) )//for (INT x = iX1; x <= iX2 ; x += DASH_LINE_WIDTH)
			{
				if ( iX1 > iX2 )
				{
					uiVtxCnt = (iX1-iX2 + 1 + step - 1)/step;
				}
				else if( iX1 < iX2 )
				{
					uiVtxCnt = (iX2-iX1 + 1 + step-1)/step;
				}
			}
			else //for (INT y = iY1; y <= iY2 ; y += DASH_LINE_WIDTH)
			{
				if ( iY1 > iY2 )
				{
					uiVtxCnt = (iY1-iY2 + 1 +step-1)/step;
				}
				else if( iY1 < iY2 )
				{
					uiVtxCnt = (iY2-iY1 + 1 +step-1)/step;
				}
			}
			
			if( uiVtxCnt < 4 )
			{	
				uiVtxCnt = 2;
			}
			
			return uiVtxCnt;
		}
	}

	void CMeshFactory::constructLine(FLOAT* pVertexBuf, USHORT* pIdxBuf, UINT uiVtxOffset,UINT uiIdxOffset, INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine)
	{
		UINT uiVtxidx = uiVtxOffset;
		UINT uiIdxidx = 0;
		const UINT uiVtxSize = 6;
		C3DColor color(uiColor);
		UINT cnt = 0;
		FLOAT* pCurVtxBuf = pVertexBuf + uiVtxOffset * uiVtxSize;
		USHORT* pCurIdxBuf = pIdxBuf + uiIdxOffset;

		if ( bDashLine )
		{
			INT step = DASH_LINE_WIDTH ;
			INT dx = iX2-iX1;
			INT dy = iY2-iY1;

			if ( dx == 0 )
			{
				INT startY = iY1;
				INT endY = iY2;
				if ( iY1 > iY2 )
				{
					startY = iY2;
					endY = iY1;
				}

				INT  y = startY;

				for(; y <= endY; y += step )   
				{   
					pCurVtxBuf[cnt*uiVtxSize + 0] = FLOAT(iX1);
					pCurVtxBuf[cnt*uiVtxSize + 1] = FLOAT(y);
					pCurVtxBuf[cnt*uiVtxSize + 2] = color.getR();
					pCurVtxBuf[cnt*uiVtxSize + 3] = color.getG();
					pCurVtxBuf[cnt*uiVtxSize + 4] = color.getB();
					pCurVtxBuf[cnt*uiVtxSize + 5] = color.getA();
					++cnt;
				} 

				if( y < (endY + step) )
				{
					pCurVtxBuf[(cnt-1)*uiVtxSize + 1] = FLOAT(endY);
				}

				UINT uiAdjustVtxCnt = (cnt/2)*2;
				for( UINT i = 0; i < uiAdjustVtxCnt ; i+=2 )
				{
					pCurIdxBuf[uiIdxidx++] = uiVtxidx + i;
					pCurIdxBuf[uiIdxidx++] = uiVtxidx + i+1;
				}
				if ( uiAdjustVtxCnt != cnt )
				{
					pCurIdxBuf[uiIdxidx-1] = uiVtxOffset + cnt-1;
				}

				return;
			}
			else if ( Math::abs(dx) >= Math::abs(dy) )
			{
				if ( iX1 > iX2 )
				{
					constructObliqueLine(pVertexBuf,pIdxBuf,uiVtxOffset,uiIdxOffset, iX2, iY2, iX1, iY1, color, TRUE);
				}
				else if( iX1 < iX2 )
				{
					constructObliqueLine(pVertexBuf,pIdxBuf,uiVtxOffset,uiIdxOffset, iX1, iY1, iX2, iY2, color, TRUE);
				}
			}
			else 
			{
				if ( iY1 > iY2 )
				{
					constructObliqueLine(pVertexBuf,pIdxBuf,uiVtxOffset,uiIdxOffset, iX2, iY2, iX1, iY1, color, FALSE);
				}
				else if( iY1 < iY2 )
				{
					constructObliqueLine(pVertexBuf,pIdxBuf,uiVtxOffset,uiIdxOffset, iX1, iY1, iX2, iY2, color,  FALSE);
				}
			}
		}
		else
		{
			pCurIdxBuf[uiIdxidx+0] = uiVtxidx + 0;
			pCurIdxBuf[uiIdxidx+1] = uiVtxidx + 1;

			pCurVtxBuf[cnt*uiVtxSize + 0] = FLOAT(iX1);
			pCurVtxBuf[cnt*uiVtxSize + 1] = FLOAT(iY1);
			pCurVtxBuf[cnt*uiVtxSize + 2] = color.getR();
			pCurVtxBuf[cnt*uiVtxSize + 3] = color.getG();
			pCurVtxBuf[cnt*uiVtxSize + 4] = color.getB();
			pCurVtxBuf[cnt*uiVtxSize + 5] = color.getA();
			++cnt;

			pCurVtxBuf[cnt*uiVtxSize + 0] = FLOAT(iX2);
			pCurVtxBuf[cnt*uiVtxSize + 1] = FLOAT(iY2);
			pCurVtxBuf[cnt*uiVtxSize + 2] = color.getR();
			pCurVtxBuf[cnt*uiVtxSize + 3] = color.getG();
			pCurVtxBuf[cnt*uiVtxSize + 4] = color.getB();
			pCurVtxBuf[cnt*uiVtxSize + 5] = color.getA();
			++cnt;
		}
	}

	void CMeshFactory::constructObliqueLine(FLOAT* pVertexBuf, USHORT* pIdxBuf,UINT uiVtxOffset,UINT uiIdxOffset, INT iX1, INT iY1, INT iX2, INT iY2, const C3DColor& color, BOOLEAN bIncX)
	{
		INT dx = iX2-iX1;
		INT dy = iY2-iY1;

		UINT uiVtxidx = uiVtxOffset;
		UINT uiIdxidx = 0;
		const UINT uiVtxSize = 6;
		UINT cnt = 0;
		FLOAT* pCurVtxBuf = pVertexBuf + uiVtxOffset* uiVtxSize;
		USHORT* pCurIdxBuf = pIdxBuf + uiIdxOffset;

		if ( bIncX )
		{ // |k| <= 1		
			FLOAT k = FLOAT(dy)/dx;
			FLOAT y= FLOAT(iY1);        
			INT x = iX1;
			INT iy = 0;

			for ( ; x <= iX2 ; x += DASH_LINE_WIDTH)
			{
				iy = INT(y+0.5f);

				pCurVtxBuf[cnt*uiVtxSize + 0] = FLOAT(x);
				pCurVtxBuf[cnt*uiVtxSize + 1] = FLOAT(iy);
				pCurVtxBuf[cnt*uiVtxSize + 2] = color.getR();
				pCurVtxBuf[cnt*uiVtxSize + 3] = color.getG();
				pCurVtxBuf[cnt*uiVtxSize + 4] = color.getB();
				pCurVtxBuf[cnt*uiVtxSize + 5] = color.getA();

				y += DASH_LINE_WIDTH * k;
				++cnt;
			} 
		}
		else
		{ // |k| > 1
			FLOAT k = FLOAT(dx)/dy;
			FLOAT x= FLOAT(iX1);   
			INT y = iY1;
			INT ix = 0;

			for ( ; y <= iY2 ; y += DASH_LINE_WIDTH)
			{
				ix = INT(x+0.5f);

				pCurVtxBuf[cnt*uiVtxSize + 0] = FLOAT(ix);
				pCurVtxBuf[cnt*uiVtxSize + 1] = FLOAT(y);
				pCurVtxBuf[cnt*uiVtxSize + 2] = color.getR();
				pCurVtxBuf[cnt*uiVtxSize + 3] = color.getG();
				pCurVtxBuf[cnt*uiVtxSize + 4] = color.getB();
				pCurVtxBuf[cnt*uiVtxSize + 5] = color.getA();

				x+= DASH_LINE_WIDTH * k;
				++cnt;
			}	
		}

		pCurVtxBuf[(cnt-1)*uiVtxSize + 0] = FLOAT(iX2);
		pCurVtxBuf[(cnt-1)*uiVtxSize + 1] = FLOAT(iY2);

		UINT uiAdjustVtxCnt = (cnt/2)*2;
		for( UINT i = 0; i < uiAdjustVtxCnt ; i+=2 )
		{
			pCurIdxBuf[uiIdxidx++] = uiVtxidx + i;
			pCurIdxBuf[uiIdxidx++] = uiVtxidx + i+1;
		}
		if ( uiAdjustVtxCnt != cnt )
		{
			pCurIdxBuf[uiIdxidx-1] = uiVtxOffset + cnt-1;
		}
	}
}
	