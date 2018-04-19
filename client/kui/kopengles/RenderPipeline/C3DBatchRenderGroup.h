#pragma once

namespace OpenHMI
{
	class C3DBatchRenderGroup : public CBatchRenderGroup
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		C3DBatchRenderGroup();
		virtual ~C3DBatchRenderGroup();

		void	add(C3DBatchRender* p);
		void	render();

		void		setCamera(CCamera* pCamera)	{ m_pCamera = pCamera;	}
		CCamera*	getCamera()					{ return m_pCamera;		}

		C3DBatchRender*			getFreeBatchRender();

		virtual void			release();

		static	void			releaseBatchRender();


	protected:// method
		C3DBatchRenderGroup(const C3DBatchRenderGroup&)	{}
		C3DBatchRenderGroup &operator = (const C3DBatchRenderGroup &){return *this;}

	private:// method

	protected:// property

	private:// property
		Array2<C3DBatchRender*>				m_aryUsedBatchRenderArray;
		CCamera*							m_pCamera;

		static CPool<C3DBatchRender>		sm_listFreeBatchRender;
	};

	typedef ObjectPtr<C3DBatchRenderGroup>	C3DBatchRenderGroupPtr;
	
}