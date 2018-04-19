#pragma once

namespace OpenHMI
{
	class CFilterGlass : public CBaseFilter
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		
	private:// embed class or struct or enum

	public:// method	
		CFilterGlass();
		virtual ~CFilterGlass();

		virtual BOOLEAN fromRenderFilter(CRenderGroupCacheData* pRndGroupCacheData, const CRenderBaseFilter* pFilter);
		virtual void	doFilter(C2DBatchRenderGroup* pIn, C2DBatchRenderGroup* pbg, Array2<C2DBatchRenderGroup*>& aryOut);

	protected:// method
		CFilterGlass(const CFilterGlass&){}
		CFilterGlass& operator =(const CFilterGlass&){return *this;}

	private:// method

	protected:// property

	private:// property
		ObjectPtr<CCacheMaterial>	m_pMaskMaterial;
		//tagResourceLoc						m_iMaskTexID;
		INT						m_iBlurSize;
		INT						m_iBlurDirection;
		CRenderFltGlass	   *	m_pRndFilter;
		C2DBatchRenderGroupPtr	m_pBatchRenderGroup;
		C2DBatchRenderGroupPtr	m_pBatchRenderGroupTemp;
	};
}