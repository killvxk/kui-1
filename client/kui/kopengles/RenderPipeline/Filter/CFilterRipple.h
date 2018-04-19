#pragma once

namespace OpenHMI
{
	class CFilterRipple : public CBaseFilter
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CFilterRipple();
		virtual ~CFilterRipple();

		virtual BOOLEAN fromRenderFilter(CRenderGroupCacheData* pRndGroupCacheData, const CRenderBaseFilter* pFilter);
		virtual void	doFilter(C2DBatchRenderGroup* pIn, C2DBatchRenderGroup* pbg, Array2<C2DBatchRenderGroup*>& aryOut);

	protected:// method
		CFilterRipple(const CFilterRipple&){}
		CFilterRipple& operator =(const CFilterRipple&){return *this;}

	private:// method
		void init();
		void initMesh();
		void updateRipple();
		void drop(FLOAT x, FLOAT y, FLOAT fWeight);
		
	protected:// property

	private:// property
		UINT					m_uiTouchRadius;
		UINT					m_uiDamping;

		UINT					m_uiWidth;
		UINT					m_uiHeight;
		UINT					m_uiPoolWidth;
		UINT					m_uiPoolHeight;

		FLOAT*					m_pfRippleCoeff;
		FLOAT*					m_pfRippleSource;
		FLOAT*					m_pfRippleDest;

		CCacheMeshPtr			m_pMesh;
		CCacheMeshPtr			m_pVBOMesh;

		CRenderFltRipple*		m_pRndFilter;
		C2DBatchRenderGroupPtr	m_pBatchRenderGroup;
	};
}