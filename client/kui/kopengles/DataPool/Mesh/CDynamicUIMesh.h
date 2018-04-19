#pragma once

#include "../../RenderPipeline/C2DBatchRender.h"

namespace OpenHMI
{
	class CDynamicUIMesh : public CCacheMesh
	{
	public:// const define

	private:// const define

	public:// embed class or struct or enum

	private:// embed class or struct or enum

	public:// method	
		CDynamicUIMesh();
		virtual ~CDynamicUIMesh();

		void resizeQuad(UINT uisize);
		void fillQuad(UINT uidx,const C2DBatchRender::tagBatchRenderItem& rItem);
		void fillQuad(UINT uidx,const C2DBatchRender::tagBatchRenderItem& rItem,const C3DMatrix& matrix);

		void reset();
		
	protected:// method
		CDynamicUIMesh &operator = (const CDynamicUIMesh &){return *this;}	

		void clear();

	private:// method

	protected:// property	
		
	private:// property		
		UINT	m_uiQuadSize;
		UINT	m_uiQuadCapacity;

	};

	typedef ObjectPtr<CDynamicUIMesh> CDynamicUIMeshPtr;
}