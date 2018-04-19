#pragma once

namespace OpenHMI
{
	class C3DBatchRender : public Object
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		C3DBatchRender();
		virtual ~C3DBatchRender();

		void					setParent(C3DBatchRenderGroup* pGroup)		{ m_pParent = pGroup;	}
		C3DBatchRenderGroup*	getParent()									{ return m_pParent;		}

		void	addItem(CRenderModelCacheData* pModel);
		void	render();
		void	reset();

		const C3DMatrix& getModelMatrix();

	protected:// method
		C3DBatchRender(const C3DBatchRender&)	{}
		C3DBatchRender &operator = (const C3DBatchRender &){return *this;}

	private:// method
		void	bindAttributes();
		void	unBindAttributes();
		void	draw();

	protected:// property

	private:// property
		CRenderModelCacheData*	m_pModel;
		C3DBatchRenderGroupPtr	m_pParent;	

	};
}