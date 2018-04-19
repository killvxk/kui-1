#pragma once

namespace OpenHMI
{
	class CMeshPool
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum


	public:// method	
		static	CMeshPool*	getInstance();
		static	void		deleteInstance();

		void		initialize();
		void		unInitialize();
		void		clear();

		CCacheMesh*		getExternalMesh(const String &strFilePath);

		CCacheMesh*		getRectangleMesh(UINT uiWidth,UINT uiHeight,UINT uiColor,BOOLEAN bDashLine);
		CCacheMesh*		getTriangleMesh(const CPoint& p1, const CPoint& p2, const CPoint& p3, UINT uiColor, BOOLEAN bDashLine);
		CCacheMesh*		getLineMesh(INT iX1, INT iY1, INT iX2, INT iY2, UINT uiColor, BOOLEAN bDashLine);
		CCacheMesh*		getCircleMesh(UINT uiRadius, UINT uiColor, BOOLEAN bDashLine);
		CCacheMesh*		getSolidRectangleMesh(UINT uiWidth,UINT uiHeight,UINT uiColor);	

		CCacheMesh*		getWaverMesh(UINT width, UINT height, UINT row, UINT col);
		CCacheMesh*		getRippleMesh(UINT width, UINT height, UINT row, UINT col);
		CCacheMesh*		getDynamicUIGPUMesh(UINT uiFmt,UINT uiVtxCount,UINT uiIdxCount);

		CDynamicUIMesh*	getDynamicUIMesh();

		void			freeMesh(CCacheMesh* pMesh);

	protected:// method	
		CMeshPool();
		virtual ~CMeshPool();

	private:// method		

	protected:// property

	private:// property
		static	CMeshPool	*s_pInstance;

		CachePool<String, CCacheMesh, tagStringCompare>				m_cachePool;
		ObjectPtr<CDynamicUIMesh>									m_ptrDynamicUIMesh;
	};
}