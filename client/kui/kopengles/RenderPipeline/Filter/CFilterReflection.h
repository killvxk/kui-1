#pragma once

namespace OpenHMI
{
	class CFilterReflection : public CBaseFilter
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum
		
	private:// embed class or struct or enum

	public:// method	
		CFilterReflection();
		virtual ~CFilterReflection();

		virtual BOOLEAN fromRenderFilter(CRenderGroupCacheData* pRndGroupCacheData, const CRenderBaseFilter* pFilter);
		virtual void	doFilter(C2DBatchRenderGroup* pIn, C2DBatchRenderGroup* pbg, Array2<C2DBatchRenderGroup*>& aryOut);

	protected:// method
		CFilterReflection(const CFilterReflection&){}
		CFilterReflection& operator =(const CFilterReflection&){return *this;}

	private:// method

	protected:// property

	private:// property
		ObjectPtr<CCacheMaterial>	m_pMaskMaterial;
		CRenderFltReflection *		m_pRndFilter;
		C3DMatrix*					m_pTransformMatrix;
		C2DBatchRenderGroupPtr		m_pBatchRenderGroup;
	};
}