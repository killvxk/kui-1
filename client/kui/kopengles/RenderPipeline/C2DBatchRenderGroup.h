#pragma once

#include "./RenderPipeline/CBatchRenderGroup.h"
namespace OpenHMI
{
	class C2DBatchRenderGroup : public CBatchRenderGroup
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		C2DBatchRenderGroup();
		virtual ~C2DBatchRenderGroup();

		virtual void			add(C2DBatchRender* p);
		virtual void			insert(UINT uidx, C2DBatchRender* p);
		virtual void			render();
		virtual void			flush(BOOLEAN bGenerateRT = FALSE);
		virtual void			clear();
		virtual void			reset();

		void					copyfrom(C2DBatchRenderGroup* pBRG, UINT uiCnt, const CRectangle& rc);

		//C2DBatchRender*			getBatchRender(CRenderImage* pRndImage);
		C2DBatchRender*			getBatchRender(CRenderItem* pRndItem);
		C2DBatchRender*			getBatchRender(CCacheMaterial* pMtl);
		C2DBatchRender*			getFreeBatchRender();

		UINT					getCount() const								{ return m_aryUsedBatchRenderArray.size();	}
		BOOLEAN					isBlank() const									{ return m_bBlank;							}

		const C3DMatrix&		getProjectMatrix(BOOLEAN b25D);

		virtual void			release();

		static	void			releaseBatchRender();

	protected:// method
		C2DBatchRenderGroup(const C2DBatchRenderGroup&)	{}
		C2DBatchRenderGroup &operator = (const C2DBatchRenderGroup &){return *this;}	

	private:// method

	protected:// property

	private:// property
		Array2<C2DBatchRender*>	m_aryUsedBatchRenderArray;
		//Array2<C2DBatchRender*>	m_aryMergedUsedBatchRender;

		C3DMatrix			m_projectMatrix;
		C3DMatrix			m_perspectiveProjectMatrix;

		CDynamicUIMeshPtr	m_pMesh;
		CCacheMeshPtr		m_pVBOMesh;

		BOOLEAN				m_bBlank;

		static CPool<C2DBatchRender>		sm_listFreeBatchRender;
		
	};

	typedef ObjectPtr<C2DBatchRenderGroup>	C2DBatchRenderGroupPtr;
}