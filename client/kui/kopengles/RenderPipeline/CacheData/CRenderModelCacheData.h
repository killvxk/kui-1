#pragma once

namespace OpenHMI
{
	class CRenderModelCacheData : public CMaterialRenderCacheData
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CRenderModelCacheData();
		virtual ~CRenderModelCacheData();

		void		updateMatrix(const C3DVector& pos, const C3DVector& rot, const C3DVector& scale);
		void		setTexture(CCacheTexture* pTex);

		CCacheMesh*	getMesh();
		void		setMesh(CCacheMesh* pMesh);

		const C3DMatrix& getModelMatrix() const		{ return m_matrix;	}

	protected:// method
		CRenderModelCacheData(const CRenderModelCacheData&)	{}
		CRenderModelCacheData &operator = (const CRenderModelCacheData &){return *this;}

		virtual void	flushMaterial();

	private:// method

	protected:// property

	private:// property
		CCacheMeshPtr		m_pMesh;
		C3DMatrix			m_matrix;
		CCacheTexturePtr	m_pTex;

		CWaveRenderDelegate*	m_pRenderDelegate;
	};
}