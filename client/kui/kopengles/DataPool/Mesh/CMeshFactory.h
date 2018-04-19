#pragma once

namespace OpenHMI
{
	class CMeshFactory
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		static void	generateLine(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine, 
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt);

		static void	generateRectangle(UINT uiWidth, UINT uiHeight, UINT uiColor, BOOLEAN bDashLine, 
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt);

		static void	generateTriangle(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine,
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt);

		static void	generateCircle(UINT uiRadius, UINT uiColor, BOOLEAN bDashLine, 
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt);

		static void	generateSolidRectangle(UINT uiWidth, UINT uiHeight, UINT uiColor,
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt);

		static void generatePlaneMeshV(UINT sx, UINT sy, INT dx, INT dy,
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt);

		static void generatePlaneMeshH(UINT sx, UINT sy, INT dx, INT dy,CRectangleF* pRoi, 
									FLOAT** ppVtxBuf, UINT& uiVtxCnt, USHORT** ppIdxBuf, UINT& uiIdxCnt);
		
	protected:// method
		CMeshFactory(){}
		CMeshFactory(const CMeshFactory&){}
		virtual ~CMeshFactory(){}		
		CMeshFactory& operator =(const CMeshFactory&){return *this;}

	private:// method
		static UINT getLineVtxCnt(INT iX1, INT iY1, INT iX2, INT iY2, BOOLEAN bDashLine);
		static void constructLine(FLOAT* pVertexBuf, USHORT* pIdxBuf, UINT uiVtxOffset, UINT uiIdxOffset ,INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine);
		static void constructObliqueLine(FLOAT* pVertexBuf, USHORT* pIdxBuf,UINT uiVtxOffset, UINT uiIdxOffset, INT iX1, INT iY1, INT iX2, INT iY2, const C3DColor& color, BOOLEAN bIncX);
		
	protected:// property	
		
	private:// property		

	};
}