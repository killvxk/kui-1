#pragma once

namespace OpenHMI
{
	class CMaterialRenderCacheData : public CRenderCacheData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CMaterialRenderCacheData();
		virtual ~CMaterialRenderCacheData();

		virtual CRenderItem*	getRenderItem()	{ return NULL;	}

		CCacheMaterial*			getMaterial();

	protected:// method
		CMaterialRenderCacheData(const CMaterialRenderCacheData&)	{}
		CMaterialRenderCacheData &operator = (const CMaterialRenderCacheData &){return *this;}	
		
		virtual void	flushMaterial();

	private:// method

	protected:// property
		//INT		m_iMtlID;
		ObjectPtr<CCacheMaterial>	m_pMtl;

	private:// property	
				
	};
}