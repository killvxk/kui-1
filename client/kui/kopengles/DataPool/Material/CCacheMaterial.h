#pragma once

namespace OpenHMI
{
	class CCacheMaterial : public CacheObject
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CCacheMaterial();
		virtual ~CCacheMaterial();

		void			addCustomTexture(CCacheTexture* pTex);
		void			setCustomTexture(UINT uIdx, CCacheTexture* pTex);
		CCacheTexture*	getCustomTexture(UINT uIdx);
		void			resetTextures();
		UINT			getCustomTextureNum() const			{  return m_aryTexture.size() ; }

		void			begin();		
		void			apply(Object* p, CRenderState* pRndState);
		void			end();

		void			addPass(CPass* pPass);
		void			activePass(UINT idx);
		CPass*			getCurrentPass()		{ return m_pCurrentPass;	}

		UINT			merge(CCacheMaterial* pMtl);

		void			reset();



	protected:// method
		CCacheMaterial(const CCacheMaterial &){}
		CCacheMaterial &operator = (const CCacheMaterial &){return *this;}	

	private:// method

	protected:// property

	private:// property	
		Array2<CCacheTexturePtr>	m_aryTexture;
		Array<CPass>				m_aryPass;
		CPass*						m_pCurrentPass;
		
	};

	typedef ObjectPtr<CCacheMaterial> CCacheMaterialPtr;
}