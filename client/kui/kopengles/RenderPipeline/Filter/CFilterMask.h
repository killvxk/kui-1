#pragma once

namespace OpenHMI
{
	class CFilterMask : public CBaseFilter
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		
	private:// embed class or struct or enum

	public:// method	
		CFilterMask();
		virtual ~CFilterMask();

		virtual BOOLEAN fromRenderFilter(CRenderGroupCacheData* pRndGroupCacheData, const CRenderBaseFilter* pFilter);
		virtual void	doFilter(C2DBatchRenderGroup* pIn, C2DBatchRenderGroup* pbg, Array2<C2DBatchRenderGroup*>& aryOut);

	protected:// method
		CFilterMask(const CFilterMask&){}
		CFilterMask& operator =(const CFilterMask&){return *this;}

	private:// method

	protected:// property

	private:// property
		CCacheTexturePtr			m_pTex;
		CRenderFltMask*				m_pRndFilter;
		C2DBatchRenderGroupPtr		m_pBatchRenderGroup;
	};
}